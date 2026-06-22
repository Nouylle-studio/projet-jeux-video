#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Tram.h"
#include "TramStop.generated.h"

UCLASS()
class SOAF_API ATramStop : public AActor
{
	GENERATED_BODY()

public:
	ATramStop();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TramStop")
	class UBoxComponent* CallTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TramStop")
	ATram* TramRef;

	UPROPERTY(EditAnywhere, Category = "TramStop")
	TSubclassOf<UUserWidget> CallWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "TramStop")
	void CallTram();

protected:
	virtual void BeginPlay() override;

private:
	class ACharacter* NearbyPlayer;
	UUserWidget* CallWidget;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
