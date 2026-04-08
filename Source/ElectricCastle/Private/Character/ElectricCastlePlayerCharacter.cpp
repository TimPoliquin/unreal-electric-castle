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
#include "Camera/CameraComponent.h"
#include "Game/Save/SaveGameManager.h"
#include "Game/Subsystem/ElectricCastleAIDirectorGameInstanceSubsystem.h"
#include "Game/Subsystem/ElectricCastleLevelManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/Progression/ProgressionComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/BoxComponent.h"
#include "Interaction/FadeInterface.h"
#include "Player/Form/PlayerFormChangeComponent.h"
#include "GroomComponent.h"
#include "LiveLinkInstance.h"
#include "MetaHumanComponentUE.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"

#include "Actor/Attack/Component/AttackWindowManager.h"
#include "Actor/Block/Components/BlockController.h"
#include "Actor/MagicTether/TetherAbilityComponent.h"
#include "Actor/Mesh/SocketManagerComponent.h"
#include "Components/LODSyncComponent.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Game/Subsystem/PlayerManager.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Player/Aim/AimController.h"
#include "Player/Equipment/WeaponInterface.h"
#include "Player/Form/FormConfigTypes.h"
#include "Player/LockOn/LockOnController.h"

AElectricCastlePlayerCharacter::AElectricCastlePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(TAG_PLAYER);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 400.f, 0);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	ClothingMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Clothing Mesh"));
	ClothingMesh->SetupAttachment(GetMesh());
	FaceMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face Mesh"));
	FaceMesh->SetupAttachment(GetMesh());
	Groom_Beard = CreateDefaultSubobject<UGroomComponent>(TEXT("Beard"));
	Groom_Beard->AttachmentName = FString("FACIAL_C_FacialRoot");
	Groom_Beard->SetupAttachment(FaceMesh);
	Groom_Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Groom_Eyebrows->AttachmentName = FString("FACIAL_C_FacialRoot");
	Groom_Eyebrows->SetupAttachment(FaceMesh);
	Groom_Eyelashes = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyelashes"));
	Groom_Eyelashes->AttachmentName = FString("FACIAL_C_FacialRoot");
	Groom_Eyelashes->SetupAttachment(FaceMesh);
	Groom_Fuzz = CreateDefaultSubobject<UGroomComponent>(TEXT("Fuzz"));
	Groom_Fuzz->AttachmentName = FString("FACIAL_C_FacialRoot");
	Groom_Fuzz->SetupAttachment(FaceMesh);
	Groom_Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Groom_Hair->AttachmentName = FString("FACIAL_C_FacialRoot");
	Groom_Hair->SetupAttachment(FaceMesh);
	Groom_Moustache = CreateDefaultSubobject<UGroomComponent>(TEXT("Moustache"));
	Groom_Moustache->AttachmentName = FString("FACIAL_C_FacialRoot");
	Groom_Moustache->SetupAttachment(FaceMesh);
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
	SpringArmComponent->bDoCollisionTest = true;
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera Component"));
	PlayerCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = true;
	FadeDetectionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Fade Detection Component"));
	FadeDetectionComponent->SetupAttachment(SpringArmComponent);
	FadeDetectionComponent->SetBoxExtent(FVector(370.f, 26.f, 32.f));
	FadeDetectionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	FadeDetectionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	FadeDetectionComponent->SetGenerateOverlapEvents(true);
	EquipmentComponent = CreateDefaultSubobject<UPlayerEquipmentComponent>(TEXT("Equipment Component"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping"));
	FormChangeComponent = CreateDefaultSubobject<UPlayerFormChangeComponent>(TEXT("Form Change Component"));
	AimController = CreateDefaultSubobject<UAimController>(TEXT("Aim Controller"));
	LockOnController = CreateDefaultSubobject<ULockOnController>(TEXT("LockOn Controller"));
	BlockController = CreateDefaultSubobject<UBlockController>(TEXT("Block Controller"));
	TetherComponent = CreateDefaultSubobject<UTetherAbilityComponent>(TEXT("Tether Component"));
	AttackWindowManager = CreateDefaultSubobject<UAttackWindowManager>(TEXT("Attack Window Manager"));
	MetaHumanComponent = CreateDefaultSubobject<UMetaHumanComponentUE>(TEXT("MetaHuman"));
	LODSyncComponent = CreateDefaultSubobject<ULODSyncComponent>(TEXT("LODSync"));
	LODSyncComponent->NumLODs = 8;
	LODSyncComponent->ForcedLOD = 1;
	LODSyncComponent->MinLOD = 0;
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Body"), ESyncOption::Drive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Face"), ESyncOption::Drive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Torso"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Legs"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Feet"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Hair"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Eyebrows"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Eyelashes"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Mustache"), ESyncOption::Passive));
	LODSyncComponent->ComponentsToSync.Add(FComponentSync(FName("Beard"), ESyncOption::Passive));
	const FLODMappingData CustomLODMapping = CreateCustomLODMappingDefault();
	LODSyncComponent->CustomLODMapping.Add(FName("Body"), CustomLODMapping);
	LODSyncComponent->CustomLODMapping.Add(FName("Torso"), CustomLODMapping);
	LODSyncComponent->CustomLODMapping.Add(FName("Legs"), CustomLODMapping);
	LODSyncComponent->CustomLODMapping.Add(FName("Feet"), CustomLODMapping);
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	GetMesh()->SetIsReplicated(true);
	bReplicates = true;
	LeftHandConfig.SocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_LeftHand;
	RightHandConfig.SocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_RightHand;
	LeftHandConfig.SocketName = TEXT("Socket_Hand_Left");
	RightHandConfig.SocketName = TEXT("Socket_Hand_Right");
}

USceneComponent* AElectricCastlePlayerCharacter::GetMagicHandAttachComponent_Implementation(FName& AttachBoneName)
{
	AttachBoneName = LeftHandConfig.SocketName;
	return GetMesh();
}

UAttackWindowManager* AElectricCastlePlayerCharacter::GetAttackWindowManager_Implementation() const
{
	return AttackWindowManager;
}

void AElectricCastlePlayerCharacter::Falling()
{
	Super::Falling();
	if (!JumpEffectHandle.IsValid())
	{
		JumpEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(this, FElectricCastleGameplayTags::Get().Effect_State_Jumping);
	}
}

void AElectricCastlePlayerCharacter::Jump()
{
	Super::Jump();
	if (!JumpEffectHandle.IsValid())
	{
		JumpEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(this, FElectricCastleGameplayTags::Get().Effect_State_Jumping);
	}
}

void AElectricCastlePlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (JumpEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(this, JumpEffectHandle);
		JumpEffectHandle.Invalidate();
	}
}

void AElectricCastlePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UElectricCastleAIDirectorGameInstanceSubsystem* AIDirectorSubsystem =
		UElectricCastleAIDirectorGameInstanceSubsystem::Get(this))
	{
		AIDirectorSubsystem->RegisterActivePlayer(this);
	}
	FormChangeComponent->OnPlayerFormChange.AddUniqueDynamic(this, &AElectricCastlePlayerCharacter::OnFormChange);
	FaceMesh->OnAnimInitialized.AddUniqueDynamic(this, &AElectricCastlePlayerCharacter::PrepareLiveLinkSetup);
	FadeDetectionComponent->OnComponentBeginOverlap.AddUniqueDynamic(
		this,
		&AElectricCastlePlayerCharacter::OnFadeDetectionBeginOverlap
	);
	FadeDetectionComponent->OnComponentEndOverlap.AddUniqueDynamic(
		this,
		&AElectricCastlePlayerCharacter::OnFadeDetectionEndOverlap
	);
	LockOnController->OnLockOnTarget.AddUniqueDynamic(this, &AElectricCastlePlayerCharacter::HandleLockOnTarget);
	LockOnController->OnLockOnRelease.AddUniqueDynamic(this, &AElectricCastlePlayerCharacter::HandleLockOnRelease);
	GetCharacterMovement()->MaxWalkSpeed = StandardMoveSpeed;
	AimGameplayEffectHandle.Invalidate();
	JumpEffectHandle.Invalidate();
}

void AElectricCastlePlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	if (UElectricCastleAIDirectorGameInstanceSubsystem* AIDirectorSubsystem =
		UElectricCastleAIDirectorGameInstanceSubsystem::Get(this))
	{
		AIDirectorSubsystem->UnregisterActivePlayer(this);
	}
}

void AElectricCastlePlayerCharacter::HandleLockOnTarget_Implementation(const FLockOnTargetPayload& Payload)
{
}

void AElectricCastlePlayerCharacter::HandleLockOnRelease_Implementation()
{
}

void AElectricCastlePlayerCharacter::HandleTagChange_EffectMovementTurnInPlace_Implementation(FGameplayTag EffectMovementTurnInPlaceTag, int Count)
{
	if (const AElectricCastlePlayerController* PlayerController = Cast<AElectricCastlePlayerController>(GetController()))
	{
		if (Count > 0)
		{
			PlayerController->LockPlayerRotationToCamera();
		}
		else
		{
			PlayerController->FreePlayerRotationFromCamera();
		}
	}
}

void AElectricCastlePlayerCharacter::EnableMasterPose_Implementation(USkeletalMeshComponent* SkeletalMeshComponent)
{
	if (IsValid(SkeletalMeshComponent->GetSkeletalMeshAsset()))
	{
		if (UKismetSystemLibrary::IsValidClass(SkeletalMeshComponent->GetAnimClass()) ||
			UKismetSystemLibrary::IsValidClass(
				SkeletalMeshComponent->GetSkeletalMeshAsset()->GetPostProcessAnimBlueprint()
			))
		{
			SkeletalMeshComponent->SetLeaderPoseComponent(GetMesh());
		}
	}
}

void AElectricCastlePlayerCharacter::LiveLink_SetUpdateAnimationInEditor_Implementation()
{
	if (!LiveLinkConfig.bUseLiveLink)
	{
		return;
	}
	TArray<USceneComponent*> AllChildComponents;
	GetRootComponent()->GetChildrenComponents(true, AllChildComponents);
	for (UActorComponent* Component : AllChildComponents)
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component))
		{
			SkeletalMeshComponent->SetUpdateAnimationInEditor(true);
		}
	}
}

void AElectricCastlePlayerCharacter::LiveLink_SetupLiveLinkInstance_Implementation(UAnimInstance* AnimInstance)
{
	ULiveLinkInstance* LiveLinkInstance = Cast<ULiveLinkInstance>(AnimInstance);
	LiveLinkInstance->SetSubject(LiveLinkConfig.LiveLinkSubject);
	LiveLinkInstance->SetRetargetAsset(LiveLinkConfig.RetargetAsset);
}

void AElectricCastlePlayerCharacter::PrepareLiveLinkSetup_Implementation()
{
	if (LiveLinkConfig.bUseLiveLink)
	{
		if (LiveLinkConfig.bUseARKit)
		{
			LiveLink_SetupARKit(LiveLinkConfig.LiveLinkSubject, GetMesh()->GetAnimInstance());
		}
		else
		{
			LiveLink_SetupLiveLinkInstance(GetMesh()->GetAnimInstance());
		}
	}
}

void AElectricCastlePlayerCharacter::OnFormChange_Implementation(const FPlayerFormChangeEventPayload& Payload)
{
	FormChangeComponent->FormChange_PlayEffect(Payload);
	FormChangeComponent->FormChange_UpdateCharacterMesh(Payload);
	FormChangeComponent->FormChange_UpdateInputMappingContext(Payload);
	FormChangeComponent->FormChange_UpdateAttributes(Payload);
	FormChangeComponent->FormChange_UpdateAbilities(Payload);
}

void AElectricCastlePlayerCharacter::OnEquipmentAnimationRequest_Implementation(
	const FEquipmentDelegatePayload& Payload
)
{
	EquipmentComponent->UseEquipment(Payload.EquipmentSlot);
}

void AElectricCastlePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator RotationAdjustment = GetCharacterMovement()->bUseControllerDesiredRotation ? GetControlRotation() : GetActorRotation();
	RotationAdjustment.Pitch = 0.f;
	RotationAdjustment.Roll = 0.f;
	SetActorRotation(RotationAdjustment);
	UpdatePlayerPositionInMaterialParameterCollections();
}

void AElectricCastlePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AElectricCastlePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AElectricCastlePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SocketManagerComponent->RegisterSocket(GetMesh(), LeftHandConfig.SocketTag, LeftHandConfig.SocketName);
	SocketManagerComponent->RegisterSocket(GetMesh(), RightHandConfig.SocketTag, RightHandConfig.SocketName);
	AimController->OnAimStart.AddUniqueDynamic(this, &AElectricCastlePlayerCharacter::AimStart);
	AimController->OnAimEnd.AddUniqueDynamic(this, &AElectricCastlePlayerCharacter::AimEnd);
	BlockController->InitializeParryCamera(PlayerCameraComponent);
}

void AElectricCastlePlayerCharacter::FaceRotation(const FRotator NewControlRotation, const float DeltaTime)
{
	Super::FaceRotation(NewControlRotation, DeltaTime);
	if (bUseControllerRotationYaw)
	{
		Server_UpdateFacingRotation(GetActorRotation());
	}
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
	if (UPlayerManager* PlayerManager = UPlayerManager::Get(this))
	{
		PlayerManager->RegisterPlayer(Cast<AElectricCastlePlayerController>(NewController), this);
	}
	InitializeAbilityActorInfo();
	if (UElectricCastleLevelManager* LevelManager = UElectricCastleLevelManager::Get(this); LevelManager->IsTransitioningLevels())
	{
		LevelManager->OnLevelTransitionComplete.AddWeakLambda(
			this,
			[&]()
			{
				OnLevelLoaded();
			}
		);
	}
	else
	{
		OnLevelLoaded();
	}
}

void AElectricCastlePlayerCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	if (TetherComponent)
	{
		TetherComponent->ModifyInputMovementByTetherLimits(WorldDirection, ScaleValue, WorldDirection, ScaleValue);
	}
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void AElectricCastlePlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init ability actor info for the client
	InitializeAbilityActorInfo();
}

void AElectricCastlePlayerCharacter::OnRep_ActiveAbilityTag()
{
	Super::OnRep_ActiveAbilityTag();
}

void AElectricCastlePlayerCharacter::OnRep_StatusEffectTags()
{
	Super::OnRep_StatusEffectTags();
	if (UElectricCastleAbilitySystemComponent* ElectricCastleAbilitySystemComponent =
		GetElectricCastleAbilitySystemComponent())
	{
		const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_Movement);
		BlockedTags.AddTag(GameplayTags.Player_Block_Rotation);
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

void AElectricCastlePlayerCharacter::OnAbilitySystemReady_Implementation(
	UElectricCastleAbilitySystemComponent* InAbilitySystemComponent
)
{
	Super::OnAbilitySystemReady_Implementation(InAbilitySystemComponent);
	EquipmentComponent->OnEquipmentAnimationRequest.AddUniqueDynamic(
		this,
		&AElectricCastlePlayerCharacter::OnEquipmentAnimationRequest
	);
	InAbilitySystemComponent->RegisterGameplayTagEvent(FElectricCastleGameplayTags::Get().Effect_Movement_TurnInPlace, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AElectricCastlePlayerCharacter::HandleTagChange_EffectMovementTurnInPlace
	);
}

void AElectricCastlePlayerCharacter::OnEffectAdd_LightningDamage_Implementation()
{
	if (EquipmentComponent && EquipmentComponent->IsUsingWeapon())
	{
		IWeaponInterface::EffectAdd_Lightning(EquipmentComponent->GetWeapon());
	}
}

void AElectricCastlePlayerCharacter::OnEffectRemove_LightningDamage_Implementation()
{
	if (EquipmentComponent && EquipmentComponent->IsUsingWeapon())
	{
		IWeaponInterface::EffectRemove_Lightning(EquipmentComponent->GetWeapon());
	}
}

AElectricCastlePlayerState* AElectricCastlePlayerCharacter::GetElectricCastlePlayerState() const
{
	return Cast<AElectricCastlePlayerState>(GetPlayerState());
}

UElectricCastleAbilitySystemComponent* AElectricCastlePlayerCharacter::GetElectricCastleAbilitySystemComponent() const
{
	return Cast<UElectricCastleAbilitySystemComponent>(GetAbilitySystemComponent());
}

void AElectricCastlePlayerCharacter::AimStart_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = AimMoveSpeed;
	AimGameplayEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(
		this,
		FElectricCastleGameplayTags::Get().Effect_Movement_TurnInPlace
	);
}


void AElectricCastlePlayerCharacter::AimEnd_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = StandardMoveSpeed;
	UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(
		this,
		AimGameplayEffectHandle,
		false
	);
	AimGameplayEffectHandle.Invalidate();
}

void AElectricCastlePlayerCharacter::Multicast_SetTimeDilation_Implementation(const float Magnitude)
{
	CustomTimeDilation = Magnitude;
}

void AElectricCastlePlayerCharacter::InitializeAbilityActorInfo()
{
	AElectricCastlePlayerState* ElectricCastlePlayerState = GetElectricCastlePlayerState();
	check(ElectricCastlePlayerState);
	ElectricCastlePlayerState->InitializeAbilityActorInfo();
	AbilitySystemComponent = ElectricCastlePlayerState->GetElectricCastleAbilitySystemComponent();
	EquipmentComponent->InitializeEquipment();
	// Broadcast Ability System Setup
	OnAbilitySystemReady(GetElectricCastleAbilitySystemComponent());
}


void AElectricCastlePlayerCharacter::Server_UpdateFacingRotation_Implementation(FRotator NewControlRotation)
{
	SetActorRotation(NewControlRotation);
}

void AElectricCastlePlayerCharacter::OnLevelLoaded()
{
	if (UElectricCastleLevelManager* LevelManager = UElectricCastleLevelManager::Get(this))
	{
		LevelManager->OnLevelTransitionComplete.RemoveAll(this);
	}
	AddCharacterAbilities();
}

void AElectricCastlePlayerCharacter::OnFadeDetectionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor->Implements<UFadeInterface>())
	{
		IFadeInterface::Execute_FadeOut(OtherActor);
	}
}

void AElectricCastlePlayerCharacter::OnFadeDetectionEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (OtherActor->Implements<UFadeInterface>())
	{
		IFadeInterface::Execute_FadeIn(OtherActor);
	}
}

void AElectricCastlePlayerCharacter::UpdatePlayerPositionInMaterialParameterCollections()
{
	UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this);
	if (!GameDataSubsystem || !AbilitySystemComponent)
	{
		return;
	}
	if (AbilitySystemComponent->HasMatchingGameplayTag(
		FElectricCastleGameplayTags::Get().Effect_State_PsychedelicVision
	))
	{
		// Get the Material Parameter Collection Instance
		UMaterialParameterCollectionInstance* CollectionInstance = GetWorld()->GetParameterCollectionInstance(
			GameDataSubsystem->GetPsychedelicVisionParameters()
		);
		if (!CollectionInstance)
		{
			return;
		}
		// Update the vector parameter value
		CollectionInstance->SetVectorParameterValue(FName("PlayerPosition"), GetActorLocation());
	}
}

FLODMappingData AElectricCastlePlayerCharacter::CreateCustomLODMappingDefault() const
{
	FLODMappingData CustomLODMapping;
	CustomLODMapping.Mapping.Add(0);
	CustomLODMapping.Mapping.Add(0);
	CustomLODMapping.Mapping.Add(1);
	CustomLODMapping.Mapping.Add(1);
	CustomLODMapping.Mapping.Add(2);
	CustomLODMapping.Mapping.Add(2);
	CustomLODMapping.Mapping.Add(3);
	CustomLODMapping.Mapping.Add(3);
	return CustomLODMapping;
}

int32 AElectricCastlePlayerCharacter::GetCharacterLevel_Implementation() const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(
		ProgressionComponent,
		TEXT("[%s] No access to progression component while trying to get character level"),
		*GetName()
	);
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
	if (PlayerCameraComponent)
	{
		PlayerCameraComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}

AActor* AElectricCastlePlayerCharacter::GetWeapon_Implementation() const
{
	return EquipmentComponent->GetWeapon();
}

void AElectricCastlePlayerCharacter::UpdateFacingTarget_Implementation(const FVector& InFacingTarget)
{
	FacingTarget = InFacingTarget;
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FName("FacingTarget"), FacingTarget);
}

void AElectricCastlePlayerCharacter::ClearFacingTarget_Implementation()
{
	FacingTarget = FVector::ZeroVector;
	MotionWarpingComponent->RemoveWarpTarget(FName("FacingTarget"));
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
		const FVector CameraLocation = PlayerCameraComponent->GetComponentLocation();
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
	checkf(
		ProgressionComponent,
		TEXT("[%s] No access to progression component while trying to FindLevelByXP"),
		*GetName()
	);
	return ProgressionComponent->FindLevelByXP(InXP);
}

FLevelUpRewards AElectricCastlePlayerCharacter::GetLevelUpRewards_Implementation(const int32 InLevel) const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(
		ProgressionComponent,
		TEXT("[%s] No access to progression component while trying to GetLevelUpRewards"),
		*GetName()
	);
	return ProgressionComponent->GetLevelUpRewards(InLevel);
}

void AElectricCastlePlayerCharacter::ApplyLevelUpRewards_Implementation(
	const int32 LevelIncrement,
	const FLevelUpRewards& InLevelUpRewards
)
{
	UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(
		ProgressionComponent,
		TEXT("[%s] No access to progression component while trying to ApplyLevelUpRewards"),
		*GetName()
	);
	ProgressionComponent->AddSpellPoints(InLevelUpRewards.SpellPoints);
	ProgressionComponent->AddToLevel(LevelIncrement);
}

int32 AElectricCastlePlayerCharacter::GetSpellPoints_Implementation() const
{
	const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(
		ProgressionComponent,
		TEXT("[%s] No access to progression component while trying to GetSpellPoints"),
		*GetName()
	);
	return ProgressionComponent->GetSpellPoints();
}

void AElectricCastlePlayerCharacter::SpendSpellPoints_Implementation(const int32 SpentPoints)
{
	UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this);
	checkf(
		ProgressionComponent,
		TEXT("[%s] No access to progression component while trying to AddSpellPoints"),
		*GetName()
	);
	return ProgressionComponent->AddSpellPoints(-1 * SpentPoints);
}

void AElectricCastlePlayerCharacter::SetFormMeshes_Implementation(const FFormMeshConfig& FormMeshConfig)
{
	FormMeshConfig.Body.SetToComponent(GetMesh());
	FormMeshConfig.Face.SetToComponent(FaceMesh);
	FormMeshConfig.Clothing.SetToComponent(ClothingMesh);
	TryApplyGroomAssets(FormMeshConfig);
}

void AElectricCastlePlayerCharacter::TryApplyGroomAssets_Implementation(const FFormMeshConfig& FormMeshConfig)
{
	if (FaceMesh && FaceMesh->GetSkeletalMeshAsset() && FaceMesh->IsRegistered())
	{
		SetGroomAssets(FormMeshConfig);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			[this, FormMeshConfig]()
			{
				TryApplyGroomAssets(FormMeshConfig);
			}
		);
	}
}

void AElectricCastlePlayerCharacter::SetGroomAssets_Implementation(const FFormMeshConfig& FormMeshConfig)
{
	FormMeshConfig.Beard.SetToComponent(Groom_Beard);
	FormMeshConfig.Eyebrows.SetToComponent(Groom_Eyebrows);
	FormMeshConfig.Eyelashes.SetToComponent(Groom_Eyelashes);
	FormMeshConfig.Fuzz.SetToComponent(Groom_Fuzz);
	FormMeshConfig.Hair.SetToComponent(Groom_Hair);
	FormMeshConfig.Moustache.SetToComponent(Groom_Moustache);
}


UPlayerFormChangeComponent* AElectricCastlePlayerCharacter::GetFormChangeComponent_Implementation() const
{
	return FormChangeComponent;
}

void AElectricCastlePlayerCharacter::SetAnimInstanceClass(const TSubclassOf<UAnimInstance> InAnimInstance)
{
	GetMesh()->SetAnimInstanceClass(InAnimInstance);
}

UAimController* AElectricCastlePlayerCharacter::GetAimController_Implementation() const
{
	return AimController;
}

ULockOnController* AElectricCastlePlayerCharacter::GetLockOnController_Implementation() const
{
	return LockOnController;
}

void AElectricCastlePlayerCharacter::Construction_SetupMetaHuman_Implementation()
{
	TArray<USceneComponent*> BodyChildComponents;
	GetMesh()->GetChildrenComponents(false, BodyChildComponents);
	for (USceneComponent* ChildComponent : BodyChildComponents)
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(ChildComponent))
		{
			if (SkeletalMeshComponent != FaceMesh)
			{
				EnableMasterPose(SkeletalMeshComponent);
			}
		}
	}
	if (LiveLinkConfig.bUseLiveLink)
	{
		if (LiveLinkConfig.bUseARKit)
		{
			GetMesh()->SetAnimInstanceClass(LiveLinkConfig.BodyAnimInstanceClass);
		}
		else
		{
			GetMesh()->SetAnimInstanceClass(ULiveLinkInstance::StaticClass());
		}
	}
}
