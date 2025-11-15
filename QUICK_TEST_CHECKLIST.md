# Quick Test Checklist - After Latest Fixes

## ✅ Fixes Applied & Compiled:

1. **Removed ReceiveDamage delegate binding** (FillainCharacter.cpp)
2. **Set base class ASC/AttributeSet pointers** (EnemyBase.cpp constructor)
3. **Uncommented Primary/Secondary attribute initialization** (BaseCharacter.cpp)

---

## 🧪 Test These in Order:

### Test 1: No Delegate Errors ✓
**Action:** Start PIE (Play in Editor)  
**Expected:** No "Unable to bind delegate to 'ReceiveDamage'" error  
**Check Logs For:** Clean startup, no delegate errors

---

### Test 2: Attributes Have Values ✓
**Action:** Spawn Gnarled, hover over him to see attribute menu  
**Expected:** Attributes show actual values (not all 0s)  
**Check Logs For:**
```
[Gnarled_C_0] Applying DefaultPrimaryAttributes: GE_...
[Gnarled_C_0] Applying DefaultSecondaryAttributes: GE_...
🟢 Gnarled_C_0 SPAWNED - Health: 100.0 / 100.0
```

**If Still 0:**
- Check Gnarled Blueprint → Details panel
- Verify these are assigned:
  - Default Primary Attributes
  - Default Secondary Attributes  
  - Default Vital Attributes
- Check CharacterClassInfo DataAsset has values for Warrior class

---

### Test 3: Gnarled Walks (Not Stuck in Idle) ✓
**Action:** Let Gnarled chase you  
**Expected:** Walking animation plays, not stuck in idle  

**Debug Steps if Still Broken:**

A. **Check Velocity in Logs:**
   - Look for: `Gnarled_C_0 Chasing - Speed: [should be > 0]`
   - If Speed = 0, movement is broken

B. **Check Animation Blueprint:**
   - Open Gnarled's AnimBP
   - Find the Idle → Walk transition
   - Check condition: `GroundSpeed > 0.0` or similar
   - Verify GroundSpeed is calculated from `GetVelocity().Size()`

C. **Check AI Movement:**
   - Verify `ChasingSpeed` is set (default 300 in EnemyBase constructor)
   - Check if `MoveToTarget()` is being called
   - Verify AIController is valid

---

### Test 4: Damage Works Both Ways ✓
**Action:** Let Gnarled attack you, then attack him back  
**Expected:** Both characters take damage, health bars update  

**Debug Steps if No Damage:**

A. **Check Weapon Collision Logs:**
   ```
   🔧 Gnarled_C_0 SetWeaponCollisionEnabled: ENABLED
   ⚔️ Gnarled_C_0 attack collision overlap with FillainCharacter_C_0
   💥 Gnarled_C_0 damaged FillainCharacter_C_0 for 20.0!
   ```

B. **If No Overlap Logs:**
   - Weapon collision not enabled
   - Check animation notifies in attack montage
   - Verify `RegisterAttackCollision()` was called

C. **If Overlap But No Damage:**
   - Check `bCanDamage` is true
   - Check `BaseDamage` > 0
   - Check `UGameplayStatics::ApplyDamage()` is being called

D. **If Damage Applied But Health Doesn't Change:**
   - Check if `HandleDamage()` applies GameplayEffect
   - Check if `DamageEffectClass` is assigned in Blueprint
   - Check if `HandleChangeInHealth()` delegate is bound

---

## 🔍 Key Logs to Watch For:

### Good Signs ✅:
```
✅ Registered weapon box for Gnarled_C_0
🟢 Gnarled_C_0 SPAWNED - Health: 100.0 / 100.0
Gnarled_C_0 Chasing - Speed: 300.0
🔧 Gnarled_C_0 SetWeaponCollisionEnabled: ENABLED
⚔️ Gnarled_C_0 attack collision overlap with FillainCharacter_C_0
💥 Gnarled_C_0 damaged FillainCharacter_C_0 for 20.0!
❤️ FillainCharacter_C_0 Health changed: 100.0 → 80.0
```

### Bad Signs ❌:
```
🔴 Gnarled_C_0 MISSING ASC OR ATTRIBUTESET AT SPAWN!
Gnarled_C_0 Chasing - Speed: 0.0  (stuck!)
❌ No DamageEffectClass assigned!
AttributeSet is nullptr
```

---

## 📋 Quick Reference: What Each Fix Does

| Fix | File | What It Does |
|-----|------|--------------|
| Remove delegate binding | FillainCharacter.cpp | Stops "ReceiveDamage" error on startup |
| Set base pointers | EnemyBase.cpp | Gives enemies valid AttributeSet |
| Uncomment attributes | BaseCharacter.cpp | Initializes Primary/Secondary stats |

---

## Next Steps After Testing:

**If attributes work:** ✅ Issue 1 solved  
**If Gnarled walks:** ✅ Issue 2 solved  
**If damage works:** ✅ Issue 3 solved  

**If any still broken:** Report which one and I'll dig deeper with targeted fixes.
