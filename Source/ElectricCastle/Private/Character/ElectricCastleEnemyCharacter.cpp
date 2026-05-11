// Copyright Alien Shores


#include "Character/ElectricCastleEnemyCharacter.h"

#include "MotionWarpingComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Actor/Effect/DissolveEffectComponent.h"
#include "AI/ElectricCastleAIController.h"
#include "AI/Alert/AIAlertComponent.h"
#include "AI/Engagement/AIEngagementController.h"
#include "Actor/Attack/Component/AttackWindowManager.h"
#include "Actor/Highlight/HighlightComponent.h"
#include "Actor/Patrol/PatrolComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "Actor/Spawn/TrackableInterface.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Item/Component/LootSpawnComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

AElectricCastleEnemyCharacter::AElectricCastleEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	TeamAffiliation = ETeamAffiliation::Enemy;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetRenderCustomDepth(false);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	AbilitySystemComponent = CreateDefaultSubobject<UElectricCastleAbilitySystemComponent>(
		TEXT("AbilitySystemComponent")
	);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponent->bShouldSave = false;
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	AIAlertComponent = CreateDefaultSubobject<UAIAlertComponent>("AI Alert Component");
	AIEngagementController = CreateDefaultSubobject<UAIEngagementController>("AI Engagement Controller");
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

void AElectricCastleEnemyCharacter::OnStatusShockAdded()
{
	Super::OnStatusShockAdded();
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), true);
	}
}

void AElectricCastleEnemyCharacter::OnStatusShockRemoved()
{
	Super::OnStatusShockRemoved();
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), false);
	}
}

void AElectricCastleEnemyCharacter::OnStatusStaggeredAdded_Implementation()
{
	Super::OnStatusStaggeredAdded_Implementation();
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStaggered"), true);
		AIController->StopMovement();
	}
	GetMovementComponent()->StopMovementImmediately();
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.f);
	}
	LaunchCharacter(GetActorForwardVector() * -1 * GetStaggerLaunchForce() + FVector::UpVector * GetStaggerLaunchUpwardForce(), true, true);
}

void AElectricCastleEnemyCharacter::OnStatusStaggeredRemoved_Implementation()
{
	Super::OnStatusStaggeredRemoved_Implementation();
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStaggered"), false);
	}
}

float AElectricCastleEnemyCharacter::GetStaggerLaunchUpwardForce_Implementation() const
{
	return StaggerLaunchUpwardForce;
}

float AElectricCastleEnemyCharacter::GetStaggerLaunchForce_Implementation() const
{
	return StaggerLaunchForce;
}

UAnimMontage* AElectricCastleEnemyCharacter::GetStaggerMontage_Implementation() const
{
	return Execute_GetHitReactMontage(this, FElectricCastleGameplayTags::Get().Effect_HitReact);
}

void AElectricCastleEnemyCharacter::HandleGameDataLoaded_Implementation()
{
	InitializeAIController();
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

ECharacterClass AElectricCastleEnemyCharacter::GetCharacterClass() const
{
	return CharacterClass;
}

void AElectricCastleEnemyCharacter::SetCharacterClass(const ECharacterClass InCharacterClass)
{
	this->CharacterClass = InCharacterClass;
}

void AElectricCastleEnemyCharacter::InitializeStartupAbilities()
{
	if (HasAuthority())
	{
		AddCharacterAbilities();
	}
}

void AElectricCastleEnemyCharacter::InitializeAIController()
{
	if (AIController->GetBlackboardComponent() && BehaviorTree)
	{
		AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
	AIController->RunBehaviorTree(BehaviorTree);
	AIController->InitializeDependencies();
	AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AIController->GetBlackboardComponent()->SetValueAsBool(
		FName("IsRanged"),
		CharacterClassUtils::IsRangedAttacker(CharacterClass)
	);
	AIController->GetBlackboardComponent()->SetValueAsFloat(FName("TargetingRange"), TargetingRange);
	AIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackRange_Min"),
		AttackRange - AttackRangeTolerance
	);
	AIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackRange_Max"),
		AttackRange + AttackRangeTolerance
	);
	AIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackWaitTime"),
		AttackWaitTime
	);
	AIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackWaitDeviation"),
		AttackWaitDeviation
	);
}

void AElectricCastleEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AElectricCastleEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!HasAuthority())
	{
		return;
	}
	AIController = CastChecked<AElectricCastleAIController>(NewController);
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this))
	{
		if (GameDataSubsystem->IsGameDataLoaded())
		{
			InitializeAIController();
		}
		else
		{
			GameDataSubsystem->OnGameDataLoaded.AddUniqueDynamic(this, &AElectricCastleEnemyCharacter::HandleGameDataLoaded);
		}
	}
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

FOnTrackableStopTrackingSignature& AElectricCastleEnemyCharacter::GetStopTrackingDelegate()
{
	return OnTrackableStopTracking;
}

UAttackWindowManager* AElectricCastleEnemyCharacter::GetAttackWindowManager_Implementation() const
{
	return AttackWindowManager;
}

void AElectricCastleEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RegisterSockets(SocketManagerComponent);
}

void AElectricCastleEnemyCharacter::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::OnHitReactTagChanged(CallbackTag, NewCount);
	if (AIController && AIController->GetBlackboardComponent())
	{
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}
