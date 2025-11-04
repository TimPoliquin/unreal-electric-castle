// Copyright Alien Shores


#include "Character/ElectricCastleCharacter.h"

#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Actor/Effect/DissolveEffectComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Tags/ElectricCastleGameplayTags.h"

AElectricCastleCharacter::AElectricCastleCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ExcludeCharacters, ECR_Overlap);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_ExcludeCharacters, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	// fixes animations not replicating bone positions correctly on the server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Effect Attach Component"));
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	EffectAttachComponent->SetAbsolute(false, true, false);
	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("Burn Debuff Niagara Component"));
	BurnDebuffComponent->SetupAttachment(EffectAttachComponent);
	BurnDebuffComponent->DebuffTag = FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Burn;
	ShockDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("Shock Debuff Niagara Component"));
	ShockDebuffComponent->SetupAttachment(EffectAttachComponent);
	ShockDebuffComponent->DebuffTag = FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Shock;
	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>(
		TEXT("Halo of Protection Niagara Component")
	);
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);
	HaloOfProtectionNiagaraComponent->PassiveSpellTag = FElectricCastleGameplayTags::Get().
		Abilities_Passive_HaloOfProtection;
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>(
		TEXT("Life Siphon Niagara Component")
	);
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	LifeSiphonNiagaraComponent->PassiveSpellTag = FElectricCastleGameplayTags::Get().Abilities_Passive_LifeSiphon;
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>(
		TEXT("Mana Siphon Niagara Component")
	);
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	ManaSiphonNiagaraComponent->PassiveSpellTag = FElectricCastleGameplayTags::Get().Abilities_Passive_ManaSiphon;
}

void AElectricCastleCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AElectricCastleCharacter, ActiveAbilityTag);
	DOREPLIFETIME(AElectricCastleCharacter, StatusEffectTags);
}


FGameplayTag AElectricCastleCharacter::GetHitReactAbilityTagByDamageType_Implementation(
	const FGameplayTag& DamageTypeTag
) const
{
	if (HitReactionsByDamageType.Contains(DamageTypeTag))
	{
		return HitReactionsByDamageType[DamageTypeTag];
	}
	return FElectricCastleGameplayTags::Get().Effect_HitReact_Default;
}

UShapeComponent* AElectricCastleCharacter::GetPrimaryCollisionComponent() const
{
	return GetCapsuleComponent();
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


FVector AElectricCastleCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	if (const FTaggedMontage* ActiveMontageDef = AttackMontages.FindByPredicate(
		[MontageTag](const FTaggedMontage& Item)
		{
			return Item.MontageTag.MatchesTagExact(MontageTag) || Item.SocketTag.MatchesTagExact(MontageTag);
		}
	))
	{
		const FName& SocketName = ActiveMontageDef->SocketName;
		USkeletalMeshComponent* Weapon = Execute_GetWeapon(this);
		if (IsValid(Weapon) && Weapon->HasAnySockets() && Weapon->GetSocketByName(SocketName))
		{
			return Weapon->GetSocketLocation(SocketName);
		}
		if (GetMesh()->GetSocketByName(SocketName))
		{
			return GetMesh()->GetSocketLocation(SocketName);
		}
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
	if (!AbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
	{
		AbilitySystemComponent->AddCharacterAbilities(StartingAbilities, StartingPassiveAbilities);
	}
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FElectricCastleGameplayTags::Get().Effect_HitReact_Default,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(
		this,
		&AElectricCastleCharacter::OnHitReactTagChanged
	);
	TArray<FGameplayTag> HitReactionKeys;
	HitReactionMontageByMontageTag.GetKeys(HitReactionKeys);
	for (const FGameplayTag& HitReactionTag : HitReactionKeys)
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(
			HitReactionTag,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(
			this,
			&AElectricCastleCharacter::OnHitReactTagChanged
		);
	}
}

void AElectricCastleCharacter::OnHitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting
		                                       ? 0
		                                       : BaseWalkSpeed;
	if (bHitReacting)
	{
		GetCharacterMovement()->DisableMovement();
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

bool AElectricCastleCharacter::IsHitReacting_Implementation() const
{
	return bHitReacting;
}

bool AElectricCastleCharacter::IsShocked() const
{
	return StatusEffectTags.Contains(FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Shock);
}

bool AElectricCastleCharacter::IsBurned() const
{
	return StatusEffectTags.Contains(FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Burn);
}

void AElectricCastleCharacter::Dissolve_Implementation() const
{
	if (CharacterDissolveComponent)
	{
		CharacterDissolveComponent->SetTargetMeshComponent(GetMesh());
		CharacterDissolveComponent->PlayTimelineFromStart();
	}
	if (UMeshComponent* WeaponMesh = Execute_GetWeapon(this); WeaponDissolveComponent && WeaponMesh)
	{
		WeaponDissolveComponent->SetTargetMeshComponent(WeaponMesh);
		WeaponDissolveComponent->PlayTimelineFromStart();
	}
}

AActor* AElectricCastleCharacter::GetAvatar_Implementation()
{
	return this;
}

UAnimMontage* AElectricCastleCharacter::GetHitReactMontage_Implementation(const FGameplayTag& HitReactTypeTag)
{
	if (HitReactionMontageByMontageTag.Contains(HitReactTypeTag))
	{
		return HitReactionMontageByMontageTag[HitReactTypeTag];
	}
	return HitReactMontage;
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
	if (USkeletalMeshComponent* Weapon = Execute_GetWeapon(this); IsValid(Weapon))
	{
		Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	}
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

int32 AElectricCastleCharacter::GetMinionCount_Implementation() const
{
	return MinionCount;
}

void AElectricCastleCharacter::ChangeMinionCount_Implementation(const int32 Delta)
{
	MinionCount += Delta;
}

void AElectricCastleCharacter::ApplyDeathImpulse(const FVector& DeathImpulse)
{
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);
	if (USkeletalMeshComponent* Weapon = Execute_GetWeapon(this))
	{
		Weapon->AddImpulse(DeathImpulse, NAME_None, true);
	}
}

void AElectricCastleCharacter::MulticastHandleDeath_Implementation()
{
	bDead = true;
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	}
	if (USkeletalMeshComponent* Weapon = Execute_GetWeapon(this))
	{
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	OnDeathDelegate.Broadcast(this);
}

void AElectricCastleCharacter::OnAbilitySystemReady_Implementation(
	UElectricCastleAbilitySystemComponent* InAbilitySystemComponent
)
{
	GetOnAbilitySystemRegisteredDelegate().Broadcast(InAbilitySystemComponent);
}

void AElectricCastleCharacter::OnEffectChange_LightningDamage(FGameplayTag LightningDamageTag, int Count)
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

void AElectricCastleCharacter::OnDebuffTypeBurnChanged(FGameplayTag GameplayTag, int StackCount)
{
	if (StackCount > 0)
	{
		StatusEffectTags.AddUnique(GameplayTag);
		OnStatusBurnAdded();
	}
	else
	{
		StatusEffectTags.Remove(GameplayTag);
		OnStatusBurnRemoved();
	}
}

void AElectricCastleCharacter::OnDebuffTypeShockChanged(FGameplayTag StunTag, int32 Count)
{
	if (Count > 0)
	{
		StatusEffectTags.AddUnique(StunTag);
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] is Stunned!"), *GetName());
		OnStatusShockAdded();
	}
	else
	{
		StatusEffectTags.Remove(StunTag);
		OnStatusShockRemoved();
	}
}


void AElectricCastleCharacter::RegisterStatusEffectTags(UAbilitySystemComponent* InAbilitySystemComponent)
{
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Shock,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AElectricCastleCharacter::OnDebuffTypeShockChanged);
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FElectricCastleGameplayTags::Get().Effect_Debuff_Type_Burn,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AElectricCastleCharacter::OnDebuffTypeBurnChanged);
	InAbilitySystemComponent->RegisterGameplayTagEvent(
		FElectricCastleGameplayTags::Get().Effect_Damage_Magic_Lightning,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AElectricCastleCharacter::OnEffectChange_LightningDamage);
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
