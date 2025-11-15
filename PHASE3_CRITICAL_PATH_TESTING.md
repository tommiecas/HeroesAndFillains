# Phase 3: Critical-Path Testing Checklist

## 🎯 Test Focus Areas

Testing only the critical systems affected by our cleanup:
1. **Damage System** (removed TakeDamage/HandleDamage)
2. **Weapon Equipping** (camera watchdog stubs)
3. **Player Death/Respawn** (damage flow to elimination)

---

## ✅ Test Procedures

### **Test 1: Player Damage System**
**Goal:** Verify GAS damage flow works without old TakeDamage system

**Steps:**
1. Launch game in PIE (Play In Editor)
2. Spawn player character
3. Find an enemy
4. Let enemy hit you
5. **Verify:**
   - ✅ Health bar decreases
   - ✅ Shield absorbs damage first (if you have shield)
   - ✅ Hit reaction plays
   - ✅ No errors in log about TakeDamage/HandleDamage
   - ✅ No errors about cached damage parameters

**Expected Result:** Damage flows through GAS → ReceiveDamage → GE_DamageSplit → Health/Shield reduction

---

### **Test 2: Weapon Equipping**
**Goal:** Verify weapon equipping works with camera watchdog stubs

**Steps:**
1. Find a weapon on the ground
2. Press E to equip
3. **Verify:**
   - ✅ Weapon attaches to character
   - ✅ Camera doesn't glitch/zoom into character
   - ✅ No errors about FixSelfCameraCollision
   - ✅ No errors about StartCamWatchdog
   - ✅ No errors about Client_OnEquipped
   - ✅ Weapon shows in hand correctly

**Expected Result:** Weapon equips smoothly, camera stays stable

---

### **Test 3: Player Death & Respawn**
**Goal:** Verify death flow works with new damage system

**Steps:**
1. Let enemies kill you (health reaches 0)
2. Wait for respawn
3. **Verify:**
   - ✅ Death animation plays
   - ✅ Elimination effects show
   - ✅ Player respawns after delay
   - ✅ Health/shield restored on respawn
   - ✅ No errors about damage system
   - ✅ No errors about PlayerEliminated

**Expected Result:** Death → Elimination → Respawn cycle works correctly

---

## 📊 Success Criteria

**All 3 tests must pass with:**
- No crashes
- No error logs related to removed systems
- Visual/gameplay behavior matches expected results

**If any test fails:**
- Document the error
- Fix the issue
- Rebuild and retest

---

## 🚀 Ready to Test

Launch the game and run through these 3 critical tests!
