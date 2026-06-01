// Copyright Alien Shores 2025

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrackFollowingComponent.generated.h"

class UCharacterMovementComponent;
class USplineComponent;
class UTimelineComponent;

UENUM(BlueprintType)
enum class ETrackLoopMode : uint8
{
	UniDirectional,
	BiDirectional,
	BiDirectionalAuto
};

UENUM(BlueprintType)
enum class ETrackDirection : uint8
{
	Forward,
	Reverse
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnTrackCompleted,
	AActor*,
	Cart,
	const USplineComponent*,
	Track,
	const float,
	Overflow
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UTrackFollowingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTrackFollowingComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USplineComponent* GetTrack() const;
	UFUNCTION(BlueprintCallable)
	void SetTrack(USplineComponent* InTrack);
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetTrackDistance() const;
	UFUNCTION(BlueprintCallable)
	void SetDirection(const ETrackDirection InDirection);
	UFUNCTION(BlueprintCallable)
	void ChangeSpeed(float InMovementSpeed);

	UPROPERTY(BlueprintAssignable)
	FOnTrackCompleted OnTrackCompleted;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Track", meta=(ExposeOnSpawn))
	TObjectPtr<USplineComponent> Track;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Track", meta=(ExposeOnSpawn))
	float StartingDistanceAlongTrack = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Track", meta=(ExposeOnSpawn))
	float MovementSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track", meta=(ExposeOnSpawn))
	float MaxDistanceFromSpline = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track", meta=(ExposeOnSpawn))
	ETrackLoopMode LoopMode = ETrackLoopMode::UniDirectional;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track", meta=(ExposeOnSpawn, EditCondition="LoopMode == ETrackLoopMode::BiDirectionalAuto", EditConditionHides))
	float LoopDelay = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Track", meta=(ExposeOnSpawn))
	bool bDebug = false;

private:
	void MoveAlongTrack(float DeltaTime);
	void HandleTrackEnd();
	bool IsAtStartOfTrack() const;
	bool IsAtEndOfTrack() const;

	UPROPERTY(VisibleInstanceOnly)
	ETrackDirection Direction = ETrackDirection::Forward;
	UPROPERTY(Replicated)
	bool bActive = false;
	UPROPERTY()
	FTimerHandle LoopDelayTimer;
	float DistanceAlongTrack = 0.f;
};
