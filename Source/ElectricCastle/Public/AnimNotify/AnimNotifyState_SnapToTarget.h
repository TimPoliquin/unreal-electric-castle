// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SnapToTarget.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAnimNotifyState_SnapToTarget : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationSpeed = 5.f;
};
