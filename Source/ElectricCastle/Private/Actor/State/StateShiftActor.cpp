// Copyright Alien Shores


#include "Actor/State/StateShiftActor.h"

#include "Components/ShapeComponent.h"
#include "ElectricCastle/ElectricCastle.h"

void IStateShiftActor::EnableStateShiftCollision(UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UStateShiftActor>())
	{
		if (UShapeComponent* CollisionComponent = Execute_GetStateShiftCollisionComponent(Object))
		{
			CollisionComponent->SetGenerateOverlapEvents(true);
			CollisionComponent->SetCollisionResponseToChannel(ECC_StateShift, ECR_Overlap);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Red,
				TEXT("[%s] Must implement IStateShiftActor::GetStateShiftCollisionComponent!"),
				*Object->GetName()
			);
		}
	}
}
