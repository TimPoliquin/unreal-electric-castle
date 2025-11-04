// Copyright Alien Shores


#include "Player/Form/FormConfigLoadRequest.h"

void UFormConfigLoadRequest::AddToLoad(const TSoftObjectPtr<>& ToLoad)
{
	if (!ToLoad.IsNull())
	{
		Assets.Add(ToLoad);
	}
}

void UFormConfigLoadRequest::AddToLoad(const TSoftClassPtr<>& ToLoad)
{
	if (!ToLoad.IsNull())
	{
		Classes.Add(ToLoad);
	}
}

void UFormConfigLoadRequest::LoadAsync()
{
	FLoadSoftObjectPathAsyncDelegate LoadDelegate;
	LoadDelegate.BindLambda([this](FSoftObjectPath ObjectPath, UObject* Loaded)
		{
			if (ShouldNotify())
			{
				Notify();
			}
		}
	);
	if (Assets.Num() == 0 && Classes.Num() == 0)
	{
		Notify();
	}
	for (TSoftObjectPtr<>& Asset : Assets)
	{
		Asset.LoadAsync(LoadDelegate);
	}
	for (TSoftClassPtr<>& Class : Classes)
	{
		Class.LoadAsync(LoadDelegate);
	}
}

void UFormConfigLoadRequest::Notify()
{
	OnLoadComplete.Broadcast(GetPlayerFormConfigRow());
	bHasNotified = true;
}

bool UFormConfigLoadRequest::IsValid() const
{
	return this->FormTag.IsValid();
}

bool UFormConfigLoadRequest::HasNotified() const
{
	return bHasNotified;
}

bool UFormConfigLoadRequest::ShouldNotify() const
{
	if (!IsValid() || HasNotified())
	{
		return false;
	}
	if (const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow(); !Row.IsValid())
	{
		return true;
	}
	for (TSoftObjectPtr<> Asset : Assets)
	{
		if (!Asset.IsValid())
		{
			return false;
		}
	}
	for (TSoftClassPtr<> Class : Classes)
	{
		if (!Class.IsValid())
		{
			return false;
		}
	}
	return true;
}

FPlayerFormConfigRow UFormConfigLoadRequest::GetPlayerFormConfigRow() const
{
	if (!FormConfig)
	{
		return FPlayerFormConfigRow();
	}
	return FormConfig->GetPlayerFormConfigRowByTag(FormTag);
}

UFormConfigLoadRequest* UFormConfigLoadRequest::Create(const FGameplayTag& FormTag, UPlayerFormConfig* Config)
{
	UFormConfigLoadRequest* LoadRequest = NewObject<UFormConfigLoadRequest>();
	LoadRequest->FormTag = FormTag;
	LoadRequest->FormConfig = Config;
	return LoadRequest;
}
