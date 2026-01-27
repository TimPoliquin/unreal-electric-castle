// Copyright Alien Shores


#include "Actor/MagicHand/MagicHand.h"

#include "NiagaraComponent.h"
#include "Actor/MagicHand/MagicHandPossessableInterface.h"
#include "Actor/MagicHand/MagicHandPossessorInterface.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

AMagicHand::AMagicHand()
{
	PrimaryActorTick.bCanEverTick = true;
	// scene components
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"))->SetupAttachment(GetRootComponent());
	HandMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand Mesh"));
	HandMeshComponent->SetupAttachment(GetRootComponent());
	GrabCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Grab Collision"));
	GrabCollisionComponent->SetupAttachment(HandMeshComponent);
	GrabCollisionComponent->bAutoActivate = false;
	TetherFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Tether FX"));
	TetherFXComponent->SetupAttachment(GetRootComponent());
	// functional components
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovementComponent->InitialSpeed = 0.f;
	ProjectileMovementComponent->MaxSpeed = 0.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->bAutoActivate = false;
	bReplicates = true;
}

void AMagicHand::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMagicHand, Speed);
	DOREPLIFETIME(AMagicHand, Range);
	DOREPLIFETIME(AMagicHand, State);
	DOREPLIFETIME(AMagicHand, Target);
	DOREPLIFETIME(AMagicHand, Possessor);
}

// Called when the game starts or when spawned
void AMagicHand::BeginPlay()
{
	Super::BeginPlay();
	GrabCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMagicHand::OnGrabCollisionBeginOverlap);
	PreviousLocation = GetActorLocation();
}

void AMagicHand::ReleaseTarget_Implementation()
{
	State = EMagicHandState::None;
	IMagicHandPossessableInterface::OnMagicHandUnpossess(Target.Get());
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Target = nullptr;
}

void AMagicHand::UpdateTetherFXBeamEnd_Implementation()
{
	if (FName AttachBone; const USceneComponent* PossessorAttachComponent = IMagicHandPossessorInterface::GetMagicHandAttachComponent(Possessor.Get(), AttachBone))
	{
		if (AttachBone.IsValid())
		{
			SetTetherFXBeamEnd(PossessorAttachComponent->GetSocketLocation(AttachBone));
		}
		else
		{
			SetTetherFXBeamEnd(PossessorAttachComponent->GetComponentLocation());
		}
	}
}

void AMagicHand::MissTarget_Implementation()
{
	State = EMagicHandState::None;
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->Deactivate();
	GrabCollisionComponent->Deactivate();
	OnMiss.Broadcast();
}

void AMagicHand::PossessTarget_Implementation(AActor* InTarget)
{
	State = EMagicHandState::Holding;
	Target = InTarget;
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->Deactivate();
	GrabCollisionComponent->Deactivate();
	IMagicHandPossessableInterface::OnMagicHandPossess(InTarget);
	if (FName AttachBoneName; USceneComponent* AttachComponent = IMagicHandPossessableInterface::GetMagicHandAttachComponent(InTarget, AttachBoneName))
	{
		if (!AttachBoneName.IsNone())
		{
			AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachBoneName);
		}
		else
		{
			AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(Possessor.Get());
			if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, PreviousLocation, InTarget->GetActorLocation(), ECC_Visibility, QueryParams))
			{
				FRotator Rotation = HitResult.ImpactNormal.Rotation();
				Rotation.Yaw = Rotation.Yaw - 180.f;
				Rotation.Normalize();
				SetActorLocation(HitResult.ImpactPoint);
				SetActorRotation(Rotation);
			}
		}
	}
	OnPossess.Broadcast(FMagicHandPossessPayload(this, Possessor.Get(), Target.Get()));
}

void AMagicHand::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (State)
	{
	case EMagicHandState::Casting:
		Tick_Casting();
		break;
	case EMagicHandState::Holding:
		Tick_Holding();
		break;
	default:
		break;
	}
	PreviousLocation = GetActorLocation();
}

void AMagicHand::Launch_Implementation(AActor* InPossessor, const float InSpeed, const float InRange)
{
	State = EMagicHandState::Casting;
	Possessor = InPossessor;
	Range = InRange;
	GrabCollisionComponent->Activate();
	ProjectileMovementComponent->InitialSpeed = InSpeed;
	ProjectileMovementComponent->MaxSpeed = InSpeed;
	ProjectileMovementComponent->Velocity = InPossessor->GetActorForwardVector() * InSpeed;
	ProjectileMovementComponent->Activate();
	UpdateTetherFXBeamEnd();
}


void AMagicHand::SetTetherFXBeamEnd_Implementation(const FVector EndLocation)
{
	TetherFXComponent->SetVectorParameter(FName("Beam End"), EndLocation);
}

void AMagicHand::Tick_Casting()
{
	if (!Possessor.IsValid())
	{
		// no possessor - just call it a miss.
		MissTarget();
		return;
	}
	if (FVector::DistSquared(Possessor->GetActorLocation(), GetActorLocation()) > (Range * Range))
	{
		MissTarget();
	}
	else
	{
		UpdateTetherFXBeamEnd();
	}
}

void AMagicHand::Tick_Holding()
{
	if (!Possessor.IsValid())
	{
		// no possessor is odd, and we may be a dangling object at this point, but we'll leave that problem for another time.
		return;
	}
	UpdateTetherFXBeamEnd();
}

void AMagicHand::OnGrabCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	if (State != EMagicHandState::Casting)
	{
		return;
	}
	if (OtherActor == Possessor)
	{
		return;
	}
	if (IMagicHandPossessableInterface::CanBePossessedByMagicHand(OtherActor))
	{
		PossessTarget(OtherActor);
	}
}
