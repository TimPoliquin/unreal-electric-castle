#include "AbilitySystem/ElectricCastleAbilitySystemTypes.h"

bool FElectricCastleGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint32 RepBits = 0;
	if (Ar.IsSaving())
	{
		GetSavingBits(RepBits);
	}
	Ar.SerializeBits(&RepBits, 23);
	AssignValuesToArchive(Ar, Map, RepBits, bOutSuccess);
	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}
	bOutSuccess = true;
	return true;
}

void FElectricCastleGameplayEffectContext::GetSavingBits(uint32& RepBits) const
{
	if (bReplicateInstigator && Instigator.IsValid())
	{
		RepBits |= 1 << 0;
	}
	if (bReplicateEffectCauser && EffectCauser.IsValid())
	{
		RepBits |= 1 << 1;
	}
	if (AbilityCDO.IsValid())
	{
		RepBits |= 1 << 2;
	}
	if (bReplicateSourceObject && SourceObject.IsValid())
	{
		RepBits |= 1 << 3;
	}
	if (Actors.Num() > 0)
	{
		RepBits |= 1 << 4;
	}
	if (HitResult.IsValid())
	{
		RepBits |= 1 << 5;
	}
	if (bHasWorldOrigin)
	{
		RepBits |= 1 << 6;
	}
	if (bIsBlockedHit)
	{
		RepBits |= 1 << 7;
	}
	if (bIsEvadedAttack)
	{
		RepBits |= 1 << 8;
	}
	if (bIsParriedAttack)
	{
		RepBits |= 1 << 9;
	}
	if (bIsCriticalHit)
	{
		RepBits |= 1 << 10;
	}
	if (bIsSuccessfulDebuff)
	{
		RepBits |= 1 << 11;
		if (DebuffDamage > 0)
		{
			RepBits |= 1 << 12;
		}
		if (DebuffDamage > 0.f)
		{
			RepBits |= 1 << 13;
		}
		if (DebuffDuration > 0.f)
		{
			RepBits |= 1 << 14;
		}
		if (DebuffFrequency > 0.f)
		{
			RepBits |= 1 << 15;
		}
	}
	if (DebuffType.IsValid())
	{
		RepBits |= 1 << 16;
	}
	if (DamageType.IsValid())
	{
		RepBits |= 1 << 17;
	}
	if (!DeathImpulse.IsZero())
	{
		RepBits |= 1 << 18;
	}
	if (!KnockbackVector.IsZero())
	{
		RepBits |= 1 << 19;
	}
	if (bIsRadialDamage)
	{
		RepBits |= 1 << 20;
		if (RadialDamageInnerRadius > 0.f)
		{
			RepBits |= 1 << 21;
		}
		if (RadialDamageOuterRadius > 0.f)
		{
			RepBits |= 1 << 22;
		}
		if (RadialDamageOrigin.IsZero())
		{
			RepBits |= 1 << 23;
		}
	}
}

void FElectricCastleGameplayEffectContext::AssignValuesToArchive(
	FArchive& Ar,
	UPackageMap* Map,
	const uint32& RepBits,
	bool& bOutSuccess
)
{
	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = MakeShared<FHitResult>();
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		Ar << bIsBlockedHit;
	}
	if (RepBits & (1 << 8))
	{
		Ar << bIsEvadedAttack;
	}
	if (RepBits & (1 << 9))
	{
		Ar << bIsParriedAttack;
	}
	if (RepBits & (1 << 10))
	{
		Ar << bIsCriticalHit;
	}
	if (RepBits & (1 << 11))
	{
		Ar << bIsSuccessfulDebuff;
	}
	if (RepBits & (1 << 12))
	{
		Ar << DebuffLevel;
	}
	if (RepBits & (1 << 13))
	{
		Ar << DebuffDamage;
	}
	if (RepBits & (1 << 14))
	{
		Ar << DebuffDuration;
	}
	if (RepBits & (1 << 15))
	{
		Ar << DebuffFrequency;
	}
	if (RepBits & (1 << 16))
	{
		if (Ar.IsLoading())
		{
			if (!DebuffType.IsValid())
			{
				DebuffType = MakeShared<FGameplayTag>();
			}
		}
		DebuffType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 17))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = MakeShared<FGameplayTag>();
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 18))
	{
		DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 19))
	{
		KnockbackVector.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 20))
	{
		Ar << bIsRadialDamage;
		if (RepBits & (1 << 21))
		{
			Ar << RadialDamageInnerRadius;
		}
		if (RepBits & (1 << 22))
		{
			Ar << RadialDamageOuterRadius;
		}
		if (RepBits & (1 << 23))
		{
			RadialDamageOrigin.NetSerialize(Ar, Map, bOutSuccess);
		}
	}
}
