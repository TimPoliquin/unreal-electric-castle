#include "System/Loading/SLoadingScreenWidget.h"

#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Styling/AppStyle.h"
#include "Widgets/Images/SThrobber.h"

void SLoadingScreenWidget::Construct(const FArguments& InArgs)
{
	Config = InArgs._Config;

	FSlateFontInfo TitleFont = FCoreStyle::GetDefaultFontStyle("Bold", 18);
	FSlateFontInfo TipFont = FCoreStyle::GetDefaultFontStyle("Regular", 16);

	ChildSlot
	[
		SNew(SOverlay)

		// ── Layer 1: Background image ─────────────────────────────────────
		+ SOverlay::Slot()
		[
			SNew(SScaleBox)
			.Stretch(EStretch::ScaleToFill)
			[
				SNew(SImage)
				.Image(Config.BackgroundBrush ? Config.BackgroundBrush : FAppStyle::GetBrush("WhiteBrush"))
			]
		]

		// ── Layer 2: Bottom bar ───────────────────────────────────────────
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(60.f, 0.f, 60.f, 50.f))
		[
			SNew(SVerticalBox)

			// Row 1: Tip text (left-aligned, full width)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 0.f, 0.f, 20.f)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Text(this, &SLoadingScreenWidget::GetCurrentTipText)
				.Visibility(this, &SLoadingScreenWidget::GetTipVisibility)
				.Font(TipFont)
				.ColorAndOpacity(FLinearColor(0.85f, 0.85f, 0.85f, 1.f))
				.ShadowOffset(FVector2D(1.f, 1.f))
				.ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.9f))
				.WrapTextAt(1400.f)
			]

			// Row 2: Spinner + "Loading..." label, right-aligned, 16px above progress bar
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.Padding(0.f, 0.f, 0.f, 16.f)
			[
				SNew(SHorizontalBox)

				// Spinning graphic
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.f, 0.f, 10.f, 0.f)
				[
					SNew(SCircularThrobber)
					                       .Radius(10.f) // Matches the 20x20 size we had before (radius = half diameter)
					                       .Period(0.75f) // Seconds per full rotation — lower is faster
					                       .NumPieces(8) // Number of dots in the ring
					                       .ColorAndOpacity(FLinearColor::White)
				]

				// "Loading..." label
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(Config.TitleText)
					.Font(TitleFont)
					.ColorAndOpacity(FLinearColor::White)
					.ShadowOffset(FVector2D(1.f, 1.f))
					.ShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.8f))
				]
			]

			// Row 3: Progress bar (full width, anchors the right edge for the row above)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SProgressBar)
				.FillColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.9f))
			]
		]
	];
}

FText SLoadingScreenWidget::GetCurrentTipText() const
{
	if (Config.Tips.IsEmpty())
	{
		return FText::GetEmpty();
	}

	const int32 Index = static_cast<int32>(FPlatformTime::Seconds() / Config.TipRotationInterval)
		% Config.Tips.Num();

	return Config.Tips[Index];
}

EVisibility SLoadingScreenWidget::GetTipVisibility() const
{
	return Config.Tips.IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}
