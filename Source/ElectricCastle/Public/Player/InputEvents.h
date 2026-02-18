#pragma once

#include "CoreMinimal.h"
#include "InputEvents.generated.h"

class AElectricCastlePlayerController;
class AElectricCastlePlayerCharacter;
class AElectricCastlePlayerState;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FOnPlayerFormWheelVisibilityChangePayload
{
	GENERATED_BODY()
	TWeakObjectPtr<AElectricCastlePlayerController> PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AElectricCastlePlayerState> PlayerState = nullptr;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AElectricCastlePlayerCharacter> PlayerCharacter = nullptr;
	UPROPERTY(BlueprintReadOnly)
	bool bIsVisible = false;

	AElectricCastlePlayerController* GetPlayerController() const;
	AElectricCastlePlayerState* GetPlayerState() const;
	AElectricCastlePlayerCharacter* GetPlayerCharacter() const;

	bool IsValid() const;
	bool IsVisible() const;
	bool IsHidden() const;
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPlayerFormWheelVisibilityChangeSignature,
	const FOnPlayerFormWheelVisibilityChangePayload&,
	Payload
);
