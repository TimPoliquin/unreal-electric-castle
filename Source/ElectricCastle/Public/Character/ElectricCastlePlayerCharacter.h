// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleCharacter.h"
#include "Camera/ElectricCastleCameraComponent.h"
#include "Interaction/FishingActorInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Equipment/EquipmentEvents.h"
#include "Player/Form/FormChangeActorInterface.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "ElectricCastlePlayerCharacter.generated.h"

class UPlayerFormChangeComponent;
class UBoxComponent;
class UPlayerEquipmentComponent;
class UFishingComponentInterface;
class UFishingComponent;
class UInventoryComponent;
class UElectricCastleCameraComponent;
class UElectricCastleAbilitySystemComponent;
class UElectricCastleAttributeSet;
class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
class APlayerState;
class UMotionWarpingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAnimationCompleteSignature,
	const EEquipmentSlot&,
	Slot
);

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastlePlayerCharacter : public AElectricCastleCharacter, public IPlayerInterface, public IFishingActorInterface, public IFormChangeActorInterface
{
	GENERATED_BODY()

public:
	AElectricCastlePlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual UElectricCastleAttributeSet* GetAttributeSet() const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_ActiveAbilityTag() override;
	virtual void OnRep_StatusEffectTags() override;
	virtual void OnAbilitySystemReady_Implementation(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AElectricCastlePlayerState* GetElectricCastlePlayerState() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UElectricCastleAbilitySystemComponent* GetElectricCastleAbilitySystemComponent() const;
	/** Start IElectricCastleAbilitySystemInterface **/
	virtual int32 GetCharacterLevel_Implementation() const override;
	/** End IElectricCastleAbilitySystemInterface **/
	// ICombatInterface
	virtual TArray<FName> GetTargetTagsToIgnore_Implementation() const override;
	virtual void Die() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() const override;
	virtual void UpdateFacingTarget_Implementation(const FVector& InFacingTarget) override;

	// Player Interface
	virtual int32 GetXP_Implementation() override;
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 FindLevelForXP_Implementation(const int32 InXP) const override;
	virtual FLevelUpRewards GetLevelUpRewards_Implementation(const int32 InLevel) const override;
	virtual void ApplyLevelUpRewards_Implementation(
		const int32 LevelIncrement,
		const FLevelUpRewards& InLevelUpRewards
	) override;
	virtual int32 GetAttributePoints_Implementation() const override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void SpendAttributePoints_Implementation(int32 SpentPoints) override;
	virtual void SpendSpellPoints_Implementation(int32 SpentPoints) override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial = nullptr) override;
	virtual void HideMagicCircle_Implementation() override;
	virtual void MoveCameraToPoint_Implementation(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve
	) override;
	virtual void MoveCameraToPointWithCallback(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve,
		FOnCameraMoveFinishedSignature& OnCameraMoveFinishedSignature
	) override;
	virtual void ReturnCamera_Implementation(
		UCurveFloat* AnimationCurve
	) override;

	/** FishingActorInterface Start */
	virtual UFishingComponent* GetFishingComponent_Implementation() const override;
	virtual void ShowFishingStatusEffect_Implementation(UNiagaraSystem* EffectSystem) override;
	/** FishingActorInterface End */

	/** FormChangeActorInterface Start */
	virtual UPlayerFormChangeComponent* GetFormChangeComponent_Implementation() const override;
	/** FormChangeActorInterface End */

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	UFUNCTION(BlueprintImplementableEvent)
	void PlayFishingRodCastMontage();
	UFUNCTION(BlueprintNativeEvent)
	void OnEquipmentAnimationRequest(const FEquipmentDelegatePayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void OnFormChange(const FPlayerFormChangeEventPayload& Payload);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> FadeDetectionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UFishingComponent> FishingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UElectricCastleCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> FishingStatusEffectNiagaraComponent;
	UPROPERTY(EditDefaultsOnly, Category="Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPlayerFormChangeComponent> FormChangeComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> LevelUpSound;
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MotionWarping")
	FVector FacingTarget = FVector::ZeroVector;

private:
	UPROPERTY()
	FTimerHandle DeathTimer;
	FOnCameraMoveFinishedSignature OnCameraReturnDelegate;

	virtual void InitializeAbilityActorInfo() override;
	void InitializePlayerControllerHUD(APlayerController* InPlayerController, AElectricCastlePlayerState* InPlayerState);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_LevelUpParticles() const;
	UFUNCTION()
	void OnCameraReturned();
	UFUNCTION()
	void OnLevelLoaded();
	UFUNCTION()
	void OnFadeDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                 const FHitResult& SweepResult);
	UFUNCTION()
	void OnFadeDetectionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector DesiredCameraForwardVector;
};
