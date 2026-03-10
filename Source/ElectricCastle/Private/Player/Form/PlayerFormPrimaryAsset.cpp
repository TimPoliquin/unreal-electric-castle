// Copyright Alien Shores


#include "Player/Form/PlayerFormPrimaryAsset.h"
#include "Player/Form/FormConfigTypes.h"
#include "Player/Form/Config/FormConfig.h"

FPrimaryAssetId UPlayerFormPrimaryAsset::GetPrimaryAssetId() const
{
	// Use the asset's name if FName is empty
	FName AssetName = GetFName();
	if (AssetName == NAME_None && GetOuter())
	{
		AssetName = GetOuter()->GetFName();
	}

	FPrimaryAssetId Id = FPrimaryAssetId(FPrimaryAssetType("PlayerForm"), AssetName);
	UE_LOG(LogTemp, VeryVerbose, TEXT("GetPrimaryAssetId for %s: %s"), *GetName(), *Id.ToString());
	return Id;
}

void UPlayerFormPrimaryAsset::Initialize()
{
	MeshConfig.Initialize();
}

void UPlayerFormPrimaryAsset::OnFormActivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) const
{
	for (UFormConfig* FormConfig : FormConfigs)
	{
		FormConfig->OnFormActivated(Actor, FormChangeComponent);
	}
}


void UPlayerFormPrimaryAsset::OnFormDeactivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) const
{
	for (UFormConfig* FormConfig : FormConfigs)
	{
		FormConfig->OnFormDeactivated(Actor, FormChangeComponent);
	}
}

bool UPlayerFormPrimaryAsset::IsValid() const
{
	return FormTag.IsValid() && MeshConfig.IsLoaded();
}
