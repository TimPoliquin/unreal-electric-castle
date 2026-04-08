// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ParryCameraEffect.generated.h"

class UParryBlurConfig;
/**
 * 
 */
UCLASS(DisplayName="Parry Camera Effect State")
class ELECTRICCASTLE_API UAnimNotifyState_ParryCameraEffect : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", Instanced)
	TObjectPtr<UParryBlurConfig> ParryBlurConfig;
};
