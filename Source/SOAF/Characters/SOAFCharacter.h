#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SOAFCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class ABasePickupItem;
class UAnimMontage;

UCLASS()
class SOAF_API ASOAFCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASOAFCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Interact;

	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	AActor* HeldItem;

	bool bIsOnTram;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	UAnimMontage* PickupMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	FName PickupSocketName = FName("Guitar_Socket");

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractWithActor(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void OnItemDropped();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void ApplyKeyMapping(FName MappingName, FKey NewKey);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RegisterMappingContextForUserSettings();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void StartPickup(ABasePickupItem* PickupActor, TSubclassOf<AActor> InHeldItemClass);

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void OnInteract();
	void DropHeldItem();
	void OnPickupMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	TWeakObjectPtr<ABasePickupItem> PendingPickupActor;
	TSubclassOf<AActor> CurrentPickupClass;
};
