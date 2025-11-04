#include "Asset/Utils/ResaveMetaHumansUtility.h"

#include "FileHelpers.h"
#include "GroomBindingAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"


#if WITH_EDITOR

#include "Engine/SkeletalMesh.h"
#include "Editor.h"
#include "Misc/ScopedSlowTask.h"


void FMetaHumanMeshGroomResaveUtility::ResaveMetaHumanMeshesAndBindings()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");
	const FName MetaHumansPath = TEXT("/Game/MetaHumans");

	// --- Phase 1: Skeletal Meshes ---
	{
		FARFilter MeshFilter;
		MeshFilter.PackagePaths.Add(MetaHumansPath);
		MeshFilter.bRecursivePaths = true;
		MeshFilter.ClassNames.Add(USkeletalMesh::StaticClass()->GetFName());

		TArray<FAssetData> MeshAssets;
		AssetRegistryModule.Get().GetAssets(MeshFilter, MeshAssets);

		FScopedSlowTask SlowTask(MeshAssets.Num(), FText::FromString(TEXT("Resaving MetaHuman Skeletal Meshes...")));
		SlowTask.MakeDialog();

		TArray<UPackage*> MeshPackages;

		for (const FAssetData& AssetData : MeshAssets)
		{
			SlowTask.EnterProgressFrame(1);

			if (USkeletalMesh* SkelMesh = Cast<USkeletalMesh>(AssetData.GetAsset()))
			{
				SkelMesh->InvalidateDeriveDataCacheGUID();
				SkelMesh->MarkPackageDirty();
				MeshPackages.AddUnique(SkelMesh->GetOutermost());
			}
		}

		if (MeshPackages.Num() > 0)
		{
			FEditorFileUtils::PromptForCheckoutAndSave(MeshPackages, /*bCheckDirty=*/false, /*bPromptToSave=*/false);
			// Unload packages after saving
			for (UPackage* Package : MeshPackages)
			{
				ResetLoaders(Package);
			}
			CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		}
	}

	// --- Phase 2: Groom Bindings ---
	{
		FARFilter BindingFilter;
		BindingFilter.PackagePaths.Add(MetaHumansPath);
		BindingFilter.bRecursivePaths = true;
		BindingFilter.ClassNames.Add(UGroomBindingAsset::StaticClass()->GetFName());

		TArray<FAssetData> BindingAssets;
		AssetRegistryModule.Get().GetAssets(BindingFilter, BindingAssets);

		FScopedSlowTask SlowTask(BindingAssets.Num(), FText::FromString(TEXT("Resaving MetaHuman Groom Bindings...")));
		SlowTask.MakeDialog();

		TArray<UPackage*> BindingPackages;

		for (const FAssetData& AssetData : BindingAssets)
		{
			SlowTask.EnterProgressFrame(1);

			if (UGroomBindingAsset* Binding = Cast<UGroomBindingAsset>(AssetData.GetAsset()))
			{
				Binding->InvalidateBinding();
				Binding->MarkPackageDirty();
				BindingPackages.AddUnique(Binding->GetOutermost());
			}
		}

		if (BindingPackages.Num() > 0)
		{
			FEditorFileUtils::PromptForCheckoutAndSave(BindingPackages, /*bCheckDirty=*/false, /*bPromptToSave=*/false);
			// Unload packages after saving
			for (UPackage* Package : BindingPackages)
			{
				ResetLoaders(Package);
			}
			CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
		}
	}
}
#endif
