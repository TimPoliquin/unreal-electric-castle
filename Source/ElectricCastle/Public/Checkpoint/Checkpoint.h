// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ACheckpoint : public APlayerStart, public ISaveableInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	/**
	 * Save Interface
	 */
	virtual void PostLoad_Implementation() override;
	/**
	 *Save Interface End
	 */

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicInstance);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayActivatedEffect();

	/** Highlight Interface*/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** Highlight Interface End**/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bHasBeenActivated = false;
	UPROPERTY(BlueprintReadOnly)
	bool bDisableAfterActivation = true;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;
};
