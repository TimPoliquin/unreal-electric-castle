#include "AssetToolsModule.h"
#include "BlueprintActionDatabase.h"
#include "BlueprintNodeSpawner.h"
#include "ElectricCastleEditor.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"
#include "Asset/Utils/ResaveMetaHumansUtility.h"

void FElectricCastleEditorModule::StartupModule()
{
	// Register ToolMenus if not already
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FElectricCastleEditorModule::RegisterMenus)
	);
}

void FElectricCastleEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
}

void FElectricCastleEditorModule::RegisterMenus()
{
	// Get the Tools menu
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");

	FToolMenuSection& Section = Menu->AddSection("ElectricCastleTools", FText::FromString("Electric Castle"));

	Section.AddMenuEntry(
		"ResaveMetaHumans",
		FText::FromString("Resave MetaHumans"),
		FText::FromString("Rebuild and resave all MetaHuman skeletal meshes and groom bindings."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateStatic(&FMetaHumanMeshGroomResaveUtility::ResaveMetaHumanMeshesAndBindings))
	);
}

IMPLEMENT_MODULE(FElectricCastleEditorModule, ElectricCastleEditor);
