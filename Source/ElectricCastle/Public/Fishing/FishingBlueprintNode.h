// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FishTypes.h"
#include "GameplayEffect.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Utils/RandUtils.h"
#include "FishingBlueprintNode.generated.h"


class UFishCatch;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGoFishingCatchEventSignature, AActor*, FishingActor, const UFishCatch*, Catch);

USTRUCT(BlueprintType)
struct FGoFishingParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	AActor* CameraTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AActor* PlayerTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AActor* FishingTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	UCurveFloat* CameraMovementCurve = nullptr;
	UPROPERTY(BlueprintReadWrite)
	FRandRange TimeToLure;
	UPROPERTY(BlueprintReadWrite)
	FRandRange LureToBiteTime;
	UPROPERTY(BlueprintReadWrite)
	FRandRange BiteToFleeTime;
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> ReelEffectClass;
};

/**
 * 
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class ELECTRICCASTLE_API UFishingBlueprintNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
		Category = "Flow Control"
	)
	static UFishingBlueprintNode* GoFishing(
		UObject* WorldContextObject,
		AActor* Player,
		const FGoFishingParams& Params
	);
	UFUNCTION(BlueprintCallable)
	void WaitForFishToBeLured();
	UFUNCTION(BlueprintCallable)
	void LureAndWaitForABite(const FGameplayTag& FishType);
	UFUNCTION(BlueprintCallable)
	void BiteAndWaitForPlayerOrFlee();
	UFUNCTION(BlueprintCallable)
	void Flee();
	UFUNCTION(BlueprintCallable)
	void Reel();
	UFUNCTION(BlueprintCallable)
	void Catch();
	void Cleanup();
	UFUNCTION(BlueprintCallable)
	void PrepareForContinue();

	virtual void Activate() override;

protected:
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnCameraInPositionDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingCancelledDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnPlayerInPositionDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingRodEquippedDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingRodCastDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingLuredDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGoFishingCatchEventSignature OnFishingBiteDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingFishHasFledDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingFishReelingDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCatchEventSignature OnFishingFishCaughtDelegate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FGameplayTag ActiveFishType = FGameplayTag::EmptyTag;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EFishState ActiveFishState = EFishState::None;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFishCatch> CaughtFish;

	UFUNCTION(BlueprintCallable)
	void End();

private:
	UPROPERTY()
	AActor* PlayerActor;
	UPROPERTY()
	FActiveGameplayEffectHandle FishingEffectHandle;
	UPROPERTY()
	FGoFishingParams GoFishingParams;

	UPROPERTY()
	FTimerHandle PlayerMoveToTargetTimerHandle;
	FTimerDelegate PlayerMoveToTargetTimerDelegate;
	UPROPERTY()
	FTimerHandle FishInterestToLureTimerHandle;
	UPROPERTY()
	FTimerHandle FishLureToBiteTimerHandle;
	UPROPERTY()
	FTimerHandle FishBiteToFleeTimerHandle;

	FOnCameraMoveFinishedSignature OnCameraMoveFinishedDelegate;
	void MoveCameraToPosition();
	void MovePlayerToPosition();
	UFUNCTION()
	void OnCameraInPosition();
	UFUNCTION()
	void OnFishingRodCast();
	UFUNCTION()
	void OnFishingRodEquipped();
	UFUNCTION()
	void OnFishingStateChanged(EFishingState FishingState);
	void SetFishState(const EFishState& InFishState);
};
