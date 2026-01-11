// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Interfaces/EnemyInterface.h"
#include "FillainPlayerController.generated.h"

class UDamageTextComponent;
class UAbilitySystemComponent;
class UHAFInputConfig;
class UTextBlock;
class AMagicCircle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

struct FInputActionValue;
class AFillainCharacter;
class IEnemyInterface;
class UHAFAbilitySystemComponent;
class USplineComponent;




/**
 * 
 */
UCLASS()
class HEROESANDFILLAINS_API AFillainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFillainPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDSoulsCount(int32 SoulsCount);
	void SetHUDWeaponAmmo(int32 WeaponAmmo);
	void SetHUDGoldCount(int32 GoldAmount);
	void SetHUDCarriedAmmo(int32 CarriedAmmo);
	void SetHUDWeaponType(APawn* InPawn);
	void ShowEliminationUI(FString Victim, FString Killer, FText Message);
	void UpdateEliminationMessageForPvE(AFillainPlayerController* VictimPlayerController, AController* InstigatorController);
	void UpdateEliminationMessageForPvP(AFillainPlayerController* KillerPlayerController, AFillainPlayerController* VictimPlayerController);
	void InitializeHUDEliminationMessage(AFillainPlayerController* KillerPlayerController, AFillainPlayerController* VictimPlayerController, AController* InstigatorController);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void SetHUDGrenades(int32 Grenades);

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, AActor* TargetAvatarActor, bool bBlockedHit, bool bCriticalHit);

	UFUNCTION(BlueprintCallable)
	bool IsHUDReady() const;

	UFUNCTION(BlueprintCallable)
	void CursorTrace();
	
	FHitResult CursorHit;

	bool GetClickHit(FHitResult& OutHit) const;


	void HideTeamScores();
	void InitTeamScores();
	void SetHUDRedTeamScore(int32 RedScore);
	void SetHUDBlueTeamScore(int32 BlueScore);

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual float GetServerTime(); // Synced with server World clock
	virtual void ReceivedPlayer() override;
	void InitOverlayIfNeeded();
	
	virtual void OnRep_PlayerState() override;

	UPROPERTY()
	bool bOverlayInitialized = false;
	
	float MatchTimeElapsedTime = 0.f;
	bool bIsMatchCountdownVisible = true;
	FLinearColor MatchCountdownColor;
	FLinearColor MatchCountdownBlinkingColor;

	void UpdateMatchCountdownColor();
	void ToggleMatchCountdownVisibility();

	UPROPERTY()
	AFillainCharacter* VictimCharacter;

	UPROPERTY()
	AFillainCharacter* KillerCharacter;

	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	void HandleCooldown();

	float SingleTripTime = 0.f;

	UPROPERTY()
	class AWeaponBase* EquippedWeapon;

	UPROPERTY()
	class ARangedWeapon* EquippedRangedWeapon;

	UPROPERTY()
	class AMeleeWeapon* EquippedMeleeWeapon;

	FHighPingDelegate HighPingDelegate;

	void BroadcastElimination(APlayerState* Killer, APlayerState* Victim);
	
	UFUNCTION()
	void ToggleInputChatBox();

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(Server, Reliable)
	void ServerSetText(const FString& Text, const FString& PlayerName);

	UFUNCTION(Client, Reliable)
	void ClientSetText(const FString& Text, const FString& PlayerName);

	UPROPERTY()
	class AHAFGameMode* GameMode;


	virtual void SetupInputComponent() override;

	UFUNCTION()
	void ActivateByTag(FGameplayTag Tag);

	void DebugCursorTrace();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	TObjectPtr<AMagicCircle> MagicCircle;

	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);

	UFUNCTION(BlueprintCallable)
	void HideMagicCircle();

	UFUNCTION(BlueprintCallable)
	void UpdateMagicCircleLocation();


private:
	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UPlayerChat> PlayerChatClass;

	UPROPERTY()
	UPlayerChat* PlayerChatWidget;

	UPROPERTY() UAbilitySystemComponent* ASC = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Input|Abilities")
	FGameplayTag FireballTag; // e.g. set to "Ability.Fireball" in defaults

protected:
	virtual void BeginPlay() override;
	void SetHUDTime();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* FillainMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* QuitAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ChatAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ShiftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleGameModeAction;

	void ShiftPressed() {bShiftKeyDown = true;};
	void ShiftReleased() {bShiftKeyDown = false;};
	bool bShiftKeyDown = false;

	/**************************************
	* Sync Time Between Client And Server *
	***************************************/

	//Requests the current server time, passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // Difference between Client and Server time
	
	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;


	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	void PollInit();

	void ShowReturnToMainMenu();

	void HandleMatchHasStarted(bool bTeamsMatch = false);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

	UFUNCTION(Client, Reliable)
	void ClientEliminationAnnouncement(APlayerState* Killer, APlayerState* Victim);

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamScores();
	FString GetInfoText(TArray<class AHAFPlayerState*>& Players);
	FString GetTeamsInfoText (class AHAFGameState* HAFGameState);

private:
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleGameMode();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UHAFInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UHAFAbilitySystemComponent> HAFAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UFUNCTION()
	UHAFAbilitySystemComponent* GetASC();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;
	
	FString GetWeaponTypeDisplayName(EWeaponType TypeOfWeapon);

	UPROPERTY()
	class AFillainHUD* FillainHUD;

	/**********************************
	****    RETURN TO MAIN MENU    ****
	**********************************/

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

	UPROPERTY()
	class UReturnToMainMenu* ReturnToMainMenu;

	bool bReturnToMainMenuOpen = false;

    UPROPERTY(meta = (AllowPrivateAccess = "true"))
    class AFillainCharacter* Fillain;

    UPROPERTY(meta = (AllowPrivateAccess = "true"))
    class AHAFPlayerState* State;

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	class AHAFGameMode* ModeBase;




	float MatchTime = 5400.f;
	float WarmupTime = 0.f;
	float LevelStartingTime = 0.f;
	float CooldownTime = 0.f;
	int32 CountdownInt;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UOverlayWidget* OverlayWidget;
	bool bInitializeScore = false;
	bool bInitializeSouls = false;
	bool bInitializeGold = false;
	bool bInitializeDefeats = false;
	bool bInitializeGrenades = false;
	bool bInitializeWeaponAmmo = false;
	bool bInitializeCarriedAmmo = false;

	
	float HUDScore;
	int32 HUDDefeats;
	int32 HUDGrenades;
	int32 HUDCarriedAmmo;
	int32 HUDWeaponAmmo;
	int32 HUDSouls;
	int32 HUDGold;

	int32 ThirtySecondsOnTheClock = 30;

	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;

	float PingAnimationRunningTime = 0.f;

	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 20.f;

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;






	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

public:
	FORCEINLINE AFillainCharacter* GetFillain() const { return Fillain; }
	FORCEINLINE AHAFPlayerState* GetHAFPlayerState() const { return State; }
	FORCEINLINE AHAFGameMode* GetHAFGameMode() const { return ModeBase; }
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE float GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE float GetLevelStartingTime() const { return LevelStartingTime; }
	FORCEINLINE float GetCooldownTime() const { return CooldownTime; }
	FORCEINLINE FName GetMatchState() const { return MatchState; }
	FORCEINLINE bool GetShowTeamScores() const { return bShowTeamScores; }
	FORCEINLINE float GetHighPingDuration() const { return HighPingDuration; }
	FORCEINLINE float GetHighPingThreshold() const { return HighPingThreshold; }
	FORCEINLINE float GetCheckPingFrequency() const { return CheckPingFrequency; }
	FORCEINLINE float GetHighPingRunningTime() const { return HighPingRunningTime; }

	



	
};