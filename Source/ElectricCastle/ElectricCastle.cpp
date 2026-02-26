// Copyright Alien Shores

#include "ElectricCastle.h"

#include "Modules/ModuleManager.h"
#include "System/Loading/LoadingScreenManager.h"

void FElectricCastleModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
	FLoadingScreenManager::Initialize();
}

void FElectricCastleModule::ShutdownModule()
{
	FLoadingScreenManager::Shutdown();
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FElectricCastleModule, ElectricCastle, "ElectricCastle");
