// Copyright Alien Shores


#include "Game/ElectricCastleGameInstance.h"

#include "Kismet/GameplayStatics.h"

UElectricCastleGameInstance::UElectricCastleGameInstance()
{
	// This is a hack to disable Virtual Accept
	// Associated with FCommonAnalogCursor::HandleKeyDownEvent and FAnalogCursor::HandleKeyDownEvent
	FKey& virtual_key = const_cast<FKey&>(EKeys::Virtual_Gamepad_Accept);
	virtual_key = EKeys::Invalid;
}

UElectricCastleGameInstance* UElectricCastleGameInstance::Get(const UObject* WorldContextObject)
{
	if (IsValid(WorldContextObject))
	{
		return Cast<UElectricCastleGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	}
	return nullptr;
}
