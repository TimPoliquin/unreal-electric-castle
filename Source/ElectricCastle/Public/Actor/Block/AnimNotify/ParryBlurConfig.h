// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ParryBlurConfig.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew)
class ELECTRICCASTLE_API UParryBlurConfig : public UObject
{
	GENERATED_BODY()

public:
	void Start(const float InDuration);
	void Tick(const float InDeltaTime);
	float GetBlurValue() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Blur")
	float BlurIntensity = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Blur")
	float FadeInTime = .05f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Blur")
	float FadeOutTime = 0.05f;

private:
	float Duration = 0.f;
	float FadeOutStartTime = 0.f;
	float CurrentTime = 0.f;
};
