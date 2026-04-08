// Copyright Alien Shores


#include "Camera/AnimNotify/AnimNotifyState_CameraShake.h"

void UAnimNotifyState_CameraShake::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!IsValid(CameraShake))
	{
		return;
	}
	if (const APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			PlayerController->ClientStartCameraShake(CameraShake, Intensity);
		}
	}
}

void UAnimNotifyState_CameraShake::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!IsValid(CameraShake))
	{
		return;
	}
	if (const APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(OwnerPawn->GetController()))
		{
			PlayerController->ClientStopCameraShake(CameraShake);
		}
	}
}
