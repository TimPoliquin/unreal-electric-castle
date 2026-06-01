// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SignificanceCalculation.h"
#include "SignificanceTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "WorldSignificanceSubsystem.generated.h"

class UActorSignificance;
class USignificanceManager;

USTRUCT(BlueprintType)
struct FSignificanceLimit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=-1, ClampMax=1, UIMin=-1, UIMax=1))
	float Threshold = -1.f;
	// Limit the number of active objects in this significance tier. 
	// -1 indicates that there are no limits.
	// 0 indicates that no instances should be active 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=-1, UIMin=-1))
	int32 Limit = -1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESignificanceLevel SignificanceLevel = ESignificanceLevel::Insignificant;

	bool operator ==(const FSignificanceLimit& Other) const;
	bool operator<(const FSignificanceLimit& Other) const;
	bool operator>(const FSignificanceLimit& Other) const;
	bool operator<=(const FSignificanceLimit& Other) const;
	bool operator>=(const FSignificanceLimit& Other) const;
};

USTRUCT(BlueprintType)
struct FSignificanceConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Significance"))
	FGameplayTag SignificanceTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSignificanceLimit> Limits;

	void SortAscendingByThreshold();
	const FSignificanceLimit* FindSignificanceLimit(const float SignificanceScore) const;
};

USTRUCT(BlueprintType)
struct FActorSignificanceBucket
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<TObjectPtr<UActorSignificance>> ActorSignificances;
	UPROPERTY()
	ESignificanceLevel MaxSignificanceLevel = ESignificanceLevel::None;

	void AddActorSignificance(UActorSignificance* Significance);
	uint32 RemoveActor(UObject* Actor);
	void Reset();
	void CleanupStaleActors();
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class ELECTRICCASTLE_API UWorldSignificanceSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UWorldSignificanceSubsystem* Get(const UObject* WorldContextObject);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void ProcessSignificance(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void SetPlayerController(APlayerController* InPlayerController);
	UFUNCTION(BlueprintCallable)
	void RegisterObject(UObject* Object, const FGameplayTag& SignificanceTag, USignificanceCalculation* SignificanceCalculation);
	UFUNCTION(BlueprintCallable)
	void UnregisterObject(UObject* Object);
	UFUNCTION(BlueprintCallable)
	void UnregisterObjects(const FGameplayTag& SignificanceTag);
	UFUNCTION(BlueprintCallable)
	bool HasAnySignificantActors(const FGameplayTag& SignificanceTag, const ESignificanceLevel MinimumSignificance = ESignificanceLevel::PartiallySignificant);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSignificanceConfig> SignificanceConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0, ClampMax=1, UIMin=0, UIMax=1))
	float SignificanceUpdateRate = .033f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;

private:
	USignificanceManager* GetSignificanceManager();
	UActorSignificance* CreateActorSignificance(UObject* Object, const FGameplayTag& SignificanceTag);
	void CleanupStaleActors();
	void ProcessSignificanceManager();
	void ProcessSignificanceBuckets();
	void AssignSignificanceLevel(UActorSignificance* ActorSignificance, ESignificanceLevel Level);
	ESignificanceLevel GetSignificanceLevelByTagAndSignificance(const FGameplayTag& SignificanceTag, const float Significance) const;
	UFUNCTION()
	void HandleActorDestroyed(AActor* DestroyedActor);
	UPROPERTY()
	TWeakObjectPtr<USignificanceManager> CachedSignificanceManager;
	UPROPERTY()
	TWeakObjectPtr<APlayerController> PlayerController;
	UPROPERTY()
	TMap<FGameplayTag, FActorSignificanceBucket> ActorSignificancesByTag;
	UPROPERTY()
	float TimeSinceLastUpdate = 1.f;
};
