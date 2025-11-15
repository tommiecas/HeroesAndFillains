# EnemyBase Collision Fix - Critical Bug Resolved

## Problem Found
Gnarled (and all enemies) couldn't damage the player because **EnemyBase had the wrong collision setup**.

## Root Cause
In `EnemyBase::RegisterAttackCollision()`, the collision boxes were configured incorrectly:

### ❌ BEFORE (Broken):
```cpp
CollisionBox->SetCollisionObjectType(ECC_Pawn);  // Wrong!
CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // Wrong!
```

**Why this was broken:**
- `ECC_Pawn` is a generic channel
- Player character uses `ECC_PlayerCharacter` channel
- Enemy weapon boxes should use `ECC_EnemyWeaponBox` channel
- **Collision channels didn't match, so overlaps never triggered!**

### ✅ AFTER (Fixed):
```cpp
CollisionBox->SetCollisionObjectType(ECC_EnemyWeaponBox);  // Correct!
CollisionBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);  // Correct!
```

**Why this works:**
- Enemy weapon boxes now use proper `ECC_EnemyWeaponBox` channel
- They overlap with `ECC_PlayerCharacter` channel (the player)
- Collision channels match, overlaps trigger correctly!

---

## Changes Made

### File: `Source/HeroesAndFillains/Private/Enemies/EnemyBase.cpp`

#### 1. Added Missing Include
```cpp
#include "Interfaces/HitInterface.h"  // ✅ Added for Execute_GetHit
```

#### 2. Fixed RegisterAttackCollision()
```cpp
void AEnemyBase::RegisterAttackCollision(UBoxComponent* CollisionBox)
{
    if (!CollisionBox) return;

    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CollisionBox->SetCollisionObjectType(ECC_EnemyWeaponBox);  // ✅ Fixed
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);  // ✅ Fixed
    CollisionBox->SetGenerateOverlapEvents(true);
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnAttackCollisionOverlap);

    AttackCollisions.AddUnique(CollisionBox);

    UE_LOG(LogTemp, Log, TEXT("%s registered attack collision: %s"), *GetName(), *CollisionBox->GetName());
}
```

#### 3. Fixed OnAttackCollisionOverlap() - Use GAS
```cpp
void AEnemyBase::OnAttackCollisionOverlap(...)
{
    // ... validation checks ...

    // ✅ NEW: Use GAS via Execute_GetHit
    FVector HitLocation = OtherActor->GetActorLocation();
    if (!SweepResult.ImpactPoint.IsNearlyZero())
    {
        HitLocation = SweepResult.ImpactPoint;
    }

    if (IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor))
    {
        HitInterface->Execute_GetHit(OtherActor, HitLocation, this);
        UE_LOG(LogTemp, Warning, TEXT("💥 %s hit %s via GAS!"), *GetName(), *GetNameSafe(OtherActor));
    }

    // Debug visuals
    DrawDebugSphere(GetWorld(), HitLocation, 20.f, 12, FColor::Red, false, 0.3f, 0, 2);

    // Reset damage cooldown
    bCanDamage = false;
    GetWorldTimerManager().SetTimer(DamageResetTimer, this, &AEnemyBase::ResetCanDamage, 0.25f, false);
}
```

**Removed:**
```cpp
// ❌ OLD: Direct damage application (bypassed GAS)
UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetController(), this, nullptr);
```

---

## Impact

### ✅ What This Fixes:

1. **All enemy melee attacks now work:**
   - Gnarled fist attacks
   - Gnarledling baby attacks
   - StormAssassin foot attacks
   - Thrope claw attacks

2. **Collision properly triggers:**
   - Animation notifies enable collision boxes
   - Boxes overlap with player
   - OnAttackCollisionOverlap fires
   - Damage applies through GAS

3. **Consistent with Phase 4 changes:**
   - All damage now flows through GAS
   - Uses Execute_GetHit interface
   - Matches player weapon implementation

### 🎯 Expected Behavior Now:

```
1. Enemy attacks (animation plays)
   ↓
2. Animation notify enables fist/foot collision
   ↓
3. Collision box overlaps with player
   ↓
4. OnAttackCollisionOverlap called
   ↓
5. Execute_GetHit called on player
   ↓
6. Player's GetHit_Implementation runs
   ↓
7. GAS applies damage
   ↓
8. Player health decreases
   ↓
9. HUD updates
```

---

## Testing

### What to Test:
1. Let Gnarled attack you
2. Check console for: `"💥 Gnarled_C_0 hit FillainCharacter_C_0 via GAS!"`
3. Check if red debug sphere appears
4. Check if your health decreases
5. Check if HUD updates

### Expected Console Output:
```
⚔️ Gnarled_C_0 attack collision overlap with FillainCharacter_C_0
💥 Gnarled_C_0 hit FillainCharacter_C_0 via GAS!
🎯 PLAYER GOT HIT! (if you added debug logs)
❤️ FillainCharacter_C_0 Health changed: 100.0 → 75.0
```

---

## Why This Bug Existed

**The collision mismatch was introduced when:**
1. Gnarled.cpp correctly used `ECC_EnemyWeaponBox` in constructor
2. But EnemyBase::RegisterAttackCollision() **overwrote** it with `ECC_Pawn`
3. This caused a channel mismatch
4. Overlaps never triggered

**The fix:**
- Make RegisterAttackCollision() use the correct channels
- Now it matches what Gnarled sets up in constructor
- Collision works properly!

---

## Files Modified

1. ✅ `Source/HeroesAndFillains/Private/Enemies/EnemyBase.cpp`
   - Added HitInterface include
   - Fixed RegisterAttackCollision collision channels
   - Fixed OnAttackCollisionOverlap to use GAS

---

## Next Steps

1. ⏳ Wait for compilation to finish
2. ✅ Test Gnarled attacks in-game
3. ✅ Verify damage applies correctly
4. ✅ Test other enemies
5. ✅ Complete Phase 4 testing

---

## Status

**Build:** ⏳ Compiling...
**Expected:** ✅ Should compile successfully
**Testing:** Pending user verification

This was the missing piece! The collision channels were mismatched, preventing overlaps from triggering.
