// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "HordeModeConfig.h"
#include "Actor/Spawn/SpawnEventPayloadDefinitions.h"
#include "Components/ActorComponent.h"
#include "HordeModeComponent.generated.h"

class AItemPickupBase;

USTRUCT(BlueprintType)
struct FOnHordeModeRoundCompletePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	int32 CompletedRound = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 NextRound = 0;
	UPROPERTY(BlueprintReadOnly)
	bool HasNextRound = false;
	UPROPERTY(BlueprintReadOnly)
	TArray<FHordeRoundRewards> Rewards;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHordeModeRoundCompleteSignature, const FOnHordeModeRoundCompletePayload&, Payload);

class UHordeModeConfig;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UHordeModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHordeModeComponent();

	UFUNCTION(BlueprintCallable)
	void StartRound();
	UFUNCTION(BlueprintCallable)
	void EndRound();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasNextRound() const;
	UPROPERTY(BlueprintAssignable)
	FOnHordeModeRoundCompleteSignature OnHordeModeRoundComplete;
	UFUNCTION(BlueprintCallable)
	TArray<FAuraSpawnParams> ConvertRewardsToSpawnParams(const FVector& Center, float Radius, const TArray<FHordeRoundRewards>& Rewards);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UHordeModeConfig> HordeModeConfig;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(MustImplement="AuraSpawnInterface"))
	TArray<AActor*> SpawnPoints;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentRound = 0;

private:
	UPROPERTY()
	TArray<AActor*> ActiveSpawnPoints;
	UFUNCTION()
	void OnSpawnPointCountChanged(const FOnActorTrackerCountChangedPayload& Payload);
	void PrepareNextRound();
};
