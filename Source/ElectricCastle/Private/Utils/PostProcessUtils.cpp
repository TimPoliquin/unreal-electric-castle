// Copyright Alien Shores


#include "Utils/PostProcessUtils.h"

#include "EngineUtils.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

#include "Engine/PostProcessVolume.h"

#include "Kismet/GameplayStatics.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

void UPostProcessUtils::SetMotionBlurAmount(const UObject* WorldContextObject, const float Amount)
{
	if (!IsValid(WorldContextObject))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[UPostProcessUtils::SetMotionBlurAmount]: Invalid world context object."));
		return;
	}
	TArray<AActor*> PostProcessVolumes;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, APostProcessVolume::StaticClass(), PostProcessVolumes);
	for (AActor* Actor : PostProcessVolumes)
	{
		if (APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(Actor))
		{
			PostProcessVolume->Settings.MotionBlurAmount = Amount;
		}
	}
}

UMaterialInstanceDynamic* UPostProcessUtils::MakePostProcessMaterialDynamic(const UObject* WorldContextObject, UMaterialInterface* MaterialType)
{
	APostProcessVolume* PostProcessVolume = nullptr;
	for (TActorIterator<APostProcessVolume> Iterator(WorldContextObject->GetWorld()); Iterator; ++Iterator)
	{
		if (Iterator->bUnbound)
		{
			PostProcessVolume = *Iterator;
			break;
		}
	}
	if (IsValid(PostProcessVolume) && IsValid(MaterialType))
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialType, PostProcessVolume);
		FWeightedBlendable NewBlendable;
		NewBlendable.Weight = 1;
		NewBlendable.Object = DynamicMaterial;
		PostProcessVolume->Settings.WeightedBlendables.Array.Add(NewBlendable);
		return DynamicMaterial;
	}
	return nullptr;
}
