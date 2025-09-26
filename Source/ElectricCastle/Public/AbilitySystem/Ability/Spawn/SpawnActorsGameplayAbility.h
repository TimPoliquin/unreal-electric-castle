// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/ElectricCastleGameplayAbility.h"
#include "SpawnActorsGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API USpawnActorsGameplayAbility : public UElectricCastleGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AuraSummonAbility")
	TArray<FVector> GetSpawnLocations() const;
	UFUNCTION(BlueprintCallable, Category = "Summoning")
	TSubclassOf<APawn> GetRandomSpawnClass() const;


	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	int32 NumToSpawn = 5;
	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	TArray<TSubclassOf<APawn>> SpawnClasses;
	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float MinSpawnRadius = 75.f;
	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float MaxSpawnRadius = 300.f;
	UPROPERTY(EditDefaultsOnly, Category="Summoning")
	float SpawnSpread = 120.f;
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShowDebug;

private:
	void DrawDebugRange(
		const FVector& Location,
		const FVector& Direction,
		const FVector& LeftExtent
	) const;
};
