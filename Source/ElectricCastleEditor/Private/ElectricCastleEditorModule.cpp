#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "ToolMenuSection.h"
#include "Asset/Utils/ResaveMetaHumansUtility.h"

class FElectricCastleEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        // Register ToolMenus if not already
        UToolMenus::RegisterStartupCallback(
            FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FElectricCastleEditorModule::RegisterMenus)
        );
    }
    virtual void ShutdownModule() override
    {
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);
    }

    void RegisterMenus()
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
};

IMPLEMENT_MODULE(FElectricCastleEditorModule, ElectricCastleEditor);