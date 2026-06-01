// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "AI/Affiliation/TeamAffiliation.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TeamUtils.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UTeamUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FGenericTeamId GetTeamId(const AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ETeamAttitude::Type GetTeamAttitude(const AActor* Source, const AActor* Target);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool HaveHostileRelationship(const AActor* Source, const AActor* Target);
};
