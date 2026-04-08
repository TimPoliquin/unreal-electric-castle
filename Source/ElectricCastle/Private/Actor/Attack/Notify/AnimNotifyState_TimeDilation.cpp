// Copyright Alien Shores


#include "Actor/Attack/Notify/AnimNotifyState_TimeDilation.h"

#include "Game/Time/TimeManager.h"

void UAnimNotifyState_TimeDilation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (UTimeManager* TimeManager = UTimeManager::Get(MeshComp->GetOwner()))
	{
		TimeDilationHandle = TimeManager->StartTimeDilation(MeshComp->GetOwner(), WorldTimeScale, PlayerTimeScale);
	}
}

void UAnimNotifyState_TimeDilation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (UTimeManager* TimeManager = UTimeManager::Get(MeshComp->GetOwner()))
	{
		TimeManager->ClearTimeDilation(MeshComp->GetOwner(), TimeDilationHandle);
	}
}
