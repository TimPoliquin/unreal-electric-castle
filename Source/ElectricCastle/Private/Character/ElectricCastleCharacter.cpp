// Copyright Alien Shores


#include "Character/ElectricCastleCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/Attribute/ElectricCastleAttributeTypes.h"
#include "Actor/Cinematic/CinematicHandlerComponent.h"
#include "Actor/Effect/DissolvableActor.h"
#include "Actor/Effect/DissolveEffectComponent.h"
#include "Actor/Highlight/HighlightComponent.h"
#include "Actor/Mesh/SocketManagerComponent.h"
#include "Actor/Status/StatusEffectManagerComponent.h"
#include "Animation/AnimInstance.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Equipment/EquipmentActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tags/ElectricCastleGameplayTags.h"

AElectricCastleCharacter::AElectricCastleCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ExcludeCharacters, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_ExcludeCharacters, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Target, ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);
	// fixes animations not replicating bone positions correctly on the server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Effect Attach Component"));
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	EffectAttachComponent->SetAbsolute(false, true, false);
	SocketManagerComponent = CreateDefaultSubobject<USocketManagerComponent>(TEXT("Socket Manager Component"));
	StatusEffectManagerComponent = CreateDefaultSubobject<UStatusEffectManagerComponent>(TEXT("Status Effect Manager Component"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("Highlight Component"));
	HighlightComponent->SetHighlightType(EHighlightType::Friendly);
	CinematicHandlerComponent = CreateDefaultSubobject<UCinematicHandlerComponent>(TEXT("Cinematic Handler Component"));
}

void AElectricCastleCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AElectricCastleCharacter, ActiveAbilityTag);
	DOREPLIFETIME(AElectricCastleCharacter, StatusEffectTags);
}

UShapeComponent* AElectricCastleCharacter::GetPrimaryCollisionComponent() const
{
	return GetCapsuleComponent();
}

USocketManagerComponent* AElectricCastleCharacter::GetSocketManagerComponent_Implementation() const
{
	return SocketManagerComponent;
}

void AElectricCastleCharacter::BeginPlay()
{
	Super::BeginPlay();
	BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (!AbilitySystemComponent)
	{
		GetOnAbilitySystemRegisteredDelegate().AddUObject(this, &AElectricCastleCharacter::RegisterStatusEffectTags);
	}
	else
	{
		RegisterStatusEffectTags(AbilitySystemComponent);
	}
}

void AElectricCastleCharacter::SetActorTickEnabled(bool bEnabled)
{
	Super::SetActorTickEnabled(bEnabled);
	GetMesh()->SetComponentTickEnabled(bEnabled);
	GetMesh()->SetEnableAnimation(bEnabled);
	GetCharacterMovement()->SetComponentTickEnabled(bEnabled);
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->EnableUpdateAnimation(bEnabled);
		if (!bEnabled)
		{
			AnimInstance->StopAllMontages(0);
		}
	}
}


FVector AElectricCastleCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	if (const FTaggedMontage* ActiveMontageDef = AttackMontages.FindByPredicate(
		[MontageTag](const FTaggedMontage& Item)
		{
			return Item.MontageTag.MatchesTagExact(MontageTag) || Item.SocketTag.MatchesTagExact(MontageTag);
		}
	))
	{
		// try to get the location via a socket manager
		if (const USocketManagerComponent* MontageSocketManagerComponent = ActiveMontageDef->IsWeaponMontage
			                                                                   ? GetSocketManagerComponent(Execute_GetWeapon(this))
			                                                                   : SocketManagerComponent.Get())
		{
			if (MontageSocketManagerComponent->HasSocket(ActiveMontageDef->SocketTag))
			{
				return MontageSocketManagerComponent->GetSocketLocation(ActiveMontageDef->SocketTag);
			}
		}
		// try to get the location from the mesh
		if (const FName& SocketName = ActiveMontageDef->SocketName; GetMesh()->GetSocketByName(SocketName))
		{
			return GetMesh()->GetSocketLocation(SocketName);
		}
		// basically failure - just return the actor location
		return GetActorLocation();
	}
	UE_LOG(LogTemp, Warning, TEXT("%s: No montage definition found for tag [%s]"), *GetName(), *MontageTag.ToString());
	return GetActorLocation();
}

void AElectricCastleCharacter::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}
	AbilitySystemComponent->GrantAbilities(Abilities);
	TArray<FGameplayTag> HitReactionKeys;
	HitReactionMontageByMontageTag.GetKeys(HitReactionKeys);
}

void AElectricCastleCharacter::HandleMovementSpeedAttributeChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
	{
		CharacterMovementComponent->MaxWalkSpeed = OnAttributeChangeData.NewValue;
	}
}

void AElectricCastleCharacter::Dissolve_Implementation() const
{
	if (CharacterDissolveComponent)
	{
		CharacterDissolveComponent->SetTargetMeshComponent(GetMesh());
		CharacterDissolveComponent->PlayTimelineFromStart();
	}
	IDissolvableActor::Dissolve(Execute_GetWeapon(this));
}

AActor* AElectricCastleCharacter::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AElectricCastleCharacter::GetAttackMontages_Implementation() const
{
	return AttackMontages;
}

FTaggedMontage AElectricCastleCharacter::GetTagMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (const FTaggedMontage& Item : AttackMontages)
	{
		if (Item.MontageTag.MatchesTagExact(MontageTag))
		{
			return Item;
		}
	}
	return FTaggedMontage();
}

void AElectricCastleCharacter::Die()
{
	MulticastHandleDeath();
}

bool AElectricCastleCharacter::IsDead_Implementation() const
{
	return bDead;
}

UNiagaraSystem* AElectricCastleCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

int32 AElectricCastleCharacter::GetXPReward_Implementation() const
{
	return 0;
}

AActor* AElectricCastleCharacter::GetWeapon_Implementation() const
{
	return nullptr;
}

void AElectricCastleCharacter::HandleTakeDamage(const float IncomingDamage, const FEffectProperties& Props)
{
	if (IncomingDamage > 0)
	{
		if (UElectricCastleAbilitySystemLibrary::IsRadialDamage(Props.EffectContextHandle))
		{
			FRadialDamageEvent RadialDamageEvent;
			RadialDamageEvent.Origin = UElectricCastleAbilitySystemLibrary::GetRadialDamageOrigin(Props.EffectContextHandle);
			RadialDamageEvent.Params.InnerRadius = UElectricCastleAbilitySystemLibrary::GetRadialDamageInnerRadius(Props.EffectContextHandle);
			RadialDamageEvent.Params.OuterRadius = UElectricCastleAbilitySystemLibrary::GetRadialDamageOuterRadius(Props.EffectContextHandle);
			RadialDamageEvent.Params.BaseDamage = IncomingDamage;
			TakeDamage(IncomingDamage, RadialDamageEvent, Props.Source.Controller, Props.Source.AvatarActor);
		}
		else
		{
			FPointDamageEvent PointDamageEvent;
			PointDamageEvent.Damage = IncomingDamage;
			PointDamageEvent.HitInfo.ImpactPoint = UElectricCastleAbilitySystemLibrary::GetRadialDamageOrigin(Props.EffectContextHandle);
			TakeDamage(IncomingDamage, PointDamageEvent, Props.Source.Controller, Props.Source.AvatarActor);
		}
	}
}

void AElectricCastleCharacter::HandleDodge_Implementation()
{
	// do nothing by default
}

void AElectricCastleCharacter::ApplyDeathImpulse(const FVector& DeathImpulse)
{
	if (GetMesh() && GetMesh()->IsSimulatingPhysics())
	{
		GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	}
	if (AActor* Weapon = Execute_GetWeapon(this); IsValid(Weapon) && Weapon->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_AddImpulse(Weapon, DeathImpulse);
	}
}

void AElectricCastleCharacter::MulticastHandleDeath_Implementation()
{
	bDead = true;
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	}
	if (AActor* Weapon = Execute_GetWeapon(this); IsValid(Weapon) && Weapon->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_Unequip(Weapon, this);
		IEquipmentActor::Execute_Detach(Weapon);
	}
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetEnableAnimation(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	HandleDeathLocal();
	OnDeathDelegate.Broadcast(this);
}

void AElectricCastleCharacter::OnAbilitySystemReady_Implementation(
	UElectricCastleAbilitySystemComponent* InAbilitySystemComponent
)
{
	if (UElectricCastleAttributeSet* AttributeSet = GetAttributeSet())
	{
		// initialize movement speed attribute to match character movement speed
		if (const UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
		{
			AttributeSet->SetMovementSpeed(CharacterMovementComponent->MaxWalkSpeed);
		}
		// event handler to sync movement speed attribute and movement component value
		InAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UElectricCastleAttributeSet::GetMovementSpeedAttribute()).AddUObject(
			this,
			&AElectricCastleCharacter::HandleMovementSpeedAttributeChanged
		);
	}
	GetOnAbilitySystemRegisteredDelegate().Broadcast(InAbilitySystemComponent);
}

void AElectricCastleCharacter::OnEffectChange_LightningDamage_Implementation(
	FGameplayTag LightningDamageTag,
	const int Count
)
{
	if (Count > 0)
	{
		OnEffectAdd_LightningDamage();
	}
	else
	{
		OnEffectRemove_LightningDamage();
	}
}

void AElectricCastleCharacter::OnEffectRemove_LightningDamage_Implementation()
{
	// TODO
}

void AElectricCastleCharacter::OnEffectAdd_LightningDamage_Implementation()
{
	// TODO
}

void AElectricCastleCharacter::OnStatusStaggeredAdded_Implementation()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->StopAllMontages(0.f);
	}
	LaunchCharacter(GetActorForwardVector() * -1 * GetStaggerLaunchForce() + FVector::UpVector * GetStaggerLaunchUpwardForce(), true, true);
}

void AElectricCastleCharacter::OnStatusStaggeredRemoved_Implementation()
{
	// TOD
}

float AElectricCastleCharacter::GetStaggerLaunchUpwardForce_Implementation() const
{
	return StaggerLaunchUpwardForce;
}

float AElectricCastleCharacter::GetStaggerLaunchForce_Implementation() const
{
	return StaggerLaunchForce;
}

void AElectricCastleCharacter::Dissolve(
	UMeshComponent* InMesh,
	UMaterialInstance* MaterialInstance,
	void (AElectricCastleCharacter::*Callback)(UMaterialInstanceDynamic*)
)
{
	if (IsValid(InMesh) && IsValid(MaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(
			MaterialInstance,
			this
		);
		InMesh->SetMaterial(0, DynamicMaterialInstance);
		(this->*Callback)(DynamicMaterialInstance);
	}
}

void AElectricCastleCharacter::OnEffectChange_Staggered(FGameplayTag StaggeredTag, int Count)
{
	if (Count > 0)
	{
		StatusEffectTags.AddUnique(StaggeredTag);
		OnStatusStaggeredAdded();
	}
	else
	{
		StatusEffectTags.Remove(StaggeredTag);
		OnStatusStaggeredRemoved();
	}
}


void AElectricCastleCharacter::RegisterStatusEffectTags(UElectricCastleAbilitySystemComponent* InAbilitySystemComponent)
{
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FElectricCastleGameplayTags::Get().Effect_Damage_Magic_Lightning,
		EGameplayTagEventType::NewOrRemoved
	).AddLambda(
		[this](const FGameplayTag& Tag, const int32 Count)
		{
			OnEffectChange_LightningDamage(Tag, Count);
		}
	);
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Staggered,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AElectricCastleCharacter::OnEffectChange_Staggered);
}

void AElectricCastleCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> Attributes, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(Attributes);

	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle GameplayEffectSpec = GetAbilitySystemComponent()->MakeOutgoingSpec(
		Attributes,
		Level,
		EffectContextHandle
	);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
		*GameplayEffectSpec.Data.Get(),
		GetAbilitySystemComponent()
	);
}


UAbilitySystemComponent* AElectricCastleCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AElectricCastleCharacter::SetActorHiddenInGame(const bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);
	TArray<AActor*> ChildActors;
	GetAllChildActors(ChildActors, true);
	for (AActor* Child : ChildActors)
	{
		if (IsValid(Child))
		{
			Child->SetActorHiddenInGame(bNewHidden);
		}
	}
	if (AActor* Weapon = GetWeapon(this))
	{
		Weapon->SetActorHiddenInGame(bNewHidden);
	}
}

float AElectricCastleCharacter::TakeDamage(
	float DamageAmount,
	const struct FDamageEvent& DamageEvent,
	class AController* EventInstigator,
	AActor* DamageCauser
)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(Damage);
	return Damage;
}

FGenericTeamId AElectricCastleCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(static_cast<uint8>(TeamAffiliation));
}

UHighlightComponent* AElectricCastleCharacter::GetHighlightComponent_Implementation() const
{
	return HighlightComponent;
}

void AElectricCastleCharacter::GetHighlightMeshes_Implementation(TArray<UMeshComponent*>& OutHighlightMeshes)
{
	OutHighlightMeshes.Add(GetMesh());
}
