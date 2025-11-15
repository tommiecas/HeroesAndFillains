# Gnarled Not Damaging Player - Debug Guide

## Issue
Gnarled attacks but doesn't reduce player health.

## Quick Checks

### 1. Check Console Output
**Look for this log message when Gnarled hits you:**
```
💥 Gnarled_C_0 hit FillainCharacter_C_0 via GAS!
```

**If you see this:** Code is working, issue is in GAS setup
**If you don't see this:** Attack collision isn't triggering

---

## Scenario A: No Console Log (Collision Not Triggering)

### Check 1: Collision Boxes Enabled
**In Gnarled Blueprint:**
1. Check `LeftFistCollision` component
2. Check `RightFistCollision` component
3. Verify they're attached to correct sockets
4. Verify collision is enabled during attack animation

### Check 2: Animation Notifies
**In Gnarled's attack animation:**
1. Should have `EnableLeftSideMeleeAttack` notify
2. Should have `EnableRightSideMeleeAttack` notify
3. Should have `DisableLeftSideMeleeAttack` notify
4. Should have `DisableRightSideMeleeAttack` notify

**To verify:**
```cpp
// These should be called from animation:
- EnableLeftSideMeleeAttack()  // Enables collision
- DisableLeftSideMeleeAttack() // Disables collision
```

### Check 3: Collision Settings
**In Gnarled.cpp constructor, verify:**
```cpp
// Should be ECC_EnemyWeaponBox
RightFistCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);
LeftFistCollision->SetCollisionObjectType(ECC_EnemyWeaponBox);

// Should respond to player
RightFistCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
LeftFistCollision->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECR_Overlap);
```

### Check 4: Player Collision
**Your FillainCharacter should:**
- Have collision channel `ECC_PlayerCharacter`
- Respond to `ECC_EnemyWeaponBox` with `ECR_Overlap` or `ECR_Block`

---

## Scenario B: Console Log Appears (GAS Not Applying Damage)

### Check 1: Player Has GAS Components
**Verify FillainCharacter has:**
```cpp
// In FillainCharacter:
- AbilitySystemComponent (from PlayerState)
- AttributeSet (HAFAttributeSet)
- Health attribute initialized
```

**To verify in code:**
```cpp
// Add to FillainCharacter::BeginPlay() temporarily:
UE_LOG(LogTemp, Warning, TEXT("ASC: %s"), *GetNameSafe(GetAbilitySystemComponent()));
UE_LOG(LogTemp, Warning, TEXT("AttributeSet: %s"), *GetNameSafe(AttributeSet));
if (AttributeSet)
{
    UHAFAttributeSet* HAFAttr = Cast<UHAFAttributeSet>(AttributeSet);
    if (HAFAttr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Health: %.1f / %.1f"), HAFAttr->GetHealth(), HAFAttr->GetMaxHealth());
    }
}
```

### Check 2: GetHit_Implementation Exists
**Verify FillainCharacter has:**
```cpp
void AFillainCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    // Should apply GameplayEffect for damage
    // Should play hit reaction
    // Should update HUD
}
```

**Check if it's being called:**
```cpp
// Add to GetHit_Implementation:
UE_LOG(LogTemp, Error, TEXT("🎯 GetHit called! Hitter: %s, ImpactPoint: %s"), 
    *GetNameSafe(Hitter), *ImpactPoint.ToString());
```

### Check 3: GameplayEffect Setup
**Verify damage GameplayEffect exists:**
- Should have `GE_Damage` or similar
- Should modify Health attribute
- Should use ExecutionCalculation for damage

**Check Gnarled's damage effect:**
```cpp
// In EnemyBase or Gnarled:
UPROPERTY(EditAnywhere, Category = "Combat|Damage")
TSubclassOf<UGameplayEffect> DamageEffectClass;
```

**This should be set in Blueprint to your damage GameplayEffect**

### Check 4: Execution Calculation
**Verify ExecutionCalculation_Damage_Gnarled:**
1. Exists and is compiled
2. Is assigned to the GameplayEffect
3. Actually modifies Health attribute
4. Calculates damage correctly

---

## Debug Commands

### Enable GAS Debug
```cpp
// In console:
ShowDebug AbilitySystem

// This shows:
- Active GameplayEffects
- Attribute values
- Ability activations
```

### Enable Collision Debug
```cpp
// In console:
Show Collision

// Or in code:
DrawDebugBox(GetWorld(), 
    LeftFistCollision->GetComponentLocation(),
    LeftFistCollision->GetScaledBoxExtent(),
    FColor::Red, false, 5.0f);
```

### Check Attributes
```cpp
// In console (if you have a command for it):
DumpAttributes

// Or add to code:
if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
{
    ASC->GetGameplayAttributeValueChangeDelegate(
        UHAFAttributeSet::GetHealthAttribute()
    ).AddUObject(this, &AFillainCharacter::OnHealthChanged);
}
```

---

## Common Issues & Fixes

### Issue 1: Collision Not Triggering
**Fix:**
- Verify animation notifies are set up
- Check collision box size (might be too small)
- Verify collision channels match

### Issue 2: GetHit Not Called
**Fix:**
- Verify FillainCharacter implements IHitInterface
- Check if Execute_GetHit is being called correctly
- Verify cast to IHitInterface succeeds

### Issue 3: GAS Not Initialized
**Fix:**
```cpp
// In FillainCharacter::PossessedBy():
if (AHAFPlayerState* PS = GetPlayerState<AHAFPlayerState>())
{
    AbilitySystemComponent = PS->GetAbilitySystemComponent();
    AttributeSet = PS->GetAttributeSet();
    
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(PS, this);
    }
}
```

### Issue 4: Damage Effect Not Applied
**Fix:**
- Verify DamageEffectClass is set in Blueprint
- Check ExecutionCalculation is assigned
- Verify damage calculation logic

### Issue 5: Health Not Decreasing
**Fix:**
```cpp
// In GetHit_Implementation, ensure:
if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
{
    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(Hitter);
    
    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
        DamageEffectClass, 1.0f, EffectContext
    );
    
    if (SpecHandle.IsValid())
    {
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }
}
```

---

## Step-by-Step Debug Process

### Step 1: Verify Collision (5 min)
1. Add debug sphere in OnAttackCollisionOverlap
2. Let Gnarled attack
3. See if red sphere appears

**If no sphere:** Collision not triggering → Check animation notifies
**If sphere appears:** Collision works → Check GAS

### Step 2: Verify GetHit Called (5 min)
1. Add log in GetHit_Implementation
2. Let Gnarled attack
3. Check console

**If no log:** GetHit not called → Check IHitInterface implementation
**If log appears:** GetHit works → Check damage application

### Step 3: Verify GAS Setup (10 min)
1. Check ASC exists
2. Check AttributeSet exists
3. Check Health attribute initialized
4. Check DamageEffect assigned

### Step 4: Verify Damage Applied (10 min)
1. Add log before applying effect
2. Add log after applying effect
3. Check if Health value changes
4. Check if HUD updates

---

## Quick Test Code

### Add to Gnarled.cpp OnAttackCollisionOverlap:
```cpp
// After the Execute_GetHit call:
UE_LOG(LogTemp, Error, TEXT("🔴 GNARLED HIT PLAYER!"));
UE_LOG(LogTemp, Error, TEXT("   Player: %s"), *GetNameSafe(Player));
UE_LOG(LogTemp, Error, TEXT("   HitLocation: %s"), *HitLocation.ToString());
UE_LOG(LogTemp, Error, TEXT("   HitInterface Valid: %s"), 
    Cast<IHitInterface>(Player) ? TEXT("YES") : TEXT("NO"));
```

### Add to FillainCharacter.cpp GetHit_Implementation:
```cpp
void AFillainCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    UE_LOG(LogTemp, Error, TEXT("🎯 PLAYER GOT HIT!"));
    UE_LOG(LogTemp, Error, TEXT("   Hitter: %s"), *GetNameSafe(Hitter));
    UE_LOG(LogTemp, Error, TEXT("   ASC: %s"), *GetNameSafe(GetAbilitySystemComponent()));
    
    // Your damage application code here...
    
    UE_LOG(LogTemp, Error, TEXT("   Health After: %.1f"), 
        Cast<UHAFAttributeSet>(AttributeSet)->GetHealth());
}
```

---

## Expected Flow

```
1. Gnarled attacks (animation plays)
   ↓
2. Animation notify enables fist collision
   ↓
3. Fist overlaps with player
   ↓
4. OnAttackCollisionOverlap called
   ↓
5. Execute_GetHit called on player
   ↓
6. Player's GetHit_Implementation runs
   ↓
7. GameplayEffect applied to player
   ↓
8. ExecutionCalculation calculates damage
   ↓
9. Health attribute modified
   ↓
10. HUD updates
```

**Find where this chain breaks!**

---

## Report Back

Please check and report:
1. ✅/❌ Do you see "💥 Gnarled hit Player via GAS!" in console?
2. ✅/❌ Do you see "🎯 PLAYER GOT HIT!" in console?
3. ✅/❌ Does your health bar decrease?
4. ✅/❌ Do you see any errors in console?

This will help me pinpoint exactly where the issue is!
