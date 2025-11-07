#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SphereMaskComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class USphereMaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USphereMaskComponent();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	// Apply sphere mask effect to this actor
	UFUNCTION(BlueprintCallable, Category = "Sphere Mask")
	void ApplySphereMask(AActor* InSphereSourceActor, float InSphereRadius, bool bAutoUpdate);

	// Remove sphere mask effect from this actor
	UFUNCTION(BlueprintCallable, Category = "Sphere Mask")
	void RemoveSphereMask();

	// Check if sphere mask is currently applied
	UFUNCTION(BlueprintPure, Category = "Sphere Mask")
	bool IsSphereMaskActive() const { return bSphereMaskActive; }

	// Update sphere center position (when effect source is moving)
	UFUNCTION(BlueprintCallable, Category = "Sphere Mask")
	void UpdateSphereCenter(FVector NewCenter);

protected:
	virtual void BeginPlay() override;

	// Material to use for sphere masking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Mask")
	UMaterialInterface* SphereMaskMaterial;

	// Whether to automatically update sphere center each tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Mask")
	bool bAutoUpdateSphereCenter = false;

	// Actor to track for sphere center (if auto-updating)
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Sphere Mask",
		meta = (EditCondition = "bAutoUpdateSphereCenter")
	)
	AActor* SphereSourceActor;

private:
	bool bSphereMaskActive = false;
	FVector CurrentSphereCenter;
	float CurrentSphereRadius;

	// Store original materials for restoration
	TMap<UPrimitiveComponent*, TArray<UMaterialInterface*>> OriginalMaterials;

	// Store dynamic material instances
	TMap<UPrimitiveComponent*, TArray<UMaterialInstanceDynamic*>> DynamicMaterials;

	void ApplyMaterialToComponents();
	void RestoreMaterialsToComponents();
	void UpdateMaterialParameters();
};
