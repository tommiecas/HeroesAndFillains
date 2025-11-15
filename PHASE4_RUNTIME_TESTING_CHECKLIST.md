# Phase 4: Runtime Testing Checklist

## Context
We've successfully migrated 10 files from the old `ApplyDamage()` system to the new GAS-based `Execute_GetHit()` system. This checklist will help verify everything works correctly.

---

## 🎯 Critical Tests (Must Pass)

### Player → Enemy Damage

#### Melee Weapons
- [ ] **Equip melee weapon** (sword, axe, etc.)
- [ ] **Attack Gnarled enemy**
  - [ ] Enemy health bar decreases
  - [ ] Enemy plays hit reaction animation
  - [ ] Blood/hit effects appear
  - [ ] Damage numbers display (if implemented)
- [ ] **Attack Gnarledling enemy**
  - [ ] Same checks as above
- [ ] **Kill an enemy with melee**
  - [ ] Enemy dies correctly
  - [ ] Death animation plays
  - [ ] Loot/soul drops (if implemented)

#### Ranged Weapons - Instant Hit
- [ ] **Equip rifle/pistol** (HitScan weapon)
- [ ] **Shoot Spectral enemy**
  - [ ] Enemy health decreases
  - [ ] Hit reaction plays
  - [ ] Hit effects appear at impact point
- [ ] **Shoot from different distances**
  - [ ] Close range works
  - [ ] Medium range works
  - [ ] Long range works

#### Ranged Weapons - Projectiles
- [ ] **Equip projectile weapon**
- [ ] **Shoot StormAssassin enemy**
  - [ ] Projectile travels correctly
  - [ ] Damage applies on impact
  - [ ] Hit effects appear
- [ ] **Test projectile collision**
  - [ ] Hits enemies correctly
  - [ ] Doesn't hit allies (if applicable)

#### Shotgun
- [ ] **Equip shotgun**
- [ ] **Shoot Thrope enemy at close range**
  - [ ] Multiple pellets hit
  - [ ] Damage is higher than single bullet
  - [ ] Hit effects appear
- [ ] **Shoot at medium range**
  - [ ] Spread pattern works
  - [ ] Some pellets miss (expected)
  - [ ] Damage is lower than close range

---

### Enemy → Player Damage

#### Melee Enemies
- [ ] **Let Gnarled attack you**
  - [ ] Your health bar decreases
  - [ ] You play hit reaction animation
  - [ ] Screen effects appear (damage indicator)
  - [ ] HUD updates correctly
- [ ] **Let Gnarledling attack you**
  - [ ] Damage is lower than Gnarled (baby enemy)
  - [ ] All effects work
- [ ] **Let StormAssassin kick you**
  - [ ] Foot attacks work
  - [ ] Damage applies correctly
- [ ] **Let Thrope claw you**
  - [ ] Hand claw attacks work
  - [ ] Foot claw attacks work
  - [ ] Damage applies from both

#### Ranged Enemies
- [ ] **Let Spectral shoot you**
  - [ ] Ranged attack hits
  - [ ] Damage applies
  - [ ] You react to hit
  - [ ] Health decreases

---

## 🔍 Detailed Verification

### HUD & UI
- [ ] **Health bar updates smoothly**
  - [ ] No flickering
  - [ ] Accurate values
  - [ ] Updates in real-time
- [ ] **Shield bar updates** (if applicable)
- [ ] **Damage numbers appear** (if implemented)
  - [ ] Correct values
  - [ ] Proper positioning
  - [ ] Readable colors

### Visual Effects
- [ ] **Blood effects spawn**
  - [ ] At correct impact point
  - [ ] Appropriate size/intensity
  - [ ] Proper orientation
- [ ] **Hit sparks/particles**
  - [ ] Appear on hit
  - [ ] Correct location
  - [ ] Proper timing
- [ ] **Screen effects**
  - [ ] Damage vignette (red edges)
  - [ ] Screen shake (if implemented)
  - [ ] Directional damage indicator

### Audio
- [ ] **Hit sounds play**
  - [ ] Weapon impact sounds
  - [ ] Enemy grunt/pain sounds
  - [ ] Player pain sounds
- [ ] **No audio glitches**
  - [ ] Sounds don't overlap excessively
  - [ ] Volume is appropriate
  - [ ] No crackling/popping

### Animations
- [ ] **Hit reactions play correctly**
  - [ ] Front hit reaction
  - [ ] Back hit reaction
  - [ ] Left hit reaction
  - [ ] Right hit reaction
- [ ] **Death animations**
  - [ ] Appropriate death pose
  - [ ] Ragdoll works (if implemented)
  - [ ] Body stays visible
- [ ] **Attack animations**
  - [ ] Complete properly
  - [ ] Don't get interrupted incorrectly
  - [ ] Blend smoothly

---

## ⚠️ Edge Cases & Stress Tests

### Multiple Enemies
- [ ] **Fight 3+ enemies at once**
  - [ ] All damage applies correctly
  - [ ] No performance issues
  - [ ] HUD updates for all hits
- [ ] **Rapid attacks**
  - [ ] Fast melee combo works
  - [ ] Rapid fire works
  - [ ] No damage is lost

### Multiple Damage Sources
- [ ] **Take damage from 2+ enemies simultaneously**
  - [ ] All damage applies
  - [ ] Effects stack correctly
  - [ ] No crashes
- [ ] **Deal damage to 2+ enemies with shotgun**
  - [ ] All enemies take damage
  - [ ] Damage is calculated per enemy
  - [ ] No performance issues

### Death & Respawn
- [ ] **Die from enemy damage**
  - [ ] Death triggers correctly
  - [ ] Death screen appears
  - [ ] Can respawn
- [ ] **Kill multiple enemies**
  - [ ] All die correctly
  - [ ] Bodies remain/despawn properly
  - [ ] Loot spawns correctly

### Network (If Multiplayer)
- [ ] **Host: Damage enemies**
  - [ ] Damage replicates to clients
  - [ ] Effects appear for all players
- [ ] **Client: Damage enemies**
  - [ ] Damage applies on server
  - [ ] No desync issues
- [ ] **PvP damage** (if applicable)
  - [ ] Player vs player damage works
  - [ ] Friendly fire settings respected

---

## 🐛 Known Issues to Watch For

### Potential Problems
- [ ] **Damage not applying**
  - Check console for errors
  - Verify GAS is initialized
  - Check GameplayEffect setup
- [ ] **Double damage**
  - Old system still active?
  - Multiple damage calls?
- [ ] **No hit reactions**
  - GetHit_Implementation called?
  - Montages assigned?
- [ ] **HUD not updating**
  - Attribute delegates bound?
  - Widget references valid?
- [ ] **Crashes on hit**
  - Null pointer checks
  - Invalid casts
  - Missing components

---

## 📊 Performance Checks

### Frame Rate
- [ ] **FPS stable during combat**
  - [ ] No significant drops
  - [ ] Smooth gameplay
- [ ] **FPS with many enemies**
  - [ ] 5+ enemies on screen
  - [ ] All attacking simultaneously

### Memory
- [ ] **No memory leaks**
  - [ ] Play for 5+ minutes
  - [ ] Memory usage stable
  - [ ] No growing allocations

---

## ✅ Console Commands for Testing

```cpp
// Show damage debug info
ShowDebug DAMAGE

// God mode (test without dying)
God

// Infinite ammo
InfiniteAmmo

// Spawn test enemies
Summon Gnarled
Summon Gnarledling
Summon SpectralBase
Summon StormAssassin
Summon Thrope

// Kill all enemies
KillAll EnemyBase

// Teleport to test area
Teleport

// Show FPS
stat fps

// Show detailed stats
stat unit
```

---

## 📝 Bug Report Template

If you find issues, note:

```
**Issue:** [Brief description]
**Steps to Reproduce:**
1. 
2. 
3. 

**Expected:** [What should happen]
**Actual:** [What actually happens]
**Frequency:** [Always/Sometimes/Rare]
**Console Errors:** [Any error messages]
**Additional Notes:** [Screenshots, videos, etc.]
```

---

## ✨ Success Criteria

### Minimum Requirements (Must Pass)
- ✅ Player can damage all enemy types
- ✅ All enemies can damage player
- ✅ HUD updates correctly
- ✅ No crashes or critical errors
- ✅ Hit reactions play

### Ideal State (Nice to Have)
- ✅ All visual effects work
- ✅ All audio works
- ✅ Smooth performance
- ✅ No edge case issues
- ✅ Network play works (if applicable)

---

## 🎬 Testing Workflow

### Quick Test (5 minutes)
1. Load game
2. Attack one enemy of each type
3. Let one enemy of each type attack you
4. Verify basic damage works
5. Check for console errors

### Thorough Test (15 minutes)
1. Test all weapon types
2. Test all enemy types
3. Test edge cases
4. Check performance
5. Verify all effects

### Full Test (30+ minutes)
1. Complete quick test
2. Complete thorough test
3. Test multiplayer (if applicable)
4. Stress test with many enemies
5. Test for extended period
6. Document all findings

---

## 📞 Next Steps After Testing

### If All Tests Pass ✅
- Report success
- Move to fixing remaining 4 files:
  - CombatComponent.cpp
  - LagCompensationComponent.cpp
  - BaseCharacter.cpp cleanup
  - EnemyBase.cpp cleanup

### If Issues Found ⚠️
- Document issues clearly
- Check console for errors
- Review relevant code
- Fix issues before proceeding
- Re-test after fixes

---

## 💡 Tips

- **Test in PIE first** (Play In Editor) - faster iteration
- **Use console commands** - speed up testing
- **Check Output Log** - catch errors early
- **Test one thing at a time** - easier to isolate issues
- **Save frequently** - don't lose progress
- **Record video** - helpful for bug reports

---

Good luck with testing! 🎮
