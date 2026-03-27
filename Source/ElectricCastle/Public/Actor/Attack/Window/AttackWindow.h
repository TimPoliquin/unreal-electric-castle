// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Curves/CurveVector.h"
#include "ElectricCastle/ElectricCastle.h"
#include "UObject/Object.h"

#include "AttackWindow.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API UAttackWindow : public UObject
{
	GENERATED_BODY()

public:
	void SetOwner(AActor* InOwner);
	void SetDuration(const float InDuration);
	void SweepForHits(const float DeltaTime, TArray<FHitResult>& OutHits);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window", meta=(Categories="Combat.Socket"))
	FGameplayTag OriginSocketTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	FVector Offset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	TArray<FRuntimeVectorCurve> SweepCurves;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	TEnumAsByte<ECollisionChannel> SweepChannel = ECC_Target;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	float SweepRadius = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	float SweepHalfHeight = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	bool bAllowRepeatHits = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack Window")
	bool bDebug = false;

private:
	FVector GetOffsetLocation(const FVector& InLocation) const;
	void SweepCurveForHits(const FRuntimeVectorCurve& InSweepCurve, const float Alpha, TArray<FHitResult>& OutHits) const;
	UPROPERTY()
	TWeakObjectPtr<AActor> Owner;
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitActors;

	float Duration = 0.f;
	float Elapsed = 0.f;
	float PreviousAlpha = 0.f;
};
