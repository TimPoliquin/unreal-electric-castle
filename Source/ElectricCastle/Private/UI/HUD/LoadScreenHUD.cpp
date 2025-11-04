// Copyright Alien Shores


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->SetLocalPlayer(GetOwningPlayerController()->GetLocalPlayer());
	LoadScreenViewModel->InitializeLoadSlots();
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(
		GetWorld(),
		LoadScreenWidgetClass
	);
	LoadScreenWidget->AddToViewport();
	// DEVNOTE - this blueprint initialize widget seems to occur prior to the MVVM auto-binding.
	// this results in the view attempting to access data in the view model before it has been set!
	LoadScreenWidget->BlueprintInitializeWidget();
	// load the save data
	LoadScreenViewModel->LoadData();
}
