// Copyright Alien Shores


#include "AnimNotify/AnimNotifyState_SnapToTarget.h"

#include "Kismet/KismetMathLibrary.h"
#include "Player/LockOn/LockOnActor.h"
#include "Player/LockOn/LockOnController.h"

void UAnimNotifyState_SnapToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	AActor* Actor = MeshComp->GetOwner();
	if (const ULockOnController* LockOnController = ILockOnActor::GetLockOnController(Actor))
	{
		if (const AActor* Target = LockOnController->GetLockOnTarget())
		{
			const FRotator ActorRotation = Actor->GetActorRotation();
			FRotator FacingRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), Target->GetActorLocation());
			FacingRotation.Pitch = ActorRotation.Pitch;
			const FRotator NewRotation = FMath::RInterpTo(ActorRotation, FacingRotation, FrameDeltaTime, RotationSpeed);
			Actor->SetActorRotation(NewRotation);
		}
	}
}
