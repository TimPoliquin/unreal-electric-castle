// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class UGameplayEffect;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEquipped() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetItemType() const;
	UFUNCTION(BlueprintCallable)
	void SetItemType(const FGameplayTag InItemType);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetEquippedTag() const;
	UFUNCTION(BlueprintCallable)
	void SetEquippedTag(const FGameplayTag InEquippedTag);
	UFUNCTION(BlueprintCallable)
	void Equip(AActor* InOwner);
	UFUNCTION(BlueprintCallable)
	void Unequip(AActor* InOwner);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Proeprties", meta=(Categories="Item.Type"))
	FGameplayTag ItemType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(Categories="Player.Equipped"))
	FGameplayTag EquippedTag;

private:
	void ApplyEquipEffect(AActor* InOwner);
	void RemoveEquipEffect(AActor* InOwner);

	FActiveGameplayEffectHandle EquipEffectHandle;
};
