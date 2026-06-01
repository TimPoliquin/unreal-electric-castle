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
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Release();

	/** Start IPoolableActor **/
	virtual void OnPool_Returned_Implementation(const FSpawnPoolEventPayload& Payload) override;
	virtual void OnPool_FinishRetrieve_Implementation(const FSpawnPoolEventPayload& Payload) override;
	/** End IPoolableActor **/

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> TrailFX;
};
