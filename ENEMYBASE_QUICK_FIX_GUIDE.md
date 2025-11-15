# EnemyBase.cpp - Quick Fix Guide

## Step-by-Step Instructions

### 1. Open the file
Open: `Source/HeroesAndFillains/Private/Enemies/EnemyBase.cpp`

---

### 2. Fix #1 - OnHoverStart() function

**Use Find & Replace (Ctrl+H):**

**FIND (enable "Match Whole Word"):**
```
    if (!CachedPC)
        CachedPC = UGameplayStatics::GetPlayerController(this, 0);

    if (!CachedPC || !EnemyAttributeMenuWidgetClass)
        return;

    // ✅ Create the widget once
    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(CachedPC, EnemyAttributeMenuWidgetClass);
```

**REPLACE WITH:**
```
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !EnemyAttributeMenuWidgetClass)
        return;

    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(PC, EnemyAttributeMenuWidgetClass);
```

**Then find:**
```
    CachedPC->GetMousePosition(MousePos.X, MousePos.Y);
```

**Replace with:**
```
    PC->GetMousePosition(MousePos.X, MousePos.Y);
```

---

### 3. Fix #2 - HitReactTagChanged() function

**FIND:**
```
    if (bHitReacting)
    {
        if (bIsCharmed)
        {
            if (AAIController* AIC = Cast<AAIController>(GetController()))
```

**REPLACE WITH:**
```
    if (bHitReacting)
    {
        if (AAIController* AIC = Cast<AAIController>(GetController()))
```

**Then find the matching closing brace and remove one level of nesting.**

**OR SIMPLER:** Just delete line with `if (bIsCharmed)` and its opening `{`, then delete the matching closing `}` a few lines down.

---

### 4. Fix #3 - Delete Charm/Flee Functions

**Search for this function and DELETE IT ENTIRELY:**
```cpp
void AEnemyBase::TriggerCharm(AActor* InPlayerActor)
```
Delete from `void AEnemyBase::TriggerCharm` through the closing `}` of that function.

**Then DELETE these 4 more functions:**
1. `void AEnemyBase::BeginFlee()`
2. `void AEnemyBase::DoNextFleeHop()`
3. `void AEnemyBase::AddStateTag(const FGameplayTag& Tag)`
4. `void AEnemyBase::RemoveStateTag(const FGameplayTag& Tag)`

**TIP:** Use Ctrl+F to search for each function name, then select from the function declaration to its closing brace and delete.

---

## Verification

After making all 3 fixes, search the file for:
- `CachedPC` - should find 0 results
- `bIsCharmed` - should find 0 results  
- `CachedPlayer` - should find 0 results
- `TriggerCharm` - should find 0 results

---

## Compile

Run this command:
```
& "C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat" HeroesAndFillainsEditor Win64 Development "D:\HeroesAndFillains\HeroesAndFillains.uproject" -waitmutex
```

Should compile successfully!

---

## What This Fixes

**The Critical Fix:** `RegisterAttackCollision()` now uses correct collision channels:
- `ECC_EnemyWeaponBox` instead of `ECC_Pawn`
- `ECC_PlayerCharacter` instead of `ECC_Pawn`

This was preventing Gnarled's weapon from triggering overlaps with the player!

**Cleanup:** Removed all references to the old charm/flee system that was deleted from the header.
