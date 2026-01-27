// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "MagicHandEvents.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "MagicHand.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EMagicHandState : uint8
{
	None,
	Casting,
	Holding,
	Max
};

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AMagicHand : public AActor
{
	GENERATED_BODY()

public:
	AMagicHand();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Launch(AActor* InPossessor, const float InSpeed, const float InRange);

	UPROPERTY(BlueprintAssignable)
	FMagicHandPossessSignature OnPossess;
	UPROPERTY(BlueprintAssignable)
	FMagicHandMissSignature OnMiss;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void PossessTarget(AActor* InTarget);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void ReleaseTarget();
	UFUNCTION(NetMulticast, Reliable)
	void MissTarget();
	UFUNCTION(BlueprintNativeEvent)
	void UpdateTetherFXBeamEnd();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetTetherFXBeamEnd(const FVector EndLocation);

	// Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> HandMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> GrabCollisionComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> TetherFXComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	// Properties
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category="Properties")
	float Range = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category="Properties")
	float Speed = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category="Properties")
	TWeakObjectPtr<AActor> Possessor;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category="Properties")
	TWeakObjectPtr<AActor> Target;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category="Properties")
	EMagicHandState State = EMagicHandState::None;

private:
	void Tick_Casting();
	void Tick_Holding();
	UFUNCTION()
	void OnGrabCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                 const FHitResult& SweepResult);
	FVector PreviousLocation = FVector::ZeroVector;
};
