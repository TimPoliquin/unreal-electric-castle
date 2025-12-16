// Copyright Alien Shores

#include "Actor/Beam/BeamActor.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/ReflectiveInterface.h"
#include "Components/AudioComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Net/UnrealNetwork.h"

ABeamActor::ABeamActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	BeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam Component"));
	BeamComponent->SetupAttachment(GetRootComponent());
	SFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SFX Component"));
	SFXComponent->SetupAttachment(GetRootComponent());
	GetRootComponent()->SetMobility(EComponentMobility::Movable);
	bReplicates = true;
}

void ABeamActor::BeginPlay()
{
	Super::BeginPlay();
	Update();
}

void ABeamActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Update();
}

void ABeamActor::Update_Implementation()
{
	FHitResult HitResult;
	TraceDestination(HitResult);
	UpdateBeamDestination(HitResult);
	UpdateBeamTargetEffect(HitResult);
	UpdateChildBeams(HitResult);
	LastTraceHitResult = HitResult;

	if (bDebug)
	{
		const FColor LineColor = HitResult.IsValidBlockingHit() ? FColor::Red : FColor::Orange;
		DrawDebugLine(GetWorld(), GetActorLocation(), HitResult.ImpactPoint, LineColor, false, 0, 0, 1);
	}
}

void ABeamActor::TraceDestination_Implementation(FHitResult& HitResult)
{
	UElectricCastleAbilitySystemLibrary::FindHitBySphereTrace(
		TraceOrigin ? TraceOrigin : this,
		FSphereTraceParams(TraceDistance, TraceRadius, TraceChannel, IgnoreActors, bDebug),
		HitResult
	);
	// Ensure we always have a valid end point for the beam
	if (!HitResult.IsValidBlockingHit())
	{
		HitResult.ImpactPoint = GetActorLocation() + GetActorForwardVector() * TraceDistance;
		HitResult.TraceEnd = HitResult.ImpactPoint;
	}
}

void ABeamActor::UpdateBeamDestination_Implementation(const FHitResult& HitResult)
{
	const FVector BeamEnd = HitResult.IsValidBlockingHit() ? HitResult.ImpactPoint : HitResult.TraceEnd;

	if (bDebug && !HitResult.IsValidBlockingHit())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No blocking hit, using trace end: %s"),
		       *GetName(), *BeamEnd.ToString());
	}

	BeamComponent->SetVectorParameter(FName("Beam End"), BeamEnd);
}

void ABeamActor::UpdateBeamTargetEffect_Implementation(const FHitResult& HitResult)
{
	if (!HasAuthority())
	{
		return;
	}
	if (HitResult.GetActor() == LastTraceHitResult.GetActor())
	{
		// Same target - just update child beam positions
		UpdateChildBeams(HitResult);
		return;
	}

	// Target changed - clean up old effects
	if (TargetEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(
			LastTraceHitResult.GetActor(),
			TargetEffectHandle,
			false
		);
		TargetEffectHandle = FActiveGameplayEffectHandle();
	}

	if (HitResult.IsValidBlockingHit() && IsValid(HitResult.GetActor()))
	{
		// Apply effect to the new target
		TargetEffectHandle = ApplyBeamTargetEffect(HitResult.GetActor());
	}
}

FActiveGameplayEffectHandle ABeamActor::ApplyBeamTargetEffect_Implementation(AActor* Target)
{
	if (ApplyEffectToTarget)
	{
		return UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(Target, ApplyEffectToTarget, EffectLevel);
	}
	return FActiveGameplayEffectHandle();
}

void ABeamActor::SpawnChildBeams_Implementation(const FHitResult& HitResult)
{
	if (!HasAuthority())
	{
		return;
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Spawning child beams!"), *GetName())
	if (const AActor* Target = HitResult.GetActor(); ShouldSpawnChildBeamsFromTarget(Target))
	{
		switch (CascadeType)
		{
		case EBeamCascadeType::Linear:
			SpawnChildBeams_Linear(HitResult);
			break;
		default:
			break;
		}
	}
}

void ABeamActor::SpawnChildBeams_Linear_Implementation(const FHitResult& HitResult)
{
	const FVector ReflectedDir = CalculateReflectedDirection(HitResult);

	if (bDebug)
	{
		DrawReflectionDebug(HitResult, ReflectedDir);
	}

	SpawnChildBeam(HitResult, ReflectedDir);
}

void ABeamActor::SpawnChildBeam_Implementation(const FHitResult& OriginHit, const FVector& ChildBeamDirection)
{
	// Create child beam with same configuration as parent
	FTransform ChildTransform;
	ChildTransform.SetLocation(OriginHit.ImpactPoint);
	ChildTransform.SetRotation(ChildBeamDirection.ToOrientationQuat());
	ChildTransform.SetScale3D(FVector::OneVector);

	ABeamActor* ChildBeam = GetWorld()->SpawnActorDeferred<ABeamActor>(
		GetClass(),
		ChildTransform,
		this,
		GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	InitializeChildBeamProperties(ChildBeam, OriginHit);
	ChildBeam->FinishSpawning(ChildTransform);

	if (bDebug)
	{
		DrawDebugDirectionalArrow(GetWorld(), OriginHit.ImpactPoint,
		                          OriginHit.ImpactPoint + ChildBeam->GetActorForwardVector() * 200.f,
		                          10.f, FColor::Orange, false, 5.f, 0, 4.f);
	}

	ChildBeams.Add(ChildBeam);
}

void ABeamActor::InitializeChildBeamProperties_Implementation(ABeamActor* ChildBeam, const FHitResult& HitResult)
{
	// Calculate cascade params for child
	// Check if the ORIGIN (what we're reflecting off) is reflective, not the destination
	const bool bOriginIsReflective = HitResult.IsValidBlockingHit() && IReflectiveInterface::IsReflective(HitResult.GetActor());

	FBeamCascadeLinearParams ChildParams = LinearCascadeParams;
	// Only decrement cascades if we're NOT reflecting off a reflective surface
	if (!bOriginIsReflective)
	{
		ChildParams.MaxCascades = FMath::Max(0, LinearCascadeParams.MaxCascades - 1);
	}
	// Configure child beam (inherits most settings from parent)
	ChildBeam->SetActorTickEnabled(false); // Parent will manually update
	ChildBeam->SetIgnoreActors({this, HitResult.GetActor()});
	ChildBeam->SetTraceParams(LinearCascadeParams.MaxDistance, TraceRadius, TraceChannel, bDebug);
	ChildBeam->SetLinearCascadeParams(ChildParams);
	ChildBeam->ApplyEffectToTarget = ApplyEffectToTarget;
	ChildBeam->EffectLevel = EffectLevel;
}

void ABeamActor::UpdateChildBeams_Implementation(const FHitResult& HitResult)
{
	// there are four options here:
	// 1. the last hit was not reflective, and this one is reflective. spawn child beams.
	// 2. the last hit was reflective, but this hit is not. destroy the child beams.
	// 3. the last hit was reflective, and this one is also reflective. update the child beams.
	// 4. the last hit was not reflective, and this one is also not reflective. do nothing.
	const bool bLastHitWasReflective = LastTraceHitResult.IsValidBlockingHit() && ShouldSpawnChildBeamsFromTarget(LastTraceHitResult.GetActor());
	const bool bThisHitIsReflective = HitResult.IsValidBlockingHit() && ShouldSpawnChildBeamsFromTarget(HitResult.GetActor());
	if (!bLastHitWasReflective && bThisHitIsReflective)
	{
		SpawnChildBeams(HitResult);
	}
	else if (bLastHitWasReflective && !bThisHitIsReflective)
	{
		TerminateChildBeams();
	}
	else if (bLastHitWasReflective && bThisHitIsReflective)
	{
		for (ABeamActor* ChildBeam : ChildBeams)
		{
			if (!IsValid(ChildBeam))
			{
				return;
			}
			switch (CascadeType)
			{
			case EBeamCascadeType::None:
				break;
			case EBeamCascadeType::Linear:
				UpdateChildBeam_Linear(ChildBeam, HitResult);
				break;
			default:
				UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Unhandled cascade type: %s"), *GetName(), *UEnum::GetValueAsString(CascadeType))
				break;
			}
		}
	}
	else
	{
		// do nothing
	}
}

void ABeamActor::UpdateChildBeam_Linear(ABeamActor* ChildBeam, const FHitResult& ParentHitResult)
{
	const FVector ReflectedDir = CalculateReflectedDirection(ParentHitResult);
	// Update child beam transform
	ChildBeam->SetActorLocationAndRotation(ParentHitResult.ImpactPoint, ReflectedDir.ToOrientationQuat());
	ChildBeam->Update();
}

void ABeamActor::TerminateChildBeams_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Terminating child beams: %d"), *GetName(), ChildBeams.Num())
	if (ChildBeams.IsEmpty())
	{
		return;
	}
	for (ABeamActor* ChildBeam : ChildBeams)
	{
		if (IsValid(ChildBeam))
		{
			ChildBeam->Terminate();
		}
	}
	ChildBeams.Empty();
}

void ABeamActor::Terminate_Implementation()
{
	TerminateChildBeams();
	Destroy();
}

FVector ABeamActor::CalculateReflectedDirection(const FHitResult& HitResult) const
{
	const FVector IncomingDir = GetActorForwardVector();
	return IncomingDir.MirrorByVector(HitResult.ImpactNormal);
}

void ABeamActor::DrawReflectionDebug(const FHitResult& HitResult, const FVector& ReflectedDir) const
{
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Reflection Debug:"), *GetName());
	UE_LOG(LogElectricCastle, Warning, TEXT("  Impact Point: %s"), *HitResult.ImpactPoint.ToString());
	UE_LOG(LogElectricCastle, Warning, TEXT("  Impact Normal: %s"), *HitResult.ImpactNormal.ToString());
	UE_LOG(LogElectricCastle, Warning, TEXT("  Incoming Dir: %s"), *GetActorForwardVector().ToString());
	UE_LOG(LogElectricCastle, Warning, TEXT("  Reflected Dir: %s"), *ReflectedDir.ToString());

	const FVector ImpactPoint = HitResult.ImpactPoint;
	DrawDebugDirectionalArrow(GetWorld(), ImpactPoint, ImpactPoint + GetActorForwardVector() * 100.f, 5.f, FColor::Red, false, 2.f, 0, 2.f);
	DrawDebugDirectionalArrow(GetWorld(), ImpactPoint, ImpactPoint + HitResult.ImpactNormal * 100.f, 5.f, FColor::Blue, false, 2.f, 0, 2.f);
	DrawDebugDirectionalArrow(GetWorld(), ImpactPoint, ImpactPoint + ReflectedDir * 100.f, 5.f, FColor::Green, false, 2.f, 0, 2.f);
}

void ABeamActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeamActor, CascadeType);
	DOREPLIFETIME(ABeamActor, LinearCascadeParams);
	DOREPLIFETIME(ABeamActor, ApplyEffectToTarget);
	DOREPLIFETIME(ABeamActor, EffectLevel);
	DOREPLIFETIME(ABeamActor, TraceDistance);
	DOREPLIFETIME(ABeamActor, TraceRadius);
	DOREPLIFETIME(ABeamActor, TraceChannel);
	DOREPLIFETIME(ABeamActor, ChildBeams);
	DOREPLIFETIME(ABeamActor, TraceOrigin);
}

void ABeamActor::SetLinearCascadeParams(const FBeamCascadeLinearParams& InParams)
{
	CascadeType = EBeamCascadeType::Linear;
	LinearCascadeParams = InParams;
}

void ABeamActor::SetNoCascadeParams()
{
	CascadeType = EBeamCascadeType::None;
}

void ABeamActor::SetTraceParams(const float InTraceDistance, const float InTraceRadius, const ECollisionChannel InTraceChannel, const bool bInDebug)
{
	TraceDistance = InTraceDistance;
	TraceRadius = InTraceRadius;
	TraceChannel = InTraceChannel;
	bDebug = bInDebug;
}

void ABeamActor::SetTraceOrigin(AActor* InTraceOrigin)
{
	TraceOrigin = InTraceOrigin;
}

void ABeamActor::SetIgnoreActors(const TArray<AActor*>& InActors)
{
	IgnoreActors = InActors;
}

bool ABeamActor::ShouldSpawnChildBeamsFromTarget(const AActor* Target) const
{
	if (!IsValid(Target))
	{
		return false;
	}

	// ALWAYS reflect off reflective surfaces, regardless of cascade count
	if (IReflectiveInterface::IsReflective(Target) && CascadeType == EBeamCascadeType::Linear)
	{
		return true;
	}

	// For combat targets, check cascade count
	if (Target->Implements<UCombatInterface>())
	{
		switch (CascadeType)
		{
		case EBeamCascadeType::None:
			return false;
		case EBeamCascadeType::Linear:
			return LinearCascadeParams.MaxCascades > 0;
		default:
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Unhandled cascade type: %s"), *GetName(), *UEnum::GetValueAsString(CascadeType))
			return false;
		}
	}
	return false;
}
