// Copyright Alien Shores


#include "AI/ElectricCastleAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AI/Alert/AIAlertActor.h"
#include "AI/Alert/AIAlertComponent.h"
#include "AI/Behavior/AIBehaviorTreeActor.h"
#include "Actor/Cinematic/CinematicHandlerComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Blackboard/ElectricCastleBlackboardKeys.h"
#include "AI/Engagement/AIEngagementActor.h"
#include "AI/Engagement/AIEngagementController.h"
#include "Actor/Patrol/PatrolComponent.h"
#include "Actor/Patrol/PatrollingActor.h"
#include "Actor/Significance/SignificanceTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Character/ElectricCastleCharacter.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Utils/RandomRange.h"


// Sets default values
AElectricCastleAIController::AElectricCastleAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bStopAILogicOnUnposses = true;
	CinematicHandlerComponent = CreateDefaultSubobject<UCinematicHandlerComponent>(TEXT("CinematicHandlerComponent"));
}

void AElectricCastleAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (!IsValid(InPawn))
	{
		return;
	}
	if (InPawn->Implements<UGenericTeamAgentInterface>())
	{
		SetGenericTeamId(Cast<IGenericTeamAgentInterface>(InPawn)->GetGenericTeamId());
	}
	AttachToActor(InPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	if (UBehaviorTree* BehaviorTree = IAIBehaviorTreeActor::GetBehaviorTree(InPawn))
	{
		UBlackboardComponent* LocalBlackboardComponent = GetBlackboardComponent();
		UseBlackboard(BehaviorTree->GetBlackboardAsset(), LocalBlackboardComponent);
		InitializeBlackboardKeys(BehaviorTree->GetBlackboardAsset());
		InitializeDependencies();
		if (IAIBehaviorTreeActor::ShouldAutoRunBehaviorTree(InPawn))
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}

void AElectricCastleAIController::OnUnPossess()
{
	Super::OnUnPossess();
	if (UAIAlertComponent* AlertComponent = IAIAlertActor::GetAIAlertComponent(GetPawn()))
	{
		AlertComponent->OnAlertLevelChanged.RemoveAll(this);
		AlertComponent->OnAlertTargetPerceiveChanged.RemoveAll(this);
	}
	if (UAIEngagementController* EngagementController = IAIEngagementActor::GetAIEngagementController(GetPawn()))
	{
		EngagementController->OnEngagementLevelChanged.RemoveAll(this);
		EngagementController->OnEngagementRangeChanged.RemoveAll(this);
		EngagementController->OnEngagementTargetChanged.RemoveAll(this);
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetPawn()))
	{
		CombatInterface->GetOnDeathDelegate().RemoveAll(this);
	}
	SetGenericTeamId(FGenericTeamId::NoTeam);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AElectricCastleAIController::InitializeBlackboardKeys(UBlackboardData* BlackboardData)
{
	if (UBlackboardComponent* LocalBlackboardComponent = GetBlackboardComponent())
	{
		LocalBlackboardComponent->InitializeBlackboard(*BlackboardData);
		// Initialize the accessors using MakeUnique to avoid assignment errors
		AlertLevel = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Alert::AlertLevel)
		);
		AlertTargetPerceived = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Alert::AlertTargetPerceived)
		);
		AlertLastKnownLocation = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Vector>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Alert::LastKnownLocation)
		);
		EffectBlockAbilities = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Effect::EffectBlockAbilities)
		);
		EffectBlockAI = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>>(*LocalBlackboardComponent, LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Effect::EffectBlockAI));
		EffectBlockMovement = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Effect::EffectBlockMovement)
		);
		EngagementControlMode = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Engagement::EngagementControlMode)
		);
		EngagementLevel = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Engagement::EngagementLevel)
		);
		EngagementAttackRate = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Object>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Engagement::EngagementAttackRate)
		);
		EngagementRange = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Engagement::EngagementRange)
		);
		EngagementPreferredDistance = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Object>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Engagement::EngagementPreferredDistance)
		);
		EngagementTarget = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Object>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Engagement::EngagementTarget)
		);
		PatrolCanPatrol = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>>(*LocalBlackboardComponent, LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Patrol::CanPatrol));
		PatrolNextPatrolPoint = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Vector>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Patrol::NextPatrolPoint)
		);
		PatrolMovementSpeed = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Float>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Patrol::PatrolMovementSpeed)
		);
		PatrolWaitTime = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Float>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Patrol::PatrolWaitTime)
		);
		StatusIsDead = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Bool>>(*LocalBlackboardComponent, LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Status::IsDead));
		SignificanceLevel = MakeUnique<FBBKeyCachedAccessor<UBlackboardKeyType_Enum>>(
			*LocalBlackboardComponent,
			LocalBlackboardComponent->GetKeyID(ElectricCastleBlackboardKeys::Significance::SignificanceLevel)
		);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No blackboard component - cannot initialize!"), *GetName())
	}
}

void AElectricCastleAIController::InitializeDependencies_Implementation()
{
	if (UAIAlertComponent* AlertComponent = IAIAlertActor::GetAIAlertComponent(GetPawn()))
	{
		AlertComponent->OnAlertLevelChanged.AddUniqueDynamic(this, &AElectricCastleAIController::HandleAlertLevelChanged);
		AlertComponent->OnAlertTargetPerceiveChanged.AddUniqueDynamic(this, &AElectricCastleAIController::HandleAlertTargetPerceiveChanged);
	}
	if (UAIEngagementController* EngagementController = IAIEngagementActor::GetAIEngagementController(GetPawn()))
	{
		EngagementController->OnEngagementLevelChanged.AddUniqueDynamic(this, &AElectricCastleAIController::HandleEngagementLevelChanged);
		EngagementController->OnEngagementRangeChanged.AddUniqueDynamic(this, &AElectricCastleAIController::HandleEngagementRangeChanged);
		EngagementController->OnEngagementTargetChanged.AddUniqueDynamic(this, &AElectricCastleAIController::HandleEngagementTargetChanged);
		EngagementControlMode->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(EngagementController->GetCurrentEngagementControlMode()));
		EngagementLevel->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(EngagementController->GetCurrentEngagementLevel()));
		EngagementRange->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(EngagementController->GetCurrentEngagementRange()));
		UpdateAttackRate(EngagementController->GetCurrentEngagementAttackRate());
		UpdatePreferredDistance(EngagementController->GetCurrentPreferredRange());
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetPawn()))
	{
		CombatInterface->GetOnDeathDelegate().AddUniqueDynamic(this, &AElectricCastleAIController::HandlePawnDeath);
	}
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
	{
		const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
		AbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Effect_Block_Abilities, EGameplayTagEventType::NewOrRemoved).AddLambda(
			[this](FGameplayTag EffectTag, int32 Count)
			{
				EffectBlockAbilities->SetValue(*GetBlackboardComponent(), Count > 0);
			}
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Effect_Block_AI, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AElectricCastleAIController::HandleEffectBlockAI);
		AbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Effect_Block_Movement, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AElectricCastleAIController::HandleEffectBlockMovement
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Significance_Level_Insignificant, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AElectricCastleAIController::HandleSignificanceLevelChanged
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Significance_Level_PartiallySignificant, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AElectricCastleAIController::HandleSignificanceLevelChanged
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(GameplayTags.Significance_Level_FullySignificant, EGameplayTagEventType::NewOrRemoved).AddUObject(
			this,
			&AElectricCastleAIController::HandleSignificanceLevelChanged
		);
	}
	if (const UPatrolComponent* PatrolComponent = IPatrollingActor::GetPatrolComponent(GetPawn()))
	{
		PatrolCanPatrol->SetValue(*GetBlackboardComponent(), PatrolComponent->CanPatrol());
		PatrolMovementSpeed->SetValue(*GetBlackboardComponent(), PatrolComponent->GetPatrolMovementSpeed());
		PatrolWaitTime->SetValue(*GetBlackboardComponent(), PatrolComponent->GetPatrolWaitTime());
	}
}

void AElectricCastleAIController::HandleEngagementLevelChanged_Implementation(const FEngagementLevelChangedPayload& Payload)
{
	if (EngagementLevel.IsValid())
	{
		EngagementLevel->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(Payload.Level));
	}
	UpdateAttackRate(Payload.AttackRate);
}

void AElectricCastleAIController::HandleEngagementRangeChanged_Implementation(const FEngagementRangeChangedPayload& Payload)
{
	if (EngagementRange.IsValid())
	{
		EngagementRange->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(Payload.Range));
	}
	UpdatePreferredDistance(Payload.PreferredDistance);
}

void AElectricCastleAIController::HandleEngagementTargetChanged_Implementation(AActor* NewTarget)
{
	if (EngagementTarget.IsValid())
	{
		EngagementTarget->SetValue(*GetBlackboardComponent(), NewTarget);
		SetFocus(NewTarget, EAIFocusPriority::Gameplay);
	}
}

void AElectricCastleAIController::HandleAlertLevelChanged_Implementation(const FAlertLevelChangePayload& Payload)
{
	if (AlertLevel.IsValid())
	{
		AlertLevel->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(Payload.NewAlertLevel));
	}
	if (AlertLastKnownLocation.IsValid())
	{
		AlertLastKnownLocation->SetValue(*GetBlackboardComponent(), Payload.LastKnownLocation);
	}
}

void AElectricCastleAIController::HandleAlertTargetPerceiveChanged_Implementation(const FAlertTargetPerceivedChangePayload& Payload)
{
	if (AlertTargetPerceived.IsValid())
	{
		AlertTargetPerceived->SetValue(*GetBlackboardComponent(), Payload.bCanPerceive);
		AlertLastKnownLocation->SetValue(*GetBlackboardComponent(), Payload.LastKnownLocation);
	}
}

void AElectricCastleAIController::HandlePawnDeath_Implementation(AActor* DeadActor)
{
	if (StatusIsDead.IsValid())
	{
		StatusIsDead->SetValue(*GetBlackboardComponent(), true);
	}
	if (UBrainComponent* LocalBrainComponent = GetBrainComponent())
	{
		LocalBrainComponent->StopLogic(TEXT("Stopped running logic - pawn is dead"));
	}
}

void AElectricCastleAIController::HandleEffectBlockAI_Implementation(FGameplayTag EffectTag, int Count)
{
	EffectBlockAI->SetValue(*GetBlackboardComponent(), Count > 0);
	if (Count > 0)
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		StopMovement();
	}
	else
	{
		if (const UAIEngagementController* EngagementController = IAIEngagementActor::GetAIEngagementController(GetPawn()))
		{
			SetFocus(EngagementController->GetEngagementTarget(), EAIFocusPriority::Gameplay);
		}
	}
}

void AElectricCastleAIController::HandleEffectBlockMovement_Implementation(FGameplayTag EffectTag, int Count)
{
	EffectBlockMovement->SetValue(*GetBlackboardComponent(), Count > 0);
	if (Count > 0)
	{
		StopMovement();
		if (UPawnMovementComponent* MovementComponent = GetPawn()->GetMovementComponent())
		{
			MovementComponent->StopMovementImmediately();
		}
	}
}

void AElectricCastleAIController::HandleSignificanceLevelChanged_Implementation(FGameplayTag SignificanceTag, int Count)
{
	const FElectricCastleGameplayTags& GameplayTags = FElectricCastleGameplayTags::Get();
	if (SignificanceTag.MatchesTagExact(GameplayTags.Significance_Level_Insignificant))
	{
		SignificanceLevel->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(ESignificanceLevel::Insignificant));
	}
	else if (SignificanceTag.MatchesTagExact(GameplayTags.Significance_Level_PartiallySignificant))
	{
		SignificanceLevel->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(ESignificanceLevel::PartiallySignificant));
	}
	else if (SignificanceTag.MatchesTagExact(GameplayTags.Significance_Level_FullySignificant))
	{
		SignificanceLevel->SetValue(*GetBlackboardComponent(), StaticCast<uint8>(ESignificanceLevel::FullySignificant));
	}
}

void AElectricCastleAIController::UpdateAttackRate(const FFloatRange& InAttackRate)
{
	if (EngagementAttackRate.IsValid())
	{
		AttackRatePtr = NewObject<URandomRange>();
		AttackRatePtr->SetRange(InAttackRate);
		EngagementAttackRate->SetValue(*GetBlackboardComponent(), AttackRatePtr);
	}
}

void AElectricCastleAIController::UpdatePreferredDistance(const FFloatRange& InPreferredDistance)
{
	if (EngagementPreferredDistance.IsValid())
	{
		PreferredDistancePtr = NewObject<URandomRange>();
		PreferredDistancePtr->SetRange(InPreferredDistance);
		EngagementPreferredDistance->SetValue(*GetBlackboardComponent(), PreferredDistancePtr);
	}
}
