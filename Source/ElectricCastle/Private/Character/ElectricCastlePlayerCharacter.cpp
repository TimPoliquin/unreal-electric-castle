// Copyright Alien Shores


#include "Character/ElectricCastlePlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ElectricCastlePlayerController.h"
#include "Player/ElectricCastlePlayerState.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Camera/ElectricCastleCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Fishing/FishingComponent.h"
#include "Game/Save/SaveGameManager.h"
#include "Game/Subsystem/ElectricCastleAIDirectorGameInstanceSubsystem.h"
#include "Game/Subsystem/ElectricCastleLevelManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "UI/HUD/ElectricCastleHUD.h"
#include "Interaction/PlayerInterface.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/Progression/ProgressionComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/BoxComponent.h"
#include "Interaction/FadeInterface.h"
#include "Player/Form/PlayerFormChangeComponent.h"

class UPlayerFormConfig;

AElectricCastlePlayerCharacter::AElectricCastlePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(TAG_PLAYER);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Level Up Effect"));
	LevelUpNiagaraComponent->SetupAttachment(EffectAttachComponent);
	LevelUpNiagaraComponent->SetAutoActivate(false);
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->bDoCollisionTest = false;
	CameraComponent = CreateDefaultSubobject<UElectricCastleCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	FadeDetectionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Fade Detection Component"));
	FadeDetectionComponent->SetupAttachment(SpringArmComponent);
	FadeDetectionComponent->SetBoxExtent(FVector(370.f, 26.f, 32.f));
	FadeDetectionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	FadeDetectionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	FadeDetectionComponent->SetGenerateOverlapEvents(true);
	EquipmentComponent = CreateDefaultSubobject<UPlayerEquipmentComponent>(TEXT("Equipment Component"));
	FishingComponent = CreateDefaultSubobject<UFishingComponent>(TEXT("Fishing Component"));
	FishingStatusEffectNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Fishing Status Effect"));
	FishingStatusEffectNiagaraComponent->SetupAttachment(EffectAttachComponent);
	FishingStatusEffectNiagaraComponent->SetAutoActivate(false);
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));
	FormChangeComponent = CreateDefaultSubobject<UPlayerFormChangeComponent>(TEXT("Form Change Component"));
	FormChangeComponent->OnPlayerFormChange.AddDynamic(this, &AElectricCastlePlayerCharacter::OnFormChange);
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
}

void AElectricCastlePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UElectricCastleAIDirectorGameInstanceSubsystem* AIDirectorSubsystem = UElectricCastleAIDirectorGameInstanceSubsystem::Get(this))
	{
		AIDirectorSubsystem->RegisterActivePlayer(this);
	}
	OnCameraReturnDelegate.BindUObject(this, &AElectricCastlePlayerCharacter::OnCameraReturned);
	FadeDetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &AElectricCastlePlayerCharacter::OnFadeDetectionBeginOverlap);
	FadeDetectionComponent->OnComponentEndOverlap.AddDynamic(this, &AElectricCastlePlayerCharacter::OnFadeDetectionEndOverlap);
}

void AElectricCastlePlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	if (UElectricCastleAIDirectorGameInstanceSubsystem* AIDirectorSubsystem = UElectricCastleAIDirectorGameInstanceSubsystem::Get(this))
	{
		AIDirectorSubsystem->UnregisterActivePlayer(this);
	}
}

void AElectricCastlePlayerCharacter::OnFormChange_Implementation(const FPlayerFormChangeEventPayload& Payload)
{
	FormChangeComponent->FormChange_PlayEffect(Payload);
	FormChangeComponent->FormChange_UpdateCharacterMesh(Payload);
}

void AElectricCastlePlayerCharacter::OnEquipmentAnimationRequest_Implementation(const FEquipmentDelegatePayload& Payload)
{
	EquipmentComponent->UseEquipment(Payload.EquipmentSlot);
}

void AElectricCastlePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectricCastlePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UElectricCastleAttributeSet* AElectricCastlePlayerCharacter::GetAttributeSet() const
{
	if (const AElectricCastlePlayerState* ElectricCastlePlayerState = GetElectricCastlePlayerState())
	{
		return ElectricCastlePlayerState->GetAttributeSet();
	}
	return nullptr;
}

void AElectricCastlePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Init ability actor info for the server
	InitializeAbilityActorInfo();
	if (UElectricCastleLevelManager* LevelManager = UElectricCastleLevelManager::Get(this); LevelManager->IsTransitioningLevels())
	{
		LevelManager->OnLevelTransitionComplete.AddWeakLambda(this, [&]()
		{
			OnLevelLoaded();
		});
	}
	else
	{
		OnLevelLoaded();
	}
}

void AElectricCastlePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the client
	InitializeAbilityActorInfo();
	if (APlayerController* PlayerController = Cast<AElectricCastlePlayerController>(GetController()))
	{
		InitializePlayerControllerHUD(PlayerController, GetElectricCastlePlayerState());
	}
}

void AElectricCastlePlayerCharacter::OnRep_ActiveAbilityTag()
{
	Super::OnRep_ActiveAbilityTag();
}

void AElectricCastlePlayerCharacter::OnRep_StatusEffectTags()
{
	Super::OnRep_StatusEffectTags();
	if (UElectricCastleAbilitySystemComponent* ElectricCastleAbilitySystemComponent = GetElectricCastleAbilitySystemComponent())
	{
		const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_Movement);
		BlockedTags.AddTag(GameplayTags.Player_Block_Ability_Offensive);
		BlockedTags.AddTag(GameplayTags.Player_Block_Interaction);
		if (IsShocked())
		{
			ElectricCastleAbilitySystemComponent->AddLooseGameplayTags(BlockedTags);
			ShockDebuffComponent->Activate();
		}
		else
		{
			ElectricCastleAbilitySystemComponent->RemoveLooseGameplayTags(BlockedTags);
			ShockDebuffComponent->Deactivate();
		}
		if (IsBurned())
		{
			BurnDebuffComponent->Activate();
		}
		else
		{
			BurnDebuffComponent->Deactivate();
		}
	}
}

void AElectricCastlePlayerCharacter::OnAbilitySystemReady_Implementation(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent)
{
	Super::OnAbilitySystemReady_Implementation(InAbilitySystemComponent);
	FishingComponent->OnFishingComponentCastAnimationDelegate.AddDynamic(this, &AElectricCastlePlayerCharacter::PlayFishingRodCastMontage);
	EquipmentComponent->OnEquipmentAnimationRequest.AddDynamic(this, &AElectricCastlePlayerCharacter::OnEquipmentAnimationRequest);
}

AElectricCastlePlayerState* AElectricCastlePlayerCharacter::GetElectricCastlePlayerState() const
{
	return Cast<AElectricCastlePlayerState>(GetPlayerState());
}

UElectricCastleAbilitySystemComponent* AElectricCastlePlayerCharacter::GetElectricCastleAbilitySystemComponent() const
{
	return Cast<UElectricCastleAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AElectricCastlePlayerCharacter::InitializeAbilityActorInfo()
{
	AElectricCastlePlayerState* ElectricCastlePlayerState = GetElectricCastlePlayerState();
	check(ElectricCastlePlayerState);
	ElectricCastlePlayerState->InitializeAbilityActorInfo();
	AbilitySystemComponent = ElectricCastlePlayerState->GetElectricCastleAbilitySystemComponent();
	EquipmentComponent->InitializeEquipment();
	FishingComponent->SetPlayerEquipmentComponent(EquipmentComponent);
	// Broadcast Ability System Setup
	OnAbilitySystemReady(GetElectricCastleAbilitySystemComponent());
}

void AElectricCastlePlayerCharacter::InitializePlayerControllerHUD(
	APlayerController* InPlayerController,
	AElectricCastlePlayerState* InPlayerState
)
{
	if (AElectricCastleHUD* HUD = Cast<AElectricCastleHUD>(InPlayerController->GetHUD()))
	{
		HUD->InitializeWidgets(
			this,
			InPlayerController,
			InPlayerState,
			AbilitySystemComponent,
			GetAttributeSet()
		);
	}
}

void AElectricCastlePlayerCharacter::OnCameraReturned()
{
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
}

void AElectricCastlePlayerCharacter::OnLevelLoaded()
{
	if (UElectricCastleLevelManager* LevelManager = UElectricCastleLevelManager::Get(this))
	{
		LevelManager->OnLevelTransitionComplete.RemoveAll(this);
	}
	if (APlayerController* PlayerController = Cast<AElectricCastlePlayerController>(GetController()))
	{
		InitializePlayerControllerHUD(PlayerController, GetElectricCastlePlayerState());
	}
	AddCharacterAbilities();
}

void AElectricCastlePlayerCharacter::OnFadeDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                                 const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UFadeInterface>())
	{
		IFadeInterface::Execute_FadeOut(OtherActor);
	}
}

void AElectricCastlePlayerCharacter::OnFadeDetectionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->Implements<UFadeInterface>())
	{
		IFadeInterface::Execute_FadeIn(OtherActor);
	}
}

int32 AElectricCastlePlayerCharacter::GetCharacterLevel_Implementation() const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to get character level"), *GetName());
	return ProgressionComponent->GetCharacterLevel();
}

TArray<FName> AElectricCastlePlayerCharacter::GetTargetTagsToIgnore_Implementation() const
{
	TArray<FName> IgnoreTags;
	IgnoreTags.Add(TAG_PLAYER);
	return IgnoreTags;
}

void AElectricCastlePlayerCharacter::Die()
{
	Super::Die();
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindLambda(
		[this]()
		{
			if (USaveGameManager* SaveGameManager = USaveGameManager::Get(this))
			{
				SaveGameManager->LoadMostRecentGame();
			}
		}
	);
	GetWorldTimerManager().SetTimer(DeathTimer, DeathTimerDelegate, DeathTime, false);
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

USkeletalMeshComponent* AElectricCastlePlayerCharacter::GetWeapon_Implementation() const
{
	return EquipmentComponent->GetWeapon();
}

void AElectricCastlePlayerCharacter::UpdateFacingTarget_Implementation(const FVector& InFacingTarget)
{
	FacingTarget = InFacingTarget;
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FName("FacingTarget"), FacingTarget);
}

int32 AElectricCastlePlayerCharacter::GetXP_Implementation()
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to get xp"), *GetName());
	return ProgressionComponent->GetXP();
}

void AElectricCastlePlayerCharacter::AddToXP_Implementation(int32 InXP)
{
	UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to add xp"), *GetName());
	return ProgressionComponent->AddToXP(InXP);
}

void AElectricCastlePlayerCharacter::LevelUp_Implementation()
{
	Multicast_LevelUpParticles();
}

void AElectricCastlePlayerCharacter::Multicast_LevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = CameraComponent->GetComponentLocation();
		const FVector EffectLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - EffectLocation).Rotation();
		LevelUpNiagaraComponent->Activate(true);
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
	}
	if (LevelUpSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LevelUpSound, GetActorLocation(), GetActorRotation());
	}
}

int32 AElectricCastlePlayerCharacter::FindLevelForXP_Implementation(const int32 InXP) const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to FindLevelByXP"), *GetName());
	return ProgressionComponent->FindLevelByXP(InXP);
}

FLevelUpRewards AElectricCastlePlayerCharacter::GetLevelUpRewards_Implementation(const int32 InLevel) const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to GetLevelUpRewards"), *GetName());
	return ProgressionComponent->GetLevelUpRewards(InLevel);
}

void AElectricCastlePlayerCharacter::ApplyLevelUpRewards_Implementation(
	const int32 LevelIncrement,
	const FLevelUpRewards& InLevelUpRewards
)
{
	UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to ApplyLevelUpRewards"), *GetName());
	ProgressionComponent->AddAttributePoints(InLevelUpRewards.AttributePoints);
	ProgressionComponent->AddSpellPoints(InLevelUpRewards.SpellPoints);
	ProgressionComponent->AddToLevel(LevelIncrement);
}

int32 AElectricCastlePlayerCharacter::GetAttributePoints_Implementation() const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to GetAttributePoints"), *GetName());
	return ProgressionComponent->GetAttributePoints();
}

int32 AElectricCastlePlayerCharacter::GetSpellPoints_Implementation() const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to GetSpellPoints"), *GetName());
	return ProgressionComponent->GetSpellPoints();
}

void AElectricCastlePlayerCharacter::SpendAttributePoints_Implementation(int32 SpentPoints)
{
	UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to AddAttributePoints"), *GetName());
	return ProgressionComponent->AddAttributePoints(-1 * SpentPoints);
}

void AElectricCastlePlayerCharacter::SpendSpellPoints_Implementation(const int32 SpentPoints)
{
	UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(ProgressionComponent, TEXT("[%s] No access to progression component while trying to AddSpellPoints"), *GetName());
	return ProgressionComponent->AddSpellPoints(-1 * SpentPoints);
}

void AElectricCastlePlayerCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if (AElectricCastlePlayerController* AuraPlayerController = Cast<AElectricCastlePlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
	}
}

void AElectricCastlePlayerCharacter::HideMagicCircle_Implementation()
{
	if (AElectricCastlePlayerController* AuraPlayerController = Cast<AElectricCastlePlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
	}
}

void AElectricCastlePlayerCharacter::MoveCameraToPoint_Implementation(
	const FVector& Destination,
	const FVector& Direction,
	UCurveFloat* AnimationCurve
)
{
	DesiredCameraForwardVector = CameraComponent->GetForwardVector();
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraComponent->MoveToLocation(Destination, Direction, AnimationCurve);
}

void AElectricCastlePlayerCharacter::MoveCameraToPointWithCallback(
	const FVector& Destination,
	const FVector& Direction,
	UCurveFloat* AnimationCurve,
	FOnCameraMoveFinishedSignature& OnCameraMoveFinishedSignature
)
{
	DesiredCameraForwardVector = CameraComponent->GetForwardVector();
	CameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraComponent->MoveToLocation(Destination, Direction, AnimationCurve, &OnCameraMoveFinishedSignature);
}

void AElectricCastlePlayerCharacter::ReturnCamera_Implementation(
	UCurveFloat* AnimationCurve
)
{
	CameraComponent->AttachToComponent(
		SpringArmComponent,
		FAttachmentTransformRules::KeepWorldTransform,
		USpringArmComponent::SocketName
	);
	CameraComponent->MoveToLocation(
		SpringArmComponent->GetSocketTransform(USpringArmComponent::SocketName).GetLocation(),
		DesiredCameraForwardVector,
		AnimationCurve,
		&OnCameraReturnDelegate
	);
}

UFishingComponent* AElectricCastlePlayerCharacter::GetFishingComponent_Implementation() const
{
	return FishingComponent;
}

void AElectricCastlePlayerCharacter::ShowFishingStatusEffect_Implementation(UNiagaraSystem* EffectSystem)
{
	if (EffectSystem)
	{
		FishingStatusEffectNiagaraComponent->SetAsset(EffectSystem);
		FishingStatusEffectNiagaraComponent->Activate(true);
	}
	else
	{
		FishingStatusEffectNiagaraComponent->SetAsset(nullptr);
		FishingStatusEffectNiagaraComponent->DeactivateImmediate();
	}
}

UPlayerFormChangeComponent* AElectricCastlePlayerCharacter::GetFormChangeComponent_Implementation() const
{
	return FormChangeComponent;
}
