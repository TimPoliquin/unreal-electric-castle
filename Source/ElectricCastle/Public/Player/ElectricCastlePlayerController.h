// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "InputActionValue.h"
#include "InputEvents.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "SelectionWheel/SelectionWheelManagerActorInterface.h"
#include "Interaction/HighlightInterface.h"
#include "SelectionWheel/SelectionWheelManagerComponent.h"
#include "ElectricCastlePlayerController.generated.h"

class USelectionWheelManagerComponent;
class UGameplayEffect;
enum class ECommonInputType : uint8;
class AMagicCircle;
class UNiagaraSystem;
class UDamageTextComponent;
class USplineComponent;
class UElectricCastleAbilitySystemComponent;
struct FGameplayTag;
class UInputConfiguration;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class EAuraInputMode : uint8
{
	MouseAndKeyboard,
	Gamepad
};

enum class ETargetingStatus
{
	NotTargeting,
	TargetingEnemy,
	TargetingOther,
};

UENUM(BlueprintType)
enum class EAttackMessageType : uint8
{
	Normal,
	Critical,
	Miss
};

USTRUCT()
struct FHighlightContext
{
	GENERATED_BODY()

	FHighlightContext()
	{
	}

	UPROPERTY()
	TObjectPtr<AActor> LastActor;
	UPROPERTY()
	TObjectPtr<AActor> CurrentActor;

	void Track(AActor* Actor)
	{
		LastActor = CurrentActor;
		if (IHighlightInterface::IsHighlightActor(Actor))
		{
			CurrentActor = Actor;
		}
		else
		{
			CurrentActor = nullptr;
		}
		if (IsDifferentPtr())
		{
			UnHighlightLast();
			HighlightCurrent();
		}
	}

	void Clear()
	{
		UnHighlightCurrent();
		UnHighlightLast();
		CurrentActor = nullptr;
		LastActor = nullptr;
	}

	bool HasCurrentTarget() const
	{
		return CurrentActor != nullptr;
	}

	bool IsDifferentPtr() const
	{
		return LastActor != CurrentActor;
	}

	void HighlightCurrent() const
	{
		if (CurrentActor != nullptr)
		{
			IHighlightInterface::HighlightActor(CurrentActor);
		}
	}

	void UnHighlightCurrent() const
	{
		if (CurrentActor != nullptr)
		{
			IHighlightInterface::UnHighlightActor(CurrentActor);
		}
	}

	void UnHighlightLast() const
	{
		if (LastActor != nullptr)
		{
			IHighlightInterface::UnHighlightActor(LastActor);
		}
	}
};

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API AElectricCastlePlayerController : public APlayerController, public ISelectionWheelManagerActorInterface
{
	GENERATED_BODY()

public:
	AElectricCastlePlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Client, Reliable)
	virtual void ShowDamageNumber(
		AActor* Target,
		float DamageAmount,
		const EAttackMessageType InAttackMessageType
	);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInputTypeMouse() const { return InputType == EAuraInputMode::MouseAndKeyboard; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInputTypeGamepad() const { return InputType == EAuraInputMode::Gamepad; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTargetingEnemy() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsTargetingOther() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsNotTargeting() const;
	void SetupInputMode();
	FInputModeGameAndUI BuildGameAndUIInputMode() const;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerFormWheelVisibilityChangeSignature OnFormWheelVisibilityChange;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void GetMovementVectors(const AController* Controller, FVector& OutForward, FVector& OutRight);

	/** Start SelectionWheelManagerActor Interface **/
	virtual USelectionWheelManagerComponent* GetSelectionWheelManagerComponent_Implementation() const override;
	/** End SelectionWheelManagerActor Interface **/

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSubclassOf<UGameplayEffect> MovementEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float AimClampMin = -1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	float AimClampMax = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FormWheelAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputConfiguration> InputConfig;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FormChangeAction;
	UPROPERTY()
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	// UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<USelectionWheelManagerComponent> SelectionWheelManager;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<AMagicCircle> MagicCircleClass;
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TObjectPtr<AMagicCircle> MagicCircle;
	// Debug
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebug = false;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	UFUNCTION(BlueprintNativeEvent)
	void OnGameDataLoaded();

private:
	FHighlightContext HighlightContext;
	FActiveGameplayEffectHandle MovementEffectHandle;

	void Move(const FInputActionValue& Value);
	void MoveEnd(const FInputActionValue& Value);
	void Aim(const FInputActionValue& InputActionValue);
	void CursorTrace();
	void CursorTrace_Mouse();
	void CursorTrace_Gamepad();
	UElectricCastleAbilitySystemComponent* GetAbilitySystemComponent();
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	UFUNCTION()
	void ShowFormWheel(const FInputActionValue& InputActionValue);
	UFUNCTION()
	void HideFormWheel(const FInputActionValue& InputActionValue);
	UFUNCTION()
	void HandleFormChangeInputAction(const FInputActionValue& InputActionValue);
	void UpdateMagicCircleLocation() const;
	void OnInputTypeChange(ECommonInputType NewInputType);
	UFUNCTION(Server, Reliable)
	void SetInputMode_Gamepad_Server();
	UFUNCTION(Server, Reliable)
	void SetInputMode_KeyboardAndMouse_Server();
	UFUNCTION()
	void OnEffectStateChanged_Aiming(FGameplayTag AimingTag, int TagCount);
	UFUNCTION()
	void HandleSelectionWheelStateChanged(const FSelectionWheelStateChangedPayload& Payload);

	bool IsAiming();

	// Character Movement / Targeting
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	FHitResult CursorHit;
	UPROPERTY(Replicated)
	EAuraInputMode InputType = EAuraInputMode::MouseAndKeyboard;
};
