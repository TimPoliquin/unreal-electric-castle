// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "BlockController.generated.h"


class UParryBlurConfig;
class UCameraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UBlockController : public UActorComponent
{
	GENERATED_BODY()

public:
	UBlockController();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsBlocking() const;
	UFUNCTION(BlueprintCallable)
	void StartBlocking();
	UFUNCTION(BlueprintCallable)
	void EndBlocking();
	UFUNCTION(BlueprintCallable)
	void InitializeParryCamera(UCameraComponent* CameraComponent);
	UFUNCTION(BlueprintCallable)
	void StartParryBlur(UParryBlurConfig* InParryBlurConfig);
	UFUNCTION(BlueprintCallable)
	void EndParryBlur();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag BlockingTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BlockingMovementSpeed = 200.f;
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	bool bIsBlocking = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> ParryCameraBlurMaterial;

private:
	void SetParryBlurStrength(const float ParryBlurStrength) const;

	UPROPERTY()
	TObjectPtr<UParryBlurConfig> ParryBlurConfig;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> ParryCameraBlurDynamicMaterial;

	float RestoreMaxSpeed = 0.f;
	FActiveGameplayEffectHandle BlockingEffectHandle;
};
