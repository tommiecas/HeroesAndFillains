# Phase 2: EnemyBase Cleanup Plan

## Issues Identified in EnemyBase

### 1. **Charm/Flee System** (Already Commented Out)
- `TriggerCharm()` - Empty stub with commented code
- `BeginFlee()` - Empty stub with commented code  
- `DoNextFleeHop()` - Empty stub with commented code
- `CachedPlayer`, `bIsCharmed`, `bIsFleeing`, `FleeHopDistance` - Unused properties
- **Action:** Remove completely

### 2. **Old Damage System** (Competing with GAS)
- `TakeDamage()` override - Sets CombatTarget, calls HandleDamage
- `HandleDamage()` override - Applies damage through GAS but also has old logic
- **Action:** Simplify to GAS-only flow

### 3. **Duplicate Weapon Properties**
- `EquippedEnemyWeapon` (base)
- `EquippedEnemyMeleeWeapon` (typed)
- `EquippedEnemyRangedWeapon` (typed)
- All three used inconsistently throughout code
- **Action:** Consolidate to use base class `EquippedWeapon` + typed pointers

### 4. **Unused/Redundant Properties**
- `LastHoveredEnemy` - Never used
- `CachedPC` - Could use `GetPlayerController(0)` directly
- `bCanDamage` - Redundant with `DamagedActors` array
- Duplicate `Particles/ParticleSystemComponent.h` include
- **Action:** Remove

### 5. **Empty State Management Methods**
- `AddStateTag()` - Empty implementation
- `RemoveStateTag()` - Empty implementation
- **Action:** Remove

### 6. **AttributeComponent Usage**
- Still uses `AttributeComponent` in `SpawnSoul()`
- Should use GAS `AttributeSet` instead
- **Action:** Migrate to GAS

### 7. **Inconsistent Method Signatures**
- `PlayAttackMontage()` - No parameters (hides base class version)
- `PlayRandomMeleeAttackMontage()` - Overrides base stub
- `PlayRandomMajixAttackMontage()` - Overrides base stub
- **Action:** Either implement properly or remove overrides

---

## Detailed Cleanup Plan

### **Step 1: Remove Charm/Flee System**

**Files:** EnemyBase.h, EnemyBase.cpp

**Remove from Header:**
```cpp
// Charm/Flee properties
AActor* CachedPlayer;
bool bIsCharmed;
bool bIsFleeing;
float FleeHopDistance;

// Charm/Flee methods
void TriggerCharm(AActor* InPlayerActor);
void BeginFlee();
void DoNextFleeHop();
```

**Remove from CPP:**
- `TriggerCharm()` implementation
- `BeginFlee()` implementation
- `DoNextFleeHop()` implementation

---

### **Step 2: Simplify Damage System to GAS-Only**

**Current Flow:**
```
TakeDamage() → HandleDamage() → Apply GAS Effect → GetHit_Implementation()
```

**Target Flow:**
```
GAS Execution Calculation → Apply Damage Effect → OnHealthChanged → GetHit_Implementation()
```

**Changes:**
1. Remove `TakeDamage()` override completely
2. Keep `HandleDamage()` but mark as deprecated/legacy
3. Ensure damage flows through GAS ExecutionCalculations
4. Remove old damage application logic

---

### **Step 3: Consolidate Weapon Properties**

**Keep:**
- `AWeaponBase* EquippedEnemyWeapon` (primary reference)
- `AMeleeWeapon* EquippedEnemyMeleeWeapon` (typed convenience)
- `ARangedWeapon* EquippedEnemyRangedWeapon` (typed convenience)

**Update:**
- Use base class `EquippedWeapon` where possible
- Update all weapon access to be consistent
- Remove redundant null checks

---

### **Step 4: Remove Unused Properties**

**Remove:**
```cpp
UPROPERTY()
AEnemyBase* LastHoveredEnemy = nullptr;

UPROPERTY()
APlayerController* CachedPC;
```

**Replace `CachedPC` usage with:**
```cpp
UGameplayStatics::GetPlayerController(this, 0)
```

---

### **Step 5: Remove Empty State Methods**

**Remove:**
```cpp
void AEnemyBase::AddStateTag(const FGameplayTag& Tag)
{
    UE_LOG(LogTemp, Verbose, TEXT("%s adding tag %s"), *GetName(), *Tag.ToString());
}

void AEnemyBase::RemoveStateTag(const FGameplayTag& Tag)
{
    UE_LOG(LogTemp, Verbose, TEXT("%s removing tag %s"), *GetName(), *Tag.ToString());
}
```

---

### **Step 6: Migrate AttributeComponent to GAS**

**In `SpawnSoul()`:**

**Before:**
```cpp
SpawnedSoul->SetSoulValue(AttributeComponent->GetSoulsGathered());
```

**After:**
```cpp
if (UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet))
{
    SpawnedSoul->SetSoulValue(HAFAttSet->GetSoulsGathered());
}
```

**Remove:**
- `AttributeComponent` usage completely from EnemyBase

---

### **Step 7: Fix Method Signatures**

**Option A: Remove Overrides (Recommended)**
- Remove `PlayAttackMontage()` override (use base class version)
- Remove `PlayRandomMeleeAttackMontage()` override (use base class version)
- Remove `PlayRandomMajixAttackMontage()` override (use base class version)

**Option B: Implement Properly**
- Match base class signatures
- Add proper implementation

**Recommendation:** Remove overrides since they don't add value over base class

---

### **Step 8: Clean Up Duplicate Includes**

**Remove duplicate:**
```cpp
#include "Particles/ParticleSystemComponent.h"  // Listed twice
#include "MotionWarpingComponent.h"  // Listed twice
```

---

## Expected Results

**Before:**
- ~850 lines with commented code and unused systems
- Competing damage systems
- Inconsistent weapon access
- Method signature conflicts

**After:**
- ~700 lines (18% reduction)
- GAS-only damage flow
- Consistent weapon access
- No method hiding warnings
- Clear, maintainable code

---

## Files to Modify

1. `Source/HeroesAndFillains/Public/Enemies/EnemyBase.h`
2. `Source/HeroesAndFillains/Private/Enemies/EnemyBase.cpp`

---

## Dependent Code to Update

After EnemyBase cleanup, we can remove from BaseCharacter:
- `HandleDamage()` stub (if EnemyBase no longer overrides it)
- `PlayRandomMeleeAttackMontage()` stub
- `PlayRandomMajixAttackMontage()` stub

---

## Testing Required

After cleanup:
1. ✅ Compilation test
2. ✅ Enemy spawning test
3. ✅ Enemy AI behavior (patrol, chase, attack)
4. ✅ Enemy damage reception (GAS flow)
5. ✅ Enemy death and soul spawning
6. ✅ Weapon equipping (melee and ranged)

---

**Ready to proceed with EnemyBase cleanup?**
