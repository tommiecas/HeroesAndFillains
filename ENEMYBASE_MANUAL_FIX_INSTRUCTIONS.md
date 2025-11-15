# EnemyBase.cpp Manual Fix Instructions

## Current Status
- ✅ Collision channels fixed in `RegisterAttackCollision()` (lines changed: ECC_Pawn → ECC_EnemyWeaponBox, ECC_PlayerCharacter)
- ✅ HitInterface include added
- ❌ 3 compilation errors remain due to removed charm/flee system

## Fix #1: Remove CachedPC references (Lines 439-454)

**Location:** `OnHoverStart()` function around line 439

**FIND THIS CODE:**
```cpp
void AEnemyBase::OnHoverStart()
{
    if (bIsHovered) return; // 👈 Prevent re-triggering
    bIsHovered = true;

    if (!CachedPC)
        CachedPC = UGameplayStatics::GetPlayerController(this, 0);

    if (!CachedPC || !EnemyAttributeMenuWidgetClass)
        return;

    // ✅ Create the widget once
    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(CachedPC, EnemyAttributeMenuWidgetClass);
    if (!ActiveAttributeMenuWidget) return;

    ActiveAttributeMenuWidget->AddToViewport(10);
    ActiveAttributeMenuWidget->SetWidgetController(EnemyWidgetController);
    ActiveAttributeMenuWidget->FadeIn(0.2f); // smoother than ShowTemporarily

    FVector2D MousePos;
    CachedPC->GetMousePosition(MousePos.X, MousePos.Y);
    ActiveAttributeMenuWidget->SetPositionInViewport(MousePos + FVector2D(20.f, 20.f), true);

    UE_LOG(LogTemp, Log, TEXT("%s: Hover menu opened once"), *GetName());
}
```

**REPLACE WITH:**
```cpp
void AEnemyBase::OnHoverStart()
{
    if (bIsHovered) return;
    bIsHovered = true;

    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || !EnemyAttributeMenuWidgetClass)
        return;

    ActiveAttributeMenuWidget = CreateWidget<UEnemyAttributeMenuWidget>(PC, EnemyAttributeMenuWidgetClass);
    if (!ActiveAttributeMenuWidget) return;

    ActiveAttributeMenuWidget->AddToViewport(10);
    ActiveAttributeMenuWidget->SetWidgetController(EnemyWidgetController);
    ActiveAttributeMenuWidget->FadeIn(0.2f);

    FVector2D MousePos;
    PC->GetMousePosition(MousePos.X, MousePos.Y);
    ActiveAttributeMenuWidget->SetPositionInViewport(MousePos + FVector2D(20.f, 20.f), true);

    UE_LOG(LogTemp, Log, TEXT("%s: Hover menu opened once"), *GetName());
}
```

**Changes:**
- Remove lines 441-442 (`if (!CachedPC)` and `CachedPC = ...`)
- Change line 444: `if (!CachedPC` → `APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);` then `if (!PC`
- Change line 448: `CachedPC` → `PC`
- Change line 455: `CachedPC` → `PC`

---

## Fix #2: Remove bIsCharmed check (Line 1575)

**Location:** `HitReactTagChanged()` function around line 1575

**FIND THIS CODE:**
```cpp
void AEnemyBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bHitReacting = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
    HAFAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);

    if (bHitReacting)
    {
        if (bIsCharmed)
        {
            if (AAIController* AIC = Cast<AAIController>(GetController()))
            {
                if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
                {
                    BB->SetValueAsBool(TEXT("IsHitReacting"), true);
                }
            }
        }
    }
}
```

**REPLACE WITH:**
```cpp
void AEnemyBase::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    bHitReacting = NewCount > 0;
    GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
    HAFAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);

    if (bHitReacting)
    {
        if (AAIController* AIC = Cast<AAIController>(GetController()))
        {
            if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
            {
                BB->SetValueAsBool(TEXT("IsHitReacting"), true);
            }
        }
    }
}
```

**Changes:**
- Remove lines 1577-1578 (the `if (bIsCharmed)` check and its opening brace)
- Remove line 1586 (the closing brace for `if (bIsCharmed)`)

---

## Fix #3: Delete Charm/Flee Functions (Lines 1765-1795)

**Location:** After `SpawnSoul()` function, around line 1765

**DELETE THESE ENTIRE FUNCTIONS:**

```cpp
void AEnemyBase::TriggerCharm(AActor* InPlayerActor)
{
    CachedPlayer = InPlayerActor;
    bIsCharmed = true;
    UE_LOG(LogTemp, Log, TEXT("%s charmed by %s"), *GetName(), *GetNameSafe(InPlayerActor));
}

void AEnemyBase::BeginFlee()
{
    if (bIsFleeing) return;
    bIsFleeing = true;
    UE_LOG(LogTemp, Log, TEXT("%s fleeing!"), *GetName());
    DoNextFleeHop();
}

void AEnemyBase::DoNextFleeHop()
{
    if (!CachedPlayer) return;

    const FVector Dir = (GetActorLocation() - CachedPlayer->GetActorLocation()).GetSafeNormal();
    const FVector Dest = GetActorLocation() + Dir * FleeHopDistance;
    UE_LOG(LogTemp, Log, TEXT("%s hopping away to %s"), *GetName(), *Dest.ToString());

    SetActorLocation(Dest, true);
}

void AEnemyBase::AddStateTag(const FGameplayTag& Tag)
{
    UE_LOG(LogTemp, Verbose, TEXT("%s adding tag %s"), *GetName(), *Tag.ToString());
}

void AEnemyBase::RemoveStateTag(const FGameplayTag& Tag)
{
    UE_LOG(LogTemp, Verbose, TEXT("%s removing tag %s"), *GetName(), *Tag.ToString());
}
```

**Just delete all 5 functions completely** (approximately lines 1765-1795).

---

## After Making These 3 Fixes:

1. Save the file
2. Compile with: `& "C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat" HeroesAndFillainsEditor Win64 Development "D:\HeroesAndFillains\HeroesAndFillains.uproject" -waitmutex`
3. Test Gnarled damage - it should now work!

---

## Summary of What Was Fixed:
- **Collision Channels:** Enemy weapons now use correct channels to hit players
- **Removed Dead Code:** Charm/flee system completely removed
- **Cleaned References:** No more references to removed properties

The key fix is the collision channels - that's what was preventing Gnarled from damaging you!
