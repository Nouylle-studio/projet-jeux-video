#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SOAFInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USOAFInteractable : public UInterface
{
	GENERATED_BODY()
};

class SOAF_API ISOAFInteractable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void Interact(ACharacter* Interactor);
};
