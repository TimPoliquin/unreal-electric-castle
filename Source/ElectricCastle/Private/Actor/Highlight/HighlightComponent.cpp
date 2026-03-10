// Copyright Alien Shores


#include "Actor/Highlight/HighlightComponent.h"

#include "Actor/Highlight/HighlightActorInterface.h"
#include "Actor/Highlight/HighlightConfig.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"

#include "Net/UnrealNetwork.h"


UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHighlightComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHighlightComponent, bHighlightable);
}

void UHighlightComponent::AddHighlightMesh(UMeshComponent* Mesh)
{
	if (IsValid(Mesh))
	{
		HighlightMeshes.AddUnique(Mesh);
		if (bIsHighlighted)
		{
			HighlightMesh(Mesh);
		}
		else
		{
			UnhighlightMesh(Mesh);
		}
	}
}

void UHighlightComponent::RemoveHighlightMesh(UMeshComponent* Mesh)
{
	UnhighlightMesh(Mesh);
	HighlightMeshes.RemoveAll([Mesh](const TWeakObjectPtr<UMeshComponent>& OtherMesh)
	{
		return !OtherMesh.IsValid() || OtherMesh.Get() == Mesh;
	});
}

void UHighlightComponent::Highlight()
{
	if (!bHighlightable)
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Highlight not allowed!"), *GetOwner()->GetName(), *GetName());
		}
		return;
	}
	bIsHighlighted = true;
	if (HighlightMeshes.Num() == 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No highlight meshes set!"), *GetOwner()->GetName(), *GetName());
	}
	for (TWeakObjectPtr<UMeshComponent> Mesh : HighlightMeshes)
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Highlight mesh %s"), *GetOwner()->GetName(), *GetName(), *Mesh->GetName())
		}
		if (Mesh.IsValid())
		{
			HighlightMesh(Mesh.Get());
		}
	}
}

void UHighlightComponent::Unhighlight()
{
	bIsHighlighted = false;
	for (TWeakObjectPtr<UMeshComponent> Mesh : HighlightMeshes)
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Unhighlight mesh %s"), *GetOwner()->GetName(), *GetName(), *Mesh->GetName())
		}
		if (Mesh.IsValid())
		{
			UnhighlightMesh(Mesh.Get());
		}
	}
}

void UHighlightComponent::SetHighlightType(const EHighlightType InHighlightType)
{
	HighlightType = InHighlightType;
}

FLinearColor UHighlightComponent::GetHighlightColor() const
{
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetOwner());
	const UHighlightConfig* HighlightConfig = GameDataSubsystem ? GameDataSubsystem->GetHighlightConfig() : nullptr;
	return HighlightConfig ? HighlightConfig->GetHighlightConfigByHighlightType(HighlightType).HighlightColor : FLinearColor::White;
}


void UHighlightComponent::SetHighlightable(const bool bNewHighlightable)
{
	bHighlightable = bNewHighlightable;
	if (bIsHighlighted)
	{
		Unhighlight();
	}
}

void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(GetOwner()) && GetOwner()->Implements<UHighlightActorInterface>())
	{
		TArray<UMeshComponent*> MeshComponents;
		IHighlightActorInterface::Execute_GetHighlightMeshes(GetOwner(), MeshComponents);
		for (UMeshComponent* Mesh : MeshComponents)
		{
			AddHighlightMesh(Mesh);
		}
	}
}

void UHighlightComponent::HighlightMesh(UMeshComponent* Mesh) const
{
	if (const UHighlightConfig* HighlightConfig = UElectricCastleGameDataSubsystem::Get(GetOwner())->GetHighlightConfig(); IsValid(Mesh) && IsValid(HighlightConfig))
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->SetCustomDepthStencilValue(HighlightConfig->GetHighlightConfigByHighlightType(HighlightType).HighlightCode);
	}
}

void UHighlightComponent::UnhighlightMesh(UMeshComponent* Mesh) const
{
	if (IsValid(Mesh))
	{
		Mesh->SetRenderCustomDepth(false);
	}
}
