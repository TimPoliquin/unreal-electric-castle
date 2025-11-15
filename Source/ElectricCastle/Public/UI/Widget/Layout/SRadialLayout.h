// SRadialLayout.h
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SPanel.h"
#include "Layout/Children.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

/**
 * Slate panel that arranges children in a radial layout
 */
class ELECTRICCASTLE_API SRadialLayout : public SPanel
{
public:
	// Slot for radial layout
	class FSlot : public TSlotBase<FSlot>
	{
	public:
		FSlot()
			: TSlotBase<FSlot>(),
			  AngleOffset(0.0f),
			  RadiusMultiplier(1.0f)
		{
		}

		// Allow TPanelChildren to construct slots from slot-args by value or by move
		FSlot(const FSlotArguments& /*InArgs*/)
			: TSlotBase<FSlot>(),
			  AngleOffset(0.0f),
			  RadiusMultiplier(1.0f)
		{
		}

		FSlot(FSlotArguments&& /*InArgs*/)
			: TSlotBase<FSlot>(),
			  AngleOffset(0.0f),
			  RadiusMultiplier(1.0f)
		{
		}

		SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
		SLATE_SLOT_END_ARGS()

		void Construct(const FChildren& SlotOwner, FSlotArguments&& InArgs)
		{
			// This is where the widget gets bound to the slot
			TSlotBase<FSlot>::Construct(SlotOwner, MoveTemp(InArgs));
		}

		FSlot& SetAngleOffset(float InAngleOffset)
		{
			AngleOffset = InAngleOffset;
			return *this;
		}

		FSlot& SetRadiusMultiplier(float InRadiusMultiplier)
		{
			RadiusMultiplier = InRadiusMultiplier;
			return *this;
		}

		float AngleOffset;
		float RadiusMultiplier;
	};

	// ... (rest of class SRadialLayout)

	SLATE_BEGIN_ARGS(SRadialLayout)
			: _Radius(200.0f),
			  _StartAngle(0.0f),
			  _bClockwise(true),
			  _Owner(nullptr)
		{
		}

		SLATE_ARGUMENT(float, Radius)
		SLATE_ARGUMENT(float, StartAngle)
		SLATE_ARGUMENT(bool, bClockwise)
		SLATE_ARGUMENT(class URadialLayout*, Owner)
		SLATE_SLOT_ARGUMENT(SRadialLayout::FSlot, Slots)
	SLATE_END_ARGS()

	SRadialLayout();

	void Construct(const FArguments& InArgs);

	using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

	FScopedWidgetSlotArguments AddSlot()
	{
		return FScopedWidgetSlotArguments{MakeUnique<FSlot>(), Children, INDEX_NONE};
	}

	int32 GetNumSlots() const { return Children.Num(); }

	FSlot& GetSlot(int32 SlotIndex)
	{
		return Children[SlotIndex];
	}

	// SWidget interface
	virtual void OnArrangeChildren(
		const FGeometry& AllottedGeometry,
		FArrangedChildren& ArrangedChildren
	) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	virtual int32 OnPaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled
	) const override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual bool SupportsKeyboardFocus() const override { return true; }

	void SetRadius(float InRadius) { Radius = InRadius; }
	void SetStartAngle(float InStartAngle) { StartAngle = InStartAngle; }
	void SetClockwise(bool bInClockwise) { bClockwise = bInClockwise; }

	int32 RemoveSlot(const TSharedRef<SWidget>& SlotWidget);
	void ClearChildren();

protected:
	TPanelChildren<FSlot> Children;
	float Radius;
	float StartAngle;
	bool bClockwise;
	TWeakObjectPtr<class URadialLayout> Owner;

	FVector2D GetChildPosition(int32 ChildIndex, int32 TotalChildren, const FSlot& Slot) const;
};
