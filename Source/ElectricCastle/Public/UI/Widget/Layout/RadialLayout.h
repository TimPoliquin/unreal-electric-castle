// RadialLayout.h
#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "RadialLayout.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChildSelected, UWidget*, SelectedChild);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChildHovered, UWidget*, HoveredChild);

/**
 * A panel that arranges child widgets in a radial/circular layout
 */
UCLASS()
class ELECTRICCASTLE_API URadialLayout : public UPanelWidget
{
	GENERATED_BODY()

public:
	URadialLayout(const FObjectInitializer& ObjectInitializer);

	/** Event dispatched when a child is selected via gamepad */
	UPROPERTY(BlueprintAssignable, Category = "Radial Layout|Events")
	FOnChildSelected OnChildSelected;

	/** Event dispatched when a child is hovered by mouse */
	UPROPERTY(BlueprintAssignable, Category = "Radial Layout|Events")
	FOnChildHovered OnChildHovered;

	/** Get the currently selected child index */
	UFUNCTION(BlueprintCallable, Category = "Radial Layout")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	/** Set the selected child by index */
	UFUNCTION(BlueprintCallable, Category = "Radial Layout")
	void SetSelectedIndex(int32 Index);

	/** Get the currently selected child widget */
	UFUNCTION(BlueprintCallable, Category = "Radial Layout")
	UWidget* GetSelectedChild() const;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	// Input handling (called from Slate widget)
	FReply HandleKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	FReply HandleAnalogInput(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogEvent);
	void HandleMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void HandleMouseLeave(const FPointerEvent& MouseEvent);
	// Helper to update hover state and fire event
	void UpdateHoveredChild(UWidget* NewHoveredChild);

	bool ShouldRotateChildrenToAngle() const { return bRotateChildrenToAngle; }
	float GetStartAngle() const { return StartAngle; }
	bool IsClockwise() const { return bClockwise; }
	bool IsGamepadNavigationEnabled() const { return bEnableGamepadNavigation; }
	bool IsDebugOverlayEnabled() const { return bDebugOverlay; }

protected:
	/** Radius of the radial layout */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Layout")
	float Radius = 200.0f;

	/** Starting angle in degrees (0 = right, 90 = up, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Layout")
	float StartAngle = 0.0f;

	/** Whether to distribute children clockwise or counter-clockwise */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Layout")
	bool bClockwise = true;

	/** Whether to enable gamepad/keyboard navigation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Layout")
	bool bEnableGamepadNavigation = true;

	/** Whether to rotate each child to match its angle on the circle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Layout")
	bool bRotateChildrenToAngle = false;

	/** Enable debug overlay painting (radial lines, outward arrows, angle labels) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radial Layout|Debug")
	bool bDebugOverlay = false;

	// UPanelWidget interface
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* InSlot) override;
	virtual void OnSlotRemoved(UPanelSlot* InSlot) override;

	// Slate widget
	TSharedPtr<class SRadialLayout> MyRadialLayout;

	// Currently selected child index for gamepad navigation
	int32 SelectedIndex = -1;

	// Currently hovered child for mouse interaction
	TWeakObjectPtr<UWidget> HoveredChild;

	// Helper to update selection
	void UpdateSelection(int32 NewIndex);

	// Helper to find the nearest child to a given angle
	int32 FindNearestChildToAngle(float Angle) const;

	// Last analog input for stick navigation
	FVector2D LastAnalogInput;
	float AnalogDeadzone = 0.3f;

	// Allow slot to access slate widget for invalidation
	friend class URadialLayoutSlot;
};
