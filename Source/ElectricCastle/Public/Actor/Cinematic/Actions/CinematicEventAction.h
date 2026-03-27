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
	UFUNCTION(BlueprintCallable)
	virtual bool IsDebugEnabled() const { return bDebug; }

	UFUNCTION(BlueprintCallable)
	virtual void SetDebugEnabled(const bool bInDebug) { bDebug = bInDebug; }

	AActor* GetOwner() const;
	FString GetOwnerName() const;
	virtual UWorld* GetWorld() const override;
	virtual AController* GetOwnerController() const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Owner;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(Categories="Cinematic.Action.Trigger"))
	FGameplayTagContainer Triggers = FGameplayTagContainer::EmptyContainer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;
};
