// Copyright Alien Shores


#include "Player/Form/PlayerFormConfig.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Tags/ElectricCastleGameplayTags.h"

UPlayerFormConfig::UPlayerFormConfig()
{
	FElectricCastleGameplayTags GameplayTags = FElectricCastleGameplayTags::Get();
	Barbarian.FormTag = GameplayTags.Player_Form_Barbarian;
	Egyptian.FormTag = GameplayTags.Player_Form_Egyptian;
	Futureman.FormTag = GameplayTags.Player_Form_Futureman;
	Highlander.FormTag = GameplayTags.Player_Form_Highlander;
	Hippie.FormTag = GameplayTags.Player_Form_Hippie;
	Knight.FormTag = GameplayTags.Player_Form_Knight;
	Native.FormTag = GameplayTags.Player_Form_Native;
	Roman.FormTag = GameplayTags.Player_Form_Roman;
}

void UPlayerFormConfig::Initialize()
{
	FElectricCastleGameplayTags GameplayTags = FElectricCastleGameplayTags::Get();
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Barbarian, Barbarian);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Egyptian, Egyptian);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Futureman, Futureman);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Highlander, Highlander);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Hippie, Hippie);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Knight, Knight);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Native, Native);
	PlayerFormConfigByTag.Add(GameplayTags.Player_Form_Roman, Roman);
	PlayerFormConfigByEnum.Add(EPlayerForm::Barbarian, Barbarian);
	PlayerFormConfigByEnum.Add(EPlayerForm::Egyptian, Egyptian);
	PlayerFormConfigByEnum.Add(EPlayerForm::Futureman, Futureman);
	PlayerFormConfigByEnum.Add(EPlayerForm::Highlander, Highlander);
	PlayerFormConfigByEnum.Add(EPlayerForm::Hippie, Hippie);
	PlayerFormConfigByEnum.Add(EPlayerForm::Knight, Knight);
	PlayerFormConfigByEnum.Add(EPlayerForm::Native, Native);
	PlayerFormConfigByEnum.Add(EPlayerForm::Roman, Roman);
}

FPlayerFormConfigRow UPlayerFormConfig::GetPlayerFormConfigRowByTag(const FGameplayTag& FormTag) const
{
	if (PlayerFormConfigByTag.Contains(FormTag))
	{
		return PlayerFormConfigByTag[FormTag];
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Unable to find player form with provided tag: %s"), *GetName(), *FormTag.ToString())
	return FPlayerFormConfigRow(EPlayerForm::None);
}

FPlayerFormConfigRow UPlayerFormConfig::GetPlayerFormConfigRowByFormId(const int32 FormId) const
{
	if (FormId <= 0 || FormId >= static_cast<int32>(EPlayerForm::Invalid_Max))
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Requested form id is outside of known range. %d"), *GetName(), FormId)
		return FPlayerFormConfigRow(EPlayerForm::None);
	}
	const EPlayerForm Form = static_cast<EPlayerForm>(FormId);
	if (PlayerFormConfigByEnum.Contains(Form))
	{
		return PlayerFormConfigByEnum[Form];
	}
	return FPlayerFormConfigRow(EPlayerForm::None);
}
