// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/POI/PointOfInterestActor.h"
#include "Fishing/FishTypes.h"
#include "Utils/RandUtils.h"
#include "FishingPointOfInterest.generated.h"

class UAuraInteractionWidget;
class UFishCatch;
class UAuraFishingMessageWidget;
struct FGoFishingParams;
class ATargetPoint;
class UFishingBlueprintNode;
class UNiagaraSystem;
class UNiagaraComponent;
enum class EFishTag : uint8;
enum class EFishType : uint8;

DECLARE_MULTICAST_DELEGATE(FFishingCatchWidgetSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishBiteSignature, const FGameplayTag&, FishType);

USTRUCT(BlueprintType)
struct FFishConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Item.Type.Fish"))
	FGameplayTag FishType = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RarityMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeightedFish
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, meta=(Categories="Item.Type.Fish"))
	FGameplayTag FishType = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadOnly)
	float Weight = 1.f;
};

USTRUCT(BlueprintType)
struct FSplashEffect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UNiagaraSystem> SplashSystem;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundBase> SplashSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool LoopSound = false;
};


UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AFishingPointOfInterest : public APointOfInterestActor
{
	GENERATED_BODY()

public:
	AFishingPointOfInterest();

	UFUNCTION(BlueprintCallable)
	TArray<FWeightedFish> GetCurrentlyAvailableFish(const AActor* Player) const;
	UFUNCTION(BlueprintCallable)
	FGameplayTag PickFishType(const TArray<FWeightedFish>& AvailableFish) const;

protected:
	virtual void HandleInteract_Implementation(AActor* Player) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> SplashFXComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAudioComponent> SplashSFXComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	TArray<FFishConfig> FishConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing", meta=(Categories="Fish.Tag"))
	FGameplayTagContainer PoolTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fishing|Setup")
	TObjectPtr<UChildActorComponent> FishingTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fishing|Setup")
	TObjectPtr<UChildActorComponent> PlayerTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fishing|Setup")
	TObjectPtr<UChildActorComponent> CameraTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fishing|Setup")
	TObjectPtr<UCurveFloat> CameraMovementCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fishing|Setup")
	TObjectPtr<UChildActorComponent> Ripple;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fishing|Setup")
	TSubclassOf<UGameplayEffect> ReelAvailableEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	FRandRange InterestToLureTime = FRandRange(5.f, 10.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	FRandRange LureToBiteTime = FRandRange(5.f, 10.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	FRandRange BiteToFleeTime = FRandRange(1.f, 7.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Catch")
	TSubclassOf<UAuraInteractionWidget> CatchWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Catch")
	FString CatchWidgetText = TEXT("Catch");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Catch")
	TSubclassOf<UAuraFishingMessageWidget> SuccessWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Catch")
	TSubclassOf<UAuraFishingMessageWidget> FailureWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Splash")
	FSplashEffect BobSplashEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Splash")
	FSplashEffect BiteSplashEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Debug")
	bool bDebug = false;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartFishing(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CancelFishing(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerEquipFishingRod(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishingCast(AActor* Player);
	UFUNCTION(BlueprintCallable)
	void TriggerBobSplashEffect();
	UFUNCTION(BlueprintCallable)
	void TriggerBiteSplashEffect();
	UFUNCTION(BlueprintNativeEvent)
	void TriggerSplashEffect(const FSplashEffect SplashEffect);
	UFUNCTION(BlueprintNativeEvent)
	void StopSplashEffect();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishInterested(AActor* Player, UFishingBlueprintNode* FishingTask);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishBite(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishReel(AActor* Player, UFishingBlueprintNode* FishingTask);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishCatch(AActor* Player, UFishingBlueprintNode* FishingTask);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishCaught(AActor* Player, const UFishCatch* FishCatch);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TriggerFishFled(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void ShowCatchWidget(AActor* Player);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAuraInteractionWidget* CreateCatchWidget(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void ShowSuccessWidget(AActor* Player, const UFishCatch* FishCatch);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAuraFishingMessageWidget* CreateSuccessMessageWidget(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void ShowFailureWidget(AActor* Player);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAuraFishingMessageWidget* CreateFailureMessageWidget(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector2D GetScreenPositionOfFishingTarget(AActor* Player) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGoFishingParams MakeGoFishingParams() const;
	UFUNCTION(BlueprintCallable)
	void ShowWidgets();
	UFUNCTION(BlueprintCallable)
	void HideWidgets();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FinishFishingRound(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EndFishing(AActor* Player);

private:
	FFishingCatchWidgetSignature OnFishCatchWidgetHide;
	bool bIsBusy = false;
};
