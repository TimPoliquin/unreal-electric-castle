// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/Object.h"
#include "CameraMovementModifier.generated.h"

class UCameraComponent;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API UCameraMovementModifier : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void GetPlayerMovementInput(FVector& OutPlayerForwardDirection, FVector& OutPlayerRightDirection) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCameraComponent(UCameraComponent* InCameraComponent);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn))
	TObjectPtr<UCameraComponent> CameraComponent;
};
