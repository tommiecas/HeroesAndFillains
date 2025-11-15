# Phase 3: FillainCharacter - Next Steps Guide

## ✅ What's Been Completed

### Step 1: Include Cleanup ✅ DONE
- Removed 25+ duplicate includes
- Organized into logical groups
- Build successful (30.27s)

---

## 🎯 Remaining Work (~360 lines to remove)

### Step 2: Remove Camera Watchdog System (~200 lines)

This is the BIGGEST remaining task. The camera watchdog system has:

**13 Methods to Remove:**
1. `IsCameraWeird()` - Line ~270
2. `CamWatchdogTick()` - Line ~300
3. `StartCamWatchdog()` - Line ~330
4. `FixCameraIfWeird()` - Line ~345
5. `RestoreThirdPersonCameraSafe()` - Line ~350
6. `ResetCameraRig()` - Line ~2050 (in ServerEquipButtonPressed area)
7. `Client_SafeViewAfterEquip()` - Line ~2100
8. `Client_ForceFollowCamera()` - Line ~2150
9. `CamWatchdogCooldownOff()` - Line ~2200
10. `Client_NukeScreenOverlays()` - Line ~2220
11. `Client_PostEquipCameraFix()` - Line ~2270
12. `FixSelfCameraCollision()` - Called in ServerEquipButtonPressed
13. `Debug_ProbeSpringArmBlocker()` - Debug method

**Calls to Remove:**
- `StartCamWatchdog(2.0f)` in `ServerEquipButtonPressed_Implementation()` (line ~2040)
- `Client_PostEquipCameraFix()` call (line ~2035)
- `FixSelfCameraCollision()` call (line ~2000)
- `Client_OnEquipped()` call (line ~1995)

**FOV Lock Logic to Remove from Tick():**
```cpp
if (IsLocallyControlled() && bFOVLock && FollowCamera)
{
    FOVLockTimeLeft -= DeltaTime;
    const float FOV = FollowCamera->FieldOfView;
    if (FOV < MinFOV || FOV > MaxFOV)
    {
        UE_LOG(LogTemp, Warning, TEXT("[FOVGuard] Corrected FOV %.1f -> %.1f"), FOV, DefaultFOV);
        FollowCamera->SetFieldOfView(DefaultFOV);
    }
    if (FOVLockTimeLeft <= 0.f) bFOVLock = false;
}
```

**Properties to Remove from Header:**
```cpp
// Camera watchdog
FTimerHandle CamWatchdogTimer;
FTimerHandle CamFixCooldownHandle;
bool bCamFixCooldown;

// FOV management
float DefaultFOV = 90.f;
float MinFOV = 60.f;
float MaxFOV = 120.f;
bool bFOVLock = false;
float FOVLockTimeLeft = 0.f;

// Camera occlusion
float CameraSelfOcclusionThreshold = 160.f;
bool bSelfOccluded = false;
float SelfOcclEnter = 120.f;
float SelfOcclExit = 140.f;
float SelfOcclMinHold = 0.15f;
float SelfOcclStateTime = 0.f;

// Camera defaults
float DefaultArmLength = 450.f;
FVector DefaultTargetOffset = FVector::ZeroVector;
```

---

### Step 3: Remove Old Damage System (~80 lines)

**Methods to Remove:**
```cpp
float TakeDamage(...) override;          // Just calls Super
void HandleDamage(...) override;         // Just calls Super  
void ReceiveDamage(...);                 // ~50 lines - move logic to GAS
void CacheDamageParameters(...);         // ~10 lines
void ResetCachedDamageParameters();      // ~10 lines
void DetermineRolesOnPlayerDeath(...);   // ~15 lines - inline or remove
```

**Properties to Remove from Header:**
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

**Note:** The damage splitting logic in `ReceiveDamage()` should be moved to a GAS ExecutionCalculation instead.

---

### Step 4: Migrate AttributeComponent Usage (~20 lines)

**Update these methods to use GAS:**

```cpp
// Current (uses AttributeComponent):
void AFillainCharacter::AddSoulsGatheredToTotalSouls(ASoul* Soul)
{
    if (AttributeComponent)
    {
        AttributeComponent->UpdateTotalSouls(AttributeComponent->GetSoulsGathered());
        PlayerController->SetHUDSoulsCount(AttributeComponent->GetSoulsGathered());
    }
}

// Should become (uses GAS):
void AFillainCharacter::AddSoulsGatheredToTotalSouls(ASoul* Soul)
{
    if (UHAFAttributeSet* HAFAttSet = Cast<UHAFAttributeSet>(AttributeSet))
    {
        const int32 NewTotal = HAFAttSet->GetSoulsGathered() + Soul->GetSoulValue();
        HAFAttSet->SetSoulsGathered(NewTotal);
        
        if (AFillainPlayerController* PC = Cast<AFillainPlayerController>(Controller))
        {
            PC->SetHUDSoulsCount(NewTotal);
        }
    }
}
```

Same pattern for `AddGoldAcquiredToTotalGold()`.

**In Dodge():**
```cpp
// Remove:
if (!HasEnoughStamina(AttributeComponent->GetDodgeCost())) return;

// Replace with:
if (!HasEnoughStamina(DodgeStaminaCost)) return;
```

---

### Step 5: Remove Redundant Properties (~10 lines)

**From Header, remove:**
```cpp
AWeaponBase* CharactersWeapon;      // Use EquippedWeapon from base class
UHAFAttributeSet* HAFAS;            // Duplicate of AttributeSet
bool bEquipInProgress;              // Set but doesn't prevent re-entry
bool bIsTogglingWeapon;             // Redundant with ActionState
AMeleeWeapon* AcquiredMeleeWeapon;  // Unused
```

**Update all references:**
- Replace `CharactersWeapon` → `EquippedWeapon` (found in multiple equip methods)
- Remove `HAFAS` usage in `InitASC()`
- Remove `bEquipInProgress` checks
- Remove `bIsTogglingWeapon` checks (use `ActionState` instead)

---

### Step 6: Remove Commented Dead Code (~50 lines)

**Remove these commented blocks:**
```cpp
/* void AFillainCharacter::MulticastHit_Implementation() { ... } */
/* void AFillainCharacter::Restart() { ... } */
/* void AFillainCharacter::OnFillainDying(...) { ... } */
```

At end of file (lines ~3800-3820).

---

## 🔧 Recommended Approach

Given the file size (3600+ lines), I recommend:

### **Option A: Systematic Removal (Safer)**
1. Remove camera watchdog methods one at a time
2. Build and test after each major removal
3. Remove old damage system
4. Migrate AttributeComponent
5. Final cleanup

### **Option B: Bulk Removal (Faster)**
1. Remove all camera watchdog code at once
2. Remove all old damage system code
3. Do all migrations together
4. Single build and test

**I recommend Option A** for such a large file to avoid breaking everything at once.

---

## 📋 Testing Checklist (After Completion)

After all Phase 3 cleanup:
- [ ] Compilation successful
- [ ] Player spawns correctly
- [ ] Movement works (WASD, mouse look)
- [ ] **Camera behaves normally** (critical - no watchdog!)
- [ ] Weapon equipping works (melee, ranged, majix)
- [ ] Damage reception works (GAS flow)
- [ ] Player death and respawn
- [ ] Soul/gold collection
- [ ] Dodge functionality
- [ ] No camera glitches during weapon equip

---

## 🚨 Potential Issues to Watch For

1. **Camera Problems After Watchdog Removal:**
   - If camera goes weird after equipping weapons, the underlying issue wasn't fixed
   - May need to investigate weapon attachment/collision setup
   - SpringArm collision settings might need adjustment

2. **Damage System:**
   - Ensure GAS damage flow works for players
   - Shield/Health splitting should be in ExecutionCalculation
   - Death detection should trigger properly

3. **AttributeComponent Dependencies:**
   - Check if any Blueprints reference AttributeComponent
   - Ensure HUD updates work with GAS attributes

---

## 💡 Quick Win Strategy

If you want to see immediate results:

1. **Start with commented dead code** (easiest, ~50 lines)
2. **Remove redundant properties** (low risk, ~10 lines)
3. **Migrate AttributeComponent** (medium risk, ~20 lines)
4. **Remove old damage methods** (medium risk, ~80 lines)
5. **Remove camera watchdog** (highest risk, ~200 lines) - save for last

This way you get quick wins and build confidence before tackling the camera system.

---

**Ready to continue? Let me know which approach you prefer!**
