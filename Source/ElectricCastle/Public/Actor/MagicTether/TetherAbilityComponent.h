// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "TetherAbilityEvents.h"
#include "Components/ActorComponent.h"
#include "TetherAbilityComponent.generated.h"

class UAbilitySystemComponent;

UCLASS(ClassGroup=(Abilities), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UTetherAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTetherAbilityComponent();

	/** Public API for abilities to attach a target */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category="Tether")
	void AttachTarget(AActor* NewTarget);

	/** Public API for abilities to detach */
	UFUNCTION(BlueprintCallable, Category="Tether")
	void DetachTarget();

	/** Current tether length */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Tether")
	float GetCurrentTetherLength() const { return CurrentTetherLength; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Tether")
	bool IsTethered() const { return TargetActor != nullptr; }

	UFUNCTION(BlueprintCallable, Category="Tether")
	bool ModifyInputMovementByTetherLimits(const FVector& InDirection, const float InScale, FVector& OutDirection, float& OutScale) const;

	UPROPERTY(BlueprintAssignable)
	FTetherLimitExceededSignature OnTetherLimitExceeded;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// -------------------------
	// Replicated State
	// -------------------------

	/** The actor being tethered */
	UPROPERTY(ReplicatedUsing=OnRep_TargetActor)
	TObjectPtr<AActor> TargetActor;

	/** Current tether length (expands but never contracts) */
	UPROPERTY(Replicated)
	float CurrentTetherLength = 0.f;

	// -------------------------
	// Non-replicated cached references
	// -------------------------

	UPROPERTY()
	TObjectPtr<AActor> OwnerActor = nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystem = nullptr;

	/** Previous frame owner location (for delta movement) */
	FVector PreviousOwnerLocation = FVector::ZeroVector;
	UPROPERTY()
	bool bTargetWasBlockedLastFrame = false;

	// -------------------------
	// Config
	// -------------------------
	UPROPERTY(EditAnywhere, Category="Tether")
	float MaxTetherLength = 800.f;
	UPROPERTY(EditAnywhere, Category="Tether")
	float MinTetherLength = 200.f;
	UPROPERTY(EditAnywhere, Category="Tether")
	float Lift = 20.f;
	UPROPERTY(EditAnywhere, Category="Tether")
	bool bPushOut = true;
	UPROPERTY(EditAnywhere, Category="Tether", meta=(EditCondition="bPushOut", EditConditionHides))
	float PushStrength = 100.f;
	UPROPERTY(EditAnywhere, Category="Tether")
	bool bUseSoftZone = true;
	UPROPERTY(EditAnywhere, Category="Tether", meta=(EditCondition="bUseSoftZone", EditConditionHides))
	float SoftZoneWidth = 200.f;
	UPROPERTY(EditAnywhere, Category="Tether")
	int32 RecalcMaxIterations = 3;

	// -------------------------
	// Internal Logic
	// -------------------------

	void UpdateTether(float DeltaTime);

	// -------------------------
	// RPCs
	// -------------------------

	UFUNCTION(Server, Reliable)
	void ServerAttachTarget(AActor* NewTarget);

	UFUNCTION(Server, Reliable)
	void ServerDetachTarget();

	// -------------------------
	// RepNotifies
	// -------------------------

	UFUNCTION()
	void OnRep_TargetActor();
};
