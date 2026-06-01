// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AI/Alert/AlertTypes.h"
#include "AI/Engagement/AIEngagementController.h"
#include "Actor/Significance/SignificanceSensitiveActor.h"
#include "Game/Save/SaveableInterface.h"
#include "GameFramework/Actor.h"
#include "EncounterManager.generated.h"

class UActorSignificanceComponent;

USTRUCT(BlueprintType)
struct FEncounterEngagementConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ActorLimit = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInt32Range PreferredCount = FInt32Range::Inclusive(1, 1);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<AActor>> PrioritizedActors;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowNoneEngagement = false;
};

UCLASS()
class ELECTRICCASTLE_API AEncounterManager : public AActor, public ISaveableInterface, public ISignificanceSensitiveActor
{
	GENERATED_BODY()

public:
	AEncounterManager();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RegisterEnemy(AActor* Enemy);

	/** Start ISaveableInterface **/
	virtual bool ShouldSave_Implementation() const override { return true; }
	virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	/** End ISaveableInterface **/
	/** Start ISignificanceSensitiveActor **/
	virtual void EnterSignificance_FullySignificant_Implementation() override;
	virtual void EnterSignificance_PartiallySignificant_Implementation() override;
	virtual void EnterSignificance_Insignificant_Implementation() override;
	/** End ISignificanceSensitiveActor **/

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void HandleGameDataLoaded();
	UFUNCTION(BlueprintNativeEvent)
	void HandleEnemyAlertLevelChanged(const FAlertLevelChangePayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleEnemyDeath(AActor* DeadActor);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RecalibrateEncounter();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MakeEnemyAggressive(AActor* Actor, UAIEngagementController* EngagementController);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MakeEnemyPassive(AActor* Actor, UAIEngagementController* EngagementController);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UActorSignificanceComponent> SignificanceComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEncounterEngagementConfig EngagementConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FFloatRange RecalibrationTimeRange = FFloatRange::Inclusive(7.f, 15.f);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EAlertLevel EncounterAlertLevel = EAlertLevel::Idle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Enemies;
	FTimerHandle RecalibrationTimerHandle;
};
