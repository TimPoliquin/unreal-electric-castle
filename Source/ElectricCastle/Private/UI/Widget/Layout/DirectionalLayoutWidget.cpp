// Copyright Alien Shores

#include "UI/Widget/Layout/DirectionalLayoutWidget.h"
#include "UI/Widget/Layout/DirectionalLayoutSlot.h"

UClass* UDirectionalLayoutWidget::GetSlotClass() const
{
	return UDirectionalLayoutSlot::StaticClass();
}

TSharedRef<SWidget> UDirectionalLayoutWidget::RebuildWidget()
{
	MyHBox = nullptr;
	MyVBox = nullptr;

	if (!MyBox.IsValid())
	{
		MyBox = SNew(SBox);
	}

	RebuildChildren();

	return MyBox.ToSharedRef();
}

void UDirectionalLayoutWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	bool bNeedsRebuild = (IsHorizontal() && !MyHBox.IsValid()) ||
		(!IsHorizontal() && !MyVBox.IsValid());

	if (bNeedsRebuild)
	{
		RebuildChildren();
		return;
	}

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UDirectionalLayoutSlot* TypedSlot = Cast<UDirectionalLayoutSlot>(PanelSlot))
		{
			TypedSlot->SynchronizeProperties();
		}
	}
}

void UDirectionalLayoutWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyBox = nullptr;
	MyHBox = nullptr;
	MyVBox = nullptr;
}

void UDirectionalLayoutWidget::OnSlotAdded(UPanelSlot* InSlot)
{
	if (!MyBox.IsValid())
	{
		return;
	}
	RebuildChildren();
}

void UDirectionalLayoutWidget::OnSlotRemoved(UPanelSlot* InSlot)
{
	if (!MyBox.IsValid())
	{
		return;
	}
	RebuildChildren();
}

void UDirectionalLayoutWidget::SetLayoutDirection(ELayoutDirection NewDirection)
{
	if (LayoutDirection == NewDirection)
	{
		return;
	}
	LayoutDirection = NewDirection;
	RebuildChildren();
}

UDirectionalLayoutSlot* UDirectionalLayoutWidget::AddChildToDirectionalLayout(UWidget* Content)
{
	return Cast<UDirectionalLayoutSlot>(AddChild(Content));
}

void UDirectionalLayoutWidget::RebuildChildren()
{
	if (!MyBox.IsValid())
	{
		return;
	}

	MyHBox = nullptr;
	MyVBox = nullptr;

	TArray<UPanelSlot*> Ordered;
	if (IsReversed())
	{
		for (int32 i = Slots.Num() - 1; i >= 0; --i)
		{
			Ordered.Add(Slots[i]);
		}
	}
	else
	{
		Ordered = Slots;
	}

	if (IsHorizontal())
	{
		MyHBox = SNew(SHorizontalBox);
		MyBox->SetContent(MyHBox.ToSharedRef());

		for (UPanelSlot* PanelSlot : Ordered)
		{
			if (UDirectionalLayoutSlot* TypedSlot = Cast<UDirectionalLayoutSlot>(PanelSlot))
			{
				if (TypedSlot->Content)
				{
					auto SlotArgs = MyHBox->AddSlot();
					SlotArgs[TypedSlot->Content->TakeWidget()];
					TypedSlot->BuildHorizontalSlot(SlotArgs);
				}
			}
		}
	}
	else
	{
		MyVBox = SNew(SVerticalBox);
		MyBox->SetContent(MyVBox.ToSharedRef());

		for (UPanelSlot* PanelSlot : Ordered)
		{
			if (UDirectionalLayoutSlot* TypedSlot = Cast<UDirectionalLayoutSlot>(PanelSlot))
			{
				if (TypedSlot->Content)
				{
					auto SlotArgs = MyVBox->AddSlot();
					SlotArgs[TypedSlot->Content->TakeWidget()];
					TypedSlot->BuildVerticalSlot(SlotArgs);
				}
			}
		}
	}
}

#if WITH_EDITOR
void UDirectionalLayoutWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName LayoutDirectionName = GET_MEMBER_NAME_CHECKED(UDirectionalLayoutWidget, LayoutDirection);

	if (PropertyChangedEvent.GetPropertyName() == LayoutDirectionName)
	{
		TSharedPtr<SWidget> SafeWidget = GetCachedWidget();
		if (SafeWidget.IsValid())
		{
			RebuildChildren();
		}
	}
}
#endif
