#pragma once

#include "CoreMinimal.h"
#include "Items/PCPickupBaseItem.h"                    // your abstract base (owns AreaShape + HiddenTreasure)
#include "GameplayEffectTypes.h"
#include "CustomDesignedPCPickupItem.generated.h"

class UCapsuleComponent;
class UBoxComponent;
class USphereComponent;
class UHiddenTreasureComponent;
class UAbilitySystemComponent;
class UGameplayEffect;
class UShapeComponent;
class UPrimitiveComponent;
class AFillainCharacter;

/** How/when to apply an effect relative to overlaps */
UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
    None            UMETA(DisplayName="None"),
    ApplyOnOverlap  UMETA(DisplayName="Apply On Begin Overlap"),
    ApplyOnEndOverlap UMETA(DisplayName="Apply On End Overlap")
};

/** What to do with infinite effects when overlap ends */
UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
    DoNothing       UMETA(DisplayName="Do Nothing"),
    RemoveOnEndOverlap UMETA(DisplayName="Remove On End Overlap")
};

UCLASS(Blueprintable)
class HEROESANDFILLAINS_API ACustomDesignedPCPickupItem : public APCPickupBaseItem
{
    GENERATED_BODY()

public:
    ACustomDesignedPCPickupItem();
    
    // -------- Engine lifecycle --------
    virtual void BeginPlay() override;

    // -------- Overlap handlers we bind to AreaShape --------
    UFUNCTION()
    virtual void OnAreaBeginOverlap(UPrimitiveComponent* OverlappedComp,
                            AActor* OtherActor,
                            UPrimitiveComponent* OtherComp,
                            int32 OtherBodyIndex,
                            bool bFromSweep,
                            const FHitResult& SweepResult);

    UFUNCTION()
    void OnAreaEndOverlap(UPrimitiveComponent* OverlappedComp,
                          AActor* OtherActor,
                          UPrimitiveComponent* OtherComp,
                          int32 OtherBodyIndex);

    // High-level “do the thing” wrappers you were calling before
    UFUNCTION(BlueprintCallable)
    void OnOverlap(AActor* TargetActor);

    UFUNCTION(BlueprintCallable)
    void OnEndOverlap(AActor* TargetActor);

    // -------- Ability System helpers --------
    UAbilitySystemComponent* GetASCFromCharacter(AFillainCharacter* PlayerChar) const;

    UFUNCTION(BlueprintCallable, Category="Pickup|Effects")
    void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

    // -------- Policies & assets (configure per BP) --------
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup|Effects")
    TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup|Effects")
    TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup|Effects")
    TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

    UPROPERTY(EditAnywhere, Category="Pickup|Effects")
    EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::ApplyOnOverlap;

    UPROPERTY(EditAnywhere, Category="Pickup|Effects")
    EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::ApplyOnOverlap;

    UPROPERTY(EditAnywhere, Category="Pickup|Effects")
    EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::ApplyOnOverlap;

    UPROPERTY(EditAnywhere, Category="Pickup|Effects")
    EEffectRemovalPolicy  InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

    // Track infinite effects we applied so we can remove them on end overlap
    UPROPERTY()
    TMap<FActiveGameplayEffectHandle, TWeakObjectPtr<UAbilitySystemComponent>> ActiveEffectHandles;

    // Optional: message level you were broadcasting (keep if you use it)
    UPROPERTY(EditAnywhere, Category="Pickup|UI")
    int32 ActorLevel = 1;

    UPROPERTY(EditAnywhere, Category="Pickup")
    bool bRequirePhysicalOverlapForApplication = true;

    UPROPERTY(EditAnywhere) TArray<UMeshComponent*> HighlightMeshes; // assign in BP
    UPROPERTY(EditAnywhere) FName EmissiveParam = "Glow";            // match your material

    UFUNCTION(BlueprintCallable)
    void SetHighlighted(bool bOn);
    
private:
    void BindOverlapToAreaShape();     // finds AreaShape from base and binds

public:
    FORCEINLINE int32 GetActorLevel() const { return ActorLevel; }
};