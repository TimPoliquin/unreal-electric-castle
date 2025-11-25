#include "Player/Form/FormConfigTypes.h"

#include "GroomComponent.h"

bool FFormMaterialConfig::IsNull() const
{
	return MaterialAsset.IsNull();
}

bool FFormMaterialConfig::IsValid() const
{
	return MaterialAsset.IsValid();
}

bool FFormMaterialConfig::IsLoaded() const
{
	return IsNull() || IsValid();
}

void FFormMaterialConfig::SetToComponent(UMeshComponent* MeshComponent, const int32 Idx) const
{
	if (MaterialSlotName.IsNone())
	{
		MeshComponent->SetMaterial(Idx, MaterialAsset.Get());
	}
	else
	{
		MeshComponent->SetMaterialByName(MaterialSlotName, MaterialAsset.Get());
	}
}

bool FFormMeshPartConfig::IsNull() const
{
	return MeshAsset.IsNull();
}

bool FFormMeshPartConfig::IsValid() const
{
	return MeshAsset.IsValid();
}

bool FFormMeshPartConfig::IsLoaded() const
{
	if (IsValid())
	{
		return !Materials.ContainsByPredicate(
			[](const FFormMaterialConfig& Material)
			{
				return !Material.IsLoaded();
			}
		);
	}
	return false;
}

void FFormMeshPartConfig::SetToComponent(USkeletalMeshComponent* MeshComponent) const
{
	MeshComponent->SetSkeletalMesh(MeshAsset.Get());
	if (Materials.Num() > 0)
	{
		MeshComponent->GetMaterials().Empty();
		MeshComponent->GetMaterials().SetNum(Materials.Num());
		for (int Idx = 0; Idx < Materials.Num(); Idx++)
		{
			Materials[Idx].SetToComponent(MeshComponent, Idx);
		}
	}
	MeshComponent->MarkRenderStateDirty();
}

void FFormMeshPartConfig::Initialize() const
{
	MeshAsset->GetResourceForRendering(); // Force Initialization
	for (const FFormMaterialConfig& Material : Materials)
	{
		Material.MaterialAsset->EnsureIsComplete();
	}
}

bool FFormGroomConfig::IsNull() const
{
	return GroomAsset.IsNull() || GroomBindingAsset.IsNull();
}

bool FFormGroomConfig::IsValid() const
{
	return GroomAsset.IsValid() && GroomBindingAsset.IsValid();
}

bool FFormGroomConfig::IsLoaded() const
{
	return IsValid();
}

void FFormGroomConfig::Initialize() const
{
	for (const FFormMaterialConfig& Material : Materials)
	{
		Material.MaterialAsset->EnsureIsComplete();
	}
}

void FFormGroomConfig::SetToComponent(UGroomComponent* GroomComponent) const
{
	if (IsValid())
	{
		GroomComponent->SetGroomAsset(GroomAsset.Get());
		GroomComponent->SetBindingAsset(GroomBindingAsset.Get());
		GroomComponent->SetForcedLOD(0);
		GroomComponent->SetEnableSimulation(GroomAsset.Get()->IsSimulationEnable());
		GroomComponent->GetMaterials().Empty();
		GroomComponent->GetMaterials().SetNum(Materials.Num());
		for (int Idx = 0; Idx < Materials.Num(); Idx++)
		{
			Materials[Idx].SetToComponent(GroomComponent, Idx);
		}
	}
	else
	{
		GroomComponent->SetGroomAsset(nullptr);
		GroomComponent->SetBindingAsset(nullptr);
		GroomComponent->SetEnableSimulation(false);
	}
	GroomComponent->RecreateRenderState_Concurrent();
	GroomComponent->ResetSimulation();
}

bool FFormMeshConfig::IsLoaded() const
{
	return
		(Body.IsNull() || Body.IsValid()) &&
		(Face.IsNull() || Face.IsValid()) &&
		(Clothing.IsNull() || Clothing.IsValid()) &&
		(Beard.IsNull() || Beard.IsValid()) &&
		(Fuzz.IsNull() || Fuzz.IsValid()) &&
		(Eyebrows.IsNull() || Eyebrows.IsValid()) &&
		(Eyelashes.IsNull() || Eyelashes.IsValid()) &&
		(Hair.IsNull() || Hair.IsValid()) &&
		(Moustache.IsNull() || Moustache.IsValid());
}

void FFormMeshConfig::Initialize() const
{
	Body.Initialize();
	Face.Initialize();
	Clothing.Initialize();
	Beard.Initialize();
	Fuzz.Initialize();
	Eyebrows.Initialize();
	Eyelashes.Initialize();
	Hair.Initialize();
	Moustache.Initialize();
}
