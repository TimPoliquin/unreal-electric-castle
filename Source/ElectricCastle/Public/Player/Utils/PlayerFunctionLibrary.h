// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerFunctionLibrary.generated.h"

class AElectricCastlePlayerController;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UPlayerFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static AElectricCastlePlayerController* GetPlayerController(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ULocalPlayer* GetLocalPlayer(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf="Actor"))
	static bool HasWeaponEquipped(const AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf="Actor"))
	static bool IsPlayerAiming(const AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf="Actor"))
	static bool IsPlayerLockedOn(const AActor* Actor);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(DefaultToSelf="Actor"))
	static bool IsPlayerAimingOrLockedOn(const AActor* Actor);

	template <typename TPlayerSubsystem>
	static TPlayerSubsystem* GetPlayerSubsystem(UObject* WorldContextObject);
};

template <typename TPlayerSubsystem>
TPlayerSubsystem* UPlayerFunctionLibrary::GetPlayerSubsystem(UObject* WorldContextObject)
{
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer(WorldContextObject))
	{
		return LocalPlayer->GetSubsystem<TPlayerSubsystem>();
	}
	return nullptr;
};
