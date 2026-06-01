// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EngagementMovementPlugin.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ELECTRICCASTLE_API UEngagementMovementPlugin : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InitializeDependencies(AActor* InOwnerActor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetTargetActor(AActor* InTargetActor);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetPreferredDistance(const FFloatRange InPreferredDistanceRange);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void TickMovement(const float DeltaTime);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ChangeItUp();

protected:
	UPawnMovementComponent* GetMovementComponent();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetDirectionToTarget(FVector& OutDirection) const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	UPROPERTY()
	TObjectPtr<UPawnMovementComponent> MovementComponent;
	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<AActor> TargetActor;
	UPROPERTY(VisibleInstanceOnly)
	FFloatRange PreferredDistanceRange;
	UPROPERTY(VisibleInstanceOnly)
	float PreferredDistance = 200.f;
};
