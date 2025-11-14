// Copyright Alien Shores


#include "Player/Form/PlayerFormConfig.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Player/Form/FormConfigLoadRequest.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "GroomAsset.h"
#include "GroomBindingAsset.h"
#include "GroomComponent.h"

bool FFormMaterialConfig::IsNull() const
{
	return MaterialAsset.IsNull();
}

bool FFormMaterialConfig::IsValid() const
{
	return MaterialAsset.IsValid();
}

bool FFormMaterialConfig::IsLoaded() const
{
	return IsNull() || IsValid();
}

void FFormMaterialConfig::AddToLoad(UFormConfigLoadRequest* LoadRequest) const
{
	LoadRequest->AddToLoad(MaterialAsset);
}

void FFormMaterialConfig::SetToComponent(UMeshComponent* MeshComponent, const int32 Idx) const
{
	if (MaterialSlotName.IsNone())
	{
		MeshComponent->SetMaterial(Idx, MaterialAsset.Get());
	}
	else
	{
		MeshComponent->SetMaterialByName(MaterialSlotName, MaterialAsset.Get());
	}
}

bool FFormMeshPartConfig::IsNull() const
{
	return MeshAsset.IsNull();
}

bool FFormMeshPartConfig::IsValid() const
{
	return MeshAsset.IsValid();
}

bool FFormMeshPartConfig::IsLoaded() const
{
	if (IsValid())
	{
		return !Materials.ContainsByPredicate(
			[](const FFormMaterialConfig& Material)
			{
				return !Material.IsLoaded();
			}
		);
	}
	return false;
}

void FFormMeshPartConfig::AddToLoad(UFormConfigLoadRequest* LoadRequest) const
{
	LoadRequest->AddToLoad(MeshAsset);
	for (const FFormMaterialConfig Material : Materials)
	{
		Material.AddToLoad(LoadRequest);
	}
}

void FFormMeshPartConfig::SetToComponent(USkeletalMeshComponent* MeshComponent) const
{
	MeshComponent->SetSkeletalMesh(MeshAsset.Get());
	if (Materials.Num() > 0)
	{
		MeshComponent->GetMaterials().Empty();
		MeshComponent->GetMaterials().SetNum(Materials.Num());
		for (int Idx = 0; Idx < Materials.Num(); Idx++)
		{
			Materials[Idx].SetToComponent(MeshComponent, Idx);
		}
	}
	MeshComponent->MarkRenderStateDirty();
}

bool FFormGroomConfig::IsNull() const
{
	return GroomAsset.IsNull() || GroomBindingAsset.IsNull();
}

bool FFormGroomConfig::IsValid() const
{
	return GroomAsset.IsValid() && GroomBindingAsset.IsValid();
}

bool FFormGroomConfig::IsLoaded() const
{
	return IsValid();
}

void FFormGroomConfig::AddToLoad(UFormConfigLoadRequest* LoadRequest) const
{
	LoadRequest->AddToLoad(GroomAsset);
	LoadRequest->AddToLoad(GroomBindingAsset);
	for (const FFormMaterialConfig Material : Materials)
	{
		Material.AddToLoad(LoadRequest);
	}
}

void FFormGroomConfig::SetToComponent(UGroomComponent* GroomComponent) const
{
	if (IsValid())
	{
		GroomComponent->SetGroomAsset(GroomAsset.Get());
		GroomComponent->SetBindingAsset(GroomBindingAsset.Get());
		GroomComponent->SetForcedLOD(0);
		GroomComponent->SetEnableSimulation(GroomAsset.Get()->IsSimulationEnable());
		GroomComponent->GetMaterials().Empty();
		GroomComponent->GetMaterials().SetNum(Materials.Num());
		for (int Idx = 0; Idx < Materials.Num(); Idx++)
		{
			Materials[Idx].SetToComponent(GroomComponent, Idx);
		}
	}
	else
	{
		GroomComponent->SetGroomAsset(nullptr);
		GroomComponent->SetBindingAsset(nullptr);
		GroomComponent->SetEnableSimulation(false);
	}
	GroomComponent->RecreateRenderState_Concurrent();
	GroomComponent->ResetSimulation();
}

bool FFormMeshConfig::IsLoaded() const
{
	return
		(Body.IsNull() || Body.IsValid()) &&
		(Face.IsNull() || Face.IsValid()) &&
		(Clothing.IsNull() || Clothing.IsValid()) &&
		(Beard.IsNull() || Beard.IsValid()) &&
		(Fuzz.IsNull() || Fuzz.IsValid()) &&
		(Eyebrows.IsNull() || Eyebrows.IsValid()) &&
		(Eyelashes.IsNull() || Eyelashes.IsValid()) &&
		(Hair.IsNull() || Hair.IsValid()) &&
		(Moustache.IsNull() || Moustache.IsValid());
}

void FFormMeshConfig::AddToLoad(UFormConfigLoadRequest* LoadRequest) const
{
	Body.AddToLoad(LoadRequest);
	Face.AddToLoad(LoadRequest);
	Clothing.AddToLoad(LoadRequest);
	Beard.AddToLoad(LoadRequest);
	Fuzz.AddToLoad(LoadRequest);
	Eyebrows.AddToLoad(LoadRequest);
	Eyelashes.AddToLoad(LoadRequest);
	Hair.AddToLoad(LoadRequest);
	Moustache.AddToLoad(LoadRequest);
}

bool FPlayerFormConfigRow::IsLoaded() const
{
	return
		MeshConfig.IsLoaded() &&
		(AnimationBlueprint.IsNull() || AnimationBlueprint.IsValid()) &&
		(PortraitImage.IsNull() || PortraitImage.IsValid());
}

bool FPlayerFormConfigRow::IsValid() const
{
	return
		FormTag.IsValid() &&
		FormId != EPlayerForm::None && FormId != EPlayerForm::Count;
}

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
	if (FormTag.IsValid() && PlayerFormConfigByTag.Contains(FormTag))
	{
		return PlayerFormConfigByTag[FormTag];
	}
	UE_LOG(
		LogElectricCastle,
		Warning,
		TEXT("[%s] Unable to find player form with provided tag: %s"),
		*GetName(),
		*FormTag.ToString()
	)
	return FPlayerFormConfigRow(EPlayerForm::None);
}

UFormConfigLoadRequest* UPlayerFormConfig::GetOrCreateLoadRequest(const FGameplayTag& FormTag)
{
	if (!FormTag.IsValid())
	{
		return nullptr;
	}
	if (FormLoadRequests.Contains(FormTag))
	{
		return FormLoadRequests[FormTag];
	}
	UFormConfigLoadRequest* LoadRequest = UFormConfigLoadRequest::Create(FormTag, this);
	LoadRequest->OnLoadComplete.AddUniqueDynamic(this, &UPlayerFormConfig::OnLoadComplete);
	FormLoadRequests.Add(FormTag, LoadRequest);
	return LoadRequest;
}

FPlayerFormConfigRow UPlayerFormConfig::GetPlayerFormConfigRowByFormId(const int32 FormId) const
{
	if (FormId <= 0 || FormId >= static_cast<int32>(EPlayerForm::Count))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Requested form id is outside of known range. %d"),
			*GetName(),
			FormId
		)
		return FPlayerFormConfigRow(EPlayerForm::None);
	}
	return GetPlayerFormConfigRowByFormId(static_cast<EPlayerForm>(FormId));
}

FPlayerFormConfigRow UPlayerFormConfig::GetPlayerFormConfigRowByFormId(const EPlayerForm FormId) const
{
	if (PlayerFormConfigByEnum.Contains(FormId))
	{
		return PlayerFormConfigByEnum[FormId];
	}
	return FPlayerFormConfigRow(EPlayerForm::None);
}

void UPlayerFormConfig::LoadAsync(UFormConfigLoadRequest* LoadRequest)
{
	const FPlayerFormConfigRow& Row = GetPlayerFormConfigRowByTag(LoadRequest->FormTag);
	if (!Row.IsValid())
	{
		LoadRequest->Notify();
	}
	Row.MeshConfig.AddToLoad(LoadRequest);
	LoadRequest->AddToLoad(Row.PortraitImage);
	LoadRequest->AddToLoad(Row.AnimationBlueprint);
	LoadRequest->LoadAsync();
}

void UPlayerFormConfig::OnLoadComplete(const FPlayerFormConfigRow& Row)
{
	if (FormLoadRequests.Contains(Row.FormTag))
	{
		FormLoadRequests.Remove(Row.FormTag);
	}
}
