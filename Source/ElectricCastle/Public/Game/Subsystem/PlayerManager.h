// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PlayerManager.generated.h"

class AElectricCastlePlayerCharacter;
class AElectricCastlePlayerController;

USTRUCT()
struct ELECTRICCASTLE_API FPlayerPair
{
	GENERATED_BODY()
	UPROPERTY()
	TWeakObjectPtr<AElectricCastlePlayerController> PlayerController;
	UPROPERTY()
	TWeakObjectPtr<AElectricCastlePlayerCharacter> PlayerCharacter;

	bool IsValid() const
	{
		return PlayerController.IsValid() && PlayerCharacter.IsValid();
	}
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UPlayerManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UPlayerManager* Get(const UObject* WorldContextObject);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetPlayerControllers(TArray<AElectricCastlePlayerController*>& Controllers) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetPlayerCharacters(TArray<AElectricCastlePlayerCharacter*>& Characters) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPlayerCharacter(const AActor* Actor) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPlayerController(const AActor* Actor) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AElectricCastlePlayerController* GetPlayerController(const AActor* Actor) const;
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SetTimeDilation(const float WorldMagnitude, const float PlayerMagnitude);

	void RegisterPlayer(AElectricCastlePlayerController* PlayerController, AElectricCastlePlayerCharacter* PlayerCharacter);
	void UnregisterPlayer(AElectricCastlePlayerController* PlayerController);
	void UnregisterPlayer(AElectricCastlePlayerCharacter* PlayerCharacter);

private:
	static FString GetNetModeName(const ENetMode& NetMode);

	UPROPERTY()
	TArray<FPlayerPair> PlayerPairs;
};
