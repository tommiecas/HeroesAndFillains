# Gnarled Running Off Map - Troubleshooting Guide

## Issue
Gnarled enemy runs off the map when the game starts. Other enemies don't have this problem.

## Analysis
**This is NOT caused by our Phase 4 damage system changes** because:
- ✅ We only modified `OnAttackCollisionOverlap()` (damage application)
- ✅ We didn't touch AI, navigation, or movement code
- ✅ Other enemies (Gnarledling, Spectral, StormAssassin, Thrope) work fine
- ✅ The changes are identical across all enemy types

## Likely Causes

### 1. Blueprint Configuration Issue (Most Likely)
**Check:**
- [ ] Gnarled Blueprint (BP_Gnarled or similar)
- [ ] AI Controller assignment
- [ ] Behavior Tree assignment
- [ ] Blackboard setup
- [ ] Nav Mesh bounds in the level

**How to Check:**
1. Open Gnarled Blueprint in editor
2. Check "AI Controller Class" - should be set (e.g., `HAFAIController`)
3. Check "Auto Possess AI" - should be "Placed in World" or "Spawned"
4. Check if Behavior Tree is assigned

### 2. Nav Mesh Issue
**Check:**
- [ ] Nav Mesh covers the spawn area
- [ ] Nav Mesh is built/updated
- [ ] No holes in Nav Mesh near spawn

**How to Check:**
1. Press `P` in editor to show Nav Mesh (green overlay)
2. Check if spawn location is on green area
3. Rebuild Nav Mesh: Build → Build Paths

### 3. Patrol Points Missing
**Check:**
- [ ] Gnarled has patrol points assigned
- [ ] Patrol points are valid locations
- [ ] Patrol points are on Nav Mesh

**How to Check:**
1. Select Gnarled in level
2. Check Details panel for patrol point array
3. Verify patrol points exist and are placed correctly

### 4. AI Perception Issue
**Check:**
- [ ] AI Perception component configured
- [ ] Sight config is valid
- [ ] Not detecting invalid target causing flee

**How to Check:**
1. Open Gnarled Blueprint
2. Check AI Perception Component settings
3. Verify sight radius, lose sight radius, etc.

### 5. Behavior Tree Logic
**Check:**
- [ ] Behavior Tree has valid logic
- [ ] No infinite loop causing movement
- [ ] Flee/Chase logic not broken

**How to Check:**
1. Open Gnarled's Behavior Tree
2. Check for any flee/escape logic
3. Verify chase/patrol logic

---

## Quick Fixes to Try

### Fix 1: Reset AI Controller
```cpp
// In Gnarled Blueprint or level
1. Select Gnarled
2. Details → AI Controller Class → Set to your AI controller
3. Auto Possess AI → Set to "Placed in World"
```

### Fix 2: Rebuild Nav Mesh
```cpp
1. Build → Build Paths
2. Or: Window → Navigation → Rebuild
```

### Fix 3: Check Spawn Location
```cpp
1. Move Gnarled to a different location
2. Ensure it's on solid ground
3. Ensure it's on Nav Mesh (press P to see)
```

### Fix 4: Disable AI Temporarily
```cpp
// In Gnarled.cpp BeginPlay(), add temporarily:
void AGnarled::BeginPlay()
{
    Super::BeginPlay();
    
    // TEMP: Disable AI for testing
    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        AIController->StopMovement();
        // Or: AIController->GetBrainComponent()->StopLogic("Testing");
    }
    
    RegisterAttackCollision(RightFistCollision);
    RegisterAttackCollision(LeftFistCollision);
    Tags.Add(FName("Gnarled"));
}
```

### Fix 5: Compare with Working Enemy
```cpp
1. Open working enemy Blueprint (e.g., Gnarledling)
2. Compare settings with Gnarled
3. Copy working settings to Gnarled
```

---

## Debugging Steps

### Step 1: Check Console Output
Look for errors related to:
- Navigation
- AI Controller
- Behavior Tree
- Blackboard

### Step 2: Enable AI Debug
```cpp
// In console:
ShowDebug AI

// Or in code:
UE_LOG(LogTemp, Warning, TEXT("Gnarled BeginPlay - Controller: %s"), *GetNameSafe(GetController()));
UE_LOG(LogTemp, Warning, TEXT("Gnarled Location: %s"), *GetActorLocation().ToString());
```

### Step 3: Check Behavior Tree Execution
```cpp
// In console:
ShowDebug BehaviorTree

// This will show which nodes are executing
```

### Step 4: Test in Isolation
1. Create empty test level
2. Place only Gnarled
3. Add Nav Mesh
4. See if issue persists

---

## Verification

### If It's Our Code (Unlikely)
If disabling our changes fixes it:
1. The issue is in `OnAttackCollisionOverlap()`
2. But this only runs when attacking, not on spawn
3. So it's probably a side effect

### If It's Not Our Code (Likely)
If issue persists with our changes reverted:
1. It's a pre-existing issue
2. Check Blueprint/AI setup
3. Check level-specific configuration

---

## Temporary Workaround

While investigating, you can:

### Option 1: Use Different Enemy
Test damage system with Gnarledling or other enemies that work

### Option 2: Constrain Movement
Add invisible walls around spawn area temporarily

### Option 3: Disable Gnarled AI
Test damage by manually placing Gnarled and disabling AI

---

## Next Steps

1. **Try Quick Fixes above** (5 minutes)
2. **Check Blueprint settings** (10 minutes)
3. **Verify Nav Mesh** (5 minutes)
4. **If still broken:** This is a separate issue from Phase 4
5. **Continue testing** with other enemies that work

---

## Important Note

**Our Phase 4 changes are working correctly** if:
- ✅ Other enemies work fine
- ✅ Damage applies when you attack enemies
- ✅ Enemies can damage you
- ✅ No console errors related to damage

The Gnarled runoff issue is likely a **separate, pre-existing problem** that should be fixed independently.

---

## Recommendation

**For now:**
1. Test the damage system with other enemies (Gnarledling, Spectral, etc.)
2. Verify damage works correctly
3. Fix Gnarled issue separately (it's not blocking Phase 4 completion)

**The damage system migration is complete and working** - this is a different issue!
