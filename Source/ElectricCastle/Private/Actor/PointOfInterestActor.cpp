// Copyright Alien Shores


#include "Actor/POI/PointOfInterestActor.h"

#include "TimerManager.h"
#include "Actor/Highlight/HighlightComponent.h"
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
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("HighlightComponent"));
	HighlightComponent->SetHighlightType(EHighlightType::Friendly);
}

void APointOfInterestActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	OnConstruction_InitializeWidgets();
}

void APointOfInterestActor::OnConstruction_InitializeWidgets()
{
	if (UUserWidget* Widget = InteractionWidget->GetWidget())
	{
		IInteractionWidgetInterface::SetActionText(Widget, InteractText);
	}
	if (UUserWidget* Widget = PreconditionWidget->GetWidget())
	{
		IInteractionWidgetInterface::SetActionText(Widget, PreconditionText);
		IInteractionWidgetInterface::SetIcon(Widget, GetPreconditionIcon());
	}
}

void APointOfInterestActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APointOfInterestActor, InteractText))
	{
		if (UUserWidget* Widget = InteractionWidget->GetWidget())
		{
			IInteractionWidgetInterface::SetActionText(Widget, InteractText);
		}
	}
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(APointOfInterestActor, PreconditionText))
	{
		if (UUserWidget* Widget = PreconditionWidget->GetWidget())
		{
			IInteractionWidgetInterface::SetActionText(Widget, PreconditionText);
		}
	}
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
		return;
	}
	if (InteractionWidget && InteractionWidget->GetWidget()
	)
	{
		IInteractionWidgetInterface::SetActionText(InteractionWidget->GetWidget(), InteractText);
	}
	if (PreconditionWidget && PreconditionWidget->GetWidget())
	{
		IInteractionWidgetInterface::SetActionText(PreconditionWidget->GetWidget(), PreconditionText);
		IInteractionWidgetInterface::SetIcon(PreconditionWidget->GetWidget(), GetPreconditionIcon());
	}
}

void APointOfInterestActor::PostLoad_Implementation()
{
	InitializeState();
}

UHighlightComponent* APointOfInterestActor::GetHighlightComponent_Implementation() const
{
	return HighlightComponent;
}

void APointOfInterestActor::GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes)
{
	GetComponents<UMeshComponent>(OutHighlightMeshes);
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
	if (bDisabled || bIsOverlapping)
	{
		return;
	}
	if (!IsPlayerActor(OtherActor))
	{
		return;
	}
	bIsOverlapping = true;
	GetWorld()->GetTimerManager().ClearTimer(OverlapDelayTimerHandle);
	// if player, show interaction widget
	HandlePlayerOverlapBegin(OtherActor);
}

void APointOfInterestActor::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	// if player, hide interaction widget
	if (!IsPlayerActor(OtherActor))
	{
		return;
	}
	if (!bIsOverlapping)
	{
		return;
	}
	if (OverlapDetectionComponent->IsOverlappingActor(OtherActor))
	{
		// we're still overlapping the actor; some not all of the components
		return;
	}
	HandlePlayerOverlapEnd(OtherActor);
}

UTexture2D* APointOfInterestActor::GetPreconditionIcon_Implementation() const
{
	// nothing here by default
	return nullptr;
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

void APointOfInterestActor::HandlePlayerOverlapBegin_Implementation(AActor* OtherActor)
{
	HighlightComponent->Highlight();
	if (IsPreconditionMet(OtherActor))
	{
		EffectComponent->OnOverlap(OtherActor);
		IInteractionWidgetInterface::Hide(PreconditionWidget->GetWidget());
		ShowInteractWithPOIAvailable(OtherActor);
	}
	else
	{
		IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget());
		ShowPreconditionWidget(OtherActor);
		EffectComponent->OnEndOverlap(OtherActor);
	}
}

void APointOfInterestActor::HandlePlayerOverlapEnd_Implementation(AActor* OtherActor)
{
	HighlightComponent->Unhighlight();
	GetWorld()->GetTimerManager().ClearTimer(OverlapDelayTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(
		OverlapDelayTimerHandle,
		[this, OtherActor]()
		{
			if (OverlapDetectionComponent->IsOverlappingActor(OtherActor))
			{
				// we're still overlapping the actor; some not all of the components
				return;
			}
			bIsOverlapping = false;
			EffectComponent->OnEndOverlap(OtherActor);
			IInteractionWidgetInterface::Hide(InteractionWidget->GetWidget());
			IInteractionWidgetInterface::Hide(PreconditionWidget->GetWidget());
		},
		.1f,
		false
	);
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

void APointOfInterestActor::EnablePOI_Implementation()
{
	bDisabled = false;
	OverlapDetectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (POIWidget && POIWidget->GetWidget())
	{
		POIWidget->GetWidget()->SetVisibility(ESlateVisibility::Visible);
	}
}

void APointOfInterestActor::DisablePOI_Implementation()
{
	bDisabled = true;
	OverlapDetectionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HighlightComponent->SetHighlightable(false);
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
