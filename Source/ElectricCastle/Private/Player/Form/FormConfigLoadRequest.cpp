// Copyright Alien Shores


#include "Player/Form/FormConfigLoadRequest.h"

void UFormConfigLoadRequest::Notify()
{
	Callback.Broadcast(GetPlayerFormConfigRow());
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
	const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow();
	if (!Row.IsValid())
	{
		return true;
	}
	return Row.IsLoaded();
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
