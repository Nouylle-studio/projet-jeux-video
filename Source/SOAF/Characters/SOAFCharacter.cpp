#include "SOAFCharacter.h"
#include "Tram.h"
#include "TramStop.h"
#include "SOAFInteractable.h"
#include "BasePickupItem.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

ASOAFCharacter::ASOAFCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	HeldItem = nullptr;
	bIsOnTram = false;
}

void ASOAFCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (IMC_Default)
				Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}
	RegisterMappingContextForUserSettings();
}

void ASOAFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Interact)
			EIC->BindAction(IA_Interact, ETriggerEvent::Started, this, &ASOAFCharacter::OnInteract);
	}
}

void ASOAFCharacter::RegisterMappingContextForUserSettings()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (UEnhancedInputUserSettings* US = Sub->GetUserSettings())
			{
				if (IMC_Default && !US->IsMappingContextRegistered(IMC_Default))
				{
					FGameplayTagContainer Failure;
					US->RegisterInputMappingContext(IMC_Default);
				}
			}
		}
	}
}

void ASOAFCharacter::ApplyKeyMapping(FName MappingName, FKey NewKey)
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (UEnhancedInputUserSettings* US = Sub->GetUserSettings())
			{
				FMapPlayerKeyArgs Args;
				Args.MappingName = MappingName;
				Args.Slot = EPlayerMappableKeySlot::First;
				Args.NewKey = NewKey;
				Args.bCreateMatchingSlotIfNeeded = true;

				FGameplayTagContainer Failure;
				US->MapPlayerKey(Args, Failure);
				US->ApplySettings();
				US->AsyncSaveSettings();
			}
		}
	}
}

void ASOAFCharacter::OnInteract()
{
	if (bIsOnTram) return;

	if (HeldItem)
	{
		DropHeldItem();
		return;
	}

	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (ATram* Tram = Cast<ATram>(Actor))
		{
			if (Tram->IsPlayerOnBoard())
				Tram->ExitTram();
			else
				Tram->BoardTram();
			return;
		}

		if (ATramStop* Stop = Cast<ATramStop>(Actor))
		{
			Stop->CallTram();
			return;
		}

		if (Actor->Implements<USOAFInteractable>())
		{
			ISOAFInteractable::Execute_Interact(Actor, this);
			return;
		}
		OnInteractWithActor(Actor);
	}
}

void ASOAFCharacter::StartPickup(ABasePickupItem* PickupActor, TSubclassOf<AActor> InHeldItemClass)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("StartPickup C++ called"));
	if (!PickupActor || !InHeldItemClass)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartPickup EARLY RETURN — PickupActor: %s, HeldItemClass: %s"),
			PickupActor ? TEXT("OK") : TEXT("NULL"),
			InHeldItemClass ? *InHeldItemClass->GetName() : TEXT("NULL")));
		return;
	}

	PendingPickupActor = PickupActor;
	CurrentPickupClass = PickupActor->GetClass();
	PickupActor->Mesh->SetVisibility(false);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(InHeldItemClass, GetActorTransform(), Params);
	if (Spawned)
	{
		Spawned->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, PickupSocketName);
		HeldItem = Spawned;
	}

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		if (PickupMontage)
		{
			Anim->Montage_Play(PickupMontage);
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &ASOAFCharacter::OnPickupMontageEnded);
			Anim->Montage_SetEndDelegate(EndDelegate, PickupMontage);
		}
	}
}

void ASOAFCharacter::OnPickupMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (PendingPickupActor.IsValid())
	{
		PendingPickupActor->Destroy();
		PendingPickupActor = nullptr;
	}
}

void ASOAFCharacter::DropHeldItem()
{
	if (!HeldItem) return;

	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.f;

	HeldItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HeldItem->Destroy();
	HeldItem = nullptr;

	if (CurrentPickupClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AActor>(CurrentPickupClass, DropLocation, FRotator::ZeroRotator, Params);
		CurrentPickupClass = nullptr;
	}
}
