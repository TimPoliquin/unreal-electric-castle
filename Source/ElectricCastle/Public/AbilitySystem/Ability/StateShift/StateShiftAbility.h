// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "Actor/State/StateShiftTypes.h"
#include "StateShiftAbility.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UStateShiftAbility : public UElectricCastleGameplayAbility
{
	GENERATED_BODY()

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateShiftAbility")
	void ApplyStateShiftDetectionComponent();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StateShiftAbility")
	void RemoveStateShiftDetectionComponent();
	UFUNCTION(BlueprintNativeEvent, Category = "StateShiftAbility")
	void OnStateShiftBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool BFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION(BlueprintNativeEvent, Category = "StateShiftAbility")
	void OnStateShiftEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	UFUNCTION(BlueprintCallable, Category = "StateShiftAbility")
	float GetEffectRadius() const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	FScalableFloat DetectionRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	EStateShiftState ShiftState = EStateShiftState::Psychedelic;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	EStateShiftState BaseState = EStateShiftState::Normal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bDebug"))
	TObjectPtr<UMaterialInterface> DebugMaterial;
	UPROPERTY(BlueprintReadOnly, Category = "StateShiftAbility")
	TObjectPtr<USphereComponent> DetectionComponent;

private:
	void CreateDetectionComponent();
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> DebugMeshComponent;
};
