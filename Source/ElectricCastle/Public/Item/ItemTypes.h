#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Utils/RandUtils.h"
#include "ItemTypes.generated.h"


class AEquipmentActor;

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	/** Default type - these items have no use**/
	None,
	/** Consumable type - these items can be used 1x **/
	Consumable,
	/** Equipment type - these items can be equipped into equipment slots */
	Equipment,
	/** Fish type - these items are fish **/
	Fish,
	/** Instant type - these items have an effect immediately on contact with the player */
	Instant,
	/** Used to unlock doors/chests/etc */
	Key,
};

UENUM(BlueprintType)
enum class EEquipmentCategory : uint8
{
	/** Default equipment type - this probably indicates a configuration error */
	None,
	/** Tool equipment type - these items can be equipped and used to perform special tasks */
	Tool,
	/** Weapon equipment type - these items can be equipped and allow the player to perform in combat */
	Weapon,
};

USTRUCT(BlueprintType)
struct FItemDefinition
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FString ItemName = FString("INVALID");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FString ItemDescription = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	UTexture2D* ItemIcon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(Categories="Item.Type"))
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	EItemCategory ItemCategory = EItemCategory::None;
	/** Determines the maximum number of this item that can exist in the inventory **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	int32 InventoryMaxCount = 1;
	/** Determines how many slots the item occupies in the inventory **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	int32 InventorySize = 1;
	/** If the item is equipment, this specifies which type of equipment it is */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Equipment")
	EEquipmentCategory EquipmentCategory = EEquipmentCategory::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(Categories="Message"))
	FGameplayTag PickupMessageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(Categories="Message"))
	FGameplayTag UseMessageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSubclassOf<AActor> ItemClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(EditCondition="EquipmentCategory != EEquipmentCategory::None", EditConditionHides))
	TSubclassOf<AEquipmentActor> EquipmentClass;

	bool IsValid() const
	{
		return ItemType.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FItemInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Item", meta=(Categories="Item.Type"))
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Item")
	int32 ItemCount = 0;

	bool operator==(const FItemInventoryEntry& Other) const
	{
		return ItemType.MatchesTagExact(Other.ItemType);
	}

	// Serialization function
	friend FArchive& operator<<(FArchive& Ar, FItemInventoryEntry& Struct)
	{
		bool OutSuccess;
		Struct.ItemType.NetSerialize(Ar, nullptr, OutSuccess);
		Ar << Struct.ItemCount;
		return Ar;
	}

	bool IsValid() const
	{
		return ItemType.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FMessageSubstitutions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FString> Substitutions;
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Icon = nullptr;

	void Add(const FString& Key, const FString& Value)
	{
		Substitutions.Add(Key, Value);
	}
};

USTRUCT(BlueprintType)
struct FLootDefinition
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Item"))
	FGameplayTag ItemTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOverrideOwnerLevel = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bOverrideOwnerLevel", EditConditionHides))
	int32 Level = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance = 1.f;

	bool ShouldSpawn() const
	{
		return URandRangeBlueprintLibrary::ShouldAct(SpawnChance);
	}

	static void ToShouldSpawn(const TArray<FLootDefinition>& LootDefinitions, TArray<FLootDefinition>& OutToSpawn)
	{
		for (const FLootDefinition& LootDefinition : LootDefinitions)
		{
			if (LootDefinition.ShouldSpawn())
			{
				OutToSpawn.Add(LootDefinition);
			}
		}
	}
};

USTRUCT(BlueprintType)
struct FLootInstance
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TWeakObjectPtr<AActor> LootItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform SpawnTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTransform TargetTransform;
};
