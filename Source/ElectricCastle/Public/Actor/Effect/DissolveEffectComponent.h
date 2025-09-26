#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Engine.h"
#include "DissolveEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDissolveEffectCompletedSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UDissolveEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDissolveEffectComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:
	// Float curve for the timeline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline")
	TObjectPtr<UCurveFloat> TimelineCurve;

	// Material to modify
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	TObjectPtr<UMaterialInterface> DissolvableMaterial;

	// Dynamic material instance (created at runtime)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Material")
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	// Parameter name in the material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	FName MaterialParameterName = TEXT("Dissolve");

	// Mesh component to apply material to (optional - can be set externally)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Material")
	TObjectPtr<UMeshComponent> TargetMeshComponent;

	// Timeline settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
	bool bAutoPlay = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
	bool bLoop = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline Settings")
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline Control")
	FOnDissolveEffectCompletedSignature OnDissolveEffectCompleted;

	// Functions to control timeline
	UFUNCTION(BlueprintCallable, Category = "Timeline Control")
	void PlayTimeline();

	UFUNCTION(BlueprintCallable, Category = "Timeline Control")
	void PlayTimelineFromStart();

	UFUNCTION(BlueprintCallable, Category = "Timeline Control")
	void PlayTimelineReverse();

	UFUNCTION(BlueprintCallable, Category = "Timeline Control")
	void PlayTimelineReverseFromEnd();

	UFUNCTION(BlueprintCallable, Category = "Timeline Control")
	void StopTimeline();

	UFUNCTION(BlueprintCallable, Category = "Timeline Control")
	void SetTimelinePosition(float NewPosition);

	// Function to set target mesh component
	UFUNCTION(BlueprintCallable, Category = "Material")
	void SetTargetMeshComponent(UMeshComponent* MeshComponent);

private:
	UPROPERTY()
	TObjectPtr<UTimelineComponent> Timeline;
	void Initialize();
	void InitializeTimeline();
	// Function to initialize material
	UFUNCTION(BlueprintCallable, Category = "Material")
	void InitializeMaterial();
	// Timeline callback function
	UFUNCTION()
	void OnTimelineUpdate(float Value);

	// Timeline finished callback
	UFUNCTION()
	void OnTimelineFinished();

	// Float track for the timeline
	FOnTimelineFloat TimelineUpdateDelegate;
	FOnTimelineEvent TimelineFinishedDelegate;
};
