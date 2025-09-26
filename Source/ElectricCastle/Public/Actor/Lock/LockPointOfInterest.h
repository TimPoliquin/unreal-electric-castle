// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/POI/PointOfInterestActor.h"
#include "Interaction/HighlightInterface.h"
#include "LockPointOfInterest.generated.h"

class ILockedInterface;
class UUserWidget;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ALockPointOfInterest : public APointOfInterestActor, public IHighlightInterface
{
	GENERATED_BODY()

public:
	ALockPointOfInterest();

	/** IHighlightInterface Start **/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** IHighlightInterface End **/
	/** ISaveGameInterface Start **/
	virtual void PostLoad_Implementation() override;
	/** ISaveGameInterface End **/

protected:
	virtual void BeginPlay() override;
	virtual bool IsPreconditionMet_Implementation(AActor* Player) const override;
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeInteractionWidgetSettings(UUserWidget* InInteractionWidget, const FString& InInteractionText);
	UFUNCTION(BlueprintImplementableEvent)
	void InitializePreconditionWidgetSettings(UUserWidget* InPreconditionWidget, const UTexture2D* InRuneIcon);
	virtual void HandleInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintCallable)
	void Unlock(AActor* Player);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayUnlockEffect(AActor* Player);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LockMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock", meta=(Categories="Item.Type.Key"))
	FGameplayTag KeyTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Lock", meta=(AllowedClasses="/Script/Aura.AuraLockedInterface"))
	TArray<AActor*> Gates;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	TObjectPtr<UTexture2D> RuneIcon;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	TObjectPtr<USoundBase> UnlockSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	FString InteractText = FString("Unlock");
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Lock")
	bool bUnlocked = false;
};
