// Copyright Alien Shores


#include "UI/Widget/Layout/DirectionalLayoutSlot.h"

void UDirectionalLayoutSlot::BuildHorizontalSlot(SHorizontalBox::FScopedWidgetSlotArguments& SlotArgs)
{
	HSlot = nullptr;
	VSlot = nullptr;

	SlotArgs.Padding(Padding)
	        .HAlign(HAlign)
	        .VAlign(VAlign);

	if (Fill > 0.f)
	{
		SlotArgs.FillWidth(Fill);
	}
	else
	{
		SlotArgs.AutoWidth();
	}

	HSlot = SlotArgs.GetSlot();
}

void UDirectionalLayoutSlot::BuildVerticalSlot(SVerticalBox::FScopedWidgetSlotArguments& SlotArgs)
{
	HSlot = nullptr;
	VSlot = nullptr;

	SlotArgs.Padding(Padding)
	        .HAlign(HAlign)
	        .VAlign(VAlign);

	if (Fill > 0.f)
	{
		SlotArgs.FillHeight(Fill);
	}
	else
	{
		SlotArgs.AutoHeight();
	}

	VSlot = SlotArgs.GetSlot();
}

void UDirectionalLayoutSlot::SynchronizeProperties()
{
	if (HSlot)
	{
		HSlot->SetPadding(Padding);
		HSlot->SetHorizontalAlignment(HAlign);
		HSlot->SetVerticalAlignment(VAlign);
		if (Fill > 0.f)
		{
			HSlot->SetSizeParam(FStretch(Fill));
		}
		else
		{
			HSlot->SetSizeParam(FAuto());
		}
	}
	else if (VSlot)
	{
		VSlot->SetPadding(Padding);
		VSlot->SetHorizontalAlignment(HAlign);
		VSlot->SetVerticalAlignment(VAlign);
		if (Fill > 0.f)
		{
			VSlot->SetSizeParam(FStretch(Fill));
		}
		else
		{
			VSlot->SetSizeParam(FAuto());
		}
	}
}
