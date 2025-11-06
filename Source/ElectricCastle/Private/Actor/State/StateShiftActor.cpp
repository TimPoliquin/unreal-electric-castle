// Copyright Alien Shores


#include "Actor/State/StateShiftActor.h"

#include "Actor/State/StateShiftBlueprintFunctionLibrary.h"
#include "Actor/State/StateShiftComponent.h"
#include "Components/ShapeComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"


// Sets default values
AStateShiftActor::AStateShiftActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	StateShiftComponent = CreateDefaultSubobject<UStateShiftComponent>(TEXT("StateShiftComponent"));
}

UStateShiftComponent* AStateShiftActor::GetStateShiftComponent() const
{
	return StateShiftComponent;
}


UShapeComponent* AStateShiftActor::GetStateShiftCollisionComponent() const
{
	return FindStateShiftCollisionComponent();
}


void AStateShiftActor::StateShiftReaction_Implementation(const FStateShiftStateChangedPayload& Payload)
{
	if (HasAuthority())
	{
		StateShiftReaction_NetMulticast(Payload);
	}
}

void AStateShiftActor::BeginPlay()
{
	Super::BeginPlay();
	if (UShapeComponent* CollisionComponent = GetStateShiftCollisionComponent())
	{
		CollisionComponent->SetGenerateOverlapEvents(true);
		CollisionComponent->SetCollisionResponseToChannel(ECC_StateShift, ECR_Overlap);
	}
	else
	{
		const FString ErrorLog = FString::Printf(
			TEXT("[%s] must implement FindStateShiftCollisionComponent!"),
			*GetName()
		);
		UE_LOG(LogElectricCastle, Error, TEXT("%s"), *ErrorLog);
		GEngine->AddOnScreenDebugMessage(
			-1,
			20.f,
			FColor::Red,
			ErrorLog
		);
	}
}

void AStateShiftActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	StateShiftComponent->OnStateShiftStateChanged.AddDynamic(this, &AStateShiftActor::StateShiftReaction);
}

UShapeComponent* AStateShiftActor::FindStateShiftCollisionComponent_Implementation() const
{
	return nullptr;
}

void AStateShiftActor::StateShiftReaction_NetMulticast_Implementation(const FStateShiftStateChangedPayload& Payload)
{
	for (const EStateShiftReactionType Reaction : StateShiftComponent->GetReactions(Payload.NewState))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] Actor executing reaction: %s"),
			*GetName(),
			*UEnum::GetValueAsString(Reaction)
		)
		switch (Reaction)
		{
		case EStateShiftReactionType::Custom:
			StateShiftReaction_Custom();
			break;
		case EStateShiftReactionType::Visibility_Show:
			StateShiftReaction_Visibility_Show();
			break;
		case EStateShiftReactionType::Visibility_Hide:
			StateShiftReaction_Visibility_Hide();
			break;
		case EStateShiftReactionType::Visibility_FadeIn:
			StateShiftReaction_Visibility_FadeIn();
			break;
		case EStateShiftReactionType::Visibility_FadeOut:
			StateShiftReaction_Visibility_FadeOut();
			break;
		case EStateShiftReactionType::Collision_Enable:
			StateShiftReaction_Collision_Enable();
			break;
		case EStateShiftReactionType::Collision_Disable:
			StateShiftReaction_Collision_Disable();
			break;
		default:
			break;
		}
	}
}

void AStateShiftActor::StateShiftReaction_Visibility_Show_Implementation()
{
	UStateShiftBlueprintFunctionLibrary::SetActorVisibility(this, true);
}

void AStateShiftActor::StateShiftReaction_Visibility_Hide_Implementation()
{
	UStateShiftBlueprintFunctionLibrary::SetActorVisibility(this, false);
}

void AStateShiftActor::StateShiftReaction_Visibility_FadeOut_Implementation()
{
	StateShiftReaction_Visibility_Hide_Implementation();
}

void AStateShiftActor::StateShiftReaction_Visibility_FadeIn_Implementation()
{
	StateShiftReaction_Visibility_Show_Implementation();
}

void AStateShiftActor::StateShiftReaction_Collision_Enable_Implementation()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Red,
		TEXT("[%s] must implement StateShiftReaction_Collision_Enable!"),
		*GetName()
	);
}


void AStateShiftActor::StateShiftReaction_Collision_Disable_Implementation()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Red,
		TEXT("[%s] must implement StateShiftReaction_Collision_Disable!"),
		*GetName()
	);
}

void AStateShiftActor::StateShiftReaction_Custom_Implementation()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f,
		FColor::Red,
		TEXT("[%s] must implement StateShiftReaction_Custom!"),
		*GetName()
	);
}
