# Final Linker Error Fixes for FillainCharacter.cpp

## 3 Fixes Needed:

### Fix 1: Remove Duplicate GetCombatSocketLocations_Implementation
**Search for this entire function in FillainCharacter.cpp and DELETE it:**
```cpp
TArray<FVector> ABaseCharacter::GetCombatSocketLocations_Implementation(const FGameplayTag& MontageTag)
{
    // ... entire function body ...
}
```
**Reason:** This is already defined in BaseCharacter.cpp. Having it in both files causes a duplicate definition error.

---

### Fix 2: Comment Out FixSelfCameraCollision() Call
**Find this line:**
```cpp
FixSelfCameraCollision();
```

**Replace with:**
```cpp
// FixSelfCameraCollision(); // Removed as part of camera watchdog cleanup
```

---

### Fix 3: Add Missing InitializeDefaultTags() Implementation
**Add this function to FillainCharacter.cpp (anywhere after BeginPlay):**
```cpp
void AFillainCharacter::InitializeDefaultTags()
{
    Super::InitializeDefaultTags();
    // FillainCharacter-specific tag initialization can go here if needed
}
```

---

## Quick PowerShell Commands (Alternative):

### Remove duplicate GetCombatSocketLocations (manual deletion safer for large function)
### Comment out FixSelfCameraCollision:
```powershell
(Get-Content 'Source/HeroesAndFillains/Private/Characters/FillainCharacter.cpp') -replace '\tFixSelfCameraCollision\(\);', '\t// FixSelfCameraCollision(); // Removed as part of camera watchdog cleanup' | Set-Content 'Source/HeroesAndFillains/Private/Characters/FillainCharacter.cpp'
```

---

## After Fixes:
Run build again:
```powershell
& "C:\Program Files\Epic Games\UE_5.5\Engine\Build\BatchFiles\Build.bat" HeroesAndFillainsEditor Win64 Development "D:\HeroesAndFillains\HeroesAndFillains.uproject" -waitmutex
