// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AlertTypes.h"
#include "ScalableFloat.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIAlertComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAIAlertComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAIAlertComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginDestroy() override;

	/** Fires when the alert level enum changes **/
	UPROPERTY(BlueprintAssignable)
	FAlertLevelChanged OnAlertLevelChanged;
	/** Fires when the alert level raw value changes. useful for updating UI elements; should not be used for heavy processing **/
	UPROPERTY(BlueprintAssignable)
	FAlertLevelChanged OnAlertLevelRawChanged;
	UPROPERTY(BlueprintAssignable)
	FAlertTargetPerceiveChanged OnAlertTargetPerceiveChanged;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	UFUNCTION(BlueprintNativeEvent)
	void HandleGameDataLoaded();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FScalableFloat PerceptionCurve = 33.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FScalableFloat AlertDecayRate = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FScalableFloat AlertDecayDelay = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SuspiciousThreshold = 75.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AlertedThreshold = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	bool bProcessAlert = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	float AlertLevelRaw = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EAlertLevel AlertLevel = EAlertLevel::Idle;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TWeakObjectPtr<AActor> TargetActor;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FVector LastKnownLocation = FVector::ZeroVector;

private:
	EAlertLevel GetAlertLevelByValue(const float InAlertLevel) const;
	void SetAlertLevel(const float InAlertLevel);

	FTimerHandle DecayTimer;
};
