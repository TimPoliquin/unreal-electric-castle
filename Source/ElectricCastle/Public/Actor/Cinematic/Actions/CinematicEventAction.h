// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CinematicEventAction.generated.h"

class UCinematicContextHandle;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ELECTRICCASTLE_API UCinematicEventAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Initialize(AActor* InOwner);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ShouldExecute(const UCinematicContextHandle* ContextHandle) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Execute(const UCinematicContextHandle* ContextHandle) const;

	AActor* GetOwner() const;
	FString GetOwnerName() const;
	virtual UWorld* GetWorld() const override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Cinematic.Action.Trigger"))
	FGameplayTagContainer Triggers = FGameplayTagContainer::EmptyContainer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebug = false;
};
