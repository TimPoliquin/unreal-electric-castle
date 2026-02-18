// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "Components/ActorComponent.h"
#include "RadialUIInputComponent.generated.h"


enum class ECommonInputType : uint8;
class UElectricCastleInputComponent;
class UInputAction;
struct FInputActionValue;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API URadialUIInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URadialUIInputComponent();

	void SetupInputComponent(UElectricCastleInputComponent* InputComponent);
	void AddListener(UObject* Listener);
	void RemoveListener(const UObject* ToRemove);
	void SetInputType(const ECommonInputType NewInputType) { InputType = NewInputType; }

protected:
	void UpdateAngle(const FInputActionValue& InputActionValue);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UInputAction> InputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	float AnalogDeadZone = 0.3f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	float MouseSensitivity = 5.f;

private:
	bool CalculateFormWheelAngle_Gamepad(const FVector2D& InputDirection, float& OutFormWheelAngle) const;
	bool CalculateFormWheelAngle_Mouse(const FVector2D& InputDirection, float& OutFormWheelAngle) const;

	void NotifyListeners();

	UPROPERTY(VisibleAnywhere, Category="Properties")
	ECommonInputType InputType = ECommonInputType::Count;
	UPROPERTY(VisibleInstanceOnly, Category="Properties")
	float CurrentAngle = 0.f;
	UPROPERTY(VisibleInstanceOnly, Category="Properties")
	TArray<TWeakObjectPtr<UObject>> Listeners;
};
