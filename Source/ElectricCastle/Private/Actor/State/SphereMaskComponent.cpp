// SphereMaskComponent.cpp
#include "Actor/State/SphereMaskComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

USphereMaskComponent::USphereMaskComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USphereMaskComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USphereMaskComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bSphereMaskActive && bAutoUpdateSphereCenter && SphereSourceActor)
	{
		UpdateSphereCenter(SphereSourceActor->GetActorLocation());
	}
}

void USphereMaskComponent::ApplySphereMask(
	AActor* InSphereSourceActor,
	const float InSphereRadius,
	const bool bAutoUpdate
)
{
	SphereSourceActor = InSphereSourceActor;
	CurrentSphereRadius = InSphereRadius;
	bAutoUpdateSphereCenter = bAutoUpdate;
	UpdateSphereCenter(SphereSourceActor->GetActorLocation());
	if (!SphereMaskMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("SphereMaskComponent: No SphereMaskMaterial set!"));
		return;
	}
	bSphereMaskActive = true;

	ApplyMaterialToComponents();

	// Enable tick if auto-updating
	if (bAutoUpdateSphereCenter)
	{
		SetComponentTickEnabled(true);
	}
}

void USphereMaskComponent::RemoveSphereMask()
{
	if (!bSphereMaskActive)
	{
		return;
	}

	RestoreMaterialsToComponents();

	bSphereMaskActive = false;
	SetComponentTickEnabled(false);
}

void USphereMaskComponent::UpdateSphereCenter(FVector NewCenter)
{
	if (!bSphereMaskActive)
	{
		return;
	}

	CurrentSphereCenter = NewCenter;
	UpdateMaterialParameters();
}

void USphereMaskComponent::ApplyMaterialToComponents()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Component : PrimitiveComponents)
	{
		if (!Component || !Component->IsVisible())
		{
			continue;
		}

		// Skip if already processed
		if (OriginalMaterials.Contains(Component))
		{
			continue;
		}

		TArray<UMaterialInterface*> SavedMaterials;
		TArray<UMaterialInstanceDynamic*> NewDynamicMaterials;

		int32 NumMaterials = Component->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			// Save original material
			UMaterialInterface* OriginalMat = Component->GetMaterial(i);
			SavedMaterials.Add(OriginalMat);

			// Create dynamic material instance
			UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(
				SphereMaskMaterial,
				Component
			);

			if (DynMat)
			{
				// Set initial parameters
				DynMat->SetVectorParameterValue(FName("SphereCenter"), CurrentSphereCenter);
				DynMat->SetScalarParameterValue(FName("SphereRadius"), CurrentSphereRadius);

				// Apply material
				Component->SetMaterial(i, DynMat);
				NewDynamicMaterials.Add(DynMat);
			}
		}

		OriginalMaterials.Add(Component, SavedMaterials);
		DynamicMaterials.Add(Component, NewDynamicMaterials);
	}
}

void USphereMaskComponent::RestoreMaterialsToComponents()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	for (const TPair<UPrimitiveComponent*, TArray<UMaterialInterface*>>& Pair : OriginalMaterials)
	{
		UPrimitiveComponent* Component = Pair.Key;
		const TArray<UMaterialInterface*>& SavedMaterials = Pair.Value;

		if (!Component)
		{
			continue;
		}

		for (int32 i = 0; i < SavedMaterials.Num(); ++i)
		{
			Component->SetMaterial(i, SavedMaterials[i]);
		}
	}

	OriginalMaterials.Empty();
	DynamicMaterials.Empty();
}

void USphereMaskComponent::UpdateMaterialParameters()
{
	for (const TPair<UPrimitiveComponent*, TArray<UMaterialInstanceDynamic*>>& Pair : DynamicMaterials)
	{
		const TArray<UMaterialInstanceDynamic*>& DynMats = Pair.Value;

		for (UMaterialInstanceDynamic* DynMat : DynMats)
		{
			if (DynMat)
			{
				DynMat->SetVectorParameterValue(FName("SphereCenter"), CurrentSphereCenter);
			}
		}
	}
}
