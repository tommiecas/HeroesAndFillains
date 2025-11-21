# 🔍 BLUEPRINT DEBUGGING GUIDE - Finding What's Resurrecting The Enemy

## 🎯 The Mystery:

Even with **7 layers of C++ freezing**, the enemy still stands back up. This means:
- ✅ C++ code is working (logs confirm freeze happens)
- ❌ Something in Blueprint is overriding/undoing the freeze
- ❌ OR something is manually setting the mesh transform

---

## 📋 Places to Check in Blueprint:

### **1. BP_Gnarledlings Event Graph**

Look for these suspicious nodes:

**A. Tick Event:**
- Check if there's an `Event Tick` node
- Look for any code that runs continuously
- Search for: `Set Relative Transform`, `Set World Transform`, `Set Relative Rotation`

**B. Timeline Nodes:**
- Look for any `Timeline` nodes (especially for dissolve)
- Check if they're modifying mesh transform
- Timelines can override frozen poses!

**C. Custom Events:**
- Search for events like: `OnDeath`, `HandleDeath`, `Resurrect`, `Reset`
- Check if any are bound to delegates

**D. Animation Notifies:**
- Check if death montage has `AnimNotify` events
- These can trigger Blueprint code during/after animation

---

### **2. Animation Blueprint (ABP_Gnarledling)**

**A. Event Graph:**
- Look for `Event Blueprint Update Animation`
- Check if it's setting mesh transform
- Look for `Set Anim Instance Class` (might be re-enabling AnimBP!)

**B. AnimGraph:**
- Check the **Slot** node weight
- If slot weight is 0, montages won't play
- Check if there's a **Layered Blend** overriding the montage

**C. State Machine:**
- Even though you have a Dead state, check if there's a **transition OUT of Dead**
- Look for: Dead → Idle transition (even if you think you deleted it)
- Check transition **priority** - higher priority transitions can override

---

### **3. Parent Blueprints**

**Check BP_Gnarled (parent class):**
- Open `BP_Gnarled` Blueprint
- Check Event Graph for death-related code
- Parent class code runs BEFORE child class code!

**Check BP_EnemyBase (if it exists):**
- Same checks as above
- Parent blueprints can override child behavior

---

### **4. Behavior Tree / AI**

**A. Behavior Tree:**
- Open the Behavior Tree asset
- Look for tasks that run when `IsDead == true`
- Check for: `MoveTo`, `RotateTo`, `FaceTarget` tasks

**B. Blackboard:**
- Check if `IsDead` key is being set correctly
- Check if any tasks are checking this key

**C. AI Controller Blueprint:**
- Check if there's a Blueprint AI Controller
- Look for `OnPossess`, `OnUnpossess` events
- Check for movement commands

---

### **5. Dissolve Timeline**

**This is the most likely culprit!**

**A. Check the Dissolve Timeline:**
- Open `BP_Gnarledlings`
- Find the `DissolveTimeline` node
- **Double-click** it to open the timeline editor

**B. Look for Transform Tracks:**
- Check if timeline has tracks for:
  - `Location`
  - `Rotation`
  - `Transform`
- These would move the mesh during dissolve!

**C. Check Timeline Callbacks:**
- Look for `Update` pin connections
- Check what nodes are connected to the timeline
- Common issue: Timeline updates mesh transform every frame

---

## 🎯 Most Likely Culprits (In Order):

### **1. Dissolve Timeline Moving Mesh (90% chance)**
```
DissolveTimeline → Update → Set Relative Transform
```
**Fix:** Remove any transform updates from dissolve timeline

### **2. AnimBP Being Re-Enabled (5% chance)**
```
Event Tick → Set Anim Instance Class → [AnimBP Class]
```
**Fix:** Remove the Set Anim Instance Class node

### **3. Parent Blueprint Override (3% chance)**
```
BP_Gnarled Event Graph → OnDeath → [Some code]
```
**Fix:** Check parent class event graphs

### **4. Behavior Tree Task (2% chance)**
```
BT Task → MoveTo / RotateTo (when IsDead)
```
**Fix:** Add condition to skip tasks when dead

---

## 🔍 How to Debug:

### **Method 1: Blueprint Breakpoints**

1. Open `BP_Gnarledlings` Event Graph
2. **Right-click** on any execution pin
3. Select **Add Breakpoint**
4. Play the game
5. When enemy dies, game will pause at breakpoint
6. Step through to see what's executing

### **Method 2: Print String Spam**

Add `Print String` nodes everywhere:

```
Event Tick → Print String: "TICK RUNNING AFTER DEATH!"
DissolveTimeline Update → Print String: "DISSOLVE TIMELINE UPDATING!"
Any suspicious node → Print String: "THIS NODE IS RUNNING!"
```

### **Method 3: Disable Sections**

1. **Disable the Dissolve Timeline** completely
2. Test if enemy still stands up
3. If NO: Timeline is the culprit!
4. If YES: Keep looking

---

## 🎯 Quick Test:

**In BP_Gnarledlings Event Graph, add this:**

```
Event BeginPlay
    ↓
Delay (3 seconds)
    ↓
Get Mesh
    ↓
Set Anim Instance Class (None)
    ↓
Print String: "MANUALLY DISABLED ANIMBP FROM BLUEPRINT!"
```

If this works (enemy freezes after 3 seconds), then our C++ timer isn't firing or is being overridden.

---

## 💡 Nuclear Debug Option:

**Completely disable the Blueprint:**

1. Open `BP_Gnarledlings`
2. Go to **Class Settings** (top toolbar)
3. Under **Class Options**, check **Disable Blueprint Tick**
4. Save and test

If enemy STILL stands up, it's not the Blueprint Event Graph!

---

## 🔧 If You Find The Culprit:

**Once you find what's causing it, let me know and I can:**
1. Add C++ code to override it
2. Help you fix the Blueprint
3. Add additional safeguards

The most likely issue is the **Dissolve Timeline** updating the mesh transform!
