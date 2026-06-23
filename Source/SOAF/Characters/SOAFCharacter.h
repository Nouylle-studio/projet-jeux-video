#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SOAFCharacter.generated.h"

class UInputAction;
class UInputMappingContext;

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

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnInteractWithActor(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void OnDropHeldItem();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void OnInteract();
};
