# Lessons Learned from BaseCharacter Cleanup

## Key Insight: Incremental Cleanup is Essential

### What We Learned

When cleaning up a large codebase with inheritance hierarchies, **you cannot simply delete base class methods** if child classes override them. This causes immediate compilation failures.

### The Problem

**Initial Approach (Failed):**
1. Removed `TakeDamage()`, `HandleDamage()`, `ReceiveDamage()` from BaseCharacter
2. Removed `PlayRandomMeleeAttackMontage()`, `PlayRandomMajixAttackMontage()`
3. Removed `AttributeComponent`
4. Build failed with 50+ errors from child classes

**Compilation Errors:**
```
error C3668: 'AEnemyBase::HandleDamage': method with override specifier 'override' did not override any base class methods
error C3668: 'AFillainCharacter::PlayRandomMeleeAttackMontage': method with override specifier 'override' did not override any base class methods
error C2065: 'AttributeComponent': undeclared identifier
error C2065: 'CachedEventInstigator': undeclared identifier
```

### The Solution

**Revised Approach (Working):**
1. Keep methods as **empty virtual stubs** in base class
2. Mark them with **TODO comments** for future removal
3. Clean up child classes **first** in subsequent phases
4. Then remove the stubs from base class

**Example:**
```cpp
// BaseCharacter.h
// Old damage system methods - kept as virtual stubs for child class compatibility
// TODO: Remove these after cleaning up child classes
virtual void HandleDamage(...);
virtual void ReceiveDamage(...);
virtual void PlayRandomMeleeAttackMontage();
virtual void PlayRandomMajixAttackMontage();

// BaseCharacter.cpp
void ABaseCharacter::HandleDamage(...)
{
	// Empty stub - child classes may override
}
```

### Cleanup Order Matters

**Correct Order:**
1. ✅ Clean up includes and obvious dead code
2. ✅ Add TODO markers to legacy systems
3. ✅ Keep virtual stubs for compatibility
4. ⏳ Clean up child class overrides (Phase 2 & 3)
5. ⏳ Remove stubs from base class (Phase 4)

**Wrong Order:**
1. ❌ Remove everything from base class first
2. ❌ Try to fix 50+ compilation errors across multiple files
3. ❌ Get overwhelmed and give up

### Files That Depend on BaseCharacter

**Direct Dependencies Found:**
- `EnemyBase.h/.cpp` - Overrides damage methods, uses AttributeComponent
- `FillainCharacter.h/.cpp` - Overrides damage methods, uses AttributeComponent, uses cached damage params
- `CombatComponent.cpp` - Accesses cached damage parameters
- `MeleeWeapon.cpp` - Accesses cached damage parameters
- `Soul.cpp` - Uses AttributeComponent
- Multiple enemy subclasses (Feratta, Hellspawn, etc.)

### What Actually Got Cleaned Up

**Successfully Removed:**
- 15+ duplicate header includes
- 100+ lines of commented-out charm system code
- 9 cached damage parameter properties (from header)
- Redundant `USkeletalMeshComponent* Weapon` property
- Hit react cooldown system (bCanReact, HitReactTimer, ResetHitReact)
- All references to removed `Weapon` property (replaced with `EquippedWeapon`)

**Temporarily Kept (Marked for Removal):**
- `AttributeComponent` - Still used by FillainCharacter and EnemyBase
- `HandleDamage()` - Overridden by both child classes
- `ReceiveDamage()` - Overridden by FillainCharacter
- `PlayRandomMeleeAttackMontage()` - Overridden by both child classes
- `PlayRandomMajixAttackMontage()` - Overridden by both child classes

### Next Steps

**Phase 2: EnemyBase**
- Remove `HandleDamage()` override
- Remove `PlayRandomMeleeAttackMontage()` override
- Remove `PlayRandomMajixAttackMontage()` override
- Migrate from `AttributeComponent` to GAS `AttributeSet`
- Remove charm/flee system

**Phase 3: FillainCharacter**
- Remove `HandleDamage()` override
- Remove `ReceiveDamage()` override
- Remove `PlayRandomMeleeAttackMontage()` override
- Remove `PlayRandomMajixAttackMontage()` override
- Remove cached damage parameter usage
- Migrate from `AttributeComponent` to GAS `AttributeSet`
- Remove camera watchdog system

**Phase 4: Final Cleanup**
- Remove stub methods from BaseCharacter
- Remove `AttributeComponent` completely
- Verify all damage flows through GAS
- Final compilation test

### Key Takeaway

**When refactoring inheritance hierarchies:**
1. Map all dependencies first
2. Clean from leaves to root (child classes → base class)
3. Use stub methods as temporary bridges
4. Mark everything with TODO comments
5. Test compilation frequently
6. Be patient and systematic

**Don't try to do everything at once!**
