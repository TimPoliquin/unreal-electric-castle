#pragma once

#include "CoreMinimal.h"
#include "Utils/RandUtils.h"
#include "EngagementTypes.generated.h"

UENUM(BlueprintType)
enum class EEngagementLevel : uint8
{
	None,
	Passive,
	Cautious,
	Aggressive,
	AllOut,
	Count
};

UENUM(BlueprintType)
enum class EEngagementRange : uint8
{
	None,
	HangBack,
	Ranged,
	Close,
	Count
};

UENUM(BlueprintType)
enum class EEngagementAbilityMode : uint8
{
	None,
	Support,
	Defend,
	Attack,
	Count
};

UENUM(BlueprintType)
enum class EEngagementControlMode : uint8
{
	None,
	Static,
	Randomized,
	Directed,
	Count
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FEngagementAIConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<AActor> Owner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementLevel EngagementLevel = EEngagementLevel::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementRange Range = EEngagementRange::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementAbilityMode AbilityMode = EEngagementAbilityMode::None;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FEngagementLevelConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementLevel EngagementLevel = EEngagementLevel::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange AttackRate = FFloatRange::Inclusive(10.f, 30.f);
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FEngagementRangeConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementRange Range = EEngagementRange::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFloatRange PreferredDistance = FFloatRange::Inclusive(100.f, 500.f);
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FEngagementAbilityModeConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEngagementAbilityMode AbilityMode = EEngagementAbilityMode::None;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FEngagementLevelChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EEngagementLevel Level = EEngagementLevel::None;
	UPROPERTY(BlueprintReadWrite)
	FFloatRange AttackRate = FFloatRange::Empty();
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FEngagementRangeChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	EEngagementRange Range = EEngagementRange::None;
	UPROPERTY(BlueprintReadWrite)
	FFloatRange PreferredDistance = FFloatRange::Empty();
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEngagementLevelChangedSignature, const FEngagementLevelChangedPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEngagementRangeChangedSignature, const FEngagementRangeChangedPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEngagementTargetChangedSignature, AActor*, NewTarget);
