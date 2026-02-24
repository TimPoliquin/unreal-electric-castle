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

void UCinematicMetaDataUtility::SetCinematicMetaData(
	const FGameplayTagContainer& TypeTags,
	FString Description,
	const bool bSkippable)
{
	TArray<ULevelSequence*> Sequences = GetSelectedLevelSequences();

	if (Sequences.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok,
		                     FText::FromString(TEXT("No Level Sequences selected. Select one or more in the Content Browser first.")));
		return;
	}

	for (ULevelSequence* Seq : Sequences)
	{
		UCinematicSequenceMetaData* Meta = Seq->FindOrAddMetaData<UCinematicSequenceMetaData>();

		Meta->TypeTags = TypeTags;
		Meta->Description = Description;
		Meta->bSkippable = bSkippable;

		Seq->MarkPackageDirty();

		UE_LOG(LogTemp, Log, TEXT("Set metadata on: %s | Tags: %s"),
		       *Seq->GetName(),
		       *TypeTags.ToString());
	}

	const FString Msg = FString::Printf(
		TEXT("Metadata applied to %d sequence(s). Don't forget to save."), Sequences.Num());
	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Msg));
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
		const UCinematicSequenceMetaData* Meta = Seq->FindMetaData<UCinematicSequenceMetaData>();
		if (Meta)
		{
			UE_LOG(LogTemp, Log,
			       TEXT("[%s] Tags: %s | Description: %s | Skippable: %s"),
			       *Seq->GetName(),
			       *Meta->TypeTags.ToString(),
			       *Meta->Description,
			       Meta->bSkippable ? TEXT("Yes") : TEXT("No"));
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
		Seq->RemoveMetaData<UCinematicSequenceMetaData>();
		Seq->MarkPackageDirty();
		UE_LOG(LogTemp, Log, TEXT("Cleared metadata from: %s"), *Seq->GetName());
	}
}

UCinematicSequenceMetaData* UCinematicMetaDataUtility::GetMetaDataFromFirstSelected() const
{
	TArray<ULevelSequence*> Sequences = GetSelectedLevelSequences();
	if (Sequences.IsEmpty()) { return nullptr; }

	return Sequences[0]->FindMetaData<UCinematicSequenceMetaData>();
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
