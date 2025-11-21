# Weapon Equip/Draw Bug - DIAGNOSIS & FIX

## 🐛 PROBLEM

**Symptom:** Pressing E once equips weapon to back ✅, but pressing E again does nothing ❌ (should draw weapon to hand)

**Root Cause:** The `bIsTogglingWeapon` flag or `ActionState` is not being reset properly after the first equip animation completes.

---

## 🔍 CODE ANALYSIS

### Current Flow:

1. **First E Press** (Equip to back):
   ```cpp
   EquipButtonPressed() 
   → ToggleArmingAndDisarming()
   → CanDisarm() returns true
   → DisarmOneHandedWeapon() or DisarmTwoHandedWeapon()
   → PlayArmDisarmMontage("DisarmOneHanded")
   → Sets bIsTogglingWeapon = true
   → Sets ActionState = EAS_EquippingWeapon
   ```

2. **Second E Press** (Draw to hand):
   ```cpp
   EquipButtonPressed()
   → ToggleArmingAndDisarming()
   → CanArm() checks if IfPlayerIsDisarmed()
   → Should call ArmOneHandedWeapon()
   → BUT PlayArmDisarmMontage() returns early because:
      - bIsTogglingWeapon is still true, OR
      - Montage_IsPlaying returns true
   ```

### The Problem Code:

```cpp
void AFillainCharacter::PlayArmDisarmMontage(const FName& SectionName)
{
    if (!ArmDisarmMontage) return;
    
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    // ❌ THIS IS THE PROBLEM
    if (AnimInstance->Montage_IsPlaying(ArmDisarmMontage) || bIsTogglingWeapon)
    {
        return;  // Blocks second press!
    }
    
    CombatComponent->ActionState = EActionState::EAS_EquippingWeapon;
    bIsTogglingWeapon = true;
    
    AnimInstance->Montage_Play(ArmDisarmMontage);
    AnimInstance->Montage_JumpToSection(SectionName, ArmDisarmMontage);
}
```

---

## ✅ THE FIX

The issue is that `bIsTogglingWeapon` might not be getting reset, OR the montage callback isn't firing. Here's the fix:

### Fix 1: Ensure Montage Callback Fires

```cpp
void AFillainCharacter::OnArmDisarmMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == ArmDisarmMontage)
    {
        UE_LOG(LogTemp, Warning, TEXT("✅ Arm/Disarm Montage Ended"));
        bIsTogglingWeapon = false;  // ✅ Reset flag
        ResetToFightAgain();  // ✅ Reset ActionState
    }
}
```

### Fix 2: Add Safety Timeout

In case the montage callback doesn't fire, add a safety timer:

```cpp
void AFillainCharacter::PlayArmDisarmMontage(const FName& SectionName)
{
    if (!ArmDisarmMontage) return;
    
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    // ✅ IMPROVED: Only block if ACTUALLY playing, not just flag set
    if (AnimInstance->Montage_IsPlaying(ArmDisarmMontage))
    {
        UE_LOG(LogTemp, Warning, TEXT("⏸️ Montage already playing, ignoring input"));
        return;
    }
    
    // Set state BEFORE playing
    CombatComponent->ActionState = EActionState::EAS_EquippingWeapon;
    bIsTogglingWeapon = true;
    
    // Play montage
    AnimInstance->Montage_Play(ArmDisarmMontage);
    AnimInstance->Montage_JumpToSection(SectionName, ArmDisarmMontage);
    
    // ✅ SAFETY: Reset after montage duration + buffer
    float MontageDuration = ArmDisarmMontage->GetPlayLength();
    FTimerHandle SafetyTimer;
    GetWorldTimerManager().SetTimer(SafetyTimer, [this]()
    {
        if (bIsTogglingWeapon)
        {
            UE_LOG(LogTemp, Warning, TEXT("⚠️ Safety timeout - forcing reset"));
            bIsTogglingWeapon = false;
            ResetToFightAgain();
        }
    }, MontageDuration + 0.5f, false);
}
```

---

## 🎯 RECOMMENDED FIX

Apply this change to `PlayArmDisarmMontage()`:

```cpp
void AFillainCharacter::PlayArmDisarmMontage(const FName& SectionName)
{
    if (!ArmDisarmMontage) return;
    
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (!AnimInstance) return;

    // Only block if montage is ACTUALLY playing (not just flag)
    if (AnimInstance->Montage_IsPlaying(ArmDisarmMontage))
    {
        UE_LOG(LogTemp, Warning, TEXT("⏸️ Arm/Disarm montage already playing"));
        return;
    }
    
    // Reset flag before starting (in case it got stuck)
    bIsTogglingWeapon = false;
    
    // Set state
    CombatComponent->ActionState = EActionState::EAS_EquippingWeapon;
    bIsTogglingWeapon = true;
    
    // Play montage
    float Duration = AnimInstance->Montage_Play(ArmDisarmMontage);
    if (Duration > 0.f)
    {
        AnimInstance->Montage_JumpToSection(SectionName, ArmDisarmMontage);
        UE_LOG(LogTemp, Warning, TEXT("🎬 Playing Arm/Disarm: %s"), *SectionName.ToString());
    }
    else
    {
        // Failed to play - reset immediately
        bIsTogglingWeapon = false;
        ResetToFightAgain();
    }
}
```

---

## 🧪 TESTING

After applying the fix:

1. **Equip weapon** (E key) - Should attach to back ✅
2. **Wait for animation to complete**
3. **Press E again** - Should draw to hand ✅
4. **Press E again** - Should sheathe to back ✅
5. **Repeat** - Should toggle smoothly

---

## 📝 ADDITIONAL CHECKS

If the fix doesn't work, check:

1. **Is the montage callback registered?**
   ```cpp
   // In BeginPlay():
   if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
   {
       AnimInstance->OnMontageEnded.AddDynamic(this, &AFillainCharacter::OnArmDisarmMontageEnded);
   }
   ```

2. **Is ResetToFightAgain() actually resetting?**
   ```cpp
   void AFillainCharacter::ResetToFightAgain()
   {
       CombatComponent->ActionState = EActionState::EAS_Unoccupied;
       UE_LOG(LogTemp, Warning, TEXT("✅ Reset ActionState to Unoccupied"));
   }
   ```

3. **Check BattlePrepped state:**
   ```cpp
   // In ToggleArmingAndDisarming():
   UE_LOG(LogTemp, Warning, TEXT("BattlePrepped: %d"), (int32)BattlePrepped);
   ```

---

**Status:** Fix ready to apply - waiting for user confirmation to proceed
