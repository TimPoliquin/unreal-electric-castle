// RadialLayout.cpp
#include "UI/Widget/Layout/RadialLayout.h"
#include "UI/Widget/Layout/RadialLayoutSlot.h"
#include "UI/Widget/Layout/SRadialLayout.h"
#include "Blueprint/WidgetTree.h"

#define LOCTEXT_NAMESPACE "UMG"

// RadialLayout.cpp

URadialLayout::URadialLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	//SetVisibilityInternal(ESlateVisibility::SelfHitTestInvisible); // OLD
	SetVisibilityInternal(ESlateVisibility::Visible); // NEW - Must be visible to receive mouse events
}

void URadialLayout::UpdateHoveredChild(UWidget* NewHoveredChild)
{
	UWidget* OldHoveredChild = HoveredChild.Get();

	if (OldHoveredChild != NewHoveredChild)
	{
		HoveredChild = NewHoveredChild;
		OnChildHovered.Broadcast(NewHoveredChild);
	}
}

UClass* URadialLayout::GetSlotClass() const
{
	return URadialLayoutSlot::StaticClass();
}

void URadialLayout::OnSlotAdded(UPanelSlot* InSlot)
{
	if (MyRadialLayout.IsValid())
	{
		URadialLayoutSlot* RadialSlot = Cast<URadialLayoutSlot>(InSlot);
		if (RadialSlot && RadialSlot->Content)
		{
			RadialSlot->BuildSlot(MyRadialLayout.ToSharedRef());
		}
	}
}

void URadialLayout::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Adjust selected index if needed
	if (SelectedIndex >= GetChildrenCount())
	{
		SelectedIndex = GetChildrenCount() - 1;
	}
}

void URadialLayout::SetSelectedIndex(int32 Index)
{
	if (GetChildrenCount() == 0)
	{
		SelectedIndex = -1;
		return;
	}

	// Clamp index to valid range
	Index = FMath::Clamp(Index, 0, GetChildrenCount() - 1);
	UpdateSelection(Index);
}

UWidget* URadialLayout::GetSelectedChild() const
{
	if (SelectedIndex >= 0 && SelectedIndex < GetChildrenCount())
	{
		return GetChildAt(SelectedIndex);
	}
	return nullptr;
}

void URadialLayout::UpdateSelection(int32 NewIndex)
{
	if (NewIndex == SelectedIndex || NewIndex < 0 || NewIndex >= GetChildrenCount())
	{
		return;
	}

	SelectedIndex = NewIndex;
	UWidget* SelectedWidget = GetChildAt(SelectedIndex);

	if (SelectedWidget)
	{
		OnChildSelected.Broadcast(SelectedWidget);
	}
}

FReply URadialLayout::HandleKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (!bEnableGamepadNavigation || GetChildrenCount() == 0)
	{
		return FReply::Unhandled();
	}

	// Initialize selection if not set
	if (SelectedIndex < 0)
	{
		SetSelectedIndex(0);
		return FReply::Handled();
	}

	FKey Key = InKeyEvent.GetKey();

	// D-pad navigation - move to next/previous child
	if (Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_DPad_Down)
	{
		int32 NewIndex = (SelectedIndex + 1) % GetChildrenCount();
		UpdateSelection(NewIndex);
		return FReply::Handled();
	}
	if (Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_DPad_Up)
	{
		int32 NewIndex = (SelectedIndex - 1 + GetChildrenCount()) % GetChildrenCount();
		UpdateSelection(NewIndex);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply URadialLayout::HandleAnalogInput(const FGeometry& MyGeometry, const FAnalogInputEvent& InAnalogEvent)
{
	if (!bEnableGamepadNavigation || GetChildrenCount() == 0)
	{
		return FReply::Unhandled();
	}

	FKey Key = InAnalogEvent.GetKey();

	// Handle left stick or right stick
	if (Key == EKeys::Gamepad_LeftX || Key == EKeys::Gamepad_LeftY ||
		Key == EKeys::Gamepad_RightX || Key == EKeys::Gamepad_RightY)
	{
		// Build analog input vector
		if (Key == EKeys::Gamepad_LeftX || Key == EKeys::Gamepad_RightX)
		{
			LastAnalogInput.X = InAnalogEvent.GetAnalogValue();
		}
		else
		{
			LastAnalogInput.Y = InAnalogEvent.GetAnalogValue();
		}

		// Check if stick is pushed beyond deadzone
		float Magnitude = LastAnalogInput.Size();
		if (Magnitude > AnalogDeadzone)
		{
			// Calculate angle from stick input (Y is inverted in UE)
			float StickAngle = FMath::Atan2(-LastAnalogInput.Y, LastAnalogInput.X);
			StickAngle = FMath::RadiansToDegrees(StickAngle);

			// Find nearest child to stick direction
			int32 NearestIndex = FindNearestChildToAngle(StickAngle);
			if (NearestIndex != SelectedIndex)
			{
				UpdateSelection(NearestIndex);
				return FReply::Handled();
			}
		}
	}

	return FReply::Unhandled();
}

void URadialLayout::HandleMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Mouse enter handling if needed
}

void URadialLayout::HandleMouseLeave(const FPointerEvent& MouseEvent)
{
	if (HoveredChild.IsValid())
	{
		HoveredChild.Reset();
	}
}

int32 URadialLayout::FindNearestChildToAngle(float Angle) const
{
	if (GetChildrenCount() == 0)
	{
		return -1;
	}

	// Normalize angle to 0-360 range
	while (Angle < 0)
	{
		Angle += 360.0f;
	}
	while (Angle >= 360.0f)
	{
		Angle -= 360.0f;
	}

	int32 NearestIndex = 0;
	float SmallestDifference = FLT_MAX;

	// Calculate angle for each child and find nearest
	for (int32 i = 0; i < GetChildrenCount(); ++i)
	{
		float AngleStep = 360.0f / GetChildrenCount();
		float ChildAngle = StartAngle + 90.0f + (bClockwise
			                                         ? -AngleStep * i
			                                         : AngleStep * i);

		// Add custom offset if slot has one
		if (const URadialLayoutSlot* CurrentSlot = Cast<URadialLayoutSlot>(GetSlots()[i]))
		{
			ChildAngle += CurrentSlot->AngleOffset;
		}

		// Normalize child angle
		while (ChildAngle < 0)
		{
			ChildAngle += 360.0f;
		}
		while (ChildAngle >= 360.0f)
		{
			ChildAngle -= 360.0f;
		}

		// Calculate angular difference (shortest path)
		float Difference = FMath::Abs(Angle - ChildAngle);
		if (Difference > 180.0f)
		{
			Difference = 360.0f - Difference;
		}

		if (Difference < SmallestDifference)
		{
			SmallestDifference = Difference;
			NearestIndex = i;
		}
	}

	return NearestIndex;
}

TSharedRef<SWidget> URadialLayout::RebuildWidget()
{
	MyRadialLayout = SNew(SRadialLayout)
		.Radius(Radius)
		.StartAngle(StartAngle)
		.bClockwise(bClockwise)
		.Owner(this);

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (URadialLayoutSlot* TypedSlot = Cast<URadialLayoutSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyRadialLayout.ToSharedRef());
		}
	}

	return MyRadialLayout.ToSharedRef();
}

void URadialLayout::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (MyRadialLayout.IsValid())
	{
		MyRadialLayout->SetRadius(Radius);
		MyRadialLayout->SetStartAngle(StartAngle);
		MyRadialLayout->SetClockwise(bClockwise);
	}
}

void URadialLayout::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyRadialLayout.Reset();
}

#undef LOCTEXT_NAMESPACE
