// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Game/Time/TimeDilationHandle.h"
#include "AnimNotifyState_TimeDilation.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Time Dilation State")
class ELECTRICCASTLE_API UAnimNotifyState_TimeDilation : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Time Dilation")
	float WorldTimeScale = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Time Dilation")
	float PlayerTimeScale = 1.0f;

private:
	UPROPERTY()
	FTimeDilationHandle TimeDilationHandle;
};
