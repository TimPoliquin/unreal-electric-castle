// Copyright Alien Shores


#include "UI/Widget/AuraMenuWidget.h"

#include "Components/Button.h"

UAuraMenuWidget::UAuraMenuWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ActiveMenuTab = EAuraMenuTab::Attributes;
	MenuTabNames.Add(EAuraMenuTab::Attributes, FString("Attributes"));
	MenuTabNames.Add(EAuraMenuTab::Spells, FString("Spells"));
	MenuTabNames.Add(EAuraMenuTab::Inventory, FString("Inventory"));
	MenuTabNames.Add(EAuraMenuTab::System, FString("System"));
}

void UAuraMenuWidget::InitializeDependencies_Implementation(
	AActor* InPlayer
)
{
	InitializeAttributesTabDependencies(InPlayer);
	InitializeInventoryTabDependencies(InPlayer);
	InitializeSpellsTabDependencies(InPlayer);
	InitializeSystemTabDependencies(InPlayer);
}

void UAuraMenuWidget::InitializeTabButton(const EAuraMenuTab Tab, UButton* Button)
{
	switch (Tab)
	{
	case EAuraMenuTab::Attributes:
		Button->OnClicked.AddDynamic(this, &UAuraMenuWidget::ActivateAttributesTab);
		break;
	case EAuraMenuTab::Spells:
		Button->OnClicked.AddDynamic(this, &UAuraMenuWidget::ActivateSpellsTab);
		break;
	case EAuraMenuTab::Inventory:
		Button->OnClicked.AddDynamic(this, &UAuraMenuWidget::ActivateInventoryTab);
		break;
	case EAuraMenuTab::System:
		Button->OnClicked.AddDynamic(this, &UAuraMenuWidget::ActivateSystemTab);
		break;
	}
}

void UAuraMenuWidget::CloseMenu_Implementation()
{
	OnAuraMenuClosed.Broadcast();
}

void UAuraMenuWidget::ChangeActiveTab(const EAuraMenuTab& Tab)
{
	if (ActiveMenuTab != Tab)
	{
		ActiveMenuTab = Tab;
		OnActiveTabChanged(ActiveMenuTab);
	}
}

void UAuraMenuWidget::ActivateAttributesTab()
{
	ChangeActiveTab(EAuraMenuTab::Attributes);
}

void UAuraMenuWidget::ActivateSpellsTab()
{
	ChangeActiveTab(EAuraMenuTab::Spells);
}

void UAuraMenuWidget::ActivateInventoryTab()
{
	ChangeActiveTab(EAuraMenuTab::Inventory);
}

void UAuraMenuWidget::ActivateSystemTab()
{
	ChangeActiveTab(EAuraMenuTab::System);
}
