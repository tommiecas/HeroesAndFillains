# BaseCharacter.cpp - GetCombatSocketLocation Fix

## Problem
Lines 330-344 in `BaseCharacter.cpp` use the `&&` (logical AND) operator incorrectly:

```cpp
// ❌ WRONG - && returns bool (true/false), not a vector!
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothHands))
{
    return (GetMesh()->GetSocketLocation(LeftHandSocketName)) && (GetMesh()->GetSocketLocation(RightHandSocketName));
}
```

This will return `0` or `1` (converted to FVector), not the actual socket locations!

## Quick Fix (Return Center Point)

**FIND these 3 broken cases (around lines 330-344):**

```cpp
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothHands))
{
    return (GetMesh()->GetSocketLocation(LeftHandSocketName)) && (GetMesh()->GetSocketLocation(RightHandSocketName));
}
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothFeet))
{
    return (GetMesh()->GetSocketLocation(LeftFootSocketName)) && (GetMesh()->GetSocketLocation(RightFootSocketName));
}
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_HandsAndFeet))
{
    return (GetMesh()->GetSocketLocation(LeftHandSocketName)) && (GetMesh()->GetSocketLocation(RightHandSocketName)) && (GetMesh()->GetSocketLocation(LeftFootSocketName)) && (GetMesh()->GetSocketLocation(RightFootSocketName));
}
```

**REPLACE WITH:**

```cpp
// Multi-socket attacks - return center point between sockets
// NOTE: This only returns ONE location (the average). For proper multi-limb attacks,
// consider changing the interface to return TArray<FVector> (see MULTI_SOCKET_COMBAT_SOLUTION.md)
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothHands))
{
    FVector LeftHand = GetMesh()->GetSocketLocation(LeftHandSocketName);
    FVector RightHand = GetMesh()->GetSocketLocation(RightHandSocketName);
    return (LeftHand + RightHand) / 2.0f; // Center point between hands
}
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothFeet))
{
    FVector LeftFoot = GetMesh()->GetSocketLocation(LeftFootSocketName);
    FVector RightFoot = GetMesh()->GetSocketLocation(RightFootSocketName);
    return (LeftFoot + RightFoot) / 2.0f; // Center point between feet
}
if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_HandsAndFeet))
{
    FVector LeftHand = GetMesh()->GetSocketLocation(LeftHandSocketName);
    FVector RightHand = GetMesh()->GetSocketLocation(RightHandSocketName);
    FVector LeftFoot = GetMesh()->GetSocketLocation(LeftFootSocketName);
    FVector RightFoot = GetMesh()->GetSocketLocation(RightFootSocketName);
    return (LeftHand + RightHand + LeftFoot + RightFoot) / 4.0f; // Center of all 4 limbs
}
```

**Also change the final return:**
```cpp
return FVector::ZeroVector;  // Instead of return FVector();
```

## What This Does

- **BothHands:** Returns the midpoint between left and right hands
- **BothFeet:** Returns the midpoint between left and right feet  
- **HandsAndFeet:** Returns the center point of all 4 limbs

**Limitation:** This spawns only ONE projectile/effect at the center, not multiple from each limb.

For proper multi-limb attacks with multiple projectiles, see `MULTI_SOCKET_COMBAT_SOLUTION.md`.

## After Fixing

1. Save the file
2. Rebuild: `& "C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat" HeroesAndFillainsEditor Win64 Development "D:\HeroesAndFillains\HeroesAndFillains.uproject" -waitmutex`
3. Test multi-limb attacks - they should now work (spawning from center point)
