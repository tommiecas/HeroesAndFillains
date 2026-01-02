#include "AbilitySystem/ExecutionCalculations/Damage/ExecutionCalculation_DamageStatics.h"

#include "AbilitySystem/HAFAttributeSet.h"

HAFDamageStatics::HAFDamageStatics()
{
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Armor, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ArmorPenetration, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, BlockChance, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitChance, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitDamage, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, CriticalHitResistance, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Agility, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Flexibility, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Purity, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Corruptibility, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Intuition, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Vision, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Charm, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Speed, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, HealthRegeneration, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ShieldRegeneration, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, StaminaRegeneration, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MajixRegeneration, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxHealth, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxShield, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxStamina, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, MaxMajix, Source, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Fireproof, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Shockproof, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ChaosIncorruptible, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Invulnerability, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, HeartOfDarkness, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, ThermalRadiation, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Immunity, Target, false);
    DEFINE_ATTRIBUTE_CAPTUREDEF(UHAFAttributeSet, Unstoppable, Target, false);
}

const HAFDamageStatics& DamageStatics()
{
    static HAFDamageStatics DStatics;
    return DStatics;
}
