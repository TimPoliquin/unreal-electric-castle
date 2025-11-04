// Copyright Alien Shores


#include "Actor/POI/PointOfInterestActor.h"

#include "ElectricCastle/ElectricCastle.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Component/ApplyGameplayEffectComponent.h"
#include "UI/Widget/InteractionWidgetInterface.h"


APointOfInterestActor::APointOfInterestActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	OverlapDetectionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Overlap Detection Component"));
	OverlapDetectionComponent->SetCollisionObjectType(ECC_WorldStatic);
	OverlapDetectionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapDetectionComponent->SetGenerateOverlapEvents(true);
	OverlapDetectionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapDetectionComponent->SetupAttachment(GetRootComponent());
	POIWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("POIWidget"));
	POIWidget->SetupAttachment(GetRootComponent());
	POIWidget->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	POIWidget->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(GetRootComponent());
	PreconditionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PreconditionWidget"));
	PreconditionWidget->SetupAttachment(GetRootComponent());
	TArray<FGameplayEffectConfig> EffectConfigs;
	EffectConfigs.Add(FGameplayEffectConfig(nullptr, EEffectApplicationPolicy::ApplyOnOverlap, EEffectRemovalPolicy::RemoveOnEndOverlap));
	TArray<FName> ApplyToTags;
	ApplyToTags.Add(TAG_PLAYER);
	EffectComponent->SetDefaults(EffectConfigs, false, ApplyToTags);
}

void APointOfInterestActor::BeginPlay()
{
	Super::BeginPlay();
	OverlapDetectionComponent->OnComponentBeginOverlap.AddDynamic(this, &APointOfInterestActor::OnBeginOverlap);
	OverlapDetectionComponent->OnComponentEndOverlap.AddDynamic(this, &APointOfInterestActor::OnEndOverlap);
	InitializeState();
}

void APointOfInterestActor::InitializeState()
{
	if (bDisabled)
	{
		DisablePOI();
	}
}

void APointOfInterestActor::PostLoad_Implementation()
{
	InitializeState();
}

void APointOfInterestActor::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool BFromSweep,
	const FHitResult& SweepResult
)
{
	if (bDisabled)
	{
		return;
	}
	if (!IsPlayerActor(OtherActor))
	{
		return;
	}
	// if player, show interaction widget
	if (IsPreconditionMet(OtherActor))
	{
		EffectComponent->OnOverlap(OtherActor);
		ShowInteractWithPOIAvailable(OtherActor);
	}
	else
	{
		ShowPreconditionWidget(OtherActor);
	}
}

void APointOfInterestActor::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	// if player, hide interaction widget
	if (IsPlayerActor(OtherActor))
	{
		EffectComponent->OnEndOverlap(OtherActor);
		IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget());
		IInteractionWidgetInterface::Hide(PreconditionWidget->GetWidget());
	}
}

void APointOfInterestActor::ShowInteractWithPOIAvailable_Implementation(AActor* Player) const
{
	IInteractionWidgetInterface::Show(InteractionWidget->GetWidget());
}

void APointOfInterestActor::ShowPreconditionWidget_Implementation(AActor* Player) const
{
	if (PreconditionWidget && PreconditionWidget->GetWidget())
	{
		IInteractionWidgetInterface::Show(PreconditionWidget->GetWidget());
	}
}

bool APointOfInterestActor::IsPreconditionMet_Implementation(AActor* Player) const
{
	return true;
}

bool APointOfInterestActor::OnInteract_Implementation(AActor* Player)
{
	if (IsPreconditionMet(Player))
	{
		HandleInteract(Player);
	}
	// always terminate the interaction ability
	return true;
}

void APointOfInterestActor::HandleInteract_Implementation(AActor* Player)
{
	// by default, do nothing
}

void APointOfInterestActor::OnInteractionEnd_Implementation(AActor* Player, const bool bIsCancelled)
{
	// TODO ?
}

void APointOfInterestActor::EnablePOI()
{
	bDisabled = false;
	OverlapDetectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (POIWidget && POIWidget->GetWidget())
	{
		POIWidget->GetWidget()->SetVisibility(ESlateVisibility::Visible);
	}
}

void APointOfInterestActor::DisablePOI()
{
	bDisabled = true;
	OverlapDetectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (POIWidget && POIWidget->GetWidget())
	{
		POIWidget->GetWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool APointOfInterestActor::IsPOIDisabled() const
{
	return bDisabled;
}

bool APointOfInterestActor::IsPlayerActor(const AActor* Actor) const
{
	return IsValid(Actor) && Actor->ActorHasTag(TAG_PLAYER);
}
