# 🎭 ANIMATION BLUEPRINT - DEAD STATE SETUP

## ✅ Step 1: Event Graph (DONE!)

You've already set up the Event Graph correctly:
```
Event Blueprint Update Animation
    ↓
Try Get Pawn Owner
    ↓
Cast To EnemyBase
    ↓
IsDead (function call)
    ↓
SET bIsDead (AnimBP variable)
```

Perfect! ✅

---

## 📋 Step 2: Create bIsDead Variable (If Not Done)

1. In the **My Blueprint** panel (left side)
2. Under **Variables**, click **+ Variable**
3. Name it: `bIsDead`
4. Type: **Boolean**
5. Default Value: **false**
6. **Compile** the Blueprint

---

## 🎯 Step 3: Add Dead State to State Machine

### **3.1: Open Your State Machine**
1. Go to the **AnimGraph** tab
2. Double-click your **State Machine** node (usually called "Locomotion" or "StateMachine")

### **3.2: Create a New "Dead" State**
1. **Right-click** in empty space in the State Machine
2. Select **Add State** → **New State**
3. Name it: **"Dead"**

### **3.3: Add Transition from Any State to Dead**
1. **Right-click** on the **Entry** node or any existing state
2. Select **Add Transition** → **Dead**
3. Or drag from any state to the Dead state to create a transition

### **3.4: Set Transition Condition**
1. **Double-click** the transition arrow going INTO the Dead state
2. In the transition rule graph, add:
   ```
   Get bIsDead
       ↓
   (boolean output) → Result
   ```
3. This means: "Transition to Dead when bIsDead == true"

### **3.5: Configure the Dead State**
1. **Double-click** the Dead state
2. Inside the state, add a **Play Animation** node or **Sequence Player** node
3. Select your **Dead Loop Animation** (the animation showing the body on the ground)
4. Set **Loop Animation** to **true** (check the box)
5. Connect it to the **Output Animation Pose**

### **3.6: Prevent Transitions OUT of Dead**
- Make sure there are **NO transition arrows** leaving the Dead state
- If any exist, delete them
- This ensures once dead, the character stays dead

---

## 🎯 Step 4: Compile and Test

1. **Compile** the Animation Blueprint
2. **Save** the Animation Blueprint
3. **Play** the game
4. Kill a Gnarledling
5. The enemy should:
   - Play death animation
   - Stay on ground in dead pose
   - Dissolve after 5 seconds
   - **NOT stand back up!**

---

## 🐛 Troubleshooting:

### **If enemy still stands up:**

1. **Check bIsDead is being set:**
   - Add a **Print String** node after the SET node in Event Graph
   - Print the value of `bIsDead`
   - You should see "true" when enemy dies

2. **Check transition is working:**
   - In the State Machine, select the transition to Dead
   - Add a **Print String** in the transition rule
   - You should see it fire when enemy dies

3. **Check Dead state is active:**
   - While playing, press **`** (tilde key) to open console
   - Type: `showdebug animation`
   - This will show which state is active
   - It should show "Dead" when enemy dies

### **If death animation doesn't play:**
- Make sure the death montage is assigned in the Gnarledling Blueprint
- Check the logs for the death montage logging we added

---

## 💡 Alternative: Use Gameplay Tag Instead

Instead of `bIsDead` boolean, you can use a Gameplay Tag:

1. In C++, add tag `Character.State.Dead` when enemy dies
2. In AnimBP Event Graph, check for this tag:
   ```
   Try Get Pawn Owner
       ↓
   Cast to EnemyBase
       ↓
   Get Ability System Component
       ↓
   Has Matching Gameplay Tag (Tag: Character.State.Dead)
       ↓
   SET bIsDead
   ```

This is more flexible and follows GAS patterns better.

---

## 📸 Visual Reference:

Your Event Graph looks perfect! Now just:
1. Create the Dead state in State Machine
2. Add transition: Any State → Dead (when bIsDead)
3. In Dead state, play dead loop animation
4. Remove transitions OUT of Dead state

That's it!
