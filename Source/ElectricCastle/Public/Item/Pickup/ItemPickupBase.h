// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interaction/HighlightInterface.h"
#include "ItemPickupBase.generated.h"

class UCapsuleComponent;
class UNiagaraSystem;
class UApplyGameplayEffectComponent;
class USinusoidalMovementComponent;

UCLASS()
class ELECTRICCASTLE_API AItemPickupBase : public AActor, public IHighlightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemPickupBase();

	/** Start IHighlightInterface **/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual bool SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/** End IHighlightInterface **/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCapsuleComponent> CollisionComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USinusoidalMovementComponent> SinusoidalMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Spawn")
	TObjectPtr<USoundBase> SpawnSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Spawn")
	TObjectPtr<UNiagaraSystem> SpawnEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Pickup")
	TObjectPtr<USoundBase> PickupSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Pickup")
	TObjectPtr<UNiagaraSystem> PickupEffect;

	UFUNCTION(BlueprintNativeEvent)
	void PlaySpawnEffect();
	UFUNCTION(BlueprintNativeEvent)
	void PlayPickupEffect(AActor* PickupActor, const bool bAutoDestroy);
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
	{
	}

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
	{
	}
};
