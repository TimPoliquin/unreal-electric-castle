// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Interaction/UnbindableInterface.h"
#include "Player/ElectricCastlePlayerController.h"
#include "UObject/Object.h"
#include "ElectricCastleWidgetController.generated.h"

class AElectricCastlePlayerCharacter;
class UAbilityInfo;
class UElectricCastleAttributeSet;
class UElectricCastleAbilitySystemComponent;
class AElectricCastlePlayerState;
class AElectricCastlePlayerController;
class UAttributeSet;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FElectricCastleAbilityInfo&, AbilityInfo);

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	FWidgetControllerParams()
	{
	}

	FWidgetControllerParams(
		AActor* InPlayer,
		APlayerController* InPlayerController,
		AElectricCastlePlayerState* InPlayerState,
		UElectricCastleAbilitySystemComponent* InAbilitySystemComponent,
		UElectricCastleAttributeSet* InAttributeSet
	) :
		Player(InPlayer),
		PlayerController(InPlayerController),
		PlayerState(InPlayerState),
		AbilitySystemComponent(InAbilitySystemComponent),
		AttributeSet(InAttributeSet)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> Player = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AElectricCastlePlayerState> PlayerState = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UElectricCastleAttributeSet> AttributeSet = nullptr;
};


/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UElectricCastleWidgetController : public UObject, public IUnbindableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues()
	{
	}

	virtual void BindCallbacksToDependencies()
	{
	}

	virtual void BroadcastAbilityInfo();
	virtual void UnbindAll_Implementation(const UObject* BoundObject) override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

protected:
	AElectricCastlePlayerCharacter* GetAuraCharacter();
	AElectricCastlePlayerController* GetAuraPlayerController();
	AElectricCastlePlayerState* GetPlayerState();
	UElectricCastleAbilitySystemComponent* GetAbilitySystemComponent();
	UElectricCastleAttributeSet* GetAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AActor> Player;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AElectricCastlePlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UElectricCastleAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Controller")
	TObjectPtr<UElectricCastleAttributeSet> AttributeSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;

private:
	UPROPERTY()
	TObjectPtr<AElectricCastlePlayerCharacter> AuraCharacter;
	UPROPERTY()
	TObjectPtr<AElectricCastlePlayerController> AuraPlayerController;
};
