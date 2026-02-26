// Copyright Alien Shores


#include "Cinematic/CinematicMetaDataUtility.h"

#include "EditorUtilityLibrary.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"
#include "Misc/MessageDialog.h"

TArray<ULevelSequence*> UCinematicMetaDataUtility::GetSelectedLevelSequences() const
{
	TArray<ULevelSequence*> Result;
	for (UObject* Asset : UEditorUtilityLibrary::GetSelectedAssets())
	{
		if (ULevelSequence* Seq = Cast<ULevelSequence>(Asset))
		{
			Result.Add(Seq);
		}
	}
	return Result;
}

void UCinematicMetaDataUtility::LogCinematicMetaData()
{
	TArray<ULevelSequence*> Sequences = GetSelectedLevelSequences();

	if (Sequences.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("CinematicMetaDataUtility: No Level Sequences selected."));
		return;
	}

	for (const ULevelSequence* Seq : Sequences)
	{
		if (const UCinematicSequenceMetadata* Meta = Seq->FindMetaData<UCinematicSequenceMetadata>())
		{
			UE_LOG(LogTemp, Log,
			       TEXT("[%s] Tags: %s | Description: %s | Skippable: %s"),
			       *Seq->GetName(),
			       *Meta->GetTypeTags().ToString(),
			       *Meta->GetDescription(),
			       Meta->IsSkippable() ? TEXT("Yes") : TEXT("No"));
		}
		else
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("[%s] No CinematicSequenceMetaData found."), *Seq->GetName());
		}
	}
}

void UCinematicMetaDataUtility::ClearCinematicMetaData()
{
	TArray<ULevelSequence*> Sequences = GetSelectedLevelSequences();

	if (Sequences.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok,
		                     FText::FromString(TEXT("No Level Sequences selected.")));
		return;
	}

	for (ULevelSequence* Seq : Sequences)
	{
		Seq->RemoveMetaData<UCinematicSequenceMetadata>();
		Seq->MarkPackageDirty();
		UE_LOG(LogTemp, Log, TEXT("Cleared metadata from: %s"), *Seq->GetName());
	}
}

UCinematicSequenceMetadata* UCinematicMetaDataUtility::GetMetaDataFromFirstSelected() const
{
	TArray<ULevelSequence*> Sequences = GetSelectedLevelSequences();
	if (Sequences.IsEmpty()) { return nullptr; }

	return Sequences[0]->FindMetaData<UCinematicSequenceMetadata>();
}

void UCinematicMetaDataUtility::OpenMetaDataEditor()
{
	if (!EditorWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("[CinematicMetaDataUtility] No Editor Widget Configured"));
		return;
	}
	UEditorUtilityWidgetBlueprint* WidgetBP = Cast<UEditorUtilityWidgetBlueprint>(EditorWidget.ToSoftObjectPath().TryLoad());
	if (!WidgetBP)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not find EUW_CinematicMetaData — check the asset path."));
		return;
	}

	UEditorUtilitySubsystem* EUS = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
	EUS->SpawnAndRegisterTab(WidgetBP);
}
