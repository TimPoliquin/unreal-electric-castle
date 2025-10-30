// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ProjectileActor.h"
#include "ArrowProjectile.generated.h"

class UNiagaraComponent;

UCLASS()
class ELECTRICCASTLE_API AArrowProjectile : public AProjectileActor
{
	GENERATED_BODY()

public:
	AArrowProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetMatchOwnerForward(const bool bInMatchOwnerPitch);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Release();

protected:
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bMatchOwnerForward = true;
	UPROPERTY(BlueprintReadWrite, Replicated)
	float Pitch = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> TrailFX;
};
