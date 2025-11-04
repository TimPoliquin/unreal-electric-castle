// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Interaction/FadeInterface.h"
#include "FadeActor.generated.h"

class UTimelineComponent;

UCLASS()
class ELECTRICCASTLE_API AFadeActor : public AActor, public IFadeInterface
{
	GENERATED_BODY()

public:
	AFadeActor();
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	virtual void BeginPlay() override;

	/** Fade Interface **/
	virtual void FadeIn_Implementation() override;
	virtual void FadeOut_Implementation() override;
	/** Fade Interface End **/
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTimelineComponent> TimelineComponent;
	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterials;
	UPROPERTY(EditAnywhere, Category = "Fade")
	TArray<UMaterialInstance*> FadeMaterialInstances;
	UPROPERTY(EditAnywhere, Category = "Fade")
	TObjectPtr<UCurveFloat> FadeCurve;
	ECollisionResponse OriginalVisibilityResponse;
	FOnTimelineFloat OnTimelineTickDelegate;
	FOnTimelineEvent OnFadeOutFinishedDelegate;
	FOnTimelineEvent OnFadeInFinishedDelegate;

	template <typename T>
	void ReplaceMaterials(TArray<T>& Materials) const;
	UFUNCTION()
	void OnFadeInComplete();
	UFUNCTION()
	void OnFadeOutComplete();
	UFUNCTION()
	void OnTimelineTick(float FadeValue);
};

template <typename T>
void AFadeActor::ReplaceMaterials(TArray<T>& Materials) const
{
	for (int32 idx = 0; idx < Materials.Num(); idx++)
	{
		MeshComponent->SetMaterial(idx, Materials[idx]);
	}
}
