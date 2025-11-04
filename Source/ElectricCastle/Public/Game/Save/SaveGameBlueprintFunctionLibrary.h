// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SaveGameTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveGameBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API USaveGameBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AuraSaveGameBlueprintFunctionLibrary|WorldData")
	static void AddActorData(const FActorSaveData& ActorData, FWorldSaveData& WorldSaveData);

	UFUNCTION(BlueprintCallable, Category = "AuraSaveGameBlueprintFunctionLibrary|WorldData")
	static bool RemoveActorData(const FString& ActorName, FWorldSaveData& WorldSaveData);

	UFUNCTION(BlueprintCallable, Category = "AuraSaveGameBlueprintFunctionLibrary|WorldData")
	static bool FindActorData(const FString& ActorName, const FWorldSaveData& WorldSaveData, FActorSaveData& ActorSaveData);

	UFUNCTION(BlueprintCallable, Category = "AuraSaveGameBlueprintFunctionLibrary|WorldData")
	static int32 GetActorCount(FWorldSaveData& WorldSaveData);

	UFUNCTION(BlueprintCallable, Category = "AuraSaveGameBlueprintFunctionLibrary|SaveId")
	static FString GenerateSaveID(const UObject* Object);
};
