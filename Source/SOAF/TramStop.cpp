#include "TramStop.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ATramStop::ATramStop()
{
	PrimaryActorTick.bCanEverTick = false;

	CallTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("CallTrigger"));
	RootComponent = CallTrigger;
	CallTrigger->SetBoxExtent(FVector(200.f, 200.f, 100.f));
	CallTrigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	TramRef = nullptr;
	NearbyPlayer = nullptr;
	CallWidget = nullptr;
}

void ATramStop::BeginPlay()
{
	Super::BeginPlay();
	CallTrigger->OnComponentBeginOverlap.AddDynamic(this, &ATramStop::OnBeginOverlap);
	CallTrigger->OnComponentEndOverlap.AddDynamic(this, &ATramStop::OnEndOverlap);
}

void ATramStop::CallTram()
{
	if (!TramRef) return;
	TramRef->SummonTram();

	if (CallWidget)
	{
		CallWidget->RemoveFromParent();
		CallWidget = nullptr;
	}
}

void ATramStop::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character) return;

	NearbyPlayer = Character;

	if (CallWidgetClass)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (!PC) return;
		CallWidget = CreateWidget<UUserWidget>(PC, CallWidgetClass);
		if (!CallWidget) return;
		CallWidget->AddToViewport();
	}
}

void ATramStop::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character || Character != NearbyPlayer) return;

	NearbyPlayer = nullptr;

	if (CallWidget)
	{
		CallWidget->RemoveFromParent();
		CallWidget = nullptr;
	}
}
