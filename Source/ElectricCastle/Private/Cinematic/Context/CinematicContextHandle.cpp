#include "Cinematic/Context/CinematicContextHandle.h"

#include "Cinematic/Context/CinematicContext.h"

FGameplayTagContainer UCinematicContextHandle::GetCinematicTags() const
{
	return Context.IsValid()
		       ? Context->GetCinematicTagContainer()
		       : FGameplayTagContainer::EmptyContainer;
}

void UCinematicContextHandle::AddRestoreFunction(const TFunction<void()>& Func) const
{
	if (Context.IsValid())
	{
		Context->AddRestoreFunction(Func);
	}
}

bool UCinematicContextHandle::HasTag(const FGameplayTag& Tag) const
{
	return Context.IsValid() && Context->GetCinematicTagContainer().HasTag(Tag);
}

bool UCinematicContextHandle::HasAnyTag(const FGameplayTagContainer& Tags) const
{
	return Context.IsValid() && Context->GetCinematicTagContainer().HasAny(Tags);
}

bool UCinematicContextHandle::HasNoneTag(const FGameplayTagContainer& Tags) const
{
	return Context.IsValid() && !Context->GetCinematicTagContainer().HasAny(Tags);
}

void UCinematicContextHandle::Initialize(UCinematicContext* InContext)
{
	Context = InContext;
}
