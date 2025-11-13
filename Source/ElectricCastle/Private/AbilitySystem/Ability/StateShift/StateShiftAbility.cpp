// Copyright Alien Shores


#include "AbilitySystem/Ability/StateShift/StateShiftAbility.h"

#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"
#include "Actor/State/StateShiftComponent.h"
#include "Actor/State/StateShiftInterface.h"
#include "Components/PostProcessComponent.h"
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
	RemoveVisualEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float UStateShiftAbility::GetEffectRadius() const
{
	return DetectionRadius.GetValueAtLevel(GetAbilityLevel());
}

void UStateShiftAbility::ApplyVisualEffect_Implementation()
{
	if (!PostProcessMaterial)
	{
		return;
	}
	// Create and attach a post process component
	PostProcessComponent = NewObject<UPostProcessComponent>(
		GetAvatarActorFromActorInfo(),
		UPostProcessComponent::StaticClass()
	);
	if (IsValid(PostProcessComponent))
	{
		PostProcessComponent->RegisterComponent();
		PostProcessComponent->bUnbound = true; // Affect entire screen
		PostProcessComponent->Priority = 1.0f;

		// Add the post process material as a blendable
		FWeightedBlendable Blendable(1.0f, PostProcessMaterial);
		PostProcessComponent->Settings.WeightedBlendables.Array.Add(Blendable);
	}
}


void UStateShiftAbility::RemoveVisualEffect_Implementation()
{
	if (IsValid(PostProcessComponent))
	{
		PostProcessComponent->UnregisterComponent();
		PostProcessComponent->DestroyComponent();
		PostProcessComponent = nullptr;
	}
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
	HandleStateShift(OtherActor, ShiftState);
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
	HandleStateShift(OtherActor, BaseState);
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

void UStateShiftAbility::HandleStateShift(AActor* OtherActor, EStateShiftState NewState)
{
	if (!IsValid(OtherActor) || !OtherActor->Implements<UStateShiftInterface>())
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[UStateShiftBlueprintFunctionLibrary::HandleStateShift] Actor is not a UStateShiftInterface! %s"),
			OtherActor ? *OtherActor->GetName() : TEXT("NULL")
		);
		return;
	}
	if (UStateShiftComponent* StateShiftComponent = IStateShiftInterface::GetStateShiftComponent(OtherActor))
	{
		if (!StateShiftComponent->ShouldChangeState(NewState))
		{
			UE_LOG(
				LogElectricCastle,
				Warning,
				TEXT("[%s] Actor does not need to change states to %s"),
				*OtherActor->GetName(),
				*UEnum::GetValueAsString(NewState)
			)
			return;
		}
		StateShiftComponent->ChangeState(
			FStateShiftRequest(GetAvatarActorFromActorInfo(), GetEffectRadius(), NewState)
		);
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Actor does not have a state shift component"),
			*OtherActor->GetName()
		)
	}
}
