// Copyright Alien Shores


#include "UI/Widget/HoverableWidget.h"


// Add default functionality here for any IHoverableWidget functions that are not pure virtual.
void IHoverableWidget::Hover(UObject* Widget)
{
	if (IsValid(Widget) && Widget->Implements<UHoverableWidget>())
	{
		Execute_Hover(Widget);
	}
}

void IHoverableWidget::Unhover(UObject* Widget)
{
	if (IsValid(Widget) && Widget->Implements<UHoverableWidget>())
	{
		Execute_Unhover(Widget);
	}
}

void IHoverableWidget::Select(UObject* Widget)
{
	if (IsValid(Widget) && Widget->Implements<UHoverableWidget>())
	{
		Execute_Select(Widget);
	}
}
