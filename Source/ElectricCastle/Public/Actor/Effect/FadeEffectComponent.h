// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "FadeEffectComponent.generated.h"

class FOnTimelineFloat;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFadeTimelineUpdateSignature, const float, Alpha);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeTimelineFinishSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UFadeEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFadeEffectComponent();

	UPROPERTY(BlueprintAssignable)
	FOnFadeTimelineUpdateSignature OnFadeTimelineTickDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnFadeTimelineFinishSignature OnFadeTimelineFinishDelegate;

	UFUNCTION(BlueprintCallable)
	void AddFadeMesh(UMeshComponent* MeshComponent);
	UFUNCTION(BlueprintNativeEvent)
	void FadeOut();
	UFUNCTION(BlueprintNativeEvent)
	void FadeIn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> FadeCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MaterialOverrideParameterName = TEXT("Explicitly Override Opacity?");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName MaterialParameterName = TEXT("Explicit Opacity Override");
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UMeshComponent>> FadeMeshes;
	UFUNCTION(BlueprintNativeEvent)
	void OnFadeTimelineUpdate(float Alpha);
	UFUNCTION(BlueprintNativeEvent)
	void OnFadeTimelineFinish();

private:
	UTimelineComponent* CreateFadeTimeline(const FName& Name);
	void InitializeMeshMaterials();
	UPROPERTY()
	TObjectPtr<UTimelineComponent> FadeTimeline;
	FOnTimelineFloat OnFadeUpdate;
	FOnTimelineEvent OnFadeFinish;
};
