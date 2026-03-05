// Copyright Alien Shores


#include "UI/Widget/Crosshair/CrosshairWidget.h"

#include "MVVMViewModelBase.h"
#include "Components/Image.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "UI/ViewModel/MVVM_Crosshair.h"

void UCrosshairWidget::InitializeDependencies_Implementation(UMVVM_Crosshair* CrosshairViewModel)
{
	if (!IsValid(CrosshairViewModel))
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] CrosshairViewModel is null"), *GetName());
		return;
	}
	CrosshairViewModel->AddFieldValueChangedDelegate(
		UMVVM_Crosshair::FFieldNotificationClassDescriptor::GetCrosshairState,
		FFieldValueChangedDelegate::CreateUObject(this, &UCrosshairWidget::HandleCrosshairStateChanged)
	);
	CrosshairViewModel->AddFieldValueChangedDelegate(
		UMVVM_Crosshair::FFieldNotificationClassDescriptor::GetCrosshairStyle,
		FFieldValueChangedDelegate::CreateUObject(this, &UCrosshairWidget::HandleCrosshairStyleChanged)
	);
	SetCrosshairState(CrosshairViewModel->GetCrosshairState());
	SetCrosshairStyle(CrosshairViewModel->GetCrosshairStyle());
}

void UCrosshairWidget::TransitionTo_Hidden_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UCrosshairWidget::TransitionTo_Subtle_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UCrosshairWidget::TransitionTo_Visible_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UCrosshairWidget::TransitionTo_StyleTargeting_Implementation()
{
	// nothing to do right now
}

void UCrosshairWidget::TransitionTo_StyleDefault_Implementation()
{
	// nothing to do right now
}

void UCrosshairWidget::SetCrosshairState_Implementation(ECrosshairState InCrosshairState)
{
	if (InCrosshairState == CrosshairState)
	{
		return;
	}
	switch (InCrosshairState)
	{
	case ECrosshairState::Hidden:
		TransitionTo_Hidden();
		break;
	case ECrosshairState::Subtle:
		TransitionTo_Subtle();
		break;
	case ECrosshairState::Active:
		TransitionTo_Visible();
		break;
	}
	CrosshairState = InCrosshairState;
}

void UCrosshairWidget::SetCrosshairStyle_Implementation(ECrosshairStyle InCrosshairStyle)
{
	if (InCrosshairStyle == CrosshairStyle)
	{
		return;
	}
	switch (InCrosshairStyle)
	{
	case ECrosshairStyle::Default:
		TransitionTo_StyleDefault();
		break;
	case ECrosshairStyle::Targeting:
		TransitionTo_StyleTargeting();
		break;
	}
	CrosshairStyle = InCrosshairStyle;
}

void UCrosshairWidget::HandleCrosshairStateChanged(UObject* ViewModel, UE::FieldNotification::FFieldId FieldId)
{
	SetCrosshairState(CastChecked<UMVVM_Crosshair>(ViewModel)->GetCrosshairState());
}

void UCrosshairWidget::HandleCrosshairStyleChanged(UObject* Object, UE::FieldNotification::FFieldId FieldId)
{
	SetCrosshairStyle_Implementation(CastChecked<UMVVM_Crosshair>(Object)->GetCrosshairStyle());
}
