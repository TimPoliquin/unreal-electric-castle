// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackWindowManager.generated.h"


class UAttackWindowManager;
class UAttackWindow;

USTRUCT(BlueprintType)
struct FAttackWindowHitPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAttackWindowManager> AttackWindowManager;
	UPROPERTY(BlueprintReadOnly)
	TArray<FHitResult> HitResults;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackWindowHitSignature, const FAttackWindowHitPayload&, Payload);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAttackWindowManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackWindowManager();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartAttackWindow(UAttackWindow* AttackWindow);
	void EndAttackWindow(UAttackWindow* AttackWindow);

	UPROPERTY(BlueprintAssignable)
	FAttackWindowHitSignature OnAttackHit;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;

private:
	UPROPERTY()
	TArray<UAttackWindow*> ActiveAttackWindows;
};
