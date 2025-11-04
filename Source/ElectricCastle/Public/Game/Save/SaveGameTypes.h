#pragma once
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/ElectricCastlePlayerState.h"
#include "SaveGameTypes.generated.h"

class UGameplayAbility;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FComponentSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ComponentName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ComponentClass;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> ComponentData;

	FComponentSaveData()
	{
		ComponentName = "";
		ComponentClass = "";
	}

	bool IsValid() const
	{
		return !ComponentName.IsEmpty() && !ComponentClass.IsEmpty() && ComponentData.Num() > 0;
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FActorSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ActorName = "";

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString ActorClass = "";

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FTransform ActorTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	bool bShouldAutoSpawn = false;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<uint8> ActorData;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FComponentSaveData> ComponentsData;

	bool IsValid() const
	{
		return !ActorName.IsEmpty() && !ActorClass.IsEmpty();
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FWorldSaveData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString WorldName;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TMap<FString, FString> WorldProperties;

	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FActorSaveData> ActorsData;

	FWorldSaveData()
	{
		WorldName = "";
	}

	bool IsValid() const
	{
		return !WorldName.IsEmpty();
	}

	void ClearAllData()
	{
		WorldProperties.Empty();
		ActorsData.Empty();
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FMetaSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString PlayerName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FDateTime SaveTimestamp;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString MapAssetName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FString MapDisplayName;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FName PlayerStartTag = NAME_None;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	int32 PlayerLevel = 0;

	FMetaSaveData()
	{
		PlayerName = "";
		SaveTimestamp = FDateTime::Now();
		MapAssetName = "";
		MapDisplayName = "";
		PlayerStartTag = "";
		PlayerLevel = 0;
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FGlobalSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	FActorSaveData GameStateSaveData;
	UPROPERTY(BlueprintReadWrite, Category = "Save Data")
	TArray<FActorSaveData> PlayerSaveData;
	FActorSaveData GetPlayerSaveData(const AElectricCastlePlayerState* PlayerState);
	void AddPlayerSaveData(const FActorSaveData& ActorSaveData);
};

UENUM(BlueprintType)
enum class ESavedAbilityState : uint8
{
	GiveAbility,
	GiveAbilityAndActivate
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTypeTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatusTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlotTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ESavedAbilityState AbilityState = ESavedAbilityState::GiveAbility;

	FString ToString() const
	{
		return FString::Printf(
			TEXT(
				"Ability Tag: [%s] \n"
				"Ability Type: [%s] \n"
				"Ability Status: [%s] \n"
				"Ability Slot: [%s] \n"
			),
			*AbilityTag.ToString(),
			*AbilityTypeTag.ToString(),
			*AbilityStatusTag.ToString(),
			*AbilitySlotTag.ToString()
		);
	}

	// Serialization function
	friend FArchive& operator<<(FArchive& Ar, FSavedAbility& Struct)
	{
		bool SerializeSuccess;
		FString ClassPathName = Struct.GameplayAbilityClass->GetPathName();
		Ar << ClassPathName;
		Struct.AbilityTag.NetSerialize(Ar, nullptr, SerializeSuccess);
		Struct.AbilityTypeTag.NetSerialize(Ar, nullptr, SerializeSuccess);
		Struct.AbilityStatusTag.NetSerialize(Ar, nullptr, SerializeSuccess);
		Struct.AbilitySlotTag.NetSerialize(Ar, nullptr, SerializeSuccess);
		Ar << Struct.AbilityLevel;
		Ar << Struct.AbilityState;
		if (!Struct.GameplayAbilityClass && !ClassPathName.IsEmpty())
		{
			if (UClass* AbilityClass = StaticLoadClass(UGameplayAbility::StaticClass(), nullptr, *ClassPathName))
			{
				Struct.GameplayAbilityClass = AbilityClass;
			}
			else
			{
				UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to load ability class by path: %s"), *Struct.AbilityTag.ToString(), *ClassPathName);
			}
		}
		return Ar;
	}

	bool operator==(const FSavedAbility& Right) const
	{
		return AbilityTag.MatchesTagExact(Right.AbilityTag);
	}
};
