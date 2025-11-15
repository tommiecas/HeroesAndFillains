# Multi-Socket Combat Location Solution

## Problem
`GetCombatSocketLocation_Implementation()` returns a single `FVector`, but attacks using:
- **BothHands** need 2 locations (left + right hand)
- **BothFeet** need 2 locations (left + right foot)  
- **HandsAndFeet** need 4 locations (both hands + both feet)

Current code incorrectly uses `&&` (logical AND) which doesn't work:
```cpp
// ❌ WRONG - this returns 0 or 1, not a vector!
return (GetMesh()->GetSocketLocation(LeftHandSocketName)) && (GetMesh()->GetSocketLocation(RightHandSocketName));
```

## Solution: Change Interface to Return TArray<FVector>

### Step 1: Update CombatInterface.h

**FIND:**
```cpp
UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
FVector GetCombatSocketLocation(const FGameplayTag& MontageTag);
```

**REPLACE WITH:**
```cpp
UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
TArray<FVector> GetCombatSocketLocations(const FGameplayTag& MontageTag);
```

### Step 2: Update BaseCharacter.h

**FIND:**
```cpp
virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
```

**REPLACE WITH:**
```cpp
virtual TArray<FVector> GetCombatSocketLocations_Implementation(const FGameplayTag& MontageTag) override;
```

**ADD these socket name properties:**
```cpp
UPROPERTY(EditAnywhere, Category = "Combat")
FName LeftFootSocketName = FName("LeftFootSocket");

UPROPERTY(EditAnywhere, Category = "Combat")
FName RightFootSocketName = FName("RightFootSocket");
```

### Step 3: Update BaseCharacter.cpp Implementation

**REPLACE the entire function:**
```cpp
TArray<FVector> ABaseCharacter::GetCombatSocketLocations_Implementation(const FGameplayTag& MontageTag)
{
    TArray<FVector> Locations;
    const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();
    
    // Single socket attacks
    if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_Weapon))
    {
        if (IsValid(EquippedWeapon) && EquippedWeapon->GetWeaponMesh())
        {
            Locations.Add(EquippedWeapon->GetWeaponMesh()->GetSocketLocation(WeaponTipSocketName));
        }
    }
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_LeftHand))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
    }
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_RightHand))
    {
        Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
    }
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_LeftFoot))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
    }
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_RightFoot))
    {
        Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
    }
    // Multi-socket attacks
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothHands))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
    }
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothFeet))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
    }
    else if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_HandsAndFeet))
    {
        Locations.Add(GetMesh()->GetSocketLocation(LeftHandSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightHandSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(LeftFootSocketName));
        Locations.Add(GetMesh()->GetSocketLocation(RightFootSocketName));
    }
    
    return Locations;
}
```

### Step 4: Update All Callers

Find everywhere you call `GetCombatSocketLocation()` or `Execute_GetCombatSocketLocation()` and update to handle arrays:

**OLD:**
```cpp
FVector SocketLocation = Execute_GetCombatSocketLocation(TargetCharacter, MontageTag);
// Spawn projectile at SocketLocation
```

**NEW:**
```cpp
TArray<FVector> SocketLocations = Execute_GetCombatSocketLocations(TargetCharacter, MontageTag);
for (const FVector& SocketLocation : SocketLocations)
{
    // Spawn projectile at each SocketLocation
}
```

## Alternative: Keep Single FVector, Return Average/Center

If changing the interface is too much work, you could return the **center point** between multiple sockets:

```cpp
FVector ABaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
    const FHAFGameplayTags& GameplayTags = FHAFGameplayTags::Get();
    
    // ... single socket cases stay the same ...
    
    if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothHands))
    {
        FVector Left = GetMesh()->GetSocketLocation(LeftHandSocketName);
        FVector Right = GetMesh()->GetSocketLocation(RightHandSocketName);
        return (Left + Right) / 2.0f; // Center point between hands
    }
    if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_BothFeet))
    {
        FVector Left = GetMesh()->GetSocketLocation(LeftFootSocketName);
        FVector Right = GetMesh()->GetSocketLocation(RightFootSocketName);
        return (Left + Right) / 2.0f; // Center point between feet
    }
    if (MontageTag.MatchesTagExact(GameplayTags.Montage_Attack_HandsAndFeet))
    {
        FVector LeftHand = GetMesh()->GetSocketLocation(LeftHandSocketName);
        FVector RightHand = GetMesh()->GetSocketLocation(RightHandSocketName);
        FVector LeftFoot = GetMesh()->GetSocketLocation(LeftFootSocketName);
        FVector RightFoot = GetMesh()->GetSocketLocation(RightFootSocketName);
        return (LeftHand + RightHand + LeftFoot + RightFoot) / 4.0f; // Center of all 4
    }
    
    return FVector::ZeroVector;
}
```

**But this only spawns ONE projectile at the center, not multiple projectiles from each limb!**

## Recommendation

Use **Option 1** (TArray<FVector>) if you want:
- ✅ Multiple projectiles from each attacking limb
- ✅ Proper multi-hit detection for melee
- ✅ Accurate visual effects at each contact point

Use **Option 2** (Average/Center) if you want:
- ✅ Quick fix without changing interface
- ❌ Only one projectile/effect at center point
- ❌ Less accurate for multi-limb attacks

For a proper combat system, **Option 1 is strongly recommended**.
