# Gnarledling Bleeding Bug - FIX APPLIED ✅

## 🐛 ORIGINAL PROBLEM

**Reported Issues:**
1. Gnarledling not playing attack animation
2. Gnarledling bleeding for no reason (without being attacked)

---

## 🔍 ROOT CAUSE IDENTIFIED

**Method Pointer Error in Timer Callback**

Both `Gnarledling.cpp` and `Gnarled.cpp` were calling a non-existent method:

```cpp
// ❌ WRONG - Calling method that doesn't exist on derived class
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarledling::ResetCanDamage, 0.3f, false);
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarled::ResetCanDamage, 0.3f, false);
```

**Problem:**
- `ResetCanDamage()` is defined in `EnemyBase`, not in `Gnarledling` or `Gnarled`
- Calling `&AGnarledling::ResetCanDamage` tries to reference a method that doesn't exist
- This causes undefined behavior, potentially:
  - Calling random memory addresses
  - Triggering damage/effects at wrong times
  - Causing bleeding effects to play incorrectly

---

## ✅ FIX APPLIED

### File 1: Gnarledling.cpp (Line 84)

```cpp
// ✅ FIXED - Call base class method
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.3f, false);
```

### File 2: Gnarled.cpp (Line 104)

```cpp
// ✅ FIXED - Call base class method  
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.3f, false);
```

---

## 🎯 WHAT THIS FIXES

### Primary Fix:
- **Correct method pointer** - Now calls the actual `ResetCanDamage()` method from `EnemyBase`
- **Proper timer callback** - Timer will correctly reset `bCanDamage` to `true` after 0.3 seconds
- **No undefined behavior** - No more random memory access or incorrect function calls

### Expected Results:
1. ✅ **No more spontaneous bleeding** - Damage only applies during actual attacks
2. ✅ **Proper damage cooldown** - `bCanDamage` resets correctly after each hit
3. ✅ **Stable behavior** - No undefined behavior from bad method pointers

### Attack Animation Issue:
- **May still need investigation** - The attack animation issue might be separate
- **Check Animation Blueprint** - Verify attack montages are set up correctly
- **Check AI Behavior Tree** - Verify attack tasks are triggering properly

---

## 🧪 TESTING CHECKLIST

After the build completes, test:

### 1. Spawn Gnarledling
- [ ] No bleeding on spawn
- [ ] No blood effects without being hit
- [ ] Stands idle correctly

### 2. Let Gnarledling Attack
- [ ] Attack animation plays (if still broken, separate issue)
- [ ] Damage applies to player
- [ ] Blood effect only on player, not Gnarledling
- [ ] Can attack again after cooldown

### 3. Attack Gnarledling
- [ ] Takes damage correctly
- [ ] Blood effect plays on Gnarledling when hit
- [ ] Health bar updates
- [ ] Dies correctly when health reaches 0

### 4. Check Logs
Look for these debug messages:
- `🟢 Left Fistling Enabled` - When attack starts
- `🔴 Left Fistling Disabled` - When attack ends
- `💥 Gnarledling hit Player via GAS!` - When damage applies

---

## 📊 TECHNICAL DETAILS

### Why This Happened:

**C++ Method Pointer Syntax:**
```cpp
// Correct for inherited method:
&BaseClass::MethodName

// Incorrect for inherited method:
&DerivedClass::MethodName  // Only works if DerivedClass overrides it
```

**The Inheritance Chain:**
```
EnemyBase (defines ResetCanDamage())
    ↓
Gnarled (inherits ResetCanDamage())
    ↓
Gnarledling (inherits ResetCanDamage())
```

**What We Were Doing:**
- Trying to call `&AGnarledling::ResetCanDamage`
- But Gnarledling doesn't override this method
- So the compiler/runtime couldn't find it
- Resulted in undefined behavior

**What We Should Do:**
- Call `&AEnemyBase::ResetCanDamage`
- This references the actual method definition
- Works correctly for all derived classes

---

## 💡 LESSONS LEARNED

### For Future Development:

1. **Always use base class method pointers** when calling inherited methods via timers/delegates
2. **Add compiler warnings** for invalid method pointers (if possible)
3. **Test enemy spawning** immediately after creation to catch these issues early
4. **Use debug logging** to track method calls and state changes

### Pattern to Follow:

```cpp
// ✅ GOOD - Always reference the class that defines the method
GetWorldTimerManager().SetTimer(Handle, this, &BaseClass::Method, Time, false);

// ❌ BAD - Don't reference derived class unless it overrides
GetWorldTimerManager().SetTimer(Handle, this, &DerivedClass::Method, Time, false);
```

---

## 🚀 NEXT STEPS

1. **Wait for build to complete** ✅ (In progress)
2. **Test Gnarledling** - Verify no bleeding bug
3. **Test Gnarled** - Verify same fix works for parent class
4. **Investigate attack animation** - If still not playing, check:
   - Animation Blueprint setup
   - AI Behavior Tree attack tasks
   - Attack montage assignments
   - AnimNotify timing

---

## 📝 FILES MODIFIED

1. `Source/HeroesAndFillains/Private/Enemies/Gnarledling.cpp` - Line 84
2. `Source/HeroesAndFillains/Private/Enemies/Gnarled.cpp` - Line 104

**Changes:** Changed method pointer from derived class to base class in timer callbacks.

**Impact:** Low risk, high reward - Simple fix with no side effects.

---

**Status:** ✅ Fix applied, waiting for build completion and testing.
