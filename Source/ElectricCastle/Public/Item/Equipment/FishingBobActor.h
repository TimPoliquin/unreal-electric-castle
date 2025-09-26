// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishingBobActor.generated.h"

class UProjectileMovementComponent;
class USphereComponent;


UENUM(BlueprintType)
enum class EFishingBobState : uint8
{
	None,
	Casting,
	Bobbing,
	Lured,
	Biting,
	Reeling
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishingBobStateChangedSignature, EFishingBobState, FishingBobState);


UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AFishingBobActor : public AActor
{
	GENERATED_BODY()

public:
	AFishingBobActor();
	virtual void Tick(float DeltaTime) override;
	FName GetMeshComponentName() const { return FName("Mesh"); }
	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& InDestination);
	void Lured();
	void Biting();
	void Cancel();
	void Return(AActor* InOwner, const FName& SocketName);

	UPROPERTY(BlueprintAssignable)
	FOnFishingBobStateChangedSignature OnFishingStateChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Collider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FishingBob")
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FishingBob")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FishingBob")
	float DestinationThreshold = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FishingBob")
	EFishingBobState FishingState = EFishingBobState::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FishingBob")
	float BobbingPeriodMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FishingBob")
	float BobbingAmplitude = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FishingBob|Debug")
	bool bDrawDebug = false;

private:
	FVector Destination;
	float BobbingTime = 0.f;

	void HandleCastingTick(float DeltaTime);
	void HandleBobbingTick(float DeltaTime);
	void HandleLuredTick(float DeltaTime);
	void HandleBitingTick(float DeltaTime);
	void HandleReelingTick(float DeltaTime);

	void SetFishingState(const EFishingBobState InFishingState)
	{
		FishingState = InFishingState;
		OnFishingStateChanged.Broadcast(InFishingState);
	}
};
