// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Spawn/ActorTrackerComponent.h"
#include "Components/ActorComponent.h"
#include "SummonComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API USummonComponent : public UActorTrackerComponent
{
	GENERATED_BODY()

public:
	USummonComponent();

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetSummonCount() const;
	UFUNCTION(BlueprintCallable)
	void AddMinion(AActor* Minion);
};
