# 🎭 HOW TO FIX DEATH ANIMATION IN ANIMATION BLUEPRINT

## 📋 Step-by-Step Guide

The enemy stands back up because the **Animation Blueprint (AnimBP)** transitions back to idle after the death montage ends. Here's how to fix it:

---

## ✅ Solution 1: Add "IsDead" Check to AnimBP (Recommended)

### **Step 1: Open the Animation Blueprint**
1. In Unreal Editor, navigate to your Gnarledling's Animation Blueprint
2. Usually located in: `Content/Characters/Enemies/Gnarledling/Animations/ABP_Gnarledling` (or similar)
3. Double-click to open it

### **Step 2: Add IsDead Boolean Variable**
1. In the AnimBP, go to the **Variables** panel
2. Click **+ Variable** to create a new variable
3. Name it: `bIsDead`
4. Type: **Boolean**
5. Default Value: **false**

### **Step 3: Update the Variable in Event Graph**
1. Go to the **Event Graph** tab
2. Find or create **Event Blueprint Update Animation**
3. Add this logic:

```
Event Blueprint Update Animation
    ↓
Get Pawn Owner
    ↓
Cast to EnemyBase (or BaseCharacter)
    ↓
Call "IsDead" function (or check bIsCharacterDead)
    ↓
Set bIsDead (the AnimBP variable)
```

**Blueprint Nodes:**
- `Event Blueprint Update Animation`
- `Try Get Pawn Owner` → `Cast to EnemyBase`
- `IsDead` (from the interface) or `Get bIsCharacterDead`
- `Set bIsDead` (your AnimBP variable)

### **Step 4: Modify the State Machine**
1. Go to the **AnimGraph** tab
2. Open your **State Machine** (usually called "Locomotion" or "Movement")
3. Find the **Idle/Locomotion State**
4. Add a **Transition Rule** from **Any State** to a new **Dead State**:

**Transition Condition:**
```
bIsDead == true
```

### **Step 5: Create the Dead State**
1. Create a new state called **"Dead"**
2. Inside the Dead state, add:
   - **Play Animation** node
   - Select your **Dead Loop Animation** (the one that shows the body on the ground)
   - Set **Loop Animation** to **true**

### **Step 6: Prevent Transitions from Dead State**
- Make sure there are **NO transition rules** going OUT of the Dead state
- This ensures once dead, the character stays dead

---

## ✅ Solution 2: Use Slot in Death Montage (Alternative)

If you want the death montage to keep playing:

### **Step 1: Configure Death Montage**
1. Open your **Death Montage** asset
2. Make sure it has a **Slot** assigned (e.g., "DefaultSlot")
3. Set the last section to **loop back to itself** or to a "Dead Loop" section

### **Step 2: Add Slot to AnimBP**
1. In your AnimBP's **AnimGraph**
2. Add a **Slot** node with the same name as your montage slot
3. Connect it to your **Final Animation Pose**

### **Step 3: Set Montage to Loop**
In your death montage:
1. Add a section called "DeadLoop" at the end
2. Set the last frame to loop: Right-click the last section → **Set Next Section** → **DeadLoop**

---

## ✅ Solution 3: Disable AnimBP When Dead (Quick Fix)

### **In C++ (EnemyBase::Die() or PlayDeathMontage()):**

Add this after playing the death montage:

```cpp
// Disable the AnimBP after death montage finishes
FTimerHandle DisableAnimBPHandle;
GetWorldTimerManager().SetTimer(DisableAnimBPHandle, [this]()
{
    if (GetMesh() && GetMesh()->GetAnimInstance())
    {
        GetMesh()->SetAnimInstanceClass(nullptr);  // Disable AnimBP
        GetMesh()->bPauseAnims = true;
        UE_LOG(LogTemp, Warning, TEXT("💀 %s AnimBP disabled"), *GetName());
    }
}, Duration, false);
```

This completely disables the AnimBP after the death montage, freezing the character in the last pose.

---

## 🎯 Recommended Approach:

**Use Solution 1** (Add IsDead check to AnimBP) because:
- ✅ Clean and proper solution
- ✅ Follows Unreal best practices
- ✅ Easy to debug
- ✅ Reusable for other enemies

---

## 📝 Quick Checklist:

- [ ] Open Gnarledling Animation Blueprint
- [ ] Add `bIsDead` boolean variable
- [ ] Update `bIsDead` in Event Graph from character's IsDead()
- [ ] Add "Dead" state to State Machine
- [ ] Add transition: Any State → Dead (when bIsDead == true)
- [ ] In Dead state, play dead loop animation
- [ ] Remove all transitions OUT of Dead state
- [ ] Compile and save AnimBP
- [ ] Test in-game

---

## 🐛 Troubleshooting:

**If enemy still stands up:**
1. Check that `bIsDead` is actually being set to true (add a Print String in the Event Graph)
2. Verify the transition condition is checking `bIsDead == true`
3. Make sure there are NO transitions leaving the Dead state
4. Check that the Dead state is actually playing the dead animation

**If death animation doesn't play at all:**
1. Make sure the death montage is assigned in the Blueprint
2. Check that the montage slot matches between montage and AnimBP
3. Verify the montage is being called in C++ (check the logs)

---

## 💡 Pro Tip:

You can also use a **Gameplay Tag** instead of a boolean:
- Add tag `Character.State.Dead` when enemy dies
- Check for this tag in the AnimBP transition rules
- This is more flexible and follows GAS patterns

Let me know if you need help with any of these steps!
