// Copyright Alien Shores

#include "Character/ElectricCastleEnemyCharacter.h"

#include "DrawDebugHelpers.h"
#include "MotionWarpingComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Actor/Effect/DissolveEffectComponent.h"
#include "AI/ElectricCastleAIController.h"
#include "AI/Alert/AIAlertComponent.h"
#include "AI/Engagement/AIEngagementController.h"
#include "AI/Perception/AIPerceptionManager.h"
#include "AI/Targeting/AITargetingComponent.h"
#include "Actor/Attack/Component/AttackWindowManager.h"
#include "Actor/Highlight/HighlightComponent.h"
#include "Actor/Patrol/PatrolComponent.h"
#include "Actor/Significance/Component/ActorSignificanceComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "Actor/Spawn/TrackableInterface.h"
#include "Animation/AnimInstance.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Item/Component/LootSpawnComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Damage.h"
#include "Sound/SoundBase.h"
#include "Tags/ElectricCastleGameplayTags.h"

AElectricCastleEnemyCharacter::AElectricCastleEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	TeamAffiliation = ETeamAffiliation::Enemy;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetRenderCustomDepth(false);
	GetMesh()->bEnableUpdateRateOptimizations = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	AbilitySystemComponent = CreateDefaultSubobject<UElectricCastleAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponent->bShouldSave = false;
	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSourceComponent"));
	PerceptionManager = CreateDefaultSubobject<UAIPerceptionManager>(TEXT("PerceptionManager"));
	AIAlertComponent = CreateDefaultSubobject<UAIAlertComponent>("AI Alert Component");
	AIEngagementController = CreateDefaultSubobject<UAIEngagementController>("AI Engagement Controller");
	AITargeting = CreateDefaultSubobject<UAITargetingComponent>("AI Targeting Component");
	PatrolComponent = CreateDefaultSubobject<UPatrolComponent>("Patrol Component");
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
	AttributeSet = CreateDefaultSubobject<UElectricCastleAttributeSet>(TEXT("Enemy Attributes"));
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidget->SetupAttachment(GetRootComponent());
	WeaponChildActorComponent = CreateDefaultSubobject<UChildActorComponent>("Weapon");
	WeaponChildActorComponent->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	OnSpawnTimelineTick.BindDynamic(this, &AElectricCastleEnemyCharacter::SpawnAnimation_Tick);
	OnSpawnTimelineFinished.BindDynamic(this, &AElectricCastleEnemyCharacter::SpawnAnimation_Finalize);
	CharacterDissolveComponent = CreateDefaultSubobject<UDissolveEffectComponent>(TEXT("Character Dissolve Component"));
	LootSpawnComponent = CreateDefaultSubobject<ULootSpawnComponent>(TEXT("Loot Spawn Component"));
	AttackWindowManager = CreateDefaultSubobject<UAttackWindowManager>(TEXT("Attack Window Manager"));
	SignificanceComponent = CreateDefaultSubobject<UActorSignificanceComponent>(TEXT("Significance Component"));
	SignificanceComponent->SetSignificanceTag(FElectricCastleGameplayTags::Get().Significance_Category_Enemy);
	Tags.Add(TAG_ENEMY);
	HighlightComponent->SetHighlightType(EHighlightType::Enemy);
}

void AElectricCastleEnemyCharacter::InitializeAttributeDelegates()
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	if (UAuraUserWidget* HealthBarWidget = Cast<UAuraUserWidget>(HealthWidget->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(this);
	}
	if (!AttributeSet)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] No Attribute set defined! Cannot initialize delegates"),
			*GetName()
		)
		return;
	}
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).
	                        AddLambda(
		                        [&](const FOnAttributeChangeData& Data)
		                        {
			                        OnHealthChanged.Broadcast(
				                        FFloatAttributeChangedPayload(
					                        GameplayTags.Attributes_Vital_Health,
					                        Data.OldValue,
					                        Data.NewValue
				                        )
			                        );
		                        }
	                        );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).
	                        AddLambda(
		                        [&](const FOnAttributeChangeData& Data)
		                        {
			                        OnMaxHealthChanged.Broadcast(
				                        FFloatAttributeChangedPayload(
					                        GameplayTags.Attributes_Primary_MaxHealth,
					                        Data.OldValue,
					                        Data.NewValue
				                        )
			                        );
		                        }
	                        );
	OnHealthChanged.Broadcast(
		FFloatAttributeChangedPayload::CreateBroadcastPayload(
			GameplayTags.Attributes_Vital_Health,
			AttributeSet->GetHealth()
		)
	);
	OnMaxHealthChanged.Broadcast(
		FFloatAttributeChangedPayload::CreateBroadcastPayload(
			GameplayTags.Attributes_Primary_MaxHealth,
			AttributeSet->GetMaxHealth()
		)
	);
}

void AElectricCastleEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAbilityActorInfo();
	InitializeDefaultAttributes();
	InitializeAttributeDelegates();
	InitializeStartupAbilities();
	if (bShouldAnimateSpawn)
	{
		SpawnAnimation();
	}
}

void AElectricCastleEnemyCharacter::InitializeAbilityActorInfo()
{
	Super::InitializeAbilityActorInfo();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AElectricCastleEnemyCharacter::InitializeDefaultAttributes()
{
	if (HasAuthority() && AttributeSet)
	{
		AttributeSet->InitializeDefaultAttributes(GetCharacterLevel(this));
	}
	OnAbilitySystemReady(Cast<UElectricCastleAbilitySystemComponent>(AbilitySystemComponent));
}

void AElectricCastleEnemyCharacter::HandleTakeDamage_HitFlash_Implementation()
{
	// do nothing natively
}

void AElectricCastleEnemyCharacter::InitializeHitFlashMaterial_Implementation()
{
	for (int32 Idx = 0; Idx < GetMesh()->GetNumMaterials(); Idx++)
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(Idx);
		DynamicMaterialInstance->SetScalarParameterValue(FName("HitFlash"), 0);
		DynamicMeshMaterials.Add(DynamicMaterialInstance);
	}
}

void AElectricCastleEnemyCharacter::HandleTakeDamage_Knockback_Implementation(const FVector& KnockbackVector)
{
	LaunchCharacter(KnockbackVector, true, true);
}

void AElectricCastleEnemyCharacter::HandleTakeDamage_ImpactEffect_Implementation(
	const FGameplayTag& DamageType,
	const FVector& ImpactLocation,
	const bool bIsBlocked,
	const bool bIsCritical
)
{
	USoundBase* ImpactSound = nullptr;
	UNiagaraSystem* ImpactEffect = nullptr;
	UAnimMontage* ImpactMontage = nullptr;
	if (bIsBlocked)
	{
		if (BlockImpactConfigByDamageType.Contains(DamageType))
		{
			ImpactSound = BlockImpactConfigByDamageType[DamageType].ImpactSound;
			ImpactEffect = BlockImpactConfigByDamageType[DamageType].ImpactEffect;
			ImpactMontage = BlockImpactConfigByDamageType[DamageType].ImpactMontage;
		}
		if (!IsValid(ImpactSound))
		{
			ImpactSound = DefaultBlockImpactConfig.ImpactSound;
		}
		if (!IsValid(ImpactEffect))
		{
			ImpactEffect = DefaultBlockImpactConfig.ImpactEffect;
		}
		if (!IsValid(ImpactMontage))
		{
			ImpactMontage = DefaultBlockImpactConfig.ImpactMontage;
		}
	}
	else
	{
		if (ImpactConfigByDamageType.Contains(DamageType))
		{
			ImpactSound = ImpactConfigByDamageType[DamageType].ImpactSound;
			ImpactEffect = ImpactConfigByDamageType[DamageType].ImpactEffect;
			ImpactMontage = ImpactConfigByDamageType[DamageType].ImpactMontage;
		}
		if (!IsValid(ImpactSound))
		{
			ImpactSound = DefaultImpactConfig.ImpactSound;
		}
		if (!IsValid(ImpactEffect))
		{
			ImpactEffect = DefaultImpactConfig.ImpactEffect;
		}
		if (!IsValid(ImpactMontage))
		{
			ImpactMontage = DefaultImpactConfig.ImpactMontage;
		}
		if (!bIsCritical)
		{
			// do not play montage unless it is a critical hit
			ImpactMontage = nullptr;
		}
	}
	if (IsValid(ImpactSound))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("ImpactSound: %s"), *ImpactSound->GetName());
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactLocation);
	}
	if (IsValid(ImpactEffect))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("ImpactEffect: %s"), *ImpactEffect->GetName());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, ImpactLocation);
	}
	if (IsValid(ImpactMontage))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("Impact Montage: %s"), *ImpactMontage->GetName());
		PlayAnimMontage(ImpactMontage);
	}
}

UAnimMontage* AElectricCastleEnemyCharacter::GetStaggerMontage_Implementation() const
{
	return DefaultImpactConfig.ImpactMontage;
}

void AElectricCastleEnemyCharacter::HandleGameDataLoaded_Implementation()
{
	// Empty for now
}

void AElectricCastleEnemyCharacter::RegisterSockets_Implementation(USocketManagerComponent* InSocketManagerComponent)
{
	// nothing here for now
}

void AElectricCastleEnemyCharacter::SpawnLoot_Implementation()
{
	if (LootSpawnComponent)
	{
		LootSpawnComponent->GrantLoot();
	}
}

void AElectricCastleEnemyCharacter::SpawnAnimation_Implementation()
{
	SpawnAnimation_Initialize();
	SpawnTimelineComponent = NewObject<UTimelineComponent>(this, UTimelineComponent::StaticClass());
	SpawnTimelineComponent->SetComponentTickEnabled(true);
	SpawnTimelineComponent->RegisterComponent();
	SpawnTimelineComponent->AddInterpFloat(SpawnAnimationCurve, OnSpawnTimelineTick);
	SpawnTimelineComponent->SetTimelineFinishedFunc(OnSpawnTimelineFinished);
	SpawnTimelineComponent->PlayFromStart();
}

void AElectricCastleEnemyCharacter::SpawnAnimation_Tick_Implementation(const float Alpha)
{
	GetMesh()->SetRelativeScale3D(FVector(Alpha, Alpha, Alpha));
}

void AElectricCastleEnemyCharacter::SpawnAnimation_Initialize_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetCapsuleComponent()->SetEnableGravity(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	GetMesh()->SetEnableGravity(false);
	GetCharacterMovement()->GravityScale = 0.f;
	AddActorLocalOffset(FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
}

void AElectricCastleEnemyCharacter::SpawnAnimation_Finalize_Implementation()
{
	GetMesh()->SetRelativeScale3D(FVector(1, 1, 1));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetMesh()->SetEnableGravity(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GetCapsuleComponent()->SetEnableGravity(true);
	GetCharacterMovement()->GravityScale = 1.f;
	SpawnTimelineComponent->DestroyComponent();
}

void AElectricCastleEnemyCharacter::InitializeStartupAbilities()
{
	if (HasAuthority())
	{
		AddCharacterAbilities();
	}
}


void AElectricCastleEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectricCastleEnemyCharacter::SetVisible_Implementation(const bool bInVisible)
{
	GetMesh()->SetVisibility(bInVisible, true);
	if (bInVisible)
	{
		GetCapsuleComponent()->Activate();
	}
	else
	{
		GetCapsuleComponent()->Deactivate();
	}
}

int32 AElectricCastleEnemyCharacter::GetXPReward_Implementation() const
{
	return UElectricCastleAbilitySystemLibrary::GetXPReward(this, CharacterClass, Level);
}

AActor* AElectricCastleEnemyCharacter::GetWeapon_Implementation() const
{
	return WeaponChildActorComponent->GetChildActor();
}

TArray<FName> AElectricCastleEnemyCharacter::GetTargetTagsToIgnore_Implementation() const
{
	TArray<FName> IgnoreTargetTags;
	IgnoreTargetTags.Add(TAG_ENEMY);
	return IgnoreTargetTags;
}

void AElectricCastleEnemyCharacter::UpdateFacingTarget_Implementation(const FVector& FacingTarget)
{
	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(FName("FacingTarget"), FacingTarget);
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Log, TEXT("[%s] Updating facing target to: %s"), *GetName(), *FacingTarget.ToString())
		DrawDebugSphere(GetWorld(), FacingTarget, 100.f, 12, FColor::Red, false, 2.f, 0, 1.f);
	}
}

void AElectricCastleEnemyCharacter::HandleTakeDamage(const float IncomingDamage, const FEffectProperties& Props)
{
	Super::HandleTakeDamage(IncomingDamage, Props);
	if (const FVector KnockbackForce = UElectricCastleAbilitySystemLibrary::GetKnockbackVector(Props.EffectContextHandle);
		!KnockbackForce.IsNearlyZero(1.f))
	{
		HandleTakeDamage_Knockback(KnockbackForce);
	}
	HandleTakeDamage_ImpactEffect(
		UElectricCastleAbilitySystemLibrary::GetDamageTypeTag(Props.EffectContextHandle),
		UElectricCastleAbilitySystemLibrary::GetRadialDamageOrigin(Props.EffectContextHandle),
		UElectricCastleAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle),
		UElectricCastleAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle)
	);
	HandleTakeDamage_HitFlash();
}

void AElectricCastleEnemyCharacter::Die()
{
	Super::Die();
	if (HealthWidget)
	{
		HealthWidget->SetVisibility(false);
	}
	OnTrackableStopTracking.Broadcast(this);
	SetLifeSpan(LifeSpan);
	SpawnLoot();
}

AActor* AElectricCastleEnemyCharacter::GetCombatTarget_Implementation() const
{
	if (!AITargeting)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] AI Engagement Controller is not initialized!"), *GetName())
		return nullptr;
	}
	return AITargeting->GetCurrentTarget();
}

FOnTrackableStopTrackingSignature& AElectricCastleEnemyCharacter::GetStopTrackingDelegate()
{
	return OnTrackableStopTracking;
}

UAttackWindowManager* AElectricCastleEnemyCharacter::GetAttackWindowManager_Implementation() const
{
	return AttackWindowManager;
}

UBehaviorTree* AElectricCastleEnemyCharacter::GetBehaviorTree_Implementation() const
{
	return BehaviorTree;
}

bool AElectricCastleEnemyCharacter::ShouldAutoRunBehaviorTree_Implementation() const
{
	return true;
}

void AElectricCastleEnemyCharacter::EnterSignificance_FullySignificant_Implementation()
{
	constexpr float FullTickRate = 0.f;
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();
	AIAlertComponent->Activate();
	AIAlertComponent->SetComponentTickInterval(FullTickRate);
	PatrolComponent->Activate();
	AIEngagementController->Activate();
	AIEngagementController->SetComponentTickInterval(FullTickRate);
	GetCharacterMovement()->SetComponentTickInterval(FullTickRate);
	GetMesh()->bPauseAnims = false;
	GetMesh()->SetComponentTickEnabled(true);
	GetMesh()->PrimaryComponentTick.TickInterval = FullTickRate;
	SetActorTickEnabled(true);
	SetActorTickInterval(FullTickRate);
	if (SignificanceEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(this, SignificanceEffectHandle, true, true);
	}
	SignificanceEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(this, FElectricCastleGameplayTags::Get().Significance_Level_FullySignificant);
}

void AElectricCastleEnemyCharacter::EnterSignificance_PartiallySignificant_Implementation()
{
	constexpr float ReducedTickRate = 1.f / 15.f;
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();
	AIAlertComponent->Activate();
	AIAlertComponent->SetComponentTickInterval(ReducedTickRate);
	PatrolComponent->Activate();
	AIEngagementController->Activate();
	AIEngagementController->SetComponentTickInterval(0); // let engagement controller run at full tick
	GetCharacterMovement()->SetComponentTickInterval(ReducedTickRate);
	GetMesh()->bPauseAnims = false;
	GetMesh()->SetComponentTickEnabled(true);
	GetMesh()->PrimaryComponentTick.TickInterval = ReducedTickRate;
	SetActorTickEnabled(true);
	SetActorTickInterval(ReducedTickRate);
	if (SignificanceEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(this, SignificanceEffectHandle, true, true);
	}
	SignificanceEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(this, FElectricCastleGameplayTags::Get().Significance_Level_PartiallySignificant);
}

void AElectricCastleEnemyCharacter::EnterSignificance_Insignificant_Implementation()
{
	AIPerceptionStimuliSource->UnregisterFromPerceptionSystem();
	AIAlertComponent->Deactivate();
	PatrolComponent->Deactivate();
	AIEngagementController->Deactivate();
	AIEngagementController->SetComponentTickEnabled(false);
	GetCharacterMovement()->StopMovementImmediately();
	GetMesh()->GetAnimInstance()->StopAllMontages(false);
	GetMesh()->bPauseAnims = true;
	SetActorTickEnabled(false);
	GetMesh()->SetComponentTickEnabled(false);
	if (SignificanceEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(this, SignificanceEffectHandle, true, true);
	}
	SignificanceEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(this, FElectricCastleGameplayTags::Get().Significance_Level_Insignificant);
}

void AElectricCastleEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RegisterSockets(SocketManagerComponent);
	InitializeHitFlashMaterial();
}

void AElectricCastleEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (UAIPerceptionComponent* PerceptionComponent = IAIPerceptionActor::GetAIPerceptionComponent(NewController))
	{
		PerceptionManager->InitializePerception(PerceptionComponent);
		AIAlertComponent->InitializePerceptionManager(PerceptionManager);
	}
}

float AElectricCastleEnemyCharacter::TakeDamage(float DamageAmount, const struct FDamageEvent& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Damage > 0)
	{
		UAISense_Damage::ReportDamageEvent(this, this, EventInstigator, DamageAmount, GetActorLocation(), GetActorLocation(), TEXT("Damage"));
	}
	return Damage;
}
