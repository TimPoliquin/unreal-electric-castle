// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackWindow.generated.h"

class UAttackWindow;
/**
 * 
 */
UCLASS(DisplayName="Attack Window State")
class ELECTRICCASTLE_API UAnimNotifyState_AttackWindow : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window", meta=(Categories="Event.Montage"))
	FGameplayTag EventTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category="Attack Window")
	TObjectPtr<UAttackWindow> AttackWindowConfig;
};
