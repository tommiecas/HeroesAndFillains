# 🎯 STATE MACHINE - DEAD STATE VISUAL GUIDE

## ✅ Your Event Graph is Perfect!

You've correctly set up:
```
Event Blueprint Update Animation → Try Get Pawn Owner → Cast To EnemyBase → IsDead → SET bIsDead
```

Now let's use that `bIsDead` variable in the State Machine!

---

## 📋 Next Steps: State Machine Setup

### **Step 1: Open Your State Machine**
1. In the AnimBP, go to the **AnimGraph** tab
2. You should see a node called **"State Machine"** or **"Locomotion"**
3. **Double-click** it to open the State Machine graph

---

### **Step 2: Create a "Dead" State**

Inside the State Machine:

1. **Right-click** in empty space
2. Select **Add State**
3. Name it: **"Dead"**

You should now have states like:
- Idle
- Walk/Run
- **Dead** (new!)

---

### **Step 3: Add Transition to Dead State**

**Option A: From Idle State**
1. **Click and drag** from the **Idle** state to the **Dead** state
2. This creates a transition arrow

**Option B: From Any State (Better!)**
1. Look for an **"Any State"** node (or create one)
2. **Click and drag** from **Any State** to **Dead**
3. This allows transitioning to Dead from any state

---

### **Step 4: Set Transition Condition**

1. **Double-click** the transition arrow going INTO Dead
2. You'll see a **Transition Rule** graph
3. Add these nodes:

```
┌─────────────────┐
│  Get bIsDead    │ (drag from Variables panel)
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│     Result      │ (connect to the Result node)
└─────────────────┘
```

**In Blueprint:**
- Drag `bIsDead` from the Variables panel into the graph
- Connect its output pin to the **Result** node's input pin

This means: "Transition to Dead when bIsDead is true"

---

### **Step 5: Configure the Dead State**

1. **Double-click** the **Dead** state
2. Inside the state, you'll see an empty graph
3. Add a **Sequence Player** node:
   - Right-click → **Animation** → **Play [YourDeadAnimation]**
   - Or drag your dead animation from the Asset Browser
4. **Check the "Loop Animation" box** on the Sequence Player
5. Connect the **Output Pose** to the **Result** node

**Blueprint nodes inside Dead state:**
```
┌──────────────────────────┐
│  Play DeadLoopAnimation  │
│  ☑ Loop Animation        │
└────────────┬─────────────┘
             │ (Output Pose)
             ▼
      ┌─────────────┐
      │   Result    │
      └─────────────┘
```

---

### **Step 6: IMPORTANT - No Transitions OUT!**

Make sure there are **NO transition arrows** leaving the Dead state!

**Correct:**
```
Idle ──→ Dead  ✅ (can go TO Dead)
Dead ──X       ✅ (NO arrows leaving Dead)
```

**Wrong:**
```
Idle ──→ Dead ──→ Idle  ❌ (will return to Idle!)
```

If you see any arrows leaving Dead, **delete them**!

---

## 🎮 Step 7: Compile and Test

1. **Compile** the Animation Blueprint (green checkmark button)
2. **Save** the Animation Blueprint
3. **Play** the game
4. Kill a Gnarledling
5. Expected behavior:
   - ✅ Death animation plays
   - ✅ Enemy stays on ground
   - ✅ Dead loop animation plays
   - ✅ Dissolve effect happens
   - ✅ Enemy disappears
   - ✅ **NO standing back up!**

---

## 🐛 Troubleshooting:

### **Enemy still stands up:**

**Check 1: Is bIsDead being set?**
- Add a **Print String** after the SET node in Event Graph
- Print: `bIsDead`
- You should see "true" in the top-left when enemy dies

**Check 2: Is transition firing?**
- Double-click the transition to Dead
- Add a **Print String** node: "Transitioning to Dead!"
- You should see this message when enemy dies

**Check 3: Is Dead state active?**
- While playing, press **`** (tilde) to open console
- Type: `showdebug animation`
- Look for "Current State: Dead" when enemy dies

**Check 4: Are there transitions OUT of Dead?**
- Look at your State Machine
- Make sure NO arrows leave the Dead state
- Delete any you find!

---

## 💡 Pro Tips:

1. **Use "Any State" transition** instead of individual transitions from each state
2. **Set transition duration to 0.0** for instant death (no blend)
3. **Disable "Can Enter Transition" on Dead state** to prevent accidental exits
4. **Use a Gameplay Tag** instead of boolean for more flexibility

---

## 📸 What Your State Machine Should Look Like:

```
┌─────────────┐
│  Any State  │
└──────┬──────┘
       │ (when bIsDead)
       ▼
┌─────────────┐
│    Dead     │  ← NO arrows leaving!
│ (loops dead │
│  animation) │
└─────────────┘
```

---

## ✅ Summary:

1. ✅ Event Graph updates `bIsDead` (DONE!)
2. ⏳ Create Dead state in State Machine
3. ⏳ Add transition: Any State → Dead (when bIsDead)
4. ⏳ In Dead state, play dead loop animation
5. ⏳ Remove all transitions OUT of Dead
6. ⏳ Compile, save, test!

Follow these steps and your enemy will stay dead! 💀
