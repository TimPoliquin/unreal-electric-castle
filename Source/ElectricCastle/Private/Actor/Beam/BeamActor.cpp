// Copyright Alien Shores


#include "Actor/Beam/BeamActor.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABeamActor::ABeamActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	BeamComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Beam Component"));
	BeamComponent->SetupAttachment(GetRootComponent());
	SFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SFX Component"));
	SFXComponent->SetupAttachment(GetRootComponent());
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABeamActor::BeginPlay()
{
	Super::BeginPlay();
	Update();
}

void ABeamActor::Update_Implementation()
{
	FHitResult HitResult;
	TraceDestination(HitResult);
	if (HitResult.IsValidBlockingHit())
	{
		UpdateBeamDestination(HitResult);
		UpdateBeamTargetEffect(HitResult);
	}
	LastTraceHitResult = HitResult;
}

void ABeamActor::TraceDestination_Implementation(FHitResult& HitResult)
{
	UElectricCastleAbilitySystemLibrary::FindHitBySphereTrace(TraceOrigin ? TraceOrigin : this, FSphereTraceParams(TraceDistance, TraceRadius, TraceChannel, bDebug), HitResult);
}

void ABeamActor::UpdateBeamDestination_Implementation(const FHitResult& HitResult)
{
	BeamComponent->SetVectorParameter(FName("Beam End"), HitResult.ImpactPoint);
}

void ABeamActor::UpdateBeamTargetEffect_Implementation(const FHitResult& HitResult)
{
	if (HitResult.GetActor() == LastTraceHitResult.GetActor())
	{
		return;
	}
	if (TargetEffectHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(LastTraceHitResult.GetActor(), TargetEffectHandle, false);
	}
	if (IsValid(HitResult.GetActor()))
	{
		TargetEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(HitResult.GetActor(), ApplyEffectToTarget, EffectLevel);
	}
	else
	{
		TerminateChildBeams();
	}
}

void ABeamActor::SpawnChildBeams_Implementation(const FHitResult& HitResult)
{
}

void ABeamActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Update();
}

void ABeamActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeamActor, CascadeType);
	DOREPLIFETIME(ABeamActor, LinearCascadeParams);
	DOREPLIFETIME(ABeamActor, WebCascadeParams);
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

void ABeamActor::SetWebCascadeParams(const FBeamCascadeWebParams& InParams)
{
	CascadeType = EBeamCascadeType::Web;
	WebCascadeParams = InParams;
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

void ABeamActor::Terminate_Implementation()
{
	TerminateChildBeams();
	Destroy();
}

void ABeamActor::TerminateChildBeams_Implementation()
{
	for (ABeamActor* ChildBeam : ChildBeams)
	{
		if (IsValid(ChildBeam))
		{
			ChildBeam->Terminate();
		}
	}
	ChildBeams.Empty();
}
