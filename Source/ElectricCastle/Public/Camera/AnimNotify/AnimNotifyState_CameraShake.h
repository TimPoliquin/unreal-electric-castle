// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CameraShake.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UAnimNotifyState_CameraShake : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Properties")
	TSubclassOf<class UCameraShakeBase> CameraShake;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Properties")
	float Intensity = 1.f;
};
