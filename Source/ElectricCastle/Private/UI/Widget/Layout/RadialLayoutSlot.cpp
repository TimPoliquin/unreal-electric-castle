// RadialLayoutSlot.cpp
#include "UI/Widget/Layout/RadialLayoutSlot.h"
#include "UI/Widget/Layout/RadialLayout.h"
#include "UI/Widget/Layout/SRadialLayout.h"
#include "Components/Widget.h"

URadialLayoutSlot::URadialLayoutSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URadialLayoutSlot::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Update slot properties if we have a reference to it
	if (RadialLayout.IsValid())
	{
		// Find our slot index in the parent
		URadialLayout* RadialParent = Cast<URadialLayout>(Parent);
		if (RadialParent)
		{
			int32 SlotIndex = RadialParent->GetChildIndex(Content);
			if (SlotIndex >= 0 && SlotIndex < RadialLayout->GetNumSlots())
			{
				SRadialLayout::FSlot& Slot = RadialLayout->GetSlot(SlotIndex);
				Slot.SetAngleOffset(AngleOffset);
				Slot.SetRadiusMultiplier(RadiusMultiplier);
			}
		}

		// Invalidate to trigger re-layout
		RadialLayout->Invalidate(EInvalidateWidgetReason::Layout);
	}
}

void URadialLayoutSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	RadialLayout.Reset();
}

void URadialLayoutSlot::BuildSlot(TSharedRef<SRadialLayout> InRadialLayout)
{
	RadialLayout = InRadialLayout;

	if (Content)
	{
		int32 SlotIndex = RadialLayout->GetNumSlots();

		RadialLayout->AddSlot()
		[
			Content->TakeWidget()
		];

		// Configure the slot after it's been added
		SRadialLayout::FSlot& NewSlot = RadialLayout->GetSlot(SlotIndex);
		NewSlot.SetAngleOffset(AngleOffset);
		NewSlot.SetRadiusMultiplier(RadiusMultiplier);
	}
}
