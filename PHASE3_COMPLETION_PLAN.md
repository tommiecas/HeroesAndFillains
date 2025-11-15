# Phase 3: FillainCharacter - Completion Action Plan

## Current Status

✅ **Completed:**
- Include cleanup (25+ duplicates removed)
- Build successful (30.27s)

⚠️ **Remaining:** ~360 lines to remove

---

## Quick Wins Strategy - Execution Plan

### **Quick Win #1: Remove Commented Dead Code** (~50 lines) ⭐ EASIEST

**Location:** End of FillainCharacter.cpp (around line 3770-3820)

**Code to Remove:**
```cpp
/******************************************************\
| **   The following were also added for challenges.  ** |
| **   They didn't do squat.			              ** |
\******************************************************/

/* void AFillainCharacter::Restart()
{
	Super::Restart();
} 

void AFillainCharacter::OnFillainDying(AFillainCharacter* KillerFillain, AFillainCharacter* VictimFillain, AFillainPlayerController* InstigatorController)
{
	AHAFGameMode* HAFGameMode = GetWorld()->GetAuthGameMode<AHAFGameMode>();
	VictimCharacter = Cast<AFillainCharacter>(VictimFillain);
	VictimController = Cast<AFillainPlayerController>(VictimCharacter->GetController());
	AFillainPlayerController* KillerController = Cast<AFillainPlayerController>(InstigatorController);
	if (VictimCharacter && HAFGameMode && VictimController && KillerController)
	{
		HAFGameMode->PlayerEliminated(this, VictimController, KillerController);
		VictimController->SetHUDEliminationMessage(KillerController, VictimController);
		KillerController->SetHUDEliminationMessage(KillerController, VictimController);
	}
} */
```

Also remove:
```cpp
/* void AFillainCharacter::MulticastHit_Implementation()
{
	FName SectionName = FName("FromFront");
	PlayHitReactMontage(SectionName);
} */
```

**Risk:** ⭐ VERY LOW - Just commented code
**Build Impact:** None
**Testing Needed:** Compilation only

---

### **Quick Win #2: Remove Old Damage System Methods** (~80 lines) ⭐⭐ LOW RISK

**Methods to Remove from .cpp:**

1. **TakeDamage()** - Just calls Super, does nothing:
```cpp
float AFillainCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                    class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}
```

2. **HandleDamage()** - Just calls Super, does nothing:
```cpp
void AFillainCharacter::HandleDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::HandleDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
```

3. **CacheDamageParameters()** - Never actually used:
```cpp
void AFillainCharacter::CacheDamageParameters(AActor* DamagedPawn, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (CachedDamagedPawn == nullptr && CachedDamage == 0.0f && CachedDamageType == nullptr && CachedInstigatorController == nullptr && CachedCauser == nullptr)
	{
		CachedDamagedPawn = DamagedPawn;
		CachedDamage = Damage;
		CachedDamageType = DamageType;
		CachedInstigatorController = InstigatorController;
		CachedCauser = DamageCauser;
	}
}
```

4. **ResetCachedDamageParameters()** - Never called:
```cpp
void AFillainCharacter::ResetCachedDamageParameters()
{
	CachedDamageAmount = 0.f;
	CachedDamageEvent = FDamageEvent();
	CachedEventInstigator = nullptr;
	CachedDamageCauser = nullptr;
	CachedDamagedPawn = nullptr;
	CachedDamage = 0.0f;
	CachedDamageType = nullptr;
	CachedInstigatorController = nullptr;
	CachedCauser = nullptr;
}
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
```

**Keep for now:**
- `ReceiveDamage()` - Has actual damage logic (will migrate to GAS later)
- `DetermineRolesOnPlayerDeath()` - Called by ReceiveDamage

**Risk:** ⭐⭐ LOW - These methods don't do anything
**Build Impact:** Should compile fine
**Testing Needed:** Verify damage still works (GAS flow)

---

### **Quick Win #3: Remove Redundant Properties** (~10 lines) ⭐⭐ LOW RISK

**From FillainCharacter.h, remove:**
```cpp
UPROPERTY()
AWeaponBase* CharactersWeapon;  // Use EquippedWeapon from base class

UPROPERTY()
UHAFAttributeSet* HAFAS;  // Duplicate of AttributeSet

UPROPERTY()
bool bEquipInProgress;  // Set but doesn't prevent re-entry

UPROPERTY()
bool bIsTogglingWeapon;  // Redundant with ActionState

UPROPERTY()
AMeleeWeapon* AcquiredMeleeWeapon;  // Unused
```

**Update References:**
- Find/Replace `CharactersWeapon` → `EquippedWeapon` (in equip methods)
- Remove `HAFAS` assignment in `InitASC()`
- Remove `bEquipInProgress` checks
- Remove `bIsTogglingWeapon` checks

**Risk:** ⭐⭐ LOW - Just removing duplicates
**Build Impact:** May need to update a few references
**Testing Needed:** Weapon equipping

---

### **Quick Win #4: Migrate AttributeComponent Usage** (~20 lines) ⭐⭐⭐ MEDIUM RISK

**Update AddSoulsGatheredToTotalSouls():**
```cpp
// BEFORE (uses AttributeComponent):
void AFillainCharacter::AddSoulsGatheredToTotalSouls(class ASoul* Soul)
{
	UE_LOG(LogTemp, Warning, TEXT("Adding Souls: %d"), Soul->GetSoulValue());
	if (AttributeComponent)
	{
		AttributeComponent->UpdateTotalSouls(AttributeComponent->GetSoulsGathered());
		AFillainPlayerController* PlayerController = Cast<AFillainPlayerController>(Controller);
		PlayerController->SetHUDSoulsCount(AttributeComponent->GetSoulsGathered());
	}
}

// AFTER (uses GAS):
void AFillainCharacter::AddSoulsGatheredToTotalSouls(class ASoul* Soul)
{
	UE_LOG(LogTemp, Warning, TEXT("Adding Souls: %d"), Soul->GetSoulValue());
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

**Update Dodge():**
```cpp
// Remove this line:
if (!HasEnoughStamina(AttributeComponent->GetDodgeCost())) return;

// Already have this (keep it):
if (!HasEnoughStamina(DodgeStaminaCost)) return;
```

**Risk:** ⭐⭐⭐ MEDIUM - Changes game logic
**Build Impact:** Should compile
**Testing Needed:** Soul/gold collection, dodge functionality

---

### **Final Boss: Remove Camera Watchdog System** (~200 lines) ⭐⭐⭐⭐⭐ HIGHEST RISK

**This is your "nightmare" - save for last!**

**13 Methods to Remove:**
1. IsCameraWeird()
2. CamWatchdogTick()
3. StartCamWatchdog()
4. FixCameraIfWeird()
5. RestoreThirdPersonCameraSafe()
6. ResetCameraRig()
7. Client_SafeViewAfterEquip()
8. Client_ForceFollowCamera()
9. CamWatchdogCooldownOff()
10. Client_NukeScreenOverlays()
11. Client_PostEquipCameraFix()
12. FixSelfCameraCollision()
13. Debug_ProbeSpringArmBlocker()

**15+ Properties to Remove**

**Calls to Remove:**
- `StartCamWatchdog(2.0f)` in ServerEquipButtonPressed
- `Client_PostEquipCameraFix()` call
- `FixSelfCameraCollision()` call
- FOV lock logic in Tick()

**Risk:** ⭐⭐⭐⭐⭐ VERY HIGH - Camera might break
**Build Impact:** Should compile
**Testing Needed:** EXTENSIVE camera testing

---

## Execution Order (Quick Wins First)

1. ✅ **Include cleanup** - DONE
2. ⏳ **Remove commented dead code** - NEXT (50 lines, very low risk)
3. ⏳ **Remove old damage methods** - (80 lines, low risk)
4. ⏳ **Remove redundant properties** - (10 lines, low risk)
5. ⏳ **Migrate AttributeComponent** - (20 lines, medium risk)
6. ⏳ **Remove camera watchdog** - LAST (200 lines, very high risk)

**After each step:** Build → Test → Document → Proceed

---

## Recommended Testing After Each Step

**After Step 2 (commented code):** Compilation only
**After Step 3 (damage methods):** Player damage reception
**After Step 4 (redundant props):** Weapon equipping
**After Step 5 (AttributeComponent):** Soul/gold collection, dodge
**After Step 6 (camera watchdog):** FULL camera testing

---

**Ready to proceed with Step 2 (removing commented dead code)?**
