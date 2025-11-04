// Copyright Alien Shores


#include "Material/MaterialBlueprintLibrary.h"

#include "Kismet/KismetMaterialLibrary.h"

void UMaterialBlueprintLibrary::ReplaceMaterialsWithDynamicMaterials(UMeshComponent* Mesh, TArray<UMaterialInterface*>& OriginalMaterials, TArray<UMaterialInstanceDynamic*>& DynamicMaterials)
{
	OriginalMaterials.Append(Mesh->GetMaterials());
	for (int Idx = 0; Idx < Mesh->GetNumMaterials(); Idx++)
	{
		UMaterialInterface* Material = Mesh->GetMaterial(Idx);
		if (UMaterialInstanceDynamic* DynamicMaterialInstance = Cast<UMaterialInstanceDynamic>(Material))
		{
			DynamicMaterials.Add(DynamicMaterialInstance);
		}
		else
		{
			DynamicMaterials.Add(
				UKismetMaterialLibrary::CreateDynamicMaterialInstance(
					Mesh,
					Material
				)
			);
		}
		Mesh->SetMaterial(Idx, DynamicMaterials[Idx]);
	}
}

void UMaterialBlueprintLibrary::SetScalarParameterOnDynamicMaterials(UMeshComponent* Mesh, const FName Parameter, const float Value)
{
	for (UMaterialInterface* Material : Mesh->GetMaterials())
	{
		if (UMaterialInstanceDynamic* DynamicMaterialInstance = Cast<UMaterialInstanceDynamic>(Material))
		{
			DynamicMaterialInstance->SetScalarParameterValue(Parameter, Value);
		}
	}
}

void UMaterialBlueprintLibrary::SetBooleanParameterOnDynamicMaterials(UMeshComponent* Mesh, const FName Parameter, const bool Value)
{
	for (UMaterialInterface* Material : Mesh->GetMaterials())
	{
		if (UMaterialInstanceDynamic* DynamicMaterialInstance = Cast<UMaterialInstanceDynamic>(Material))
		{
			DynamicMaterialInstance->SetScalarParameterValue(Parameter, Value);
		}
	}
}
