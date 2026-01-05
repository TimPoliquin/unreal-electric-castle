// Copyright Alien Shores


#include "Utils/PostProcessUtils.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Kismet/GameplayStatics.h"

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
