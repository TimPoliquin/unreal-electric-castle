// Copyright Alien Shores


#include "Actor/Significance/SignificanceSensitiveActor.h"


bool ISignificanceSensitiveActor::IsSignificanceSensitiveActor(const UObject* Object)
{
	return IsValid(Object) && Object->Implements<USignificanceSensitiveActor>();
}

void ISignificanceSensitiveActor::EnterSignificance_FullySignificant(UObject* Object)
{
	if (IsSignificanceSensitiveActor(Object))
	{
		Execute_EnterSignificance_FullySignificant(Object);
	}
}

void ISignificanceSensitiveActor::EnterSignificance_PartiallySignificant(UObject* Object)
{
	if (IsSignificanceSensitiveActor(Object))
	{
		Execute_EnterSignificance_PartiallySignificant(Object);
	}
}

void ISignificanceSensitiveActor::EnterSignificance_Insignificant(UObject* Object)
{
	if (IsSignificanceSensitiveActor(Object))
	{
		Execute_EnterSignificance_Insignificant(Object);
	}
}
