// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Equipment/EquipmentEvents.h"
#include "Game/Save/SaveableInterface.h"
#include "Item/ItemTypes.h"
#include "PlayerEquipmentComponent.generated.h"

class AFishingRodActor;
class AEquipmentActor;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAuraPlayerEquipmentComponentSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	EEquipmentUseMode EquipmentUseMode = EEquipmentUseMode::None;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	TMap<EEquipmentSlot, FGameplayTag> EquipmentSlots;
};

DECLARE_MULTICAST_DELEGATE(FOnEquipmentUseModeChangeSignature)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEquipmentAnimationCompleteSignature,
	EEquipmentSlot,
	EquipmentSlot,
	const FGameplayTag&,
	EquippedItem
);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPlayerEquipmentComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:
	UPlayerEquipmentComponent();

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& Data) override;
	virtual bool ShouldSave_Implementation() const override { return true; }
	virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual bool ShouldAutoSpawn_Implementation() const override { return false; }
	virtual FString GetSaveID_Implementation() const override;
	/** End ISaveableInterface **/

	FOnEquipmentUseModeChangeSignature OnUseWeapon;
	FOnEquipmentUseModeChangeSignature OnUseTool;
	UPROPERTY(BlueprintAssignable)
	FEquipmentDelegateSignature OnEquipmentAnimationRequest;
	UPROPERTY(BlueprintAssignable)
	FEquipmentDelegateSignature OnEquipmentAnimationCompleteDelegate;

	void InitializeEquipment();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasToolEquipped(const FGameplayTag& ItemType) const;


	bool IsUsingTool() const;
	bool IsUsingWeapon() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UseEquipment(const EEquipmentSlot Slot);
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseTool();
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseWeapon();
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	void UseNothing();
	void Equip(const EEquipmentSlot& Slot, const FGameplayTag& ItemType);
	AEquipmentActor* GetWeapon() const;
	USkeletalMeshComponent* GetWeaponMesh() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	FGameplayTag GetToolType() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	FGameplayTag GetWeaponType() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	AFishingRodActor* GetFishingRod() const;
	UFUNCTION(BlueprintCallable, Category="Item|Equipment")
	FGameplayTag GetEquippedItem(const EEquipmentSlot Slot) const;
	EEquipmentUseMode GetEquipmentUseMode() const;
	void PlayEquipAnimation(EEquipmentSlot Slot) const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EquipAnimationCompleted(const EEquipmentSlot Slot);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	EEquipmentUseMode EquipmentUseMode = EEquipmentUseMode::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Equipment")
	TMap<EEquipmentSlot, FGameplayTag> EquipmentSlots;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Inventory|Equipment")
	TMap<EEquipmentSlot, FName> EquipmentSocketNames;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Debug")
	FString SaveID;

private:
	USkeletalMeshComponent* GetCharacterMesh() const;
	UPROPERTY()
	TObjectPtr<AEquipmentActor> Weapon;
	UPROPERTY()
	TObjectPtr<AEquipmentActor> Tool;

	AEquipmentActor* SpawnEquipment(const EEquipmentSlot& Slot);

	TArray<uint8> SerializeComponentData() const;
	bool DeserializeComponentData(const TArray<uint8>& Data);
};
