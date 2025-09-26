// Copyright Alien Shores


#include "Input/ElectricCastleInputComponent.h"


UElectricCastleInputComponent::UElectricCastleInputComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UElectricCastleInputComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UElectricCastleInputComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
