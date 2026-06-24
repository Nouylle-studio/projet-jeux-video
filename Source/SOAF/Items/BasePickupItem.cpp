#include "BasePickupItem.h"
#include "SOAFCharacter.h"

ABasePickupItem::ABasePickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetSimulatePhysics(false);
}

void ABasePickupItem::Interact_Implementation(ACharacter* Interactor)
{
	if (ASOAFCharacter* Character = Cast<ASOAFCharacter>(Interactor))
		Character->StartPickup(this, HeldItemClass);
}
