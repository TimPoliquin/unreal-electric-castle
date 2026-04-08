// Copyright Alien Shores


#include "Game/Time/TimeManager.h"

#include "Game/Subsystem/PlayerManager.h"
#include "Kismet/GameplayStatics.h"

UTimeManager* UTimeManager::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UTimeManager>();
	}
	return nullptr;
}

FTimeDilationHandle UTimeManager::StartTimeDilation(const UObject* WorldContextObject, const float WorldTimeDilation, const float PlayerTimeDilation)
{
	FTimeDilationHandle TimeDilationHandle;
	TimeDilationHandle.Handle = ++HandleIndex;
	TimeDilationHandle.WorldDilation = WorldTimeDilation;
	TimeDilationHandle.PlayerDilation = PlayerTimeDilation;
	TimeDilationHandles.Push(TimeDilationHandle);
	UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, WorldTimeDilation);
	if (UPlayerManager* PlayerManager = UPlayerManager::Get(WorldContextObject))
	{
		PlayerManager->SetPlayerTimeDilation(PlayerTimeDilation);
	}
	return TimeDilationHandle;
}

void UTimeManager::ClearTimeDilation(const UObject* WorldContextObject, FTimeDilationHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}
	const bool IsCurrent = TimeDilationHandles.Top() == Handle;
	TimeDilationHandles.Remove(Handle);
	Handle.Invalidate();
	if (TimeDilationHandles.Num() == 0)
	{
		HandleIndex = 0;
	}
	if (!IsCurrent)
	{
		// the completed time dilation is not the active time dilation, so it can be 
		return;
	}
	float NewWorldTimeDilation = 1.f;
	float NewPlayerTimeDilation = 1.f;
	if (TimeDilationHandles.Num() > 0)
	{
		const FTimeDilationHandle& NewHandle = TimeDilationHandles.Top();
		NewWorldTimeDilation = NewHandle.WorldDilation;
		NewPlayerTimeDilation = NewHandle.PlayerDilation;
	}
	UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, NewWorldTimeDilation);
	if (UPlayerManager* PlayerManager = UPlayerManager::Get(WorldContextObject))
	{
		PlayerManager->SetPlayerTimeDilation(NewPlayerTimeDilation);
	}
}
