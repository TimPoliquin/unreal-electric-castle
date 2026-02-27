// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PuzzleAction.generated.h"

class UPuzzleManagerComponent;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ELECTRICCASTLE_API UPuzzleAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Initialize(AActor* InOwner, UPuzzleManagerComponent* InPuzzleManager);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ShouldExecute() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Execute() const;
	UFUNCTION(BlueprintCallable)
	virtual bool IsDebugEnabled() const { return bDebug; }

	UFUNCTION(BlueprintCallable)
	virtual void SetDebugEnabled(const bool bInDebug) { bDebug = bInDebug; }

	AActor* GetOwner() const;
	FString GetOwnerName() const;
	UPuzzleManagerComponent* GetPuzzleManager() const;
	virtual UWorld* GetWorld() const override;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UPuzzleManagerComponent> PuzzleManager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;
};
