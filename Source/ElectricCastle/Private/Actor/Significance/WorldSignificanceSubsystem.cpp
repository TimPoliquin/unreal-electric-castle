// Copyright Alien Shores


#include "Actor/Significance/WorldSignificanceSubsystem.h"

#include "SignificanceManager.h"
#include "Actor/Significance/ActorSignificance.h"
#include "Actor/Significance/SignificanceCalculation.h"
#include "Actor/Significance/SignificanceSensitiveActor.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

bool FSignificanceLimit::operator==(const FSignificanceLimit& Other) const
{
	return Threshold == Other.Threshold && Limit == Other.Limit;
}

bool FSignificanceLimit::operator<(const FSignificanceLimit& Other) const
{
	return Threshold < Other.Threshold;
}

bool FSignificanceLimit::operator>(const FSignificanceLimit& Other) const
{
	return Threshold > Other.Threshold;
}

bool FSignificanceLimit::operator<=(const FSignificanceLimit& Other) const
{
	return Threshold <= Other.Threshold;
}

bool FSignificanceLimit::operator>=(const FSignificanceLimit& Other) const
{
	return Threshold >= Other.Threshold;
}

void FSignificanceConfig::SortAscendingByThreshold()
{
	// sort ascending by threshold
	Limits.Sort(
		[](const FSignificanceLimit& A, const FSignificanceLimit& B)
		{
			return A.Threshold < B.Threshold;
		}
	);
}

const FSignificanceLimit* FSignificanceConfig::FindSignificanceLimit(const float SignificanceScore) const
{
	return Limits.FindByPredicate(
		[SignificanceScore](const FSignificanceLimit& Limit)
		{
			return Limit.Threshold >= SignificanceScore;
		}
	);
}

void FActorSignificanceBucket::AddActorSignificance(UActorSignificance* Significance)
{
	ActorSignificances.Add(Significance);
}

uint32 FActorSignificanceBucket::RemoveActor(UObject* Actor)
{
	return ActorSignificances.RemoveAll(
		[Actor](const UActorSignificance* Significance)
		{
			return Significance->Actor.Get() == Actor;
		}
	) > 0;
}

void FActorSignificanceBucket::Reset()
{
	ActorSignificances.Reset();
}

void FActorSignificanceBucket::CleanupStaleActors()
{
	if (ActorSignificances.IsEmpty())
	{
		return;
	}
	ActorSignificances.RemoveAll(
		[](const UActorSignificance* Significance)
		{
			return !IsValid(Significance) || !Significance->Actor.IsValid();
		}
	);
}

UWorldSignificanceSubsystem* UWorldSignificanceSubsystem::Get(const UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		return WorldContextObject->GetWorld()->GetSubsystem<UWorldSignificanceSubsystem>();
	}
	return nullptr;
}

void UWorldSignificanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	for (FSignificanceConfig& Config : SignificanceConfigs)
	{
		Config.SortAscendingByThreshold();
		ActorSignificancesByTag.Add(Config.SignificanceTag, FActorSignificanceBucket());
	}
}

void UWorldSignificanceSubsystem::Deinitialize()
{
	if (CachedSignificanceManager.IsValid())
	{
		for (const FSignificanceConfig& SignificanceConfig : SignificanceConfigs)
		{
			CachedSignificanceManager->UnregisterAll(SignificanceConfig.SignificanceTag.GetTagName());
		}
		ActorSignificancesByTag.Reset();
		CachedSignificanceManager.Reset();
	}
	Super::Deinitialize();
}

void UWorldSignificanceSubsystem::ProcessSignificance(const float DeltaTime)
{
	if (!PlayerController.IsValid())
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] No PlayerController set!"), *GetName())
		return;
	}
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetWorld()))
	{
		if (!GameDataSubsystem->IsGameDataLoaded())
		{
			return;
		}
	}
	else
	{
		return;
	}
	TimeSinceLastUpdate += DeltaTime;
	if (TimeSinceLastUpdate < SignificanceUpdateRate)
	{
		return;
	}
	CleanupStaleActors();
	if (ActorSignificancesByTag.IsEmpty())
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s] No significant actors to process"), *GetName());
		}
		return;
	}
	// run the significance manager calculations
	ProcessSignificanceManager();
	// now, update actors based on their significance and configured limits
	ProcessSignificanceBuckets();
	TimeSinceLastUpdate = 0.f;
}

void UWorldSignificanceSubsystem::SetPlayerController(APlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
}

void UWorldSignificanceSubsystem::RegisterObject(UObject* Object, const FGameplayTag& SignificanceTag, USignificanceCalculation* SignificanceCalculation)
{
	if (USignificanceManager* SignificanceManager = GetSignificanceManager())
	{
		if (!ISignificanceSensitiveActor::IsSignificanceSensitiveActor(Object))
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Attempted to register an object that is not significance-sensitive: %s"), *GetName(), Object ? *Object->GetName() : *FString("NULLPTR"));
			return;
		}
		if (!ActorSignificancesByTag.Contains(SignificanceTag))
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Attempted to register an object with a tag that is not configured: %s"), *GetName(), *SignificanceTag.ToString());
			return;
		}
		UActorSignificance* ActorSignificance = CreateActorSignificance(Object, SignificanceTag);
		ActorSignificancesByTag[SignificanceTag].AddActorSignificance(ActorSignificance);
		TWeakObjectPtr<USignificanceCalculation> WeakCalc = SignificanceCalculation;
		TWeakObjectPtr<UActorSignificance> WeakActorSignificance = ActorSignificance;
		SignificanceManager->RegisterObject(
			Object,
			SignificanceTag.GetTagName(),
			[this, WeakCalc](const USignificanceManager::FManagedObjectInfo* ManagedObjectInfo, const FTransform& Transform)-> float
			{
				if (WeakCalc.IsValid())
				{
					return FMath::Clamp(WeakCalc->CalculateSignificance(ManagedObjectInfo, Transform), -1.f, 1.f);
				}
				return -1.f;
			},
			USignificanceManager::EPostSignificanceType::Sequential,
			[WeakActorSignificance](const USignificanceManager::FManagedObjectInfo* ManagedObjectInfo, const float OldScore, const float NewScore, const bool bUnregistered)
			{
				if (!WeakActorSignificance.IsValid())
				{
					return;
				}
				if (bUnregistered)
				{
					return;
				}
				WeakActorSignificance->Significance = NewScore;
			}
		);
		if (AActor* Actor = Cast<AActor>(Object))
		{
			Actor->OnDestroyed.AddUniqueDynamic(this, &UWorldSignificanceSubsystem::HandleActorDestroyed);
		}
	}
}

void UWorldSignificanceSubsystem::UnregisterObject(UObject* Object)
{
	bool bRemoved = false;
	for (TTuple KeyValuePair : ActorSignificancesByTag)
	{
		bRemoved |= KeyValuePair.Value.RemoveActor(Object) > 0;
	}
	if (bRemoved)
	{
		if (USignificanceManager* SignificanceManager = GetSignificanceManager())
		{
			SignificanceManager->UnregisterObject(Object);
		}
	}
	if (AActor* Actor = Cast<AActor>(Object))
	{
		Actor->OnDestroyed.RemoveAll(this);
	}
}

void UWorldSignificanceSubsystem::UnregisterObjects(const FGameplayTag& SignificanceTag)
{
	if (USignificanceManager* SignificanceManager = GetSignificanceManager())
	{
		SignificanceManager->UnregisterAll(SignificanceTag.GetTagName());
	}
	if (ActorSignificancesByTag.Contains(SignificanceTag))
	{
		ActorSignificancesByTag[SignificanceTag].Reset();
	}
}

bool UWorldSignificanceSubsystem::HasAnySignificantActors(const FGameplayTag& SignificanceTag, const ESignificanceLevel MinimumSignificance)
{
	if (ActorSignificancesByTag.Contains(SignificanceTag))
	{
		const FActorSignificanceBucket& Bucket = ActorSignificancesByTag[SignificanceTag];
		return Bucket.MaxSignificanceLevel >= MinimumSignificance;
	}
	return false;
}

ESignificanceLevel UWorldSignificanceSubsystem::GetSignificanceLevelByTagAndSignificance(const FGameplayTag& SignificanceTag, const float Significance) const
{
	const FSignificanceConfig* SignificanceConfig = SignificanceConfigs.FindByPredicate(
		[SignificanceTag](const FSignificanceConfig& CurrentSignificance)
		{
			return SignificanceTag.MatchesTagExact(CurrentSignificance.SignificanceTag);
		}
	);
	const FSignificanceLimit* SignificanceLimit = SignificanceConfig
		                                              ? SignificanceConfig->FindSignificanceLimit(Significance)
		                                              : nullptr;
	// default to insignificant if no matching config.
	return SignificanceLimit ? SignificanceLimit->SignificanceLevel : ESignificanceLevel::Insignificant;
}

void UWorldSignificanceSubsystem::HandleActorDestroyed(AActor* DestroyedActor)
{
	UnregisterObject(DestroyedActor);
}

USignificanceManager* UWorldSignificanceSubsystem::GetSignificanceManager()
{
	if (!CachedSignificanceManager.IsValid())
	{
		CachedSignificanceManager = USignificanceManager::Get(GetWorld());
	}
	if (CachedSignificanceManager.IsValid())
	{
		return CachedSignificanceManager.Get();
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Attempted to get SignificanceManager, but it is not valid right now."), *GetName())
	return nullptr;
}

void UWorldSignificanceSubsystem::CleanupStaleActors()
{
	for (TTuple KeyValuePair : ActorSignificancesByTag)
	{
		KeyValuePair.Value.CleanupStaleActors();
	}
}

void UWorldSignificanceSubsystem::ProcessSignificanceManager()
{
	if (USignificanceManager* SignificanceManager = GetSignificanceManager())
	{
		FVector ViewpointLocation;
		FRotator ViewpointRotation;
		PlayerController->GetPlayerViewPoint(ViewpointLocation, ViewpointRotation);
		const FTransform ViewpointTransform = FTransform(ViewpointRotation, ViewpointLocation);
		TArray<FTransform> TransformArray;
		TransformArray.Add(ViewpointTransform);
		SignificanceManager->Update(MakeArrayView(TransformArray));
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Significance Manager is not ready!"), *GetName())
	}
}

void UWorldSignificanceSubsystem::ProcessSignificanceBuckets()
{
	for (const FSignificanceConfig& SignificanceConfig : SignificanceConfigs)
	{
		FActorSignificanceBucket& Bucket = ActorSignificancesByTag[SignificanceConfig.SignificanceTag];
		TArray<TObjectPtr<UActorSignificance>>& ActorSignificances = ActorSignificancesByTag[SignificanceConfig.SignificanceTag].ActorSignificances;
		if (ActorSignificances.IsEmpty())
		{
			Bucket.MaxSignificanceLevel = ESignificanceLevel::None;
			continue;
		}
		Bucket.MaxSignificanceLevel = ESignificanceLevel::Insignificant;
		// Sort Descending: [0.9, 0.8, 0.5, 0.2]
		ActorSignificances.Sort(
			[](const UActorSignificance& A, const UActorSignificance& B)
			{
				return A.Significance > B.Significance;
			}
		);

		int32 CurrentActorIndex = 0;

		// Iterate from highest threshold tier down to lowest
		for (auto LimitIterator = SignificanceConfig.Limits.rbegin(); LimitIterator != SignificanceConfig.Limits.rend(); ++LimitIterator)
		{
			const FSignificanceLimit& Limit = *LimitIterator;
			int32 TierAssignedCount = 0;

			while (CurrentActorIndex < ActorSignificances.Num())
			{
				UActorSignificance* ActorSignificance = ActorSignificances[CurrentActorIndex];

				// If it doesn't meet the threshold, it belongs to a lower tier processed next
				if (ActorSignificance->Significance < Limit.Threshold)
				{
					break;
				}

				// If we have a budget limit and we've reached it, this actor can't fit in this tier.
				// It must cascade down to the next lower tier, so stop assigning here.
				if (Limit.Limit >= 0 && TierAssignedCount >= Limit.Limit)
				{
					break;
				}

				// Assign the tier safely
				AssignSignificanceLevel(ActorSignificance, Limit.SignificanceLevel);
				// Update the bucket's max significance
				if (Limit.SignificanceLevel > Bucket.MaxSignificanceLevel)
				{
					Bucket.MaxSignificanceLevel = Limit.SignificanceLevel;
				}
				TierAssignedCount++;
				CurrentActorIndex++;
			}
		}

		// Any remaining actors that didn't fit into any tier budget or fell below all thresholds default to Insignificant
		while (CurrentActorIndex < ActorSignificances.Num())
		{
			AssignSignificanceLevel(ActorSignificances[CurrentActorIndex], ESignificanceLevel::Insignificant);
			CurrentActorIndex++;
		}
	}
}

UActorSignificance* UWorldSignificanceSubsystem::CreateActorSignificance(UObject* Object, const FGameplayTag& SignificanceTag)
{
	UActorSignificance* ActorSignificance = NewObject<UActorSignificance>(this, UActorSignificance::StaticClass());
	ActorSignificance->Actor = Object;
	ActorSignificance->SignificanceTag = SignificanceTag;
	ActorSignificance->Significance = 0.f;
	AssignSignificanceLevel(ActorSignificance, ESignificanceLevel::Insignificant);
	return ActorSignificance;
}

void UWorldSignificanceSubsystem::AssignSignificanceLevel(UActorSignificance* ActorSignificance, const ESignificanceLevel Level)
{
	if (!IsValid(ActorSignificance))
	{
		return;
	}
	if (ActorSignificance->SignificanceLevel == Level)
	{
		return;
	}
	if (bDebug)
	{
		UE_LOG(
			LogElectricCastle,
			Display,
			TEXT("[%s] Updating actor significance level: %s %s->%s"),
			*GetName(),
			*ActorSignificance->Actor->GetName(),
			*UEnum::GetValueAsString(ActorSignificance->SignificanceLevel),
			*UEnum::GetValueAsString(Level)
		);
	}
	ActorSignificance->SignificanceLevel = Level;
	switch (Level)
	{
	case ESignificanceLevel::Insignificant:
		ISignificanceSensitiveActor::EnterSignificance_Insignificant(ActorSignificance->Actor.Get());
		break;
	case ESignificanceLevel::PartiallySignificant:
		ISignificanceSensitiveActor::EnterSignificance_PartiallySignificant(ActorSignificance->Actor.Get());
		break;
	case ESignificanceLevel::FullySignificant:
		ISignificanceSensitiveActor::EnterSignificance_FullySignificant(ActorSignificance->Actor.Get());
		break;
	default:
		break;
	}
}
