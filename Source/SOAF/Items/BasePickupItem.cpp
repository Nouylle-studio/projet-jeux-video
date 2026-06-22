#include "BasePickupItem.h"

ABasePickupItem::ABasePickupItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	Mesh->SetSimulatePhysics(false);
}

void ABasePickupItem::Interact_Implementation(ACharacter* Interactor)
{
	// Logique override dans les classes Blueprint enfants
}
