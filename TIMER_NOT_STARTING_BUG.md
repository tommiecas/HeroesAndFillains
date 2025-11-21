# 🚨 BUG #8: TIMER NOT STARTING AFTER MONTAGE ENDS!

## 🔍 The Problem:

Looking at the logs:
```
✅ BP_Gnarledlings_C_1 ATTACK MONTAGE STARTED!
⏰ BP_Gnarledlings_C_1 starting attack timer (1.43s)  ← Timer starts DURING montage
... (montage plays for 2 seconds) ...
BP_Gnarledlings_C_1 attack montage ended. Interrupted: 0
BP_Gnarledlings_C_1 set weapon collision: DISABLED
🔍 CanAttack() check: CAN ATTACK  ← Can attack but...
🔍 CanAttack() check: CAN ATTACK  ← Can attack but...
🔍 CanAttack() check: CAN ATTACK  ← Can attack but...
... (NO TIMER STARTS!) ...
```

After the montage ends, `CanAttack()` returns TRUE but `StartAttackTimer()` is NEVER called!

## 🎯 Root Cause:

Look at `CheckCombatTarget()`:
```cpp
void AEnemyBase::CheckCombatTarget()
{
    if (IsOutsideCombatRadius())
    {
        // ...
    }
    else if (IsOutsideAttackRadius() && !IsEnemyChasing())
    {
        // ...
    }
    else if (CanAttack())  // ✅ This should start timer
    {
        StartAttackTimer();
    }
}
```

But `CheckCombatTarget()` is only called from `Tick()` when `EnemyState > EES_Patrolling`:
```cpp
void AEnemyBase::Tick(float DeltaTime)
{
    if (Execute_IsDead(this)) return;
    if (EnemyState > EEnemyState::EES_Patrolling)  // ← Chasing, Attacking, Engaged
    {
        CheckCombatTarget();
    }
}
```

After the montage ends, `OnAttackMontageEnded()` sets state to `EES_Idle`:
```cpp
void AEnemyBase::OnAttackMontageEnded(...)
{
    EnemyState = EEnemyState::EES_Idle;  // ❌ IDLE < PATROLLING!
    // ...
}
```

So `EnemyState::EES_Idle` (0) is NOT > `EEnemyState::EES_Patrolling` (1), so `CheckCombatTarget()` never runs!

## ✅ The Fix:

Change `OnAttackMontageEnded()` to set state back to `EES_Chasing` instead of `EES_Idle`:

```cpp
void AEnemyBase::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    // Reset attack state to CHASING (not Idle!) so CheckCombatTarget() runs
    EnemyState = EEnemyState::EES_Chasing;  // ✅ FIX!
    bCanDamage = false;
    DamagedActors.Empty();
    
    SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // CheckCombatTarget will now run in Tick() and start next attack timer
}
```

Fixing now...
