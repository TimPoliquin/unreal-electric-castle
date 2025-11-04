// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ElectricCastleLevelManager.generated.h"

class UElectricCastleLevelTransition;
DECLARE_MULTICAST_DELEGATE(FAuraLevelManagerEventSignature);

USTRUCT(BlueprintType)
struct FAuraMapConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString MapDisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DefaultPlayerStartTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DefaultPlayerLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Map;

	bool IsValid() const
	{
		return !MapDisplayName.IsEmpty() && !Map.IsNull();
	}
};

USTRUCT(BLueprintType)
struct ELECTRICCASTLE_API FAuraLevelTransitionParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString MapAssetName = FString("");
	UPROPERTY(BlueprintReadWrite)
	FName PlayerStartTag = NAME_None;
	UPROPERTY(BlueprintReadWrite, Category="AutoSave")
	FString SaveSlot = FString("");
	UPROPERTY(BlueprintReadWrite, Category="AutoSave")
	int32 SlotIndex = 0;
	UPROPERTY(BlueprintReadWrite, Category="AutoSave")
	bool bShouldSave = false;
	UPROPERTY(BlueprintReadWrite, Category="AutoSave")
	bool bShouldLoad = false;

	bool ShouldSave() const
	{
		return bShouldSave && !SaveSlot.IsEmpty();
	}

	bool ShouldLoad() const
	{
		return bShouldLoad && !SaveSlot.IsEmpty();
	}
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UElectricCastleLevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UElectricCastleLevelManager* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	TSoftObjectPtr<UWorld> GetMapFromMapDisplayName(const FString& MapDisplayName) const;
	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	void LoadMap(const UObject* WorldContextObject, const FString& MapAssetName) const;

	FString GetDefaultMapName() const;
	FString GetDefaultMapAssetName() const;
	FName GetDefaultPlayerStartTag(const FString& MapAssetName) const;
	int32 GetDefaultPlayerLevel(const FString& MapAssetName) const;
	FName GetCurrentPlayerStartTag(const UObject* WorldContextObject, const bool bFallbackToDefault) const;
	void TransitionLevel(const FAuraLevelTransitionParams& Params);

	bool IsTransitioningLevels() const;

	FAuraLevelManagerEventSignature OnLevelTransitionComplete;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	FString DefaultMapName;
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	TArray<FAuraMapConfig> MapConfigs = TArray<FAuraMapConfig>();

private:
	FAuraMapConfig GetMapConfigByDisplayName(const FString& MapDisplayName) const;
	FAuraMapConfig GetMapConfigByMapAssetName(const FString& MapAssetName) const;
	UPROPERTY()
	TObjectPtr<UElectricCastleLevelTransition> LevelTransition;
};
