// Copyright Alien Shores


#include "UI/Widget/InteractionWidgetInterface.h"
#include "Blueprint/UserWidget.h"


// Add default functionality here for any IInteractionWidgetInterface functions that are not pure virtual.
void IInteractionWidgetInterface::Show(UUserWidget* Widget)
{
	if (IsValid(Widget) && Widget->Implements<UInteractionWidgetInterface>())
	{
		Execute_Show(Widget);
	}
}

void IInteractionWidgetInterface::Hide(UUserWidget* Widget)
{
	if (IsValid(Widget) && Widget->Implements<UInteractionWidgetInterface>())
	{
		Execute_Hide(Widget);
	}
}
