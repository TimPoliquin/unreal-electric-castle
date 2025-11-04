// Copyright Alien Shores


#include "Game/Subsystem/ElectricCastleLevelManager.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Save/SaveGameManager.h"
#include "Game/Subsystem/ElectricCastleLevelTransition.h"
#include "Kismet/GameplayStatics.h"

void UElectricCastleLevelManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Initialize"), *GetName())
}

void UElectricCastleLevelManager::Deinitialize()
{
	Super::Deinitialize();
}

TSoftObjectPtr<UWorld> UElectricCastleLevelManager::GetMapFromMapDisplayName(const FString& MapDisplayName) const
{
	return GetMapConfigByDisplayName(MapDisplayName).Map;
}

FString UElectricCastleLevelManager::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).MapDisplayName;
}

UElectricCastleLevelManager* UElectricCastleLevelManager::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject)
		                                        ? UGameplayStatics::GetGameInstance(WorldContextObject)
		                                        : nullptr)
	{
		return GameInstance->GetSubsystem<UElectricCastleLevelManager>();
	}
	return nullptr;
}

void UElectricCastleLevelManager::LoadMap(const UObject* WorldContextObject, const FString& MapAssetName) const
{
	const FAuraMapConfig& SelectedMapConfig = GetMapConfigByMapAssetName(MapAssetName);
	if (SelectedMapConfig.IsValid() && !SelectedMapConfig.Map.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(WorldContextObject, SelectedMapConfig.Map);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Failed to load map by display name: %s"), *GetName(), *MapAssetName);
		for (const FAuraMapConfig& MapConfig : MapConfigs)
		{
			UE_LOG(
				LogElectricCastle,
				Error,
				TEXT("[%s] Map: %s - %s"),
				*GetName(),
				*MapConfig.MapDisplayName,
				*MapConfig.Map.ToSoftObjectPath().GetAssetName()
			);
		}
	}
}


FString UElectricCastleLevelManager::GetDefaultMapName() const
{
	return DefaultMapName;
}

FString UElectricCastleLevelManager::GetDefaultMapAssetName() const
{
	return GetMapFromMapDisplayName(DefaultMapName).ToSoftObjectPath().GetAssetName();
}

FName UElectricCastleLevelManager::GetDefaultPlayerStartTag(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).DefaultPlayerStartTag;
}

int32 UElectricCastleLevelManager::GetDefaultPlayerLevel(const FString& MapAssetName) const
{
	return GetMapConfigByMapAssetName(MapAssetName).DefaultPlayerLevel;
}

FName UElectricCastleLevelManager::GetCurrentPlayerStartTag(
	const UObject* WorldContextObject,
	const bool bFallbackToDefault
) const
{
	if (LevelTransition)
	{
		return LevelTransition->GetDestinationPlayerStartTag();
	}
	if (bFallbackToDefault)
	{
		const UWorld* World = WorldContextObject->GetWorld();
		FString MapName = World->GetMapName();
		MapName.RemoveFromStart(World->StreamingLevelsPrefix);
		const FAuraMapConfig& MapConfig = GetMapConfigByMapAssetName(MapName);
		return MapConfig.DefaultPlayerStartTag;
	}
	return NAME_None;
}

void UElectricCastleLevelManager::TransitionLevel(const FAuraLevelTransitionParams& Params)
{
	if (Params.ShouldSave())
	{
		USaveGameManager* SaveGameManager = USaveGameManager::Get(GetWorld());
		FAuraSaveGameParams SaveGameParams;
		SaveGameParams.DestinationMapName = Params.MapAssetName;
		SaveGameParams.DestinationPlayerStartTag = Params.PlayerStartTag;
		if (SaveGameManager)
		{
			SaveGameManager->AutoSave_LevelTransition(SaveGameParams);
		}
	}
	LevelTransition = NewObject<UElectricCastleLevelTransition>(this, UElectricCastleLevelTransition::StaticClass());
	LevelTransition->Initialize(Params);
	LevelTransition->OnComplete.AddWeakLambda(this, [&](const UWorld* World)
	{
		LevelTransition->ConditionalBeginDestroy();
		LevelTransition = nullptr;
		OnLevelTransitionComplete.Broadcast();
	});
	const FAuraMapConfig& MapConfig = GetMapConfigByMapAssetName(Params.MapAssetName);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MapConfig.Map);
}

bool UElectricCastleLevelManager::IsTransitioningLevels() const
{
	return LevelTransition != nullptr;
}


FAuraMapConfig UElectricCastleLevelManager::GetMapConfigByDisplayName(const FString& MapDisplayName) const
{
	for (const FAuraMapConfig& MapConfig : MapConfigs)
	{
		if (MapConfig.MapDisplayName.Equals(MapDisplayName))
		{
			return MapConfig;
		}
	}
	FAuraMapConfig InvalidConfig = FAuraMapConfig();
	return InvalidConfig;
}

FAuraMapConfig UElectricCastleLevelManager::GetMapConfigByMapAssetName(const FString& MapAssetName) const
{
	for (const FAuraMapConfig& MapConfig : MapConfigs)
	{
		if (MapConfig.Map.GetAssetName().Equals(MapAssetName))
		{
			return MapConfig;
		}
	}
	FAuraMapConfig InvalidConfig = FAuraMapConfig();
	return InvalidConfig;
}
