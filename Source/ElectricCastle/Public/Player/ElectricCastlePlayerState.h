// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/PlayerState.h"
#include "ElectricCastlePlayerState.generated.h"

class UElectricCastleAttributeSet;
class UElectricCastleAbilitySystemComponent;
class UInventoryComponent;
class ULevelUpInfo;

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastlePlayerState : public APlayerState, public IAbilitySystemInterface, public IElectricCastleAbilitySystemInterface, public ISaveableInterface
{
	GENERATED_BODY()

public:
	AElectricCastlePlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UElectricCastleAbilitySystemComponent* GetElectricCastleAbilitySystemComponent() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UElectricCastleAttributeSet* GetAttributeSet() const;

	/** Start IElectricCastleAbilitySystemInterface **/
	virtual int32 GetCharacterLevel_Implementation() const override;
	/** End IElectricCastleAbilitySystemInterface **/

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& InData) override;
	virtual bool ShouldSave_Implementation() const override { return false; }
	/** End ISaveableInterface **/

	void InitializeAbilityActorInfo();

protected:
	virtual void BeginPlay() override;
	UPROPERTY(Replicated)
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<UElectricCastleAttributeSet> AttributeSet;
};
