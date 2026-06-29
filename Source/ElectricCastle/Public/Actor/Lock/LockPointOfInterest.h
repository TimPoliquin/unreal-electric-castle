// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/POI/PointOfInterestActor.h"
#include "LockPointOfInterest.generated.h"

class ILockInterface;
class UUserWidget;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ALockPointOfInterest : public APointOfInterestActor
{
	GENERATED_BODY()

public:
	ALockPointOfInterest();

	/** ISaveGameInterface Start **/
	virtual void PostLoad_Implementation() override;
	virtual bool IsPreconditionMet_Implementation(AActor* Player) const override;
	virtual void HandleInteract_Implementation(AActor* Player) override;
	/** ISaveGameInterface End **/

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Unlock(AActor* Player);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayUnlockEffect(AActor* Player);
	virtual UTexture2D* GetPreconditionIcon_Implementation() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LockMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock", meta=(Categories="Item.Type.Key"))
	FGameplayTag KeyTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Lock", meta=(AllowedClasses="/Script/Aura.AuraLockedInterface"))
	TArray<AActor*> Gates;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lock")
	TObjectPtr<USoundBase> UnlockSound;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Lock")
	bool bUnlocked = false;
};
