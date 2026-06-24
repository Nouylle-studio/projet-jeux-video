#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SOAFInteractable.h"
#include "BasePickupItem.generated.h"

UCLASS()
class SOAF_API ABasePickupItem : public AActor, public ISOAFInteractable
{
	GENERATED_BODY()

public:
	ABasePickupItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TSubclassOf<AActor> HeldItemClass;

	virtual void Interact_Implementation(ACharacter* Interactor) override;
};
