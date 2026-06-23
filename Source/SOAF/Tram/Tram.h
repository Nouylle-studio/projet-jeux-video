#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TramTrack.h"
#include "Tram.generated.h"

UCLASS()
class SOAF_API ATram : public AActor
{
	GENERATED_BODY()

public:
	ATram();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tram")
	UStaticMeshComponent* TramMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tram")
	class UBoxComponent* BoardTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tram")
	USceneComponent* PassengerSeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram")
	ATramTrack* TrackRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram")
	float Speed;

	UPROPERTY(EditAnywhere, Category = "Tram")
	TSubclassOf<UUserWidget> InteractionWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Tram")
	void BoardTram();

	UFUNCTION(BlueprintCallable, Category = "Tram")
	void SummonTram();

	UFUNCTION(BlueprintCallable, Category = "Tram")
	void ExitTram();

	bool IsPlayerOnBoard() const { return bPlayerOnBoard; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	float Progress;
	bool bIsMoving;
	bool bPlayerOnBoard;
	bool bGoingForward;
	class ACharacter* NearbyPlayer;
	UUserWidget* InteractionWidget;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
