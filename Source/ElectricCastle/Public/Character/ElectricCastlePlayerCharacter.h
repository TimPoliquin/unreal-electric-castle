// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ElectricCastleCharacter.h"
#include "Camera/ElectricCastleCameraComponent.h"
#include "Components/LODSyncComponent.h"
#include "Interaction/FishingActorInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Player/ElectricCastlePlayerState.h"
#include "Player/Equipment/EquipmentEvents.h"
#include "Player/Form/FormChangeActorInterface.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "LiveLinkTypes.h"
#include "Player/Equipment/EquipmentManagerInterface.h"
#include "ElectricCastlePlayerCharacter.generated.h"

class ULiveLinkRetargetAsset;
struct FLODMappingData;
class ULODSyncComponent;
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
class UGroomComponent;
class UMetaHumanComponentUE;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnAnimationCompleteSignature,
	const EEquipmentSlot&,
	Slot
);

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FLiveLinkCharacterConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUseLiveLink = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bUseLiveLink", EditConditionHides))
	bool bUseARKit = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bUseLiveLink", EditConditionHides))
	FLiveLinkSubjectName LiveLinkSubject = FLiveLinkSubjectName(NAME_None);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bUseLiveLink && !bUseARKit", EditConditionHides))
	TSubclassOf<ULiveLinkRetargetAsset> RetargetAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bUseLiveLink && bUseARKit", EditConditionHides))
	TSubclassOf<UAnimInstance> BodyAnimInstanceClass;
};

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AElectricCastlePlayerCharacter : public AElectricCastleCharacter, public IPlayerInterface, public IFishingActorInterface, public IFormChangeActorInterface, public IEquipmentManagerInterface
{
	GENERATED_BODY()

public:
	AElectricCastlePlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UElectricCastleAttributeSet* GetAttributeSet() const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_ActiveAbilityTag() override;
	virtual void OnRep_StatusEffectTags() override;
	virtual void OnAbilitySystemReady_Implementation(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent) override;
	virtual void OnEffectAdd_LightningDamage_Implementation() override;
	virtual void OnEffectRemove_LightningDamage_Implementation() override;
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
	virtual void ClearFacingTarget_Implementation() override;

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
	virtual int32 GetSpellPoints_Implementation() const override;
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
	UFUNCTION(BlueprintNativeEvent)
	void SetFormMeshes(const FFormMeshConfig& FormMeshConfig);
	UFUNCTION(BlueprintNativeEvent)
	void TryApplyGroomAssets(const FFormMeshConfig& FormMeshConfig);
	UFUNCTION(BlueprintNativeEvent)
	void SetGroomAssets(const FFormMeshConfig& FormMeshConfig);

	/** FishingActorInterface Start */
	virtual UFishingComponent* GetFishingComponent_Implementation() const override;
	virtual void ShowFishingStatusEffect_Implementation(UNiagaraSystem* EffectSystem) override;
	/** FishingActorInterface End */

	/** FormChangeActorInterface Start */
	virtual UPlayerFormChangeComponent* GetFormChangeComponent_Implementation() const override;
	void SetAnimInstanceClass(const TSubclassOf<UAnimInstance> InAnimInstance);
	/** FormChangeActorInterface End */

	/** EquipmentManagerInterface Start */
	virtual UPlayerEquipmentComponent* GetEquipmentComponent_Implementation() const override { return EquipmentComponent; }
	/** EquipmentManagerInterface End */
	

protected:
	UFUNCTION(BlueprintNativeEvent)
	void Construction_SetupMetaHuman();
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	UFUNCTION(BlueprintImplementableEvent)
	void PlayFishingRodCastMontage();
	UFUNCTION(BlueprintNativeEvent)
	void OnEquipmentAnimationRequest(const FEquipmentDelegatePayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void OnFormChange(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void PrepareLiveLinkSetup();
	UFUNCTION(BlueprintImplementableEvent)
	void LiveLink_SetupARKit(const FLiveLinkSubjectName& LiveLinkSubjectName, UAnimInstance* AnimInstance);
	UFUNCTION(BlueprintNativeEvent)
	void LiveLink_SetupLiveLinkInstance(UAnimInstance* AnimInstance);
	UFUNCTION(BlueprintNativeEvent)
	void LiveLink_SetUpdateAnimationInEditor();
	UFUNCTION(BlueprintNativeEvent)
	void EnableMasterPose(USkeletalMeshComponent* SkeletalMeshComponent);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<USkeletalMeshComponent> ClothingMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<USkeletalMeshComponent> FaceMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<UGroomComponent> Groom_Fuzz;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<UGroomComponent> Groom_Eyebrows;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<UGroomComponent> Groom_Hair;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<UGroomComponent> Groom_Eyelashes;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<UGroomComponent> Groom_Beard;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components|Metahuman")
	TObjectPtr<UGroomComponent> Groom_Moustache;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UMetaHumanComponentUE> MetaHumanComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULODSyncComponent> LODSyncComponent;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> LevelUpSound;
	UPROPERTY(EditDefaultsOnly)
	float DeathTime = 5.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MotionWarping")
	FVector FacingTarget = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LiveLink")
	FLiveLinkCharacterConfig LiveLinkConfig;

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
	FLODMappingData CreateCustomLODMappingDefault() const;
};
