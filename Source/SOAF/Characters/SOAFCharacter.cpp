#include "SOAFCharacter.h"
#include "Tram.h"
#include "TramStop.h"
#include "SOAFInteractable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

void ASOAFCharacter::OnInteract()
{
	if (bIsOnTram) return;

	if (HeldItem)
	{
		OnDropHeldItem();
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
