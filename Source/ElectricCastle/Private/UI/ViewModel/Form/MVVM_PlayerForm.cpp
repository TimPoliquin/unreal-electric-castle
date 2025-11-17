// Copyright Alien Shores


#include "UI/ViewModel/Form/MVVM_PlayerForm.h"

#include "Player/ElectricCastlePlayerState.h"
#include "Player/Form/PlayerFormChangeComponent.h"

void UMVVM_PlayerForm::InitializeDependencies(
	AElectricCastlePlayerState* PlayerState,
	const FPlayerFormConfigRow& FormConfigRow
)
{
	// Set values based on current player state and form configuration
	FormId = FormConfigRow.FormId;
	SetFormTag(FormConfigRow.FormTag);
	SetFormName(FormConfigRow.FormName);
	if (FormConfigRow.PortraitImage.IsValid())
	{
		SetFormIcon(FormConfigRow.PortraitImage.Get());
	}
	else if (!FormConfigRow.PortraitImage.IsNull())
	{
		FLoadSoftObjectPathAsyncDelegate AsyncDelegate;
		AsyncDelegate.BindLambda([this](FSoftObjectPath ObjectPath, UObject* Loaded)
		{
			SetFormIcon(CastChecked<UTexture2D>(Loaded));
		});
		FormConfigRow.PortraitImage.LoadAsync(AsyncDelegate);
	}

	if (UPlayerFormChangeComponent* FormChangeComponent = IFormChangeActorInterface::GetFormChangeComponent(PlayerState))
	{
		FormChangeComponent->OnAvailableFormsChanged.AddUniqueDynamic(this, &UMVVM_PlayerForm::OnAvailableFormsChanged);
		FormChangeComponent->OnPlayerFormChange.AddUniqueDynamic(this, &UMVVM_PlayerForm::OnPlayerFormChanged);
		SetIsSelected(FormChangeComponent->GetCurrentFormTag().MatchesTagExact(FormTag));
		SetIsAvailable(FormChangeComponent->IsFormAvailable(FormTag));
	}
}

FGameplayTag UMVVM_PlayerForm::GetFormTag() const
{
	return FormTag;
}

void UMVVM_PlayerForm::SetFormTag(const FGameplayTag& InFormTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(FormTag, InFormTag);
}

FString UMVVM_PlayerForm::GetFormName() const
{
	return FormName;
}

void UMVVM_PlayerForm::SetFormName(const FString& InFormName)
{
	UE_MVVM_SET_PROPERTY_VALUE(FormName, InFormName);
}

UTexture2D* UMVVM_PlayerForm::GetFormIcon() const
{
	return FormIcon;
}

void UMVVM_PlayerForm::SetFormIcon(UTexture2D* InFormIcon)
{
	UE_MVVM_SET_PROPERTY_VALUE(FormIcon, InFormIcon);
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetFormIconBrush);
}

FSlateBrush UMVVM_PlayerForm::GetFormIconBrush() const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(GetFormIcon());
	return Brush;
}

bool UMVVM_PlayerForm::GetIsSelected() const
{
	return IsSelected;
}

void UMVVM_PlayerForm::SetIsSelected(const bool InIsSelected)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsSelected, InIsSelected);
}

bool UMVVM_PlayerForm::GetIsAvailable() const
{
	return IsAvailable;
}

void UMVVM_PlayerForm::SetIsAvailable(const bool InIsAvailable)
{
	UE_MVVM_SET_PROPERTY_VALUE(IsAvailable, InIsAvailable);
}

void UMVVM_PlayerForm::OnAvailableFormsChanged(const FOnPlayerAvailableFormsChangedPayload& Payload)
{
	SetIsSelected(Payload.AvailableForms.HasTagExact(GetFormTag()));
}

void UMVVM_PlayerForm::OnPlayerFormChanged(const FPlayerFormChangeEventPayload& Payload)
{
	SetIsSelected(Payload.NewFormTag.MatchesTagExact(GetFormTag()));
}
