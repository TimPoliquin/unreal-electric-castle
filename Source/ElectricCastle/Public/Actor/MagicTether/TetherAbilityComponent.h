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
	UFUNCTION(BlueprintCallable, Category="Tether")
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
	TWeakObjectPtr<AActor> TargetActor;

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
	float MinTetherLength = 200.f; // or whatever feels right
	UPROPERTY(EditAnywhere, Category="Tether")
	float MinTetherBufferLength = 10.f;

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
