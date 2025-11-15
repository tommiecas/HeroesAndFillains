# Phase 4: Items & Weapons Cleanup Plan

## Overview
After successfully cleaning up the character classes (BaseCharacter, EnemyBase, FillainCharacter), we now need to clean up all Items and Weapons to remove the old damage system and ensure everything uses GAS consistently.

---

## Files Found Using Old Damage System (23 instances)

### Weapons - Ranged
1. **HitScanWeapon.cpp** - Uses `UGameplayStatics::ApplyDamage()`
2. **ProjectileBullet.cpp** - Uses `UGameplayStatics::ApplyDamage()`
3. **Shotgun.cpp** - Uses `UGameplayStatics::ApplyDamage()`
4. **HitScanWeaponAmmo.cpp** - Uses `ReceiveDamage()`

### Weapons - Melee
5. **MeleeWeapon.cpp** - ✅ ALREADY FIXED (removed `ApplyDamage` from `OnBoxOverlap`)

### Enemies (Still have old system)
6. **EnemyBase.cpp** - Has `TakeDamage()`, `HandleDamage()`, uses `ApplyDamage()`
7. **Gnarled.cpp** - Uses `UGameplayStatics::ApplyDamage()`
8. **Gnarledling.cpp** - Uses `UGameplayStatics::ApplyDamage()`
9. **SpectralBase.cpp** - Uses `UGameplayStatics::ApplyDamage()`
10. **StormAssassin.cpp** - Uses `UGameplayStatics::ApplyDamage()`
11. **Thrope.cpp** - Uses `UGameplayStatics::ApplyDamage()` (2 instances)

### Components
12. **CombatComponent.cpp** - Calls `Character->TakeDamage()`
13. **LagCompensationComponent.cpp** - Uses `UGameplayStatics::ApplyDamage()` (3 instances)

### Characters (Remnants)
14. **BaseCharacter.cpp** - Has empty `HandleDamage()`, `ReceiveDamage()` stubs
15. **FillainCharacter.cpp** - Binds `OnTakeAnyDamage` to `ReceiveDamage()`

---

## Cleanup Strategy

### Priority 1: Weapons (High Impact)
These directly cause damage and need to be fixed for gameplay to work correctly.

#### Ranged Weapons
- **HitScanWeapon.cpp**
- **HitScanWeaponAmmo.cpp**
- **ProjectileBullet.cpp**
- **Shotgun.cpp**

**Fix:** Replace `UGameplayStatics::ApplyDamage()` with `Execute_GetHit()` which triggers GAS damage flow.

#### Melee Weapons
- **MeleeWeapon.cpp** - ✅ Already fixed

### Priority 2: Enemy AI Attacks (High Impact)
Enemy melee attacks still use old system.

- **Gnarled.cpp**
- **Gnarledling.cpp**
- **SpectralBase.cpp**
- **StormAssassin.cpp**
- **Thrope.cpp**

**Fix:** Replace `UGameplayStatics::ApplyDamage()` with proper GAS damage application or `Execute_GetHit()`.

### Priority 3: Components (Medium Impact)
- **CombatComponent.cpp**
- **LagCompensationComponent.cpp**

**Fix:** Update to use GAS damage flow instead of calling `TakeDamage()` directly.

### Priority 4: Character Remnants (Low Impact - Cleanup)
- **BaseCharacter.cpp** - Remove empty `HandleDamage()`, `ReceiveDamage()` stubs
- **EnemyBase.cpp** - Remove `TakeDamage()`, `HandleDamage()` overrides
- **FillainCharacter.cpp** - Remove `OnTakeAnyDamage` binding

---

## Detailed Fix Plan

### 1. Ranged Weapons

#### HitScanWeapon.cpp
**Current:**
```cpp
UGameplayStatics::ApplyDamage(FillainCharacter, Damage, ...);
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(FillainCharacter))
{
    HitInterface->Execute_GetHit(FillainCharacter, TraceHitResult.ImpactPoint, GetOwner());
}
```

#### ProjectileBullet.cpp
**Current:**
```cpp
UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor))
{
    HitInterface->Execute_GetHit(OtherActor, Hit.ImpactPoint, GetOwner());
}
```

#### Shotgun.cpp
**Current:**
```cpp
UGameplayStatics::ApplyDamage(DamagePair.Key, DamagePair.Value, ...);
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(DamagePair.Key))
{
    HitInterface->Execute_GetHit(DamagePair.Key, HitLocation, GetOwner());
}
```

#### HitScanWeaponAmmo.cpp
**Current:**
```cpp
HitFillain->ReceiveDamage(HitFillain, Damage, DamageType, KillerController, this);
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(HitFillain))
{
    HitInterface->Execute_GetHit(HitFillain, HitResult.ImpactPoint, GetOwner());
}
```

### 2. Enemy AI Attacks

All enemy melee attacks follow the same pattern:

**Current:**
```cpp
UGameplayStatics::ApplyDamage(Player, DamageAmount, GetController(), this, nullptr);
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(Player))
{
    HitInterface->Execute_GetHit(Player, ImpactPoint, this);
}
```

**Files to update:**
- Gnarled.cpp
- Gnarledling.cpp
- SpectralBase.cpp
- StormAssassin.cpp
- Thrope.cpp (2 instances - hand and foot attacks)

### 3. Components

#### CombatComponent.cpp
**Current:**
```cpp
float FinalDamage = Character->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(Character))
{
    HitInterface->Execute_GetHit(Character, ImpactPoint, DamageCauser);
}
```

#### LagCompensationComponent.cpp
**Current:**
```cpp
UGameplayStatics::ApplyDamage(HitCharacter, Damage, ...);
```

**Fix:**
```cpp
// Damage now handled through GAS via GetHit_Implementation
if (IHitInterface* HitInterface = Cast<IHitInterface>(HitCharacter))
{
    HitInterface->Execute_GetHit(HitCharacter, ImpactPoint, GetOwner());
}
```

### 4. Character Cleanup

#### BaseCharacter.cpp
**Remove:**
- Empty `HandleDamage()` method
- Empty `ReceiveDamage()` method
- Comment about not calling TakeDamage

#### EnemyBase.cpp
**Remove:**
- `TakeDamage()` override
- `HandleDamage()` method
- `ApplyDamage()` call in overlap

#### FillainCharacter.cpp
**Remove:**
- `OnTakeAnyDamage.AddDynamic(this, &AFillainCharacter::ReceiveDamage);` binding
- `ReceiveDamage()` method

---

## Expected Benefits

1. **Consistent Damage Flow:** All damage goes through GAS → GameplayEffect → AttributeSet → GetHit_Implementation()
2. **No More Competing Systems:** Only one damage path
3. **Proper Damage Calculation:** GAS handles resistances, buffs, modifiers
4. **Better Debugging:** Single point of failure to debug
5. **Network Replication:** GAS handles replication automatically

---

## Testing Checklist

After cleanup, test:
- [ ] Player ranged weapons damage enemies
- [ ] Player melee weapons damage enemies
- [ ] Enemy ranged attacks damage player
- [ ] Enemy melee attacks damage player
- [ ] Damage shows in HUD (health/shield bars)
- [ ] Hit reactions play correctly
- [ ] Death triggers when health reaches 0
- [ ] No errors in output log

---

## Execution Order

1. ✅ **MeleeWeapon.cpp** - Already fixed
2. **Ranged Weapons** (4 files) - Fix all ranged weapon damage
3. **Enemy AI Attacks** (5 files) - Fix all enemy melee attacks
4. **Components** (2 files) - Update combat and lag compensation
5. **Character Cleanup** (3 files) - Remove old system remnants
6. **Compile and Test** - Verify everything works

---

## Notes

- All fixes follow the same pattern: Replace `ApplyDamage()` with `Execute_GetHit()`
- `GetHit_Implementation()` in BaseCharacter already handles GAS damage application
- No need to pass damage amount - GAS calculates it from GameplayEffects
- Impact point is needed for hit reactions and visual effects

---

## Success Criteria

- ✅ All 23 instances of old damage system removed
- ✅ All damage flows through GAS
- ✅ Compilation successful with no errors
- ✅ All damage types work in-game (melee, ranged, player, enemy)
- ✅ HUD updates correctly
- ✅ No damage-related errors in output log
