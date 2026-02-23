#pragma once

#include "Widgets/SCompoundWidget.h"

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "LoadingScreenConfig.h"

class SLoadingScreenWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreenWidget)
		{
		}

		SLATE_ARGUMENT(FLoadingScreenConfig, Config)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FText GetCurrentTipText() const;
	EVisibility GetTipVisibility() const;

	FLoadingScreenConfig Config;
};
