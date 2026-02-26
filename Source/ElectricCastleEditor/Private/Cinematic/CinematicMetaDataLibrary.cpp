// Copyright Alien Shores


#include "Cinematic/CinematicMetaDataLibrary.h"

#include "EditorUtilityLibrary.h"
#include "LevelSequence.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"


UCinematicSequenceMetaData* UCinematicMetaDataLibrary::GetMetaDataFromFirstSelected()
{
	TArray<UObject*> Selected = UEditorUtilityLibrary::GetSelectedAssets();
	UE_LOG(LogTemp, Log, TEXT("GetMetaDataFromFirstSelected: %d assets selected"), Selected.Num());

	for (UObject* Asset : Selected)
	{
		UE_LOG(LogTemp, Log, TEXT("  Selected asset: %s (%s)"),
		       *Asset->GetName(), *Asset->GetClass()->GetName());


		if (ULevelSequence* Seq = Cast<ULevelSequence>(Asset))
		{
			UCinematicSequenceMetaData* Meta = Seq->FindMetaData<UCinematicSequenceMetaData>();
			UE_LOG(LogTemp, Log, TEXT("  FindMetaData result: %s"),
			       Meta ? TEXT("FOUND") : TEXT("NOT FOUND"));

			if (Meta)
			{
				UE_LOG(LogTemp, Log, TEXT("  Tags: %s | Description: %s"),
				       *Meta->GetTypeTags().ToString(),
				       *Meta->GetDescription());
			}

			return Meta;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("GetMetaDataFromFirstSelected: No Level Sequence found in selection"));
	return nullptr;
}

void UCinematicMetaDataLibrary::ApplyMetaDataToSelected(ULevelSequence* LevelSequence, UCinematicSequenceMetaData* SourceMetaData)
{
	if (!SourceMetaData)
	{
		UE_LOG(LogTemp, Error, TEXT("ApplyMetaDataToSelected: SourceMetaData is null!"));
		return;
	}

	if (UCinematicSequenceMetaData* Meta = LevelSequence ? LevelSequence->FindOrAddMetaData<UCinematicSequenceMetaData>() : nullptr)
	{
		Meta->CopyFrom(SourceMetaData);
		LevelSequence->MarkPackageDirty();
	}
}

void UCinematicMetaDataLibrary::CopyMetaData(UCinematicSequenceMetaData* SourceMetaData, UCinematicSequenceMetaData* TargetMetaData)
{
	if (IsValid(SourceMetaData) && IsValid(TargetMetaData))
	{
		TargetMetaData->CopyFrom(SourceMetaData);
	}
}

ULevelSequence* UCinematicMetaDataLibrary::GetFirstSelectedLevelSequence()
{
	for (UObject* Asset : UEditorUtilityLibrary::GetSelectedAssets())
	{
		if (ULevelSequence* Seq = Cast<ULevelSequence>(Asset))
		{
			return Seq;
		}
	}
	return nullptr;
}
