// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "CloakComponent.generated.h"

UENUM(BlueprintType)
enum class ECloakFadeDirection : uint8
{
	None,
	FadeOut,
	FadeIn
};

class UMaterialInstanceDynamic;
class UGameplayEffect;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UCloakComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCloakComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartCloak();
	UFUNCTION(BlueprintCallable)
	void EndCloak();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|GameplayEffect")
	TSubclassOf<UGameplayEffect> CloakEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|Materials")
	TObjectPtr<UMaterialInterface> StealthMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|Materials")
	FName CloakMaterialStencilParameterName = FName("StencilCode");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|Materials", meta=(UIMin=1, UIMax=250))
	int32 CloakMaterialStencilCode = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|Materials")
	FName CloakMaterialParameterName = FName("Alpha");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|Materials")
	float CloakMaterialChangeDuration = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cloak|Materials", meta=(UIMin=0, UIMax=1))
	float CloakMaterialMinAlpha = .5f;
	UPROPERTY(BlueprintReadOnly)
	bool bIsCloaked = false;
	UPROPERTY(BlueprintReadOnly)
	ECloakFadeDirection FadeDirection = ECloakFadeDirection::None;
	UPROPERTY(EditAnywhere, Category="Cloak")
	bool bDebug = false;

private:
	void UpdateParentStencilValues(bool bInEnableCustomDepth);
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> StealthMaterialDynamic;
	float CloakMaterialChangeProgress = 0.f;
	FActiveGameplayEffectHandle CloakEffectHandle;
};
