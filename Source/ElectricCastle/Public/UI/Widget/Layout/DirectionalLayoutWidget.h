#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "DirectionalLayoutSlot.h"
#include "Widgets/SBoxPanel.h"
#include "DirectionalLayoutWidget.generated.h"

UENUM(BlueprintType)
enum class ELayoutDirection : uint8
{
	LeftToRight UMETA(DisplayName = "Left to Right"),
	RightToLeft UMETA(DisplayName = "Right to Left"),
	TopToBottom UMETA(DisplayName = "Top to Bottom"),
	BottomToTop UMETA(DisplayName = "Bottom to Top")
};

UCLASS()
class ELECTRICCASTLE_API UDirectionalLayoutWidget : public UPanelWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Layout")
	void SetLayoutDirection(ELayoutDirection NewDirection);

	// Convenience: add a child and return its slot for configuration
	UFUNCTION(BlueprintCallable, Category = "Layout")
	UDirectionalLayoutSlot* AddChildToDirectionalLayout(UWidget* Content);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Layout")
	ELayoutDirection LayoutDirection = ELayoutDirection::LeftToRight;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	void RebuildChildren();

	bool IsHorizontal() const
	{
		return LayoutDirection == ELayoutDirection::LeftToRight ||
			LayoutDirection == ELayoutDirection::RightToLeft;
	}

	bool IsReversed() const
	{
		return LayoutDirection == ELayoutDirection::RightToLeft ||
			LayoutDirection == ELayoutDirection::BottomToTop;
	}

	TSharedPtr<SBox> MyBox;
	TSharedPtr<SHorizontalBox> MyHBox;
	TSharedPtr<SVerticalBox> MyVBox;
};
