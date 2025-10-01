// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/HighlightInterface.h"
#include "ElectricCastlePlayerController.generated.h"

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
class ELECTRICCASTLE_API AElectricCastlePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AElectricCastlePlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	virtual void ShowDamageNumber(
		AActor* Target,
		float DamageAmount,
		const bool IsBlockedHit,
		const bool IsCriticalHit
	);

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInputTypeMouse() const { return InputType == EAuraInputMode::MouseAndKeyboard; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsInputTypeGamepad() const { return InputType == EAuraInputMode::Gamepad; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	FHighlightContext HighlightContext;

	void Move(const FInputActionValue& Value);
	void CursorTrace();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputConfiguration> InputConfig;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> FormChangeAction;

	UPROPERTY()
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	UElectricCastleAbilitySystemComponent* GetAbilitySystemComponent();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	UFUNCTION()
	void HandleFormChangeInputAction(const FInputActionValue& InputActionValue);


	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	// Character Movement / Targeting
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TSubclassOf<AMagicCircle> MagicCircleClass;
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TObjectPtr<AMagicCircle> MagicCircle;
	FHitResult CursorHit;
	EAuraInputMode InputType = EAuraInputMode::MouseAndKeyboard;

	void UpdateMagicCircleLocation() const;

	bool IsTargetingEnemy() const;
	bool IsTargetingOther() const;
	bool IsNotTargeting() const;
	void InitializeInputMode(ECommonInputType NewInputMode);
};
