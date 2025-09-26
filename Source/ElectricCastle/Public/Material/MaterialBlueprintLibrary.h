// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MaterialBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UMaterialBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "AuraMaterialBlueprintLibrary|DynamicMaterials")
	static void ReplaceMaterialsWithDynamicMaterials(UMeshComponent* Mesh, TArray<UMaterialInterface*>& OriginalMaterials, TArray<UMaterialInstanceDynamic*>& DynamicMaterials);
	UFUNCTION(BlueprintCallable, Category = "AuraMaterialBlueprintLibrary|DynamicMaterials")
	static void SetScalarParameterOnDynamicMaterials(UMeshComponent* Mesh, const FName Parameter, const float Value);
	UFUNCTION(BlueprintCallable, Category = "AuraMaterialBlueprintLibrary|DynamicMaterials")
	static void SetBooleanParameterOnDynamicMaterials(UMeshComponent* Mesh, const FName Parameter, const bool Value);
};
