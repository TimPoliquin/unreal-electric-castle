// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EncounterBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UEncounterBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static TArray<AActor*> GetAllActorsPerceivedByActors(const TArray<AActor*>& PerceivingActors);
	static TArray<AActor*> GetAllActorsPerceivedByWeakActors(const TArray<TWeakObjectPtr<AActor>>& PerceivingActors);
	UFUNCTION(BlueprintCallable, Category="Encounter")
	static AActor* GetClosestActor(const AActor* SourceActor, const TArray<AActor*>& Actors, FVector& OutLocation);
};
