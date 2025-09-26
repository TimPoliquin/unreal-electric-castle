// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/EffectAreaActor.h"
#include "Actor/InteractionInterface.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/Actor.h"
#include "PointOfInterestActor.generated.h"

class UCapsuleComponent;
class USphereComponent;
class UWidgetComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API APointOfInterestActor : public AEffectAreaActor, public IInteractionInterface, public ISaveableInterface
{
	GENERATED_BODY()

public:
	APointOfInterestActor();

protected:
	virtual void BeginPlay() override;
	void InitializeState();

	/** Start ISaveableInterface **/
	virtual void PostLoad_Implementation() override;
	/** End ISaveableInterface **/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> POIWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> PreconditionWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCapsuleComponent> OverlapDetectionComponent;

	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool BFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	void ShowInteractWithPOIAvailable(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	void ShowPreconditionWidget(AActor* Player) const;

	/** AuraInteractionInterface Start **/
	virtual bool OnInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintNativeEvent)
	void HandleInteract(AActor* Player);
	virtual void OnInteractionEnd_Implementation(AActor* Player, const bool bIsCancelled) override;
	/** AuraInteractionInterface End **/
	void EnablePOI();
	void DisablePOI();
	bool IsPOIDisabled() const;

private:
	bool IsPlayerActor(const AActor* Actor) const;
	UPROPERTY(SaveGame)
	bool bDisabled = false;
};
