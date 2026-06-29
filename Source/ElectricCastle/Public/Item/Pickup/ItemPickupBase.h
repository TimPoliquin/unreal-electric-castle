// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actor/Highlight/HighlightActorInterface.h"
#include "Item/ItemTypes.h"
#include "GameFramework/Actor.h"
#include "ItemPickupBase.generated.h"

class UTriggerComponent;
class UTriggerAction;
class UMeshComponent;
class UCapsuleComponent;
class UNiagaraSystem;
class UApplyGameplayEffectComponent;
class USinusoidalMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemPickupEventSignature, const FItemPickupEventPayload&, Payload);

UCLASS()
class ELECTRICCASTLE_API AItemPickupBase : public AActor, public IHighlightActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemPickupBase();

	/** Start IHighlightActorInterface **/
	virtual UHighlightComponent* GetHighlightComponent_Implementation() const override;
	virtual void GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes) override;
	/** End IHighlightActorInterface **/

	UPROPERTY(BlueprintAssignable)
	FItemPickupEventSignature OnItemPickup;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCapsuleComponent> CollisionComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USinusoidalMovementComponent> SinusoidalMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UHighlightComponent> HighlightComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UTriggerComponent> TriggerComponent;

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
