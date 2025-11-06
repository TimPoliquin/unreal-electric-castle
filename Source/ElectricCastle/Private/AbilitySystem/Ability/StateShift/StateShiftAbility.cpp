// Copyright Alien Shores


#include "AbilitySystem/Ability/StateShift/StateShiftAbility.h"

#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"

void UStateShiftAbility::ApplyStateShiftDetectionComponent_Implementation()
{
	if (!GetActorInfo().IsNetAuthority())
	{
		// Only do detection on authority
		return;
	}
	CreateDetectionComponent();
}

void UStateShiftAbility::RemoveStateShiftDetectionComponent_Implementation()
{
	if (DetectionComponent)
	{
		DetectionComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DetectionComponent->DestroyComponent();
		DetectionComponent = nullptr;
	}
	if (DebugMeshComponent)
	{
		DebugMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DebugMeshComponent->DestroyComponent();
		DebugMeshComponent = nullptr;
	}
}

void UStateShiftAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	RemoveStateShiftDetectionComponent();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float UStateShiftAbility::GetEffectRadius() const
{
	return DetectionRadius.GetValueAtLevel(GetAbilityLevel());
}

void UStateShiftAbility::OnStateShiftBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool BFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValid(OtherActor) || OtherActor == GetAvatarActorFromActorInfo())
	{
		return;
	}
	UStateShiftBlueprintFunctionLibrary::HandleStateShift(OtherActor, ShiftState);
}


void UStateShiftAbility::OnStateShiftEndOverlap_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	if (!IsValid(OtherActor) || OtherActor == GetAvatarActorFromActorInfo())
	{
		return;
	}
	UStateShiftBlueprintFunctionLibrary::HandleStateShift(OtherActor, BaseState);
}

void UStateShiftAbility::CreateDetectionComponent()
{
	DetectionComponent = NewObject<USphereComponent>(GetAvatarActorFromActorInfo());
	DetectionComponent->InitSphereRadius(GetEffectRadius());
	DetectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectionComponent->SetGenerateOverlapEvents(true);
	DetectionComponent->SetCollisionObjectType(ECC_StateShift);
	DetectionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	DetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &UStateShiftAbility::OnStateShiftBeginOverlap);
	DetectionComponent->OnComponentEndOverlap.AddDynamic(this, &UStateShiftAbility::OnStateShiftEndOverlap);
	DetectionComponent->RegisterComponent();
	DetectionComponent->AttachToComponent(
		GetAvatarActorFromActorInfo()->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform
	);

	if (bDebug)
	{
		DebugMeshComponent = NewObject<UStaticMeshComponent>(GetAvatarActorFromActorInfo());
		if (UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")))
		{
			DebugMeshComponent->SetStaticMesh(SphereMesh);
			float Scale = DetectionComponent->GetScaledSphereRadius() / 50.f;
			DebugMeshComponent->SetWorldScale3D(FVector(Scale));
		}
		else
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Failed to load debug sphere mesh"), *GetName());
		}
		DebugMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (DebugMaterial)
		{
			DebugMeshComponent->SetMaterial(0, DebugMaterial);
		}
		DebugMeshComponent->RegisterComponent();
		DebugMeshComponent->AttachToComponent(
			DetectionComponent,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
		);
	}
}
