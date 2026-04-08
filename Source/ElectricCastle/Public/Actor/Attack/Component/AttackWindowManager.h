// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackWindowManager.generated.h"


class UAbilitySystemComponent;
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
	virtual void BeginPlay() override;
	virtual void SetAbilitySystemComponent(UAbilitySystemComponent* InAbilitySystemComponent);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartAttackWindow(UAttackWindow* AttackWindow);
	void EndAttackWindow(UAttackWindow* AttackWindow);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;

private:
	void HandleAttackWindowHit(const UAttackWindow* AttackWindow, const FHitResult& HitResult) const;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TArray<UAttackWindow*> ActiveAttackWindows;
};
