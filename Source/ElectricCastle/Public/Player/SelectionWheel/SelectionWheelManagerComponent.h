// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "Components/ActorComponent.h"
#include "SelectionWheelManagerComponent.generated.h"


enum class ECommonInputType : uint8;
class UInputMappingContext;
class UElectricCastleInputComponent;
class UInputAction;
struct FInputActionValue;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FSelectionWheelStateChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	bool bIsActive = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectionWheelStateChangedSignature, const FSelectionWheelStateChangedPayload&, Payload);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API USelectionWheelManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USelectionWheelManagerComponent();

	void SetupInputComponent(UElectricCastleInputComponent* InputComponent);
	void AddListener(UObject* Listener);
	void RemoveListener(const UObject* ToRemove);
	void SetInputType(const ECommonInputType NewInputType) { InputType = NewInputType; }
	bool IsSelectionWheelActive() const;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FSelectionWheelStateChangedSignature OnSelectionWheelStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UInputAction> SelectInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UInputAction> ConfirmInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Properties")
	float MouseSensitivity = 5.f;

private:
	void UpdateAngle(const FInputActionValue& InputActionValue);
	void ConfirmSelection(const FInputActionValue& InputActionValue);

	bool CalculateFormWheelAngle_Gamepad(const FVector2D& InputDirection, float& OutFormWheelAngle) const;
	bool CalculateFormWheelAngle_Mouse(const FVector2D& InputDirection, float& OutFormWheelAngle) const;
	void RemoveStaleListeners();

	void NotifyListeners();

	UPROPERTY(VisibleInstanceOnly, Category="Properties")
	ECommonInputType InputType = ECommonInputType::Count;
	UPROPERTY(VisibleInstanceOnly, Category="Properties")
	float CurrentAngle = 0.f;
	UPROPERTY(VisibleInstanceOnly, Category="Properties")
	TArray<TWeakObjectPtr<UObject>> Listeners;
	UPROPERTY(VisibleInstanceOnly, Category="Properties")
	bool bHasAddedContext = false;
};
