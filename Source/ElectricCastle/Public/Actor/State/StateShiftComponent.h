// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StateShiftTypes.h"
#include "Components/ActorComponent.h"
#include "StateShiftComponent.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FStateReactions
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EStateShiftReactionType> NormalReactions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EStateShiftReactionType> PsychedelicReactions;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UStateShiftComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStateShiftComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool ShouldChangeState(const EStateShiftState NewState) const;
	UFUNCTION(BlueprintCallable)
	void SetCurrentState(EStateShiftState NewState);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<EStateShiftReactionType> GetReactions(const EStateShiftState NewState) const;

protected:
	virtual void BeginPlay() override;
	void HandleStateShift(EStateShiftState NewState);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|StateShift")
	FStateReactions StateShiftReactions;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Properties|StateShift", Replicated)
	EStateShiftState CurrentState = EStateShiftState::Normal;
};
