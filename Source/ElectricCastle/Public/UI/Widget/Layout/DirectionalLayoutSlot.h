#pragma once

#include "CoreMinimal.h"
#include "Components/PanelSlot.h"
#include "Widgets/SBoxPanel.h"
#include "DirectionalLayoutSlot.generated.h"

UCLASS()
class ELECTRICCASTLE_API UDirectionalLayoutSlot : public UPanelSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	FMargin Padding = FMargin(0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	TEnumAsByte<EHorizontalAlignment> HAlign = HAlign_Fill;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout")
	TEnumAsByte<EVerticalAlignment> VAlign = VAlign_Fill;

	// Size fill (0 = auto, >0 = proportional fill)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Layout", meta = (ClampMin = "0.0"))
	float Fill = 0.f;


	void BuildHorizontalSlot(SHorizontalBox::FScopedWidgetSlotArguments& SlotArgs);
	void BuildVerticalSlot(SVerticalBox::FScopedWidgetSlotArguments& SlotArgs);

	// UPanelSlot interface
	virtual void SynchronizeProperties() override;

private:
	SHorizontalBox::FSlot* HSlot = nullptr;
	SVerticalBox::FSlot* VSlot = nullptr;
};
