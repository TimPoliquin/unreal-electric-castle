// Copyright Alien Shores


#include "Actor/Effect/FadeEffectComponent.h"

#include "Components/TimelineComponent.h"
#include "Material/MaterialBlueprintLibrary.h"


// Sets default values for this component's properties
UFadeEffectComponent::UFadeEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	OnFadeUpdate.BindDynamic(this, &UFadeEffectComponent::OnFadeTimelineUpdate);
	OnFadeFinish.BindDynamic(this, &UFadeEffectComponent::OnFadeTimelineFinish);
}


void UFadeEffectComponent::AddFadeMesh(UMeshComponent* MeshComponent)
{
	FadeMeshes.Add(MeshComponent);
}

void UFadeEffectComponent::FadeOut_Implementation()
{
	if (!FadeTimeline)
	{
		FadeTimeline = CreateFadeTimeline(FName(GetName() + TEXT("_FadeOutTimeline")));
	}
	InitializeMeshMaterials();
	FadeTimeline->Play();
}

void UFadeEffectComponent::FadeIn_Implementation()
{
	if (!FadeTimeline)
	{
		FadeTimeline = CreateFadeTimeline(FName(GetName() + TEXT("_FadeOutTimeline")));
	}
	InitializeMeshMaterials();
	FadeTimeline->Reverse();
}

// Called when the game starts
void UFadeEffectComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UFadeEffectComponent::OnFadeTimelineUpdate_Implementation(const float Alpha)
{
	for (UMeshComponent* MeshComponent : FadeMeshes)
	{
		UMaterialBlueprintLibrary::SetScalarParameterOnDynamicMaterials(MeshComponent, MaterialParameterName, Alpha);
	}
	OnFadeTimelineTickDelegate.Broadcast(Alpha);
}

void UFadeEffectComponent::OnFadeTimelineFinish_Implementation()
{
	if (FadeTimeline)
	{
		FadeTimeline->DestroyComponent();
		FadeTimeline = nullptr;
	}
	OnFadeTimelineFinishDelegate.Broadcast();
}

UTimelineComponent* UFadeEffectComponent::CreateFadeTimeline(const FName& Name)
{
	UTimelineComponent* Timeline = NewObject<UTimelineComponent>(this, Name);
	Timeline->SetComponentTickEnabled(true);
	Timeline->RegisterComponent();
	Timeline->AddInterpFloat(FadeCurve, OnFadeUpdate);
	Timeline->SetTimelineFinishedFunc(OnFadeFinish);
	return Timeline;
}

void UFadeEffectComponent::InitializeMeshMaterials()
{
	for (UMeshComponent* MeshComponent : FadeMeshes)
	{
		TArray<UMaterialInterface*> OriginalMaterials;
		TArray<UMaterialInstanceDynamic*> DynamicMaterials;
		UMaterialBlueprintLibrary::ReplaceMaterialsWithDynamicMaterials(MeshComponent, OriginalMaterials, DynamicMaterials);
		for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
		{
			DynamicMaterial->SetScalarParameterValue(MaterialOverrideParameterName, true);
		}
	}
}
