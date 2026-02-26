#include "Cinematic/Context/CinematicContextHandle.h"

#include "Cinematic/Context/CinematicContext.h"

void UCinematicContextHandle::AddRestoreFunction(const TFunction<void()>& Func) const
{
	if (Context.IsValid())
	{
		Context->AddRestoreFunction(Func);
	}
}

bool UCinematicContextHandle::HasTag(const FGameplayTag& Tag) const
{
	return Context.IsValid() && Context->HasTag(Tag);
}

bool UCinematicContextHandle::HasAnyTag(const FGameplayTagContainer& Tags) const
{
	return Context.IsValid() && Context->HasAnyTag(Tags);
}

bool UCinematicContextHandle::ShouldRelocate() const
{
	return Context.IsValid() && Context->ShouldRelocatePlayer();
}

FVector UCinematicContextHandle::GetRelocateLocation() const
{
	return Context.IsValid() ? Context->GetPlayerRelocationLocation() : FVector::ZeroVector;
}

void UCinematicContextHandle::Initialize(UCinematicContext* InContext)
{
	Context = InContext;
}
