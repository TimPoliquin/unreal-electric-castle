// Copyright Alien Shores


#include "Actor/Highlight/HighlightComponent.h"

#include "Actor/Highlight/HighlightActorInterface.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
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
	if (IsValid(Mesh))
	{
		Mesh->SetRenderCustomDepth(true);
		Mesh->SetCustomDepthStencilValue(HighlightCustomDepthStencilValue);
	}
}

void UHighlightComponent::UnhighlightMesh(UMeshComponent* Mesh) const
{
	if (IsValid(Mesh))
	{
		Mesh->SetRenderCustomDepth(false);
	}
}
