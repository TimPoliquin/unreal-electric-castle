// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/EffectAreaActor.h"
#include "Actor/InteractionInterface.h"
#include "Actor/Highlight/HighlightActorInterface.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/Actor.h"
#include "PointOfInterestActor.generated.h"

class UCapsuleComponent;
class USphereComponent;
class UWidgetComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API APointOfInterestActor : public AEffectAreaActor, public IInteractionInterface, public ISaveableInterface, public IHighlightActorInterface
{
	GENERATED_BODY()

public:
	APointOfInterestActor();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void OnConstruction_InitializeWidgets();
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EnablePOI();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DisablePOI();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPOIDisabled() const;
	/** Start ISaveableInterface **/
	virtual void PostLoad_Implementation() override;
	/** End ISaveableInterface **/
	/** Start IHighlightActorInterface**/
	virtual UHighlightComponent* GetHighlightComponent_Implementation() const override;
	virtual void GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes) override;
	/** End IHighlightActorInterface**/

protected:
	virtual void BeginPlay() override;
	void InitializeState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> POIWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> PreconditionWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCapsuleComponent> OverlapDetectionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHighlightComponent> HighlightComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POI")
	FString InteractText = FString("Interact");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="POI")
	FString PreconditionText = FString("Precondition not met");

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
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UTexture2D* GetPreconditionIcon() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsPreconditionMet(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	void ShowInteractWithPOIAvailable(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	void ShowPreconditionWidget(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandlePlayerOverlapBegin(AActor* OtherActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandlePlayerOverlapEnd(AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeInteractionWidgetSettings(UUserWidget* InInteractionWidget, const FString& InInteractionText);
	UFUNCTION(BlueprintImplementableEvent)
	void InitializePreconditionWidgetSettings(UUserWidget* InPreconditionWidget, const UTexture2D* InRuneIcon);


	/** AuraInteractionInterface Start **/
	virtual bool OnInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintNativeEvent)
	void HandleInteract(AActor* Player);
	virtual void OnInteractionEnd_Implementation(AActor* Player, const bool bIsCancelled) override;
	/** AuraInteractionInterface End **/

private:
	bool IsPlayerActor(const AActor* Actor) const;
	UPROPERTY(SaveGame)
	bool bDisabled = false;
	UPROPERTY(SaveGame)
	bool bIsOverlapping = false;
	UPROPERTY()
	FTimerHandle OverlapDelayTimerHandle;
};
