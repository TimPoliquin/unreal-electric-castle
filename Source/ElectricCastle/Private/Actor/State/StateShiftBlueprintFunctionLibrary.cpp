// Copyright Alien Shores


#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"

void UStateShiftBlueprintFunctionLibrary::SetActorVisibility(AActor* Actor, const bool bEnableVisibility)
{
	if (IsValid(Actor))
	{
		Actor->SetActorHiddenInGame(!bEnableVisibility);
	}
}
