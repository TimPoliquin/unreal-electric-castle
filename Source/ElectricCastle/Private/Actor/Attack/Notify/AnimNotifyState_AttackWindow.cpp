// Copyright Alien Shores


#include "Actor/Attack/Notify/AnimNotifyState_AttackWindow.h"

#include "Actor/Attack/Component/AttackWindowManager.h"
#include "Actor/Attack/Component/AttackWindowManagerActor.h"
#include "Actor/Attack/Window/AttackWindow.h"

void UAnimNotifyState_AttackWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(AttackWindowConfig))
	{
		return;
	}
	const AActor* Owner = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (UAttackWindowManager* AttackWindowManager = IAttackWindowManagerActor::GetAttackWindowManager(Owner))
	{
		AttackWindowConfig->SetEventTag(EventTag);
		AttackWindowConfig->SetDuration(TotalDuration);
		AttackWindowConfig->SetOwner(MeshComp->GetOwner());
		AttackWindowManager->StartAttackWindow(AttackWindowConfig);
	}
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UAnimNotifyState_AttackWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!IsValid(AttackWindowConfig))
	{
		return;
	}
	if (UAttackWindowManager* AttackWindowManager = IAttackWindowManagerActor::GetAttackWindowManager(MeshComp->GetOwner()))
	{
		AttackWindowManager->EndAttackWindow(AttackWindowConfig);
		AttackWindowConfig->Reset();
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
