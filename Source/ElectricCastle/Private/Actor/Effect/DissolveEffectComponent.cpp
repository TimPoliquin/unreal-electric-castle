#include "Actor/Effect/DissolveEffectComponent.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/MeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"

UDissolveEffectComponent::UDissolveEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Initialize delegates
	TimelineUpdateDelegate.BindDynamic(this, &UDissolveEffectComponent::OnTimelineUpdate);
	TimelineFinishedDelegate.BindDynamic(this, &UDissolveEffectComponent::OnTimelineFinished);
}

void UDissolveEffectComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDissolveEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// Auto play if enabled
	if (bAutoPlay)
	{
		// Initialize material
		InitializeMaterial();
		// Play the timeline from the start
		PlayTimelineFromStart();
	}
}

void UDissolveEffectComponent::InitializeMaterial()
{
	if (DynamicMaterial)
	{
		return;
	}
	if (DissolvableMaterial && TargetMeshComponent)
	{
		if (!TargetMeshComponent)
		{
			TargetMeshComponent = GetOwner()->FindComponentByClass<UMeshComponent>();
		}
		// Create dynamic material instance
		DynamicMaterial = UMaterialInstanceDynamic::Create(DissolvableMaterial, this);

		if (DynamicMaterial)
		{
			// Apply the dynamic material to the mesh
			TargetMeshComponent->SetMaterial(0, DynamicMaterial);
			UE_LOG(LogElectricCastle, Log, TEXT("TimelineMaterialComponent: Dynamic material created and applied"));
		}
		else
		{
			UE_LOG(LogElectricCastle, Error, TEXT("TimelineMaterialComponent: Failed to create dynamic material instance"));
		}
	}
	else
	{
		if (!DissolvableMaterial)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("TimelineMaterialComponent: Base material not set"));
		}
		if (!TargetMeshComponent)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("TimelineMaterialComponent: Target mesh component not found"));
		}
	}
}

void UDissolveEffectComponent::Initialize()
{
	if (!Timeline)
	{
		InitializeTimeline();
	}
	if (!DynamicMaterial)
	{
		InitializeMaterial();
	}
}

void UDissolveEffectComponent::InitializeTimeline()
{
	if (Timeline)
	{
		return;
	}
	if (!TimelineCurve)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No timeline curve set!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	Timeline = NewObject<UTimelineComponent>(this, UTimelineComponent::StaticClass(), FName(GetName() + TEXT("_Timeline")));
	Timeline->SetComponentTickEnabled(true);
	Timeline->RegisterComponent();
	Timeline->AddInterpFloat(TimelineCurve, TimelineUpdateDelegate);
	Timeline->SetTimelineFinishedFunc(TimelineFinishedDelegate);
	Timeline->SetLooping(bLoop);
	Timeline->SetPlayRate(PlayRate);
}

void UDissolveEffectComponent::OnTimelineUpdate(const float Value)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(MaterialParameterName, Value);
	}
}

void UDissolveEffectComponent::OnTimelineFinished()
{
	OnDissolveEffectCompleted.Broadcast();
}


void UDissolveEffectComponent::PlayTimeline()
{
	if (!Timeline)
	{
		Initialize();
	}
	if (Timeline)
	{
		Timeline->Play();
	}
}

void UDissolveEffectComponent::PlayTimelineFromStart()
{
	if (!Timeline)
	{
		Initialize();
	}
	if (Timeline)
	{
		Timeline->PlayFromStart();
	}
}

void UDissolveEffectComponent::PlayTimelineReverse()
{
	if (!Timeline)
	{
		Initialize();
	}
	if (Timeline)
	{
		Timeline->Reverse();
	}
}

void UDissolveEffectComponent::PlayTimelineReverseFromEnd()
{
	if (!Timeline)
	{
		Initialize();
	}
	if (Timeline)
	{
		Timeline->ReverseFromEnd();
	}
}

void UDissolveEffectComponent::StopTimeline()
{
	if (!Timeline)
	{
		Initialize();
	}
	if (Timeline)
	{
		Timeline->Stop();
	}
}

void UDissolveEffectComponent::SetTimelinePosition(float NewPosition)
{
	if (!Timeline)
	{
		Initialize();
	}
	if (Timeline)
	{
		Timeline->SetPlaybackPosition(NewPosition, false);
	}
}

void UDissolveEffectComponent::SetTargetMeshComponent(UMeshComponent* MeshComponent)
{
	TargetMeshComponent = MeshComponent;
}
