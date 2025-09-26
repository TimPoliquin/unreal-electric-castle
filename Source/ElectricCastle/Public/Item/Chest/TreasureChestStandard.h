// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "TreasureChestActor.h"
#include "Components/TimelineComponent.h"
#include "TreasureChestStandard.generated.h"

class UFadeEffectComponent;
class UTimelineComponent;
class UArrowComponent;
class UBoxComponent;
class UPointLightComponent;
class UNiagaraComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ATreasureChestStandard : public ATreasureChestActor
{
	GENERATED_BODY()

public:
	ATreasureChestStandard();

	/** Start IHighlightInterface **/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** End IHighlightInterface **/

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UArrowComponent> ArrowComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> BoxCollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> ChestBaseMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LidMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> LootMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UPointLightComponent> LootLightComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> LootEffectComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> LockRingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UFadeEffectComponent> FadeEffectComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	float LidUnlockAngle = 25.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	float LidOpenAngle = 90.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UCurveFloat> UnlockCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	TObjectPtr<UCurveFloat> OpenCurve;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeHighlight();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeSwitchLockEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeInteractionWidget();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeLootLightIntensity();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeLootFadeEffect();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchLockRing_Activate();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchLockRing_Deactivate();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TreasureEffect_Activate();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LootFade_Update(const float Alpha);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LootFade_Finish();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnlockEffect_Update(float Alpha);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnlockEffect_Finish();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenEffect_Update(float Alpha);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenEffect_Finish();

	/** Overrides **/
	virtual void PlayUnlockEffect_Implementation() override;
	virtual void PlayUnlockForcedEffect_Implementation() override;
	virtual void PlayOpenEffect_Implementation(AActor* Player) override;
	virtual void PlayOpenForcedEffect_Implementation() override;
	virtual void GrantRewards_Finish_Implementation() override;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> LootMeshDynamicMaterial;
	float TreasureLightIntensity = 0;
	UPROPERTY()
	TObjectPtr<UTimelineComponent> UnlockEffectTimeline;
	UPROPERTY()
	TObjectPtr<UTimelineComponent> OpenEffectTimeline;
	UPROPERTY()
	TObjectPtr<UTimelineComponent> SwitchLockEffectTimeline;
	FOnTimelineFloat OnUnlockUpdate;
	FOnTimelineEvent OnUnlockFinish;
	FOnTimelineFloat OnOpenUpdate;
	FOnTimelineEvent OnOpenFinish;
};
