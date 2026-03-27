// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolManagerComponent.generated.h"

class UPoolManagerComponent;

USTRUCT(BlueprintType)
struct FSpawnPoolEventPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Actor;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPoolManagerComponent> PoolManagerComponent;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnPoolEventSignature, const FSpawnPoolEventPayload&, Payload);

class USpawnPoolComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPoolManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPoolManagerComponent();

	UFUNCTION(BlueprintCallable)
	void SetSpawnPool(UObject* InSpawnPoolComponent);

	void HandleBeginRetrieveFromPool();
	void HandleFinishRetrieveFromPool();
	void HandleReturnedToPool();

	UFUNCTION(BlueprintCallable)
	void ReturnToPool() const;

	UPROPERTY(BlueprintAssignable)
	FSpawnPoolEventSignature OnBeginRetrieve;
	UPROPERTY(BlueprintAssignable)
	FSpawnPoolEventSignature OnFinishRetrieve;
	UPROPERTY(BlueprintAssignable)
	FSpawnPoolEventSignature OnReturnToPool;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bAutoReturn = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(EditCondition="bAutoReturn", EditConditionHides))
	float AutoReturnTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Retrieve")
	bool bMakeVisibleOnRetrieve = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Retrieve")
	bool bEnableActorCollisionOnRetrieve = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Retrieve")
	bool bEnableTickOnRetrieve = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Return")
	bool bHideOnReturn = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Return")
	bool bDisableActorCollisionOnReturn = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Return")
	bool bDisableTickOnReturn = true;

private:
	UPROPERTY()
	TWeakObjectPtr<UObject> SpawnPoolComponent;
	UPROPERTY()
	FTimerHandle ReturnTimer;
};
