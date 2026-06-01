// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AI/Engagement/EngagementTypes.h"
#include "UObject/Object.h"
#include "EngagementLevelConfig.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API UEngagementLevelConfig : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementLevel EngagementLevel = EEngagementLevel::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange AttackRate = FFloatRange::Inclusive(10.f, 30.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	TObjectPtr<UEngagementMovementPlugin> MovementPlugin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPreferred = false;
};
