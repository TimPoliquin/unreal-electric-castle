// Copyright Alien Shores


#include "Actor/Block/AnimNotify/AnimNotifyState_ParryCameraEffect.h"

#include "Actor/Block/BlockingActorInterface.h"
#include "Actor/Block/AnimNotify/ParryBlurConfig.h"
#include "Actor/Block/Components/BlockController.h"

void UAnimNotifyState_ParryCameraEffect::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (UBlockController* BlockController = IBlockingActorInterface::GetBlockController(MeshComp->GetOwner()))
	{
		ParryBlurConfig->Start(TotalDuration);
		BlockController->StartParryBlur(ParryBlurConfig);
	}
}

void UAnimNotifyState_ParryCameraEffect::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (UBlockController* BlockController = IBlockingActorInterface::GetBlockController(MeshComp->GetOwner()))
	{
		BlockController->EndParryBlur();
	}
}
