// Copyright Alien Shores


#include "UI/Widget/AuraUserWidget.h"
#include "Interaction/UnbindableInterface.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
	if (WidgetController->Implements<UUnbindableInterface>())
	{
		OnNativeDestruct.AddLambda(
			[this](UUserWidget* Widget)
			{
				IUnbindableInterface::Execute_UnbindAll(WidgetController, Widget);
			}
		);
	}
}
