// Copyright Alien Shores


#include "Character/ElectricCastleEnemyCharacter.h"

#include "MotionWarpingComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Actor/Effect/DissolveEffectComponent.h"
#include "AI/ElectricCastleAIController.h"
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
#include "Item/Component/LootSpawnComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"

AElectricCastleEnemyCharacter::AElectricCastleEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetRenderCustomDepth(false);
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	AbilitySystemComponent = CreateDefaultSubobject<UElectricCastleAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComponent->bShouldSave = false;
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));
	AttributeSet = CreateDefaultSubobject<UElectricCastleAttributeSet>(TEXT("Enemy Attributes"));
	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthWidget->SetupAttachment(GetRootComponent());
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnSpawnTimelineTick.BindDynamic(this, &AElectricCastleEnemyCharacter::SpawnAnimation_Tick);
	OnSpawnTimelineFinished.BindDynamic(this, &AElectricCastleEnemyCharacter::SpawnAnimation_Finalize);
	CharacterDissolveComponent = CreateDefaultSubobject<UDissolveEffectComponent>(TEXT("Character Dissolve Component"));
	WeaponDissolveComponent = CreateDefaultSubobject<UDissolveEffectComponent>(TEXT("Weapon Dissolve Component"));
	LootSpawnComponent = CreateDefaultSubobject<ULootSpawnComponent>(TEXT("Loot Spawn Component"));
	Tags.Add(TAG_ENEMY);
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
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No Attribute set defined! Cannot initialize delegates"), *GetName())
		return;
	}
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).
	                        AddLambda(
		                        [&](const FOnAttributeChangeData& Data)
		                        {
			                        OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Vital_Health, Data.OldValue, Data.NewValue));
		                        }
	                        );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).
	                        AddLambda(
		                        [&](const FOnAttributeChangeData& Data)
		                        {
			                        OnMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Primary_MaxHealth, Data.OldValue, Data.NewValue));
		                        }
	                        );
	OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(GameplayTags.Attributes_Vital_Health, AttributeSet->GetHealth()));
	OnMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(GameplayTags.Attributes_Primary_MaxHealth, AttributeSet->GetMaxHealth()));
}

void AElectricCastleEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAbilityActorInfo();
	InitializeDefaultAttributes();
	InitializeAttributeDelegates();
	InitializeStartupAbilities();
	GetMesh()->SetCustomDepthStencilValue(HighlightCustomDepthStencilValue);
	if (Weapon)
	{
		Weapon->SetCustomDepthStencilValue(HighlightCustomDepthStencilValue);
	}
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
	if (HasAuthority())
	{
		UElectricCastleAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	}
	OnAbilitySystemReady(Cast<UElectricCastleAbilitySystemComponent>(AbilitySystemComponent));
}

void AElectricCastleEnemyCharacter::OnStatusShockAdded()
{
	Super::OnStatusShockAdded();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), true);
	}
}

void AElectricCastleEnemyCharacter::OnStatusShockRemoved()
{
	Super::OnStatusShockRemoved();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsStunned"), false);
	}
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
		UElectricCastleAbilitySystemLibrary::GrantStartupAbilities(
			this,
			AbilitySystemComponent,
			CharacterClass,
			GetCharacterLevel(this)
		);
		AddCharacterAbilities();
	}
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
	AuraAIController = CastChecked<AElectricCastleAIController>(NewController);
	if (AuraAIController->GetBlackboardComponent() && BehaviorTree)
	{
		AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	}
	AuraAIController->RunBehaviorTree(BehaviorTree);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(
		FName("IsRanged"),
		CharacterClassUtils::IsRangedAttacker(CharacterClass)
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(FName("TargetingRange"), TargetingRange);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackRange_Min"),
		AttackRange - AttackRangeTolerance
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackRange_Max"),
		AttackRange + AttackRangeTolerance
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackWaitTime"),
		AttackWaitTime
	);
	AuraAIController->GetBlackboardComponent()->SetValueAsFloat(
		FName("AttackWaitDeviation"),
		AttackWaitDeviation
	);
}

void AElectricCastleEnemyCharacter::HighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(true);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
	}
}

void AElectricCastleEnemyCharacter::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}

int32 AElectricCastleEnemyCharacter::GetXPReward_Implementation() const
{
	return UElectricCastleAbilitySystemLibrary::GetXPReward(this, CharacterClass, Level);
}

USkeletalMeshComponent* AElectricCastleEnemyCharacter::GetWeapon_Implementation() const
{
	return Weapon;
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
}

void AElectricCastleEnemyCharacter::Die()
{
	Super::Die();
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsDead"), true);
	}
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

void AElectricCastleEnemyCharacter::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::OnHitReactTagChanged(CallbackTag, NewCount);
	if (AuraAIController && AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
}
