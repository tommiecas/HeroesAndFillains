# Gnarledling Bleeding Bug - Diagnostic Report

## 🐛 REPORTED ISSUES

1. **Gnarledling not playing attack animation**
2. **Gnarledling bleeding for no reason** (without being attacked)

---

## 🔍 ROOT CAUSE ANALYSIS

### Issue 1: Missing `ResetCanDamage()` Method

**Location:** `Gnarledling.cpp` line 77

```cpp
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarledling::ResetCanDamage, 0.3f, false);
```

**Problem:** 
- Gnarledling calls `&AGnarledling::ResetCanDamage` 
- But `ResetCanDamage()` is defined in `EnemyBase`, not Gnarledling
- Should call `&AEnemyBase::ResetCanDamage` or `&AGnarledling::ResetCanDamage` if overridden

**Same issue in Gnarled.cpp line 97**

---

### Issue 2: Potential Self-Damage from Collision

**Hypothesis:** The bleeding might be caused by:

1. **Collision boxes overlapping on spawn**
   - Fist/foot collision boxes might be overlapping the player or other actors
   - Causing immediate damage on BeginPlay

2. **Missing `bCanDamage` initialization**
   - If `bCanDamage` starts as `true`, collisions could trigger immediately
   - Should start as `false` until attack animation enables it

3. **Animation Blueprint not calling Enable/Disable methods**
   - Attack animations should call `EnableLeftSideMeleeAttack()` etc.
   - If not called, or called at wrong time, could cause issues

---

## 🔧 FIXES NEEDED

### Fix 1: Correct ResetCanDamage Call

**In Gnarledling.cpp:**
```cpp
// ❌ WRONG
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarledling::ResetCanDamage, 0.3f, false);

// ✅ CORRECT
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.3f, false);
```

**In Gnarled.cpp:**
```cpp
// ❌ WRONG
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarled::ResetCanDamage, 0.3f, false);

// ✅ CORRECT
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.3f, false);
```

---

### Fix 2: Ensure bCanDamage Starts False

**Check EnemyBase.h:**
```cpp
// Should be initialized to false
UPROPERTY()
bool bCanDamage = false;  // ✅ Start false, enable during attack
```

---

### Fix 3: Verify Animation Blueprint Setup

**Check Gnarledling Animation Blueprint:**

1. **Attack Montage Notify States:**
   - Should have AnimNotify_EnableLeftSideMeleeAttack
   - Should have AnimNotify_DisableLeftSideMeleeAttack
   - Should have AnimNotify_EnableRightSideMeleeAttack
   - Should have AnimNotify_DisableRightSideMeleeAttack

2. **Timing:**
   - Enable should be called just before fist/foot makes contact
   - Disable should be called immediately after contact frame

3. **Missing Attack Montage:**
   - If Gnarledling doesn't have attack animations set up
   - It won't play attacks, but collisions might still be enabled

---

### Fix 4: Add Collision Debugging

**Add to Gnarledling::BeginPlay():**
```cpp
void AGnarledling::BeginPlay()
{
    Super::BeginPlay();

    RegisterAttackCollision(RightFistlingCollision);
    RegisterAttackCollision(LeftFistlingCollision);

    Tags.Add(FName("Gnarledling"));
    
    // ✅ DEBUG: Verify collision state on spawn
    UE_LOG(LogTemp, Warning, TEXT("🔍 Gnarledling spawned - bCanDamage: %s"), 
           bCanDamage ? TEXT("TRUE") : TEXT("FALSE"));
    
    if (RightFistlingCollision)
    {
        UE_LOG(LogTemp, Warning, TEXT("🔍 Right Fist Collision: %s"), 
               *UEnum::GetValueAsString(RightFistlingCollision->GetCollisionEnabled()));
    }
    
    if (LeftFistlingCollision)
    {
        UE_LOG(LogTemp, Warning, TEXT("🔍 Left Fist Collision: %s"), 
               *UEnum::GetValueAsString(LeftFistlingCollision->GetCollisionEnabled()));
    }
}
```

---

## 🎯 IMMEDIATE ACTION ITEMS

### Priority 1: Fix ResetCanDamage Calls
1. Update Gnarledling.cpp line 77
2. Update Gnarled.cpp line 97
3. Recompile

### Priority 2: Check Animation Blueprint
1. Open Gnarledling Animation Blueprint
2. Verify attack montages exist
3. Verify AnimNotify states are present
4. Check timing of Enable/Disable calls

### Priority 3: Add Debug Logging
1. Add collision state logging to BeginPlay
2. Add logging to Enable/Disable methods (already present)
3. Test and observe logs

---

## 🧪 TESTING STEPS

After fixes:

1. **Spawn Gnarledling**
   - Check logs for initial collision state
   - Verify bCanDamage = false
   - Verify collision boxes = NoCollision

2. **Trigger Attack**
   - Watch for "🟢 Fist Enabled" logs
   - Verify attack animation plays
   - Verify collision enables during attack

3. **After Attack**
   - Watch for "🔴 Fist Disabled" logs
   - Verify collision disables after attack
   - Verify no bleeding without being hit

4. **Player Attacks Gnarledling**
   - Verify blood effect only plays when hit
   - Verify GetHit_Implementation is called
   - Verify health decreases

---

## 📊 LIKELY CAUSES (Ranked)

1. **90% - ResetCanDamage method pointer error** ⭐
   - Calling non-existent method on Gnarledling
   - Causing undefined behavior

2. **70% - Animation Blueprint not set up**
   - No attack montages assigned
   - Enable/Disable methods never called
   - Collisions stay in wrong state

3. **50% - Collision boxes overlapping on spawn**
   - Boxes too large
   - Overlapping player spawn location
   - Triggering immediate damage

4. **30% - bCanDamage initialization issue**
   - Starting as true instead of false
   - Allowing damage before attack

---

## 🔧 QUICK FIX TO TEST

**Try this first:**

```cpp
// In Gnarledling.cpp, line 77:
// Change from:
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AGnarledling::ResetCanDamage, 0.3f, false);

// To:
GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.3f, false);
```

**Recompile and test.** This should fix the immediate crash/undefined behavior.

---

## 💡 PREVENTION

To prevent similar issues:

1. **Always use base class method pointers** when calling inherited methods
2. **Initialize collision to NoCollision** in constructor
3. **Initialize bCanDamage to false** in constructor
4. **Add debug logging** to track state changes
5. **Verify Animation Blueprint setup** before testing

---

**Next Steps:** Apply Fix 1, recompile, and test. Report back with results and any new log messages.
