# Phase 3: FillainCharacter Cleanup - Status Update

## Current Status: ⚠️ BUILD IN PROGRESS

---

## What's Been Done So Far

### ✅ Step 1: Cleaned Up Duplicate Includes

**File:** `Source/HeroesAndFillains/Private/Characters/FillainCharacter.cpp`

**Removed 25+ duplicate includes:**
- `Camera/CameraComponent.h` - was included 6 times, now 1
- `Components/CapsuleComponent.h` - was included 5 times, now 1
- `GameFramework/SpringArmComponent.h` - was included 4 times, now 1
- `HAL/PlatformStackWalk.h` - was included 3 times, now 0 (not needed)
- `Misc/AssertionMacros.h` - was included 2 times, now 0 (not needed)
- `Logging/LogMacros.h` - was included 2 times, now 0 (not needed)
- `Components/InputComponent.h` - was included 2 times, now 1
- `TimerManager.h` - was included 2 times, now 1
- `Components/LightComponent.h` - was included 2 times, now 1
- `Components/DecalComponent.h` - was included 2 times, now 1
- `Components/WidgetComponent.h` - was included 2 times, now 1
- `NiagaraComponent.h` - was included 2 times, now 1
- `GameFramework/PlayerController.h` - was included 2 times, now 1
- `HAFGameplayTags.h` - was included 2 times, now 1
- `AbilitySystemComponent.h` - was included 2 times, now 1
- Plus many others...

**Organized includes into logical groups:**
1. Engine Core
2. Components
3. Input
4. Gameplay Ability System
5. Game-Specific
6. Weapons
7. HAF Components
8. Items & Pickups
9. Enemies
10. Utilities
11. Project-Specific

**Fixed:**
- Added back `HeroesAndFillains/DebugMacros.h` (needed for EQTRACE_MSG macro)

**Lines Removed:** ~25 duplicate include lines

---

## What's Still To Do

### Step 2: Remove Camera Watchdog System (~200 lines)

**Methods to Remove:**
- `IsCameraWeird()` - 30 lines
- `CamWatchdogTick()` - 20 lines
- `StartCamWatchdog()` - 10 lines
- `FixCameraIfWeird()` - 5 lines
- `Debug_ProbeSpringArmBlocker()` - 40 lines
- `ResetCameraRig()` - 30 lines
- `Client_PostEquipCameraFix()` - 5 lines
- `Client_SafeViewAfterEquip()` - 40 lines
- `Client_ForceFollowCamera()` - 30 lines
- `Client_NukeScreenOverlays()` - 30 lines
- `CamWatchdogCooldownOff()` - 5 lines
- `FixSelfCameraCollision()` - 15 lines
- `RestoreThirdPersonCameraSafe()` - 25 lines

**Properties to Remove (from header):**
- `FTimerHandle CamWatchdogTimer`
- `FTimerHandle CamFixCooldownHandle`
- `bool bCamFixCooldown`
- `float DefaultArmLength`
- `FVector DefaultTargetOffset`
- `float DefaultFOV`
- `float MinFOV`
- `float MaxFOV`
- `bool bFOVLock`
- `float FOVLockTimeLeft`
- `float CameraSelfOcclusionThreshold`
- `bool bSelfOccluded`
- `float SelfOcclEnter`
- `float SelfOcclExit`
- `float SelfOcclMinHold`
- `float SelfOcclStateTime`

**Code to Update:**
- Remove `StartCamWatchdog()` call in `ServerEquipButtonPressed_Implementation()`
- Remove `Client_PostEquipCameraFix()` call
- Remove `FixSelfCameraCollision()` call
- Remove FOV lock logic in `Tick()`
- Remove FOV lock code in `Client_OnEquipped_Implementation()`

---

### Step 3: Remove Old Damage System (~80 lines)

**Methods to Remove:**
- `TakeDamage()` - Just calls Super, does nothing
- `HandleDamage()` - Just calls Super, does nothing
- `ReceiveDamage()` - 50 lines of damage splitting (move to GAS)
- `CacheDamageParameters()` - 10 lines
- `ResetCachedDamageParameters()` - 10 lines
- `DetermineRolesOnPlayerDeath()` - 15 lines (inline into ReceiveDamage or remove)

**Properties to Remove (from header):**
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

---

### Step 4: Migrate AttributeComponent Usage (~20 lines)

**Methods to Update:**
- `AddSoulsGatheredToTotalSouls()` - Use GAS AttributeSet instead
- `AddGoldAcquiredToTotalGold()` - Use GAS AttributeSet instead
- `Dodge()` - Remove AttributeComponent->GetDodgeCost() call

---

### Step 5: Remove Redundant Properties

**From Header:**
- `AWeaponBase* CharactersWeapon` - Use `EquippedWeapon` from base class
- `UHAFAttributeSet* HAFAS` - Duplicate of `AttributeSet`
- `bool bEquipInProgress` - Set but doesn't prevent re-entry
- `bool bIsTogglingWeapon` - Redundant with `ActionState`
- `AMeleeWeapon* AcquiredMeleeWeapon` - Unused

**Update References:**
- Replace all `CharactersWeapon` with `EquippedWeapon`
- Remove `HAFAS` usage in `InitASC()`

---

### Step 6: Remove Commented Dead Code (~50 lines)

**Remove:**
```cpp
/* void AFillainCharacter::MulticastHit_Implementation() { ... } */
/* void AFillainCharacter::Restart() { ... } */
/* void AFillainCharacter::OnFillainDying(...) { ... } */
```

---

## Expected Final Results

**Before Phase 3:**
- ~3,822 lines in FillainCharacter.cpp
- 25+ duplicate includes
- 200+ lines of camera workarounds
- 80+ lines of old damage system
- 50+ lines of commented code

**After Phase 3:**
- ~3,367 lines (12% reduction, ~455 lines removed)
- Clean, organized includes
- No camera watchdog system
- GAS-only damage flow
- No commented dead code
- Consolidated weapon properties

---

## Build Status

**Current Build:** ⏳ IN PROGRESS (include cleanup only)

**Next Build:** After removing camera watchdog system

**Final Build:** After all Phase 3 cleanup complete

---

## Testing Plan

After Phase 3 completion:
1. ✅ Compilation test
2. ✅ Player spawning and movement
3. ✅ **Camera behavior** (critical - verify no issues after removing watchdog)
4. ✅ Weapon equipping (melee, ranged, majix)
5. ✅ Damage reception (GAS flow)
6. ✅ Player death and respawn
7. ✅ Soul/gold collection
8. ✅ Dodge functionality

---

**Waiting for build completion before proceeding with camera watchdog removal...**
