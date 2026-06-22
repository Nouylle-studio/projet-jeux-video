#include "Tram.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "SOAFCharacter.h"

ATram::ATram()
{
	PrimaryActorTick.bCanEverTick = true;

	TramMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TramMesh"));
	RootComponent = TramMesh;
	TramMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	BoardTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardTrigger"));
	BoardTrigger->SetupAttachment(RootComponent);
	BoardTrigger->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	BoardTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	PassengerSeat = CreateDefaultSubobject<USceneComponent>(TEXT("PassengerSeat"));
	PassengerSeat->SetupAttachment(RootComponent);

	Speed = 300.f;
	Progress = 0.f;
	bIsMoving = false;
	bPlayerOnBoard = false;
	bGoingForward = true;
	NearbyPlayer = nullptr;
	InteractionWidget = nullptr;
}

void ATram::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (TrackRef && TrackRef->TramSpline)
	{
		FVector StartLocation = TrackRef->TramSpline->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		FRotator StartRotation = TrackRef->TramSpline->GetRotationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		SetActorLocationAndRotation(StartLocation, StartRotation);
	}
}

void ATram::BeginPlay()
{
	Super::BeginPlay();
	BoardTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATram::OnBeginOverlap);
	BoardTrigger->OnComponentEndOverlap.AddDynamic(this, &ATram::OnEndOverlap);

	if (TrackRef && TrackRef->TramSpline)
	{
		Progress = 0.f;
		bGoingForward = true;
		bIsMoving = false;
		FVector StartLocation = TrackRef->TramSpline->GetLocationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		FRotator StartRotation = TrackRef->TramSpline->GetRotationAtDistanceAlongSpline(0.f, ESplineCoordinateSpace::World);
		SetActorLocationAndRotation(StartLocation, StartRotation);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Tram spawned at: %s"), *StartLocation.ToString()));
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("BeginPlay: TrackRef NULL!"));
	}
}

void ATram::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsMoving) return;
	if (!TrackRef) { if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("TrackRef null!")); return; }
	if (!TrackRef->TramSpline) { if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, TEXT("Spline null!")); return; }

	float SplineLength = TrackRef->TramSpline->GetSplineLength();

	if (bGoingForward)
		Progress += Speed * DeltaTime;
	else
		Progress -= Speed * DeltaTime;

	if (Progress >= SplineLength)
	{
		Progress = SplineLength;
		bIsMoving = false;

		if (bPlayerOnBoard && IsValid(NearbyPlayer))
		{
			UCharacterMovementComponent* CMC = NearbyPlayer->GetCharacterMovement();
			if (CMC) CMC->SetMovementMode(MOVE_Walking);
			APlayerController* PC = Cast<APlayerController>(NearbyPlayer->GetController());
			if (PC) PC->ResetIgnoreMoveInput();
			if (ASOAFCharacter* SC = Cast<ASOAFCharacter>(NearbyPlayer))
				SC->bIsOnTram = false;
		}
		bPlayerOnBoard = false;
		NearbyPlayer = nullptr;
	}
	else if (Progress <= 0.f)
	{
		Progress = 0.f;
		bGoingForward = true;
		bIsMoving = false;

		if (bPlayerOnBoard && IsValid(NearbyPlayer))
		{
			UCharacterMovementComponent* CMC = NearbyPlayer->GetCharacterMovement();
			if (CMC) CMC->SetMovementMode(MOVE_Walking);
			APlayerController* PC = Cast<APlayerController>(NearbyPlayer->GetController());
			if (PC) PC->ResetIgnoreMoveInput();
			if (ASOAFCharacter* SC = Cast<ASOAFCharacter>(NearbyPlayer))
				SC->bIsOnTram = false;
		}
		bPlayerOnBoard = false;
		NearbyPlayer = nullptr;
	}

	FVector NewLocation = TrackRef->TramSpline->GetLocationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);
	FRotator NewRotation = TrackRef->TramSpline->GetRotationAtDistanceAlongSpline(Progress, ESplineCoordinateSpace::World);
	SetActorLocationAndRotation(NewLocation, NewRotation);

	if (bPlayerOnBoard && IsValid(NearbyPlayer))
	{
		NearbyPlayer->SetActorLocation(PassengerSeat->GetComponentLocation());
	}
}

void ATram::ExitTram()
{
	if (!bPlayerOnBoard || !IsValid(NearbyPlayer)) return;

	UCharacterMovementComponent* CMC = NearbyPlayer->GetCharacterMovement();
	if (CMC) CMC->SetMovementMode(MOVE_Walking);

	APlayerController* PC = Cast<APlayerController>(NearbyPlayer->GetController());
	if (PC) PC->ResetIgnoreMoveInput();

	if (ASOAFCharacter* SC = Cast<ASOAFCharacter>(NearbyPlayer))
		SC->bIsOnTram = false;

	bPlayerOnBoard = false;
	NearbyPlayer = nullptr;
}

void ATram::SummonTram()
{
	if (bIsMoving) return;

	if (bGoingForward && Progress <= 0.f) return; // déjà au départ

	bGoingForward = false;
	bIsMoving = true;
}

void ATram::BoardTram()
{
	if (!IsValid(NearbyPlayer)) return;

	bPlayerOnBoard = true;
	bIsMoving = true;

	if (TrackRef && TrackRef->TramSpline)
	{
		float SplineLength = TrackRef->TramSpline->GetSplineLength();
		bGoingForward = (Progress < SplineLength * 0.5f);
	}

	UCharacterMovementComponent* CMC = NearbyPlayer->GetCharacterMovement();
	if (CMC) CMC->DisableMovement();

	APlayerController* PC = Cast<APlayerController>(NearbyPlayer->GetController());
	if (PC) PC->SetIgnoreMoveInput(true);

	if (ASOAFCharacter* SC = Cast<ASOAFCharacter>(NearbyPlayer))
		SC->bIsOnTram = true;

	if (InteractionWidget)
	{
		InteractionWidget->RemoveFromParent();
		InteractionWidget = nullptr;
	}
}

void ATram::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Tram overlap!"));

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character) return;
	if (bPlayerOnBoard) return;

	NearbyPlayer = Character;

	if (InteractionWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (!PC)
		{
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PC null!"));
			return;
		}
		InteractionWidget = CreateWidget<UUserWidget>(PC, InteractionWidgetClass);
		if (!InteractionWidget)
		{
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Widget null!"));
			return;
		}
		InteractionWidget->AddToViewport();
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Widget added!"));
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No widget class!"));
	}
}

void ATram::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || Character != NearbyPlayer) return;

	if (bPlayerOnBoard) return;

	NearbyPlayer = nullptr;

	if (InteractionWidget)
	{
		InteractionWidget->RemoveFromParent();
		InteractionWidget = nullptr;
	}
}
