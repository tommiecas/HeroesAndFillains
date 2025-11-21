# C++ Cleanup Priority Roadmap

Based on your project structure and what we've learned from cleaning up Characters and Weapons, here's the recommended priority order for cleaning up the remaining classes:

---

## 🎯 PRIORITY ORDER (High → Low)

### **TIER 1: CRITICAL - Clean These First** 🔴

#### 1. **CombatComponent** (Highest Priority)
**Why First:**
- Central to all combat interactions
- Likely has competing systems like we saw in Characters
- Affects both players and enemies
- High bug potential due to complexity
- Dependencies: Characters, Weapons already cleaned

**Expected Issues:**
- Old damage calculation systems
- Duplicate weapon handling logic
- Cached state that's never used
- Competing attack/ability systems

**Impact:** High - Affects all combat gameplay

---

#### 2. **GAS Core Files** (AbilitySystemComponent, AttributeSet)
**Why Second:**
- Foundation for all gameplay mechanics
- Already partially cleaned during Character work
- Affects damage, abilities, attributes
- Critical for UI bindings (like we just fixed)

**Expected Issues:**
- Inconsistent attribute initialization
- Old non-GAS remnants
- Duplicate effect applications
- Unclear ownership patterns

**Impact:** High - Affects all GAS-dependent systems

---

#### 3. **Execution Calculations** (Damage calculations)
**Why Third:**
- Directly affects combat balance
- Likely has old damage formulas
- May have competing calculation methods
- Depends on clean GAS and CombatComponent

**Expected Issues:**
- Multiple damage calculation paths
- Unused modifiers
- Inconsistent resistance/armor calculations
- Dead code from old systems

**Impact:** High - Affects game balance and feel

---

### **TIER 2: IMPORTANT - Clean These Next** 🟡

#### 4. **Modified Magnitude Calculations (MMCs)**
**Why Fourth:**
- Complex attribute calculations
- Likely has redundant calculations
- Affects character stats and progression
- Depends on clean AttributeSet

**Expected Issues:**
- Duplicate calculation logic
- Unused MMC classes
- Inconsistent formulas
- Old stat systems

**Impact:** Medium-High - Affects character progression

---

#### 5. **Custom Components** (Other than Combat)
**Why Fifth:**
- Support various gameplay features
- May have competing systems
- Less critical than combat systems
- Can be cleaned individually

**Expected Issues:**
- Unused functionality
- Duplicate responsibilities
- Old event bindings
- Cached data never used

**Impact:** Medium - Affects specific features

---

### **TIER 3: MODERATE - Clean When Ready** 🟢

#### 6. **Attributes** (If separate from AttributeSet)
**Why Sixth:**
- Likely already cleaned with GAS work
- Straightforward to clean
- Low complexity
- Depends on GAS being clean

**Expected Issues:**
- Old attribute definitions
- Unused attributes
- Duplicate attribute logic

**Impact:** Medium - Affects stats display

---

#### 7. **Items & Pickups**
**Why Seventh:**
- Self-contained systems
- Less interdependencies
- Easier to test in isolation
- Lower bug potential

**Expected Issues:**
- Old pickup logic
- Duplicate item handling
- Unused item types
- Inconsistent inventory systems

**Impact:** Medium-Low - Affects item gameplay

---

### **TIER 4: LOW PRIORITY - Clean Last** 🔵

#### 8. **Widgets** (UI Components)
**Why Eighth:**
- Mostly Blueprint-driven
- Less C++ complexity
- UI bugs are visible but not critical
- Depends on clean backend systems

**Expected Issues:**
- Unused widget classes
- Old UI update methods
- Duplicate widget controllers

**Impact:** Low - Visual only, no gameplay impact

---

#### 9. **Breakables**
**Why Ninth:**
- Simple, self-contained
- Low complexity
- Minimal dependencies
- Easy to test

**Expected Issues:**
- Old destruction logic
- Unused particle effects
- Simple cleanup

**Impact:** Low - Environmental only

---

#### 10. **PCG Spawn Volumes**
**Why Last:**
- Procedural generation system
- Likely self-contained
- Minimal gameplay impact
- Can be cleaned anytime

**Expected Issues:**
- Unused spawn parameters
- Old generation logic
- Simple cleanup

**Impact:** Very Low - Level design tool

---

## 📋 RECOMMENDED CLEANUP SEQUENCE

### Phase 5: Combat Systems (2-3 sessions)
1. CombatComponent
2. GAS Core Files
3. Execution Calculations

### Phase 6: Calculations & Stats (1-2 sessions)
4. Modified Magnitude Calculations
5. Attributes
6. Custom Components

### Phase 7: Items & Interaction (1 session)
7. Items
8. Pickups

### Phase 8: Polish & Environment (1 session)
9. Widgets
10. Breakables
11. PCG Spawn Volumes

---

## 🎯 WHY THIS ORDER?

### Bottom-Up Dependency Approach:
1. **Foundation First:** GAS and Combat are the foundation
2. **Build Up:** Calculations depend on GAS
3. **Features Next:** Items/Pickups depend on clean combat
4. **Polish Last:** UI and environment are least critical

### Risk-Based Approach:
1. **Highest Bug Risk:** CombatComponent (complex, many dependencies)
2. **High Impact:** GAS (affects everything)
3. **Balance Critical:** Execution Calculations (affects gameplay feel)
4. **Lower Risk:** Items, UI, Environment (self-contained)

### Testing Efficiency:
- Clean combat systems first = easier to test everything else
- Clean GAS first = UI bindings work correctly
- Clean calculations first = accurate damage/stats for testing

---

## 💡 LESSONS FROM CHARACTER CLEANUP

### What We Learned:
1. **Competing Systems:** Look for old vs new implementations
2. **Cached Data:** Remove unused cached parameters
3. **Delegate Bindings:** Ensure GAS delegates fire correctly
4. **Duplicate Code:** Remove redundant includes and methods
5. **Commented Code:** Delete old systems entirely

### Apply These Patterns:
- Start with the most complex/central class in each tier
- Look for competing damage/calculation systems
- Remove old non-GAS remnants
- Standardize initialization patterns
- Test after each cleanup

---

## 🚀 QUICK WINS

If you want some easy wins between major cleanups:

### Easy Cleanups (30 min each):
- Breakables (simple, self-contained)
- PCG Spawn Volumes (likely clean already)
- Individual Custom Components (if small)

### Medium Cleanups (1-2 hours):
- Items & Pickups (moderate complexity)
- Widgets (mostly straightforward)

### Complex Cleanups (2-4 hours):
- CombatComponent (behemoth, like you said)
- MMCs (nightmare, as you mentioned)
- Execution Calculations (many files)

---

## 📊 ESTIMATED TIME

| Tier | Classes | Est. Time | Complexity |
|------|---------|-----------|------------|
| Tier 1 | 3 classes | 6-8 hours | Very High |
| Tier 2 | 2 categories | 3-4 hours | High |
| Tier 3 | 2 categories | 2-3 hours | Medium |
| Tier 4 | 3 categories | 2-3 hours | Low |
| **Total** | **10 categories** | **13-18 hours** | **Varies** |

---

## 🎯 RECOMMENDATION

**Start with CombatComponent next.** It's the natural continuation after Characters and Weapons, and it will likely reveal issues that affect other systems. Once combat is clean, everything else becomes easier to test and verify.

**Alternative:** If you want a confidence boost first, do a quick cleanup of Breakables or PCG Spawn Volumes to get an easy win, then tackle CombatComponent.

---

## 📝 NOTES

- Each cleanup will get faster as you develop patterns
- Some categories may be cleaner than expected
- Some may reveal unexpected issues
- Adjust priorities based on what you discover
- Take breaks between complex cleanups (CombatComponent, MMCs)

Good luck with the rest of your cleanup journey! 🚀
