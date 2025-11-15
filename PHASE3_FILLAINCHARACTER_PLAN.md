# Phase 3: FillainCharacter Cleanup Plan

## Build Status: Phase 2 Complete ✅ - All Tests Passed ✅

---

## Major Issues Identified in FillainCharacter

### 1. **MASSIVE Camera Watchdog System** (100+ lines of workaround code)
This is the biggest issue - a complex system trying to fix camera problems:

**Methods to Remove:**
- `IsCameraWeird()` - Complex camera state detection
- `CamWatchdogTick()` - Timer-based camera monitoring
- `StartCamWatchdog()` - Starts watchdog timer
- `FixCameraIfWeird()` - Attempts to fix camera
- `Debug_ProbeSpringArmBlocker()` - Debug helper
- `ResetCameraRig()` - Complex camera reset logic
- `Client_PostEquipCameraFix()` - Post-equip camera fix
- `Client_SafeViewAfterEquip()` - Hides meshes/effects to prevent camera issues
- `Client_ForceFollowCamera()` - Forces camera activation
- `Client_NukeScreenOverlays()` - Removes post-process effects
- `CamWatchdogCooldownOff()` - Cooldown management
- `FixSelfCameraCollision()` - Sets collision responses
- `RestoreThirdPersonCameraSafe()` - Another camera restore method

**Properties to Remove:**
- `FTimerHandle CamWatchdogTimer`
- `FTimerHandle CamFixCooldownHandle`
- `bool bCamFixCooldown`
- `float CameraSelfOcclusionThreshold`
- `bool bSelfOccluded`
- `float SelfOcclEnter`
- `float SelfOcclExit`
- `float SelfOcclMinHold`
- `float SelfOcclStateTime`
- `float DefaultArmLength`
- `FVector DefaultTargetOffset`
- `float DefaultFOV`
- `float MinFOV`
- `float MaxFOV`
- `bool bFOVLock`
- `float FOVLockTimeLeft`

**Estimated Removal:** ~200+ lines

---

### 2. **Duplicate Header Includes** (20+ duplicates!)

**Duplicates Found:**
- `Components/CapsuleComponent.h` - 5 times!
- `Camera/CameraComponent.h` - 6 times!
- `GameFramework/SpringArmComponent.h` - 4 times!
- `Components/InputComponent.h` - 2 times
- `TimerManager.h` - 2 times
- `HAL/PlatformStackWalk.h` - 3 times
- `Misc/AssertionMacros.h` - 2 times
- `Logging/LogMacros.h` - 2 times
- `Components/LightComponent.h` - 2 times
- `Components/DecalComponent.h` - 2 times
- `Components/WidgetComponent.h` - 2 times
- `NiagaraComponent.h` - 2 times
- `GameFramework/PlayerController.h` - 2 times
- `HAFGameplayTags.h` - 2 times
- `AbilitySystemComponent.h` - 2 times

**Estimated Removal:** ~25 duplicate lines

---

### 3. **Old Damage System** (Competing with GAS)

**Methods to Remove:**
- `TakeDamage()` override - Just calls Super, does nothing
- `HandleDamage()` override - Just calls Super, does nothing
- `ReceiveDamage()` - Complex damage splitting logic (should be in GAS)
- `CacheDamageParameters()` - Caches damage for later use
- `ResetCachedDamageParameters()` - Resets cached values

**Properties to Remove:**
- `AActor* CachedDamagedPawn`
- `float CachedDamage`
- `const UDamageType* CachedDamageType`
- `AController* CachedInstigatorController`
- `AActor* CachedCauser`
- `float CachedDamageAmount`
- `FDamageEvent CachedDamageEvent`
- `AController* CachedEventInstigator`
- `AActor* CachedDamageCauser`
- `AFillainCharacter* VictimCharacter`
- `AFillainPlayerController* VictimController`

**Estimated Removal:** ~80 lines

---

### 4. **Redundant Weapon Properties**

**Current Weapon Pointers:**
- `AWeaponBase* CharactersWeapon`
- `AMeleeWeapon* CharactersMeleeWeapon`
- `ARangedWeapon* CharactersRangedWeapon`
- `AMajixWeapon* CharactersMajixWeapon`
- `AWeaponBase* EquippedWeapon` (from base class)
- `AMeleeWeapon* EquippedMeleeWeapon` (from base class)
- `ARangedWeapon* EquippedRangedWeapon` (from base class)
- `AMeleeWeapon* AcquiredMeleeWeapon`

**Action:** Consolidate to use base class properties only

---

### 5. **Unused/Redundant Properties**

**To Remove:**
- `bool bEquipInProgress` - Set but never actually prevents re-entry
- `UHAFAttributeSet* HAFAS` - Duplicate of `AttributeSet`
- `bool bIsTogglingWeapon` - Redundant with ActionState
- Various static tracking variables in methods

---

### 6. **AttributeComponent Usage** (Old System)

**Found in:**
- `AddSoulsGatheredToTotalSouls()` - Uses AttributeComponent
- `AddGoldAcquiredToTotalGold()` - Uses AttributeComponent

**Action:** Migrate to GAS AttributeSet

---

### 7. **Commented Dead Code**

**Found:**
- Commented `MulticastHit_Implementation()`
- Commented `OnFillainDying()`
- Commented `Restart()`
- Various commented debug logs

---

## Detailed Cleanup Plan

### **Step 1: Remove Camera Watchdog System**

**Files:** FillainCharacter.h, FillainCharacter.cpp

**Remove ALL camera fix methods:**
```cpp
// From .h
void IsCameraWeird(FString& OutWhy) const;
void CamWatchdogTick();
void StartCamWatchdog(float DurationSec, float TickSec = 0.1f);
void FixCameraIfWeird(const TCHAR* Tag);
void Debug_ProbeSpringArmBlocker();
void ResetCameraRig();
void Client_PostEquipCameraFix();
void Client_SafeViewAfterEquip();
void Client_ForceFollowCamera();
void Client_NukeScreenOverlays();
void CamWatchdogCooldownOff();
void FixSelfCameraCollision();
void RestoreThirdPersonCameraSafe();

// From .cpp - all implementations
```

**Remove ALL camera watchdog properties:**
```cpp
FTimerHandle CamWatchdogTimer;
FTimerHandle CamFixCooldownHandle;
bool bCamFixCooldown;
float CameraSelfOcclusionThreshold;
bool bSelfOccluded;
float SelfOcclEnter;
float SelfOcclExit;
float SelfOcclMinHold;
float SelfOcclStateTime;
float DefaultArmLength;
FVector DefaultTargetOffset;
float DefaultFOV;
float MinFOV;
float MaxFOV;
bool bFOVLock;
float FOVLockTimeLeft;
```

**Remove camera fix calls:**
- Remove `StartCamWatchdog()` call in `ServerEquipButtonPressed_Implementation()`
- Remove `Client_PostEquipCameraFix()` call
- Remove `FixSelfCameraCollision()` call
- Remove FOV lock logic in `Tick()`
- Remove `Client_OnEquipped_Implementation()` FOV lock code

**Keep only:**
- Standard camera setup in constructor
- `HideCharacterIfCameraClose()` - This is a legitimate feature, not a workaround

---

### **Step 2: Clean Up Duplicate Includes**

**Remove duplicates:**
```cpp
// Keep only ONE of each:
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "HAL/PlatformStackWalk.h"
#include "Misc/AssertionMacros.h"
#include "Logging/LogMacros.h"
#include "Components/LightComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/PlayerController.h"
#include "HAFGameplayTags.h"
#include "AbilitySystemComponent.h"
```

---

### **Step 3: Remove Old Damage System**

**Remove methods:**
```cpp
float TakeDamage(...) override;  // Just calls Super
void HandleDamage(...) override; // Just calls Super
void ReceiveDamage(...);         // Complex damage splitting (move to GAS)
void CacheDamageParameters(...);
void ResetCachedDamageParameters();
```

**Remove properties:**
```cpp
AActor* CachedDamagedPawn;
float CachedDamage;
const UDamageType* CachedDamageType;
AController* CachedInstigatorController;
AActor* CachedCauser;
float CachedDamageAmount;
FDamageEvent CachedDamageEvent;
AController* CachedEventInstigator;
AActor* CachedDamageCauser;
AFillainCharacter* VictimCharacter;
AFillainPlayerController* VictimController;
```

**Move damage logic to GAS:**
- Shield/Health splitting should be in ExecutionCalculation
- Death detection should be in OnHealthChanged callback

---

### **Step 4: Consolidate Weapon Properties**

**Remove:**
```cpp
AWeaponBase* CharactersWeapon;        // Use EquippedWeapon
AMeleeWeapon* AcquiredMeleeWeapon;    // Unused
```

**Keep:**
```cpp
// From base class (already have these)
AWeaponBase* EquippedWeapon;
AMeleeWeapon* EquippedMeleeWeapon;
ARangedWeapon* EquippedRangedWeapon;

// FillainCharacter-specific
AMeleeWeapon* CharactersMeleeWeapon;   // Convenience pointer
ARangedWeapon* CharactersRangedWeapon; // Convenience pointer
AMajixWeapon* CharactersMajixWeapon;   // Convenience pointer
```

**Update all references:**
- Replace `CharactersWeapon` with `EquippedWeapon`

---

### **Step 5: Remove Unused Properties**

```cpp
bool bEquipInProgress;      // Set but doesn't prevent re-entry
UHAFAttributeSet* HAFAS;    // Duplicate of AttributeSet
bool bIsTogglingWeapon;     // Redundant with ActionState
```

---

### **Step 6: Migrate AttributeComponent to GAS**

**In `AddSoulsGatheredToTotalSouls()`:**
```cpp
// Before:
AttributeComponent->UpdateTotalSouls(AttributeComponent->GetSoulsGathered());

// After:
if (UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet))
{
    // Use GAS attribute for souls
    HAFAttSet->SetSoulsGathered(HAFAttSet->GetSoulsGathered() + Soul->GetSoulValue());
}
```

**In `AddGoldAcquiredToTotalGold()`:**
```cpp
// Similar migration to GAS
```

---

### **Step 7: Remove Commented Dead Code**

```cpp
/* void AFillainCharacter::MulticastHit_Implementation() { ... } */
/* void AFillainCharacter::OnFillainDying(...) { ... } */
/* void AFillainCharacter::Restart() { ... } */
```

---

## Expected Results

**Before:**
- ~1,800+ lines with camera workarounds and duplicate includes
- Complex camera fix system
- Competing damage systems
- Redundant weapon properties

**After:**
- ~1,500 lines (17% reduction, ~300 lines removed)
- Clean camera setup
- GAS-only damage flow
- Consolidated weapon access
- No AttributeComponent dependency

---

## Files to Modify

1. `Source/HeroesAndFillains/Public/Characters/FillainCharacter.h`
2. `Source/HeroesAndFillains/Private/Characters/FillainCharacter.cpp`

---

## Dependent Code to Update

After FillainCharacter cleanup, we can remove from BaseCharacter:
- `AttributeComponent` completely
- `HandleDamage()` stub
- `ReceiveDamage()` stub
- All cached damage parameters

---

## Testing Required

After cleanup:
1. ✅ Compilation test
2. ✅ Player spawning and movement
3. ✅ Camera behavior (verify no issues after removing watchdog)
4. ✅ Weapon equipping (melee, ranged, majix)
5. ✅ Damage reception (GAS flow)
6. ✅ Player death and respawn
7. ✅ Soul/gold collection

---

**This is the BIGGEST cleanup phase - removing the entire camera workaround system!**

Ready to proceed?
