// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AITargetingTypes.h"
#include "GameplayEffectTypes.h"
#include "AITargetingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAITargetChangedSignature, const FAITargetChangedPayload&, Payload);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UAITargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAITargetingComponent();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AActor* GetCurrentTarget() const;
	UFUNCTION(BlueprintCallable)
	void SetCurrentTarget(AActor* InTarget);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasCurrentTarget() const;
	UFUNCTION(BlueprintCallable)
	void SetTargetProvider(AActor* InTargetProvider);

	UPROPERTY(BlueprintAssignable)
	FAITargetChangedSignature OnTargetChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Targeting")
	bool bAutoRequestNewTarget = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|Targeting", meta=(MustImplement="/Script/ElectricCastle.AITargetProviderInterface"))
	TObjectPtr<AActor> TargetProvider;

private:
	void SetupTargetEventListeners(AActor* InTarget);
	void RemoveTargetEventListeners(AActor* InTarget);
	void HandleTargetVisibilityChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	UFUNCTION()
	void HandleTargetDeath(AActor* DeadActor);
	AActor* ToEffectiveTarget(AActor* InTarget) const;

	UPROPERTY()
	TWeakObjectPtr<AActor> Target;
};
