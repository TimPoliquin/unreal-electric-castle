// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ElectricCastleSpawnBlueprintLibrary.generated.h"

USTRUCT(BlueprintType)
struct FAuraSpawnParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<AActor> SpawnClass;
	UPROPERTY(BlueprintReadOnly)
	FTransform SpawnTransform;

	bool IsValid() const
	{
		return SpawnClass != nullptr && SpawnTransform.IsValid();
	}
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleSpawnBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static TArray<FTransform> GenerateSpawnLocations(const FVector& Center, const float Radius, const int32 NumSpawns, const float Spread = 360, const FVector ForwardVector = FVector::ForwardVector,
	                                                 const FVector UpVector = FVector::UpVector);
	UFUNCTION(BlueprintCallable)
	static FAuraSpawnParams SafePop(UPARAM(ref)
		TArray<FAuraSpawnParams>& SpawnParams);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsValid(UPARAM(ref)
		const FAuraSpawnParams& SpawnParams);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector GetGroundLocation(const AActor* Actor);
};
