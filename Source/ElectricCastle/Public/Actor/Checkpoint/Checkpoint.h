// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Highlight/HighlightActorInterface.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/PlayerStart.h"
#include "Checkpoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API ACheckpoint : public APlayerStart, public ISaveableInterface, public IHighlightActorInterface
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

	/** HighlightActor Interface*/
	virtual UHighlightComponent* GetHighlightComponent_Implementation() const override;
	virtual void GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes) override;
	/** HighlightActor Interface End**/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHighlightComponent> HighlightComponent;
	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bHasBeenActivated = false;
	UPROPERTY(BlueprintReadOnly)
	bool bDisableAfterActivation = true;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;
};
