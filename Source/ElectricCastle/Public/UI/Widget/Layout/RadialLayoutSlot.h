// RadialLayoutSlot.h
#pragma once

#include "CoreMinimal.h"
#include "Components/PanelSlot.h"
#include "RadialLayoutSlot.generated.h"

/**
 * Slot for the Radial Layout panel
 */
UCLASS()
class ELECTRICCASTLE_API URadialLayoutSlot : public UPanelSlot
{
	GENERATED_BODY()

public:
	URadialLayoutSlot(const FObjectInitializer& ObjectInitializer);

	/** Custom angle offset for this specific child (added to base angle) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	float AngleOffset = 0.0f;

	/** Custom radius multiplier for this specific child */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	float RadiusMultiplier = 1.0f;

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	virtual void BuildSlot(TSharedRef<class SRadialLayout> InRadialLayout);

	TSharedPtr<class SRadialLayout> RadialLayout;
};
