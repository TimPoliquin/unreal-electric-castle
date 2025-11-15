// SRadialLayout.cpp
#include "UI/Widget/Layout/SRadialLayout.h"
#include "Layout/LayoutUtils.h"
#include "UI/Widget/Layout/RadialLayout.h"

SRadialLayout::SRadialLayout()
	: Children(this),
	  Radius(200.0f),
	  StartAngle(0.0f),
	  bClockwise(true),
	  Owner(nullptr)
{
	SetCanTick(false);
	bCanSupportFocus = true;
}

void SRadialLayout::Construct(const FArguments& InArgs)
{
	Radius = InArgs._Radius;
	StartAngle = InArgs._StartAngle;
	bClockwise = InArgs._bClockwise;
	Owner = InArgs._Owner;

	Children.Empty();
	Children.Reserve(InArgs._Slots.Num());

	// Make a mutable view of the declarative slot array so we can move elements
	auto& MutableSlots = const_cast<TArray<FSlot::FSlotArguments>&>(InArgs._Slots);

	for (int32 i = 0; i < MutableSlots.Num(); ++i)
	{
		// Add each slot by moving its arguments into TPanelChildren
		Children.AddSlot(MoveTemp(MutableSlots[i]));
	}
}

void SRadialLayout::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	const int32 NumChildren = Children.Num();
	if (NumChildren == 0)
	{
		return;
	}

	// Center
	const FVector2D LocalCenter = AllottedGeometry.GetLocalSize() * 0.5f;
	const FVector2f LocalCenterF(LocalCenter);

	const float AngleStep = 360.f / NumChildren;
	const float BaseAngle = StartAngle + 90.0f; // North origin

	for (int32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
	{
		const FSlot& CurSlot = Children[ChildIndex];
		const TSharedRef<SWidget>& CurWidget = CurSlot.GetWidget();

		if (CurWidget->GetVisibility() != EVisibility::Collapsed)
		{
			const FVector2f ChildDesiredSizeF(CurWidget->GetDesiredSize());

			// Placement angle (used for both position and rotation)
			float AngleDeg = BaseAngle
				+ (bClockwise
					   ? -AngleStep * ChildIndex
					   : AngleStep * ChildIndex)
				+ CurSlot.AngleOffset;

			// Position on the circle
			const float AngleRad = FMath::DegreesToRadians(AngleDeg);
			const float EffectiveRadius = Radius * CurSlot.RadiusMultiplier;

			FVector2f ChildPositionF;
			ChildPositionF.X = FMath::Cos(AngleRad) * EffectiveRadius;
			ChildPositionF.Y = -FMath::Sin(AngleRad) * EffectiveRadius; // screen-space Y down

			// Center the child at its position
			ChildPositionF += LocalCenterF - (ChildDesiredSizeF * 0.5f);

			// Rotate so the child's top faces outward:
			// This matches your expected set:
			// idx0:   0°, idx1:  45°, idx2:  90°, idx3: 135°,
			// idx4: 180°, idx5: 225°, idx6: 270°, idx7: 315°
			float RotationAngleDeg = 0.f;
			if (Owner.IsValid() && Owner->ShouldRotateChildrenToAngle())
			{
				RotationAngleDeg = 90.0f - AngleDeg;
			}

			const FSlateLayoutTransform LayoutTransform(ChildPositionF); // translation
			const FSlateRenderTransform RenderTransform(FQuat2D(FMath::DegreesToRadians(RotationAngleDeg)));
			const FVector2f Pivot(0.5f, 0.5f); // rotate around center

			const FGeometry ChildGeometry = AllottedGeometry.MakeChild(
				ChildDesiredSizeF,
				LayoutTransform,
				RenderTransform,
				Pivot
			);

			ArrangedChildren.AddWidget(FArrangedWidget(CurWidget, ChildGeometry));
		}
	}
}

FVector2D SRadialLayout::ComputeDesiredSize(float) const
{
	// Desired size should accommodate the radius plus the largest child
	FVector2D MaxChildSize = FVector2D::ZeroVector;

	for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
	{
		const FSlot& CurSlot = Children[ChildIndex];
		const TSharedRef<SWidget>& CurWidget = CurSlot.GetWidget();

		if (CurWidget->GetVisibility() != EVisibility::Collapsed)
		{
			FVector2D ChildDesiredSize = CurWidget->GetDesiredSize();
			MaxChildSize.X = FMath::Max(MaxChildSize.X, ChildDesiredSize.X);
			MaxChildSize.Y = FMath::Max(MaxChildSize.Y, ChildDesiredSize.Y);
		}
	}

	// Size needs to fit the radius plus half the largest child on each side
	float MaxChildDimension = FMath::Max(MaxChildSize.X, MaxChildSize.Y);
	float TotalSize = (Radius * 2.0f) + MaxChildDimension;

	return FVector2D(TotalSize, TotalSize);
}

FChildren* SRadialLayout::GetChildren()
{
	return &Children;
}

int32 SRadialLayout::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled
) const
{
	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	OnArrangeChildren(AllottedGeometry, ArrangedChildren);

	const bool bShouldBeEnabled = ShouldBeEnabled(bParentEnabled);
	int32 MaxLayerId = LayerId;

	// Paint children
	for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
	{
		const FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];
		const int32 CurMax = CurWidget.Widget->Paint(
			Args.WithNewParent(this),
			CurWidget.Geometry,
			MyCullingRect,
			OutDrawElements,
			LayerId,
			InWidgetStyle,
			bShouldBeEnabled
		);
		MaxLayerId = FMath::Max(MaxLayerId, CurMax);
	}

	// Debug overlay only if enabled
	if (Owner.IsValid() && Owner->IsDebugOverlayEnabled())
	{
		const int32 NumChildren = Children.Num();
		if (NumChildren > 0)
		{
			const FVector2f Center = FVector2f(AllottedGeometry.GetLocalSize() * 0.5f);
			const float AngleStep = 360.f / NumChildren;
			const float BaseAngle = StartAngle + 90.0f; // North origin

			constexpr FLinearColor RadialColor(0.2f, 0.7f, 1.f, 0.8f);
			constexpr FLinearColor OutwardColor(1.f, 0.2f, 0.2f, 0.9f);

			for (int32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
			{
				const FSlot& CurSlot = Children[ChildIndex];

				float AngleDeg = BaseAngle
					+ (bClockwise
						   ? -AngleStep * ChildIndex
						   : AngleStep * ChildIndex)
					+ CurSlot.AngleOffset;

				const float AngleRad = FMath::DegreesToRadians(AngleDeg);
				const float EffectiveRadius = Radius * CurSlot.RadiusMultiplier;

				FVector2f RadialEnd(
					Center.X + FMath::Cos(AngleRad) * EffectiveRadius,
					Center.Y - FMath::Sin(AngleRad) * EffectiveRadius
				);

				// Radial line
				FSlateDrawElement::MakeLines(
					OutDrawElements,
					MaxLayerId + 1,
					AllottedGeometry.ToPaintGeometry(),
					TArray<FVector2f>{Center, RadialEnd},
					ESlateDrawEffect::None,
					RadialColor,
					true,
					2.f
				);

				// Outward arrow
				const float OutwardRad = FMath::DegreesToRadians(AngleDeg + 90.f);
				const FVector2f ArrowEnd(
					RadialEnd.X + FMath::Cos(OutwardRad) * 40.f,
					RadialEnd.Y - FMath::Sin(OutwardRad) * 40.f
				);

				FSlateDrawElement::MakeLines(
					OutDrawElements,
					MaxLayerId + 1,
					AllottedGeometry.ToPaintGeometry(),
					TArray<FVector2f>{RadialEnd, ArrowEnd},
					ESlateDrawEffect::None,
					OutwardColor,
					true,
					2.f
				);
			}
		}
	}

	return MaxLayerId + 2;
}

int32 SRadialLayout::RemoveSlot(const TSharedRef<SWidget>& SlotWidget)
{
	for (int32 SlotIdx = 0; SlotIdx < Children.Num(); ++SlotIdx)
	{
		if (SlotWidget == Children[SlotIdx].GetWidget())
		{
			Children.RemoveAt(SlotIdx);
			return SlotIdx;
		}
	}

	return -1;
}

void SRadialLayout::ClearChildren()
{
	Children.Empty();
}

FVector2D SRadialLayout::GetChildPosition(int32 ChildIndex, int32 TotalChildren, const FSlot& Slot) const
{
	if (TotalChildren == 0)
	{
		return FVector2D::ZeroVector;
	}

	// Calculate base angle for this child
	float AngleStep = 360.0f / TotalChildren;
	// NEW: rotate base by 90 degrees to start at North
	float Angle = StartAngle + 90.0f + (bClockwise
		                                    ? -AngleStep * ChildIndex
		                                    : AngleStep * ChildIndex);

	// Add custom angle offset from slot
	Angle += Slot.AngleOffset;

	// Convert to radians
	float AngleRad = FMath::DegreesToRadians(Angle);

	// Calculate effective radius with slot multiplier
	float EffectiveRadius = Radius * Slot.RadiusMultiplier;

	// Calculate position
	// Note: Unreal's Y axis points down, so we negate the Y component
	FVector2D Position;
	Position.X = FMath::Cos(AngleRad) * EffectiveRadius;
	Position.Y = -FMath::Sin(AngleRad) * EffectiveRadius; // Negate for correct screen space

	return Position;
}

FReply SRadialLayout::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (Owner.IsValid())
	{
		return Owner->HandleKeyDown(MyGeometry, InKeyEvent);
	}
	return FReply::Unhandled();
}

FReply SRadialLayout::OnAnalogValueChanged(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	if (Owner.IsValid())
	{
		return Owner->HandleAnalogInput(MyGeometry, InAnalogEvent);
	}
	return FReply::Unhandled();
}

void SRadialLayout::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SPanel::OnMouseEnter(MyGeometry, MouseEvent);
	// We no longer need to call the Owner's HandleMouseEnter
}

void SRadialLayout::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SPanel::OnMouseLeave(MouseEvent);

	if (Owner.IsValid())
	{
		//Owner->HandleMouseLeave(MouseEvent); // OLD
		Owner->UpdateHoveredChild(nullptr); // NEW - Clear the hovered child
	}
}

// NEW: Add the OnMouseMove implementation
FReply SRadialLayout::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (Owner.IsValid())
	{
		// Get the list of child widgets and their positions
		FArrangedChildren ArrangedChildren(EVisibility::Visible);
		OnArrangeChildren(MyGeometry, ArrangedChildren);

		// Find which child, if any, is under the mouse
		UWidget* NewHoveredChild = nullptr;

		// Iterate backwards (top-most widgets first)
		for (int32 ChildIndex = ArrangedChildren.Num() - 1; ChildIndex >= 0; --ChildIndex)
		{
			const FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];

			// Check if the local mouse position is inside the child's geometry
			if (CurWidget.Geometry.IsUnderLocation(MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition())))
			{
				// Found it. Get the corresponding UWidget from the owner.
				NewHoveredChild = Owner->GetChildAt(ChildIndex);
				break;
			}
		}

		// Update the owner with the new hover state
		Owner->UpdateHoveredChild(NewHoveredChild);
	}

	// We are just observing, so return Unhandled to allow children to process mouse events
	return FReply::Unhandled();
}
