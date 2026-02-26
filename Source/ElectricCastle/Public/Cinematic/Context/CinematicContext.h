// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Cinematic/CinematicEvents.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "CinematicContext.generated.h"

class UCinematicSequenceMetadata;

USTRUCT()
struct FCinematicRestoreStack
{
	GENERATED_BODY()

	void Add(TFunction<void()> Func)
	{
		Actions.Add(MoveTemp(Func));
	}

	void RestoreAll()
	{
		for (int32 i = Actions.Num() - 1; i >= 0; --i) { Actions[i](); }
		Actions.Empty();
	}

private:
	TArray<TFunction<void()>> Actions;
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UCinematicContext : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	bool HasTag(const FGameplayTag& Tag) const;
	bool HasAnyTag(const FGameplayTagContainer& Tags) const;
	bool ShouldRelocatePlayer() const;
	FVector GetPlayerRelocationLocation() const;

	void RestoreAll();

	void SetSequenceMetadata(UCinematicSequenceMetadata* InMetadata);
	void AddRestoreFunction(const TFunction<void()>& InRestoreFunc);

	FCinematicContextEventSignature OnCinematicBegin;
	FCinematicContextEventSignature OnCinematicEnd;

protected:
	UPROPERTY()
	TWeakObjectPtr<UCinematicSequenceMetadata> Metadata;
	UPROPERTY()
	FCinematicRestoreStack RestoreStack;
};
