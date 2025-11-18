// RadialLayout.cpp
#include "UI/Widget/Layout/RadialLayout.h"
#include "UI/Widget/Layout/RadialLayoutSlot.h"
#include "UI/Widget/Layout/SRadialLayout.h"
#include "Blueprint/WidgetTree.h"
#include "Framework/Application/SlateApplication.h"

#define LOCTEXT_NAMESPACE "UMG"

// RadialLayout.cpp

URadialLayout::URadialLayout(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
	SetVisibilityInternal(ESlateVisibility::Visible); // Must be visible to receive mouse events
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

float URadialLayout::GetSelectedIndexAngle()
{
	if (SelectedIndex < 0 || SelectedIndex >= GetChildrenCount())
	{
		return 0.f;
	}
	return StartAngle + SelectedIndex * 360.f / GetChildrenCount();
}

UWidget* URadialLayout::GetSelectedChild() const
{
	if (SelectedIndex >= 0 && SelectedIndex < GetChildrenCount())
	{
		return GetChildAt(SelectedIndex);
	}
	return nullptr;
}

void URadialLayout::SetKeyboardFocus()
{
	if (MyRadialLayout.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(MyRadialLayout.ToSharedRef());
	}
}

void URadialLayout::InitializeForInput(bool bSetFocus)
{
	// Initialize selection if not already set
	if (GetChildrenCount() > 0 && SelectedIndex < 0)
	{
		SelectedIndex = 0; // Don't call UpdateSelection to avoid firing event on open
	}

	// Optionally set focus
	if (bSetFocus && MyRadialLayout.IsValid())
	{
		// Use a deferred focus to avoid interfering with animations
		FSlateApplication::Get().SetKeyboardFocus(MyRadialLayout.ToSharedRef(), EFocusCause::SetDirectly);
	}
}

void URadialLayout::UpdateSelection(int32 NewIndex)
{
	if (NewIndex == SelectedIndex || NewIndex < 0 || NewIndex >= GetChildrenCount())
	{
		return;
	}
	UWidget* PreviousWidget = GetChildAt(SelectedIndex);
	SelectedIndex = NewIndex;

	if (UWidget* SelectedWidget = GetChildAt(SelectedIndex))
	{
		OnChildSelected.Broadcast(SelectedWidget, PreviousWidget);
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

bool URadialLayout::UpdateSelectionFromAngle(const float Angle)
{
	if (!bEnableGamepadNavigation || GetChildrenCount() == 0)
	{
		return false;
	}
	// Find nearest child to stick direction
	if (const int32 NearestIndex = FindNearestChildToAngle(Angle); NearestIndex != SelectedIndex)
	{
		UpdateSelection(NearestIndex);
		return true;
	}
	return false;
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

	const float AngleStep = 360.0f / GetChildrenCount();
	return FMath::FloorToInt((Angle - StartAngle + AngleStep / 2.f) / AngleStep) % GetChildrenCount();
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
