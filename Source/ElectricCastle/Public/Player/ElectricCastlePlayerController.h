// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "InputActionValue.h"
#include "InputEvents.h"
#include "InputMappingContext.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "Actor/Highlight/HighlightTypes.h"

#include "Aim/AimActorInterface.h"

#include "GameFramework/PlayerController.h"
#include "LockOn/LockOnActor.h"
#include "LockOn/LockOnEvents.h"
#include "SelectionWheel/SelectionWheelManagerActorInterface.h"
#include "SelectionWheel/SelectionWheelManagerComponent.h"
#include "ElectricCastlePlayerController.generated.h"

class UCinematicHandlerComponent;
class USelectionWheelManagerComponent;
class UGameplayEffect;
enum class ECommonInputType : uint8;
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


/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API AElectricCastlePlayerController : public APlayerController,
                                                           public ISelectionWheelManagerActorInterface,
                                                           public IAimActorInterface,
                                                           public ILockOnActor
{
	GENERATED_BODY()

public:
	AElectricCastlePlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void LockPlayerRotationToCamera() const;
	void FreePlayerRotationFromCamera() const;

	UFUNCTION(Client, Reliable)
	virtual void ShowDamageNumber(
		AActor* Target,
		float DamageAmount,
		const EAttackMessageType InAttackMessageType
	);

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

	/** Start IAimActorInterface **/
	virtual UAimController* GetAimController_Implementation() const override;
	/** End IAimActorInterface **/

	/** Start ILockOnActor **/
	virtual ULockOnController* GetLockOnController_Implementation() const override;
	/** End ILockOnActor **/
protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSubclassOf<UGameplayEffect> MovementEffect;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UCinematicHandlerComponent> CinematicHandlerComponent;
	// UI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<USelectionWheelManagerComponent> SelectionWheelManager;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	// Debug
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebug = false;

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	UFUNCTION(BlueprintNativeEvent)
	void OnGameDataLoaded();
	UFUNCTION()
	void HandleAimStart();
	UFUNCTION()
	void HandleAimEnd();
	UFUNCTION(BlueprintNativeEvent)
	void HandleLockOnTarget(const FLockOnTargetPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void HandleLockOnRelease();

private:
	FHighlightContext HighlightContext;
	FActiveGameplayEffectHandle MovementEffectHandle;

	void Move(const FInputActionValue& Value);
	void MoveEnd(const FInputActionValue& Value);
	void Look(const FInputActionValue& InputActionValue);
	void JumpStart(const FInputActionValue& InputActionValue);
	void JumpEnd(const FInputActionValue& InputActionValue);
	void CursorTrace();
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
	UFUNCTION()
	void OnInputTypeChange(ECommonInputType NewInputType);
	UFUNCTION(Server, Reliable)
	void SetInputMode_Gamepad_Server();
	UFUNCTION(Server, Reliable)
	void SetInputMode_KeyboardAndMouse_Server();
	UFUNCTION()
	void HandleSelectionWheelStateChanged(const FSelectionWheelStateChangedPayload& Payload);

	bool HasEffectiveGameplayTag(const FGameplayTag& Tag);

	// Character Movement / Targeting
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;

	FHitResult CursorHit;
	UPROPERTY(Replicated)
	EAuraInputMode InputType = EAuraInputMode::MouseAndKeyboard;
};
