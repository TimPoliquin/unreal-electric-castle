// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RecoveryEffectActor.generated.h"

class UNiagaraComponent;
class UApplyGameplayEffectComponent;
class UGameplayEffect;
class UNiagaraSystem;
class UCapsuleComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ARecoveryEffectActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARecoveryEffectActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> CollisionComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UApplyGameplayEffectComponent> RecoveryEffectComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> NiagaraEffectComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAudioComponent> AudioComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery")
	TObjectPtr<USoundBase> EnvironmentalSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery")
	TObjectPtr<USoundBase> RecoverySound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recovery")
	float TotalRecoveryTime = 10.f;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure)
	bool CheckPreRequisites(AActor* PickupActor) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasRecoveryTime() const;
	UFUNCTION(BlueprintNativeEvent)
	void ExpireRecovery();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayEnvironmentalSound();

private:
	UPROPERTY(VisibleInstanceOnly, Category = "Recovery")
	float ConsumedRecoveryTime = 0.f;
	UPROPERTY(VisibleInstanceOnly, Category = "Recovery")
	TArray<AActor*> OverlappingActors;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ClearRecoveredActors();
	void EndRecovery(AActor* OtherActor);
};
