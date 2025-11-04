// Copyright Alien Shores


#include "UI/Widget/AuraOverlayWidget.h"

void UAuraOverlayWidget::RequestOpenMenu(const EAuraMenuTab OpenTab)
{
	OnOpenMenuDelegate.Broadcast(OpenTab);
}
