// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ElectricCastleGameState.generated.h"

class UProgressionComponent;
class UInventoryComponent;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastleGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	static AElectricCastleGameState* Get(const UObject* WorldContextObject);

	AElectricCastleGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UInventoryComponent* GetInventoryComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UProgressionComponent* GetProgressionComponent() const;

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category="Components")
	TObjectPtr<UInventoryComponent> InventoryComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Replicated, Category="Components")
	TObjectPtr<UProgressionComponent> ProgressionComponent;
};
