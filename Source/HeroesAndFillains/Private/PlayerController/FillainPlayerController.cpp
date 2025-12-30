// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/FillainPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "UI/FillainHUD.h"
#include "UI/CharacterOverlayFixed.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Characters/FillainCharacter.h"
#include "PlayerState/HAFPlayerState.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/WeaponTypes.h"
#include "Weapons/WeaponBase.h"
#include "UObject/EnumProperty.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "HAFComponents/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/HaFGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Announcement.h"
#include "GameStates/HAFGameState.h"
#include "Components/Image.h"
#include "Components/InputComponent.h"
#include "AbilitySystem/HAFAbilitySystemComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/ReturnToMainMenu.h"
#include "UI/PlayerChat.h"
#include "Components/EditableText.h"
#include "HeroesAndFillains/HeroesAndFillainsTypes/Announcement.h"
#include "Input/HAFInputComponent.h"
#include "HAFGameplayTags.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Components/DecalComponent.h"
#include "Enemies/EnemyBase.h"
#include "GameMode/HybridGameMode.h"
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "UI/Widgets/DamageTextComponent.h"
#include "Interfaces/EnemyInterface.h"
#include "Weapons/Ranged/RangedWeapon.h"
#include "Weapons/Melee/MeleeWeapon.h"
#include "UI/WidgetControllers/HAFWidgetController.h"
#include "Interfaces/EnemyInterface.h"
#include "Interfaces/InterfaceHelpers.h"
#include "Items/MagicCircle.h"


AFillainPlayerController::AFillainPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	MatchTimeElapsedTime = 0.f;

	MatchCountdownColor = FLinearColor(0.10033f, 8.602301f, 10.0f, 10.0f);
	MatchCountdownBlinkingColor = FLinearColor(10.0f, 0.0f, 0.491076f, 10.0f);

	bIsMatchCountdownVisible = true;
	bReplicates = true;
}



void AFillainPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	UpdateMagicCircleLocation();
}


void AFillainPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FHAFGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->UnHighlightActor();
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_BlockAllExceptCharacters : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (LastActor != ThisActor)
	{
		// End hover on the old one
		if (LastActor)
		{
			LastActor->OnHoverEnd();
			LastActor->UnHighlightActor();
		}

		if (ThisActor)
		{
			ThisActor->OnHoverStart();
			ThisActor->HighlightActor();
		}
	}
}

void AFillainPlayerController::BroadcastElimination(APlayerState* Killer, APlayerState* Victim)
{
	ClientEliminationAnnouncement(Killer, Victim);
}

void AFillainPlayerController::ToggleInputChatBox()
{
	if (PlayerChatWidget && PlayerChatWidget->InputTextBox)
	{
		if (PlayerChatWidget->InputTextBox->GetVisibility() == ESlateVisibility::Collapsed)
		{
			PlayerChatWidget->InputTextBox->SetVisibility(ESlateVisibility::Visible);
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
			SetShowMouseCursor(true);
		}
		else
		{
			PlayerChatWidget->InputTextBox->SetVisibility(ESlateVisibility::Collapsed);
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
			SetShowMouseCursor(false);
		}
	}
}

void AFillainPlayerController::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod != ETextCommit::OnEnter) return;

	PlayerState = PlayerState == nullptr ? TObjectPtr<APlayerState>(GetPlayerState<APlayerState>()) : PlayerState;
	FString PlayerName("");
	if (PlayerState)
	{
		PlayerName = PlayerState->GetPlayerName();
	}
	if (PlayerChatWidget)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Here"));
		if (!Text.IsEmpty())
		{
			ServerSetText(Text.ToString(), PlayerName);
		}
		PlayerChatWidget->InputTextBox->SetText(FText());
		PlayerChatWidget->InputTextBox->SetVisibility(ESlateVisibility::Collapsed);
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
	}
}

void AFillainPlayerController::ClientSetText_Implementation(const FString& Text, const FString& PlayerName)
{
	PlayerState = PlayerState == nullptr ? TObjectPtr<APlayerState>(GetPlayerState<APlayerState>()) : PlayerState;
	if (PlayerChatWidget && PlayerState)
	{
		if (PlayerName == PlayerState->GetPlayerName())
		{
			PlayerChatWidget->SetChatText(Text, "You");
		}
		else
		{
			PlayerChatWidget->SetChatText(Text, PlayerName);
		}
	}
}


void AFillainPlayerController::ServerSetText_Implementation(const FString& Text, const FString& PlayerName)
{
	GameMode = GameMode == nullptr ? Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(this)) : GameMode;
	if (GameMode)
	{
		GameMode->SendChat(Text, PlayerName);
	}
}


void AFillainPlayerController::ClientEliminationAnnouncement_Implementation(APlayerState* Killer, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Killer && Victim && Self)
	{
		FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
		if (FillainHUD)
		{
			if (Killer == Self && Victim != Self)
			{
				FillainHUD->AddEliminationAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Victim == Self && Killer != Self)
			{
				FillainHUD->AddEliminationAnnouncement(Killer->GetPlayerName(), "you");
				return;
			}
			if (Killer == Victim && Killer == Self)
			{
				FillainHUD->AddEliminationAnnouncement("You", "yourself");
				return;
			}
			if (Killer == Victim && Killer != Self)
			{
				FillainHUD->AddEliminationAnnouncement(Killer->GetPlayerName(), "themselves");
				return;
			}
			FillainHUD->AddEliminationAnnouncement(Killer->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}

void AFillainPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFillainPlayerController, MatchState);
	DOREPLIFETIME(AFillainPlayerController, bShowTeamScores);
}


void AFillainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DebugCursorTrace();  // <— TEMP: remove later
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);

	/************************
	*** OPTIONAL CHALLENGE **
	************************/
	
	MatchTimeElapsedTime += DeltaTime;
	if (static_cast<int32>(ThirtySecondsOnTheClock) >= static_cast<int32>(CountdownInt) && CountdownInt >= 0)

	{
		if (MatchState == MatchState::InProgress)
		{
			UpdateMatchCountdownColor();
		}
		if (FMath::Frac(MatchTimeElapsedTime) >= 0.5f && MatchState == MatchState::InProgress)
		{
			ToggleMatchCountdownVisibility();
		}
		else if (MatchTimeElapsedTime >= MatchTime)
		{
			FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
			bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed  && FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText;
			const FString TimesUpText = TEXT("TIME'S UP!!");
			if (bIsHUDValid)
			{
				FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetText(FText::FromString(TimesUpText));
			}
		}
	}
}

void AFillainPlayerController::DebugCursorTrace()
{
	if (!IsLocalController()) return;

	// Make absolutely sure mouse is configured
	bShowMouseCursor       = true;
	bEnableClickEvents     = true;
	bEnableMouseOverEvents = true;

	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, /*bTraceComplex*/ false, Hit);

	//if (bHit && Hit.bBlockingHit && Hit.GetActor())
	//{
		//UE_LOG(LogTemp, Warning, TEXT("[CursorHit] %s @ %s"),
	//		*Hit.GetActor()->GetName(),
	//		*Hit.Location.ToString());
	//}
	//else
	//{
		// Print WHY it failed so we know the path
	//	UE_LOG(LogTemp, Warning, TEXT("[CursorHit] NO HIT  (bHit=%d, blocking=%d)  Cursor over: %s"),
	//		bHit ? 1 : 0,
	//		Hit.bBlockingHit ? 1 : 0,
	//		bShowMouseCursor ? TEXT("World/Viewport") : TEXT("Unknown"));
	//}
}

void AFillainPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
}

void AFillainPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AFillainPlayerController::UpdateMagicCircleLocation()
{
	if (!IsValid(MagicCircle))
	{
		return;
	}	
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}


void AFillainPlayerController::CheckPing(float DeltaTime)
{
	if (HasAuthority()) return;

    HighPingRunningTime += DeltaTime;
    if (HighPingRunningTime > CheckPingFrequency)
    {
        PlayerState = PlayerState == nullptr ? TObjectPtr<APlayerState>(GetPlayerState<APlayerState>()) : PlayerState;
        if (PlayerState)
        {
			// UE_LOG(LogTemp, Warning, TEXT("PlayerState->GetPingInMilliseconds() * 4 : %d"), static_cast<int32>(PlayerState->GetPingInMilliseconds() * 4));
			float PingInMs = PlayerState->GetPingInMilliseconds(); // Assuming GetPingInMilliseconds() is a method that returns the ping in milliseconds
            if (PingInMs * 4 > HighPingThreshold) // ping is compressed; it's actually ping / 4
            {
                HighPingWarning();
                PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
            }
        }
        HighPingRunningTime = 0.f;
    }
    bool bIsHighPingAnimationPlaying =
        FillainHUD &&
        FillainHUD->CharacterOverlayWidgetFixed &&
        FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation &&
        FillainHUD->CharacterOverlayWidgetFixed->IsAnimationPlaying(FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation);
    if (bIsHighPingAnimationPlaying)
    {
        PingAnimationRunningTime += DeltaTime;
        if (PingAnimationRunningTime > HighPingDuration)
        {
            StopHighPingWarning();
        }
    }
    else
    {
        StopHighPingWarning();
    }
}



// Is the ping too high?
void AFillainPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void AFillainPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AFillainPlayerController::HighPingWarning()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->HighPingImage && FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation;
	if (bIsHUDValid)
	{
		FillainHUD->CharacterOverlayWidgetFixed->HighPingImage->SetOpacity(1.f);
		FillainHUD->CharacterOverlayWidgetFixed->PlayAnimation(FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation, 0.f, 5);
	}
}

void AFillainPlayerController::StopHighPingWarning()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->HighPingImage && FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation;
	if (bIsHUDValid)
	{
		FillainHUD->CharacterOverlayWidgetFixed->HighPingImage->SetOpacity(0.f);
		if (FillainHUD->CharacterOverlayWidgetFixed->IsAnimationPlaying(FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation))
		{
			FillainHUD->CharacterOverlayWidgetFixed->StopAnimation(FillainHUD->CharacterOverlayWidgetFixed->HighPingAnimation);
		}
	}
}



void AFillainPlayerController::ServerCheckMatchState_Implementation()
{
	AHAFGameMode* HAFGameMode = Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(this));
	if (HAFGameMode)
	{
		WarmupTime = HAFGameMode->WarmupTime;
		MatchTime = HAFGameMode->MatchTime;
		CooldownTime = HAFGameMode->CooldownTime;
		LevelStartingTime = HAFGameMode->LevelStartingTime;
		MatchState = HAFGameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);

		if (FillainHUD && MatchState == MatchState::Cooldown)
		{
			HandleCooldown();
		}
	}
}

void AFillainPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (FillainHUD && MatchState == MatchState::WaitingToStart)
	{
		FillainHUD->AddAnnouncement();
	}
}

void AFillainPlayerController::SetHUDScore(float Score)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->ScoreAmount;
	if (bIsHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		FillainHUD->CharacterOverlayWidgetFixed->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
	}
}

void AFillainPlayerController::SetHUDSoulsCount(int32 SoulsCountGathered)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->SoulsCountText;

	if (bIsHUDValid)
	{
		FString SoulsText = FString::Printf(TEXT("%d"), SoulsCountGathered);
		FillainHUD->CharacterOverlayWidgetFixed->SoulsCountText->SetText(FText::FromString(SoulsText));

	}
	else
	{
		bInitializeSouls = true;
		HUDSouls = SoulsCountGathered;

	}
}

void AFillainPlayerController::SetHUDDefeats(int32 Defeats)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->DefeatsAmount;
	if (bIsHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		FillainHUD->CharacterOverlayWidgetFixed->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}
void AFillainPlayerController::SetHUDGoldCount(int32 GoldAmountOwned)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->GoldCountText;
	if (bIsHUDValid)
	{
		FString GoldText = FString::Printf(TEXT("%d"), GoldAmountOwned);
		FillainHUD->CharacterOverlayWidgetFixed->GoldCountText->SetText(FText::FromString(GoldText));
	}
	else
	{
		bInitializeGold = true;
		HUDGold = GoldAmountOwned;
	}
}

void AFillainPlayerController::SetHUDWeaponAmmo(int32 WeaponAmmo)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->WeaponAmmoAmount;
	if (bIsHUDValid)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), WeaponAmmo);
		FillainHUD->CharacterOverlayWidgetFixed->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = WeaponAmmo;
	}
}

void AFillainPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->CarriedAmmoAmount;
	if (bIsHUDValid)
	{
		FString CarriedAmmoText = FString::Printf(TEXT("%d"), CarriedAmmo);
		FillainHUD->CharacterOverlayWidgetFixed->CarriedAmmoAmount->SetText(FText::FromString(CarriedAmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = CarriedAmmo;
	}
}

void AFillainPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText;
	if (bIsHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AFillainPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->Announcement && FillainHUD->Announcement->WarmupTime;
	if (bIsHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			FillainHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		FillainHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AFillainPlayerController::SetHUDGrenades(int32 Grenades)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->GrenadesText;
	if (bIsHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		FillainHUD->CharacterOverlayWidgetFixed->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}

void AFillainPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AActor* TargetAvatarActor, bool bBlockedHit, bool bCriticalHit)
{ 
	if (IsValid(TargetAvatarActor) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetAvatarActor, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetAvatarActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

bool AFillainPlayerController::IsHUDReady() const
{
	return FillainHUD &&
		  FillainHUD->CharacterOverlayWidgetFixed &&
		  FillainHUD->CharacterOverlayWidgetFixed->SoulsCountText &&
		  FillainHUD->CharacterOverlayWidgetFixed->DefeatsAmount &&
		  FillainHUD->CharacterOverlayWidgetFixed->GoldCountText &&
		  FillainHUD->CharacterOverlayWidgetFixed->CarriedAmmoAmount &&
		  FillainHUD->CharacterOverlayWidgetFixed->ScoreAmount &&
		  FillainHUD->CharacterOverlayWidgetFixed->WeaponAmmoAmount;	
}

bool AFillainPlayerController::GetClickHit(FHitResult& OutHit) const
{
	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, /*bTraceComplex*/ true, Hit);
	if (bHit && Hit.bBlockingHit)
	{
		OutHit = Hit;
		return true;
	}
	return false;
}

void AFillainPlayerController::HideTeamScores()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->RedTeamScore && FillainHUD->CharacterOverlayWidgetFixed->BlueTeamScore && FillainHUD->CharacterOverlayWidgetFixed->ScoreSpacerText;
	if (bIsHUDValid)
	{
		FillainHUD->CharacterOverlayWidgetFixed->RedTeamScore->SetText(FText());
		FillainHUD->CharacterOverlayWidgetFixed->BlueTeamScore->SetText(FText());
		FillainHUD->CharacterOverlayWidgetFixed->ScoreSpacerText->SetText(FText());
	}
}

void AFillainPlayerController::InitTeamScores()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->RedTeamScore && FillainHUD->CharacterOverlayWidgetFixed->BlueTeamScore && FillainHUD->CharacterOverlayWidgetFixed->ScoreSpacerText;
	if (bIsHUDValid)
	{
		FString Zero("0");
		FString Spacer("|");

		FillainHUD->CharacterOverlayWidgetFixed->RedTeamScore->SetText(FText::FromString(Zero));
		FillainHUD->CharacterOverlayWidgetFixed->BlueTeamScore->SetText(FText::FromString(Zero));
		FillainHUD->CharacterOverlayWidgetFixed->ScoreSpacerText->SetText(FText::FromString(Spacer));
	}
}

void AFillainPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->RedTeamScore;;
	if (bIsHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT(" % d"), RedScore);
		FillainHUD->CharacterOverlayWidgetFixed->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void AFillainPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;

	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->BlueTeamScore;;
	if (bIsHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT(" % d"), BlueScore);
		FillainHUD->CharacterOverlayWidgetFixed->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void AFillainPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	/*
	if (HasAuthority())
	{
		Mode = Mode == nullptr ? Cast<AHAFGameMode>(UGameplayStatics::GetGameMode(this)) : Mode;
		if (Mode)
		{
			SecondsLeft = FMath::CeilToInt(Mode->GetCountdownTime() + LevelStartingTime);
		}
	} */

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void AFillainPlayerController::PollInit()
{
	if (CharacterOverlayFixed == nullptr)
	{
		if (FillainHUD && FillainHUD->CharacterOverlayWidgetFixed)
		{
			CharacterOverlayFixed = FillainHUD->CharacterOverlayWidgetFixed;
			if (CharacterOverlayFixed)
			{
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeGrenades) SetHUDGrenades(HUDGrenades);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

				AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(GetPawn());
				if (FillainCharacter && FillainCharacter->GetCombatComponent())
				{
					if (bInitializeGrenades)
					{
						SetHUDGrenades(FillainCharacter->GetCombatComponent()->GetGrenades());
					}
				}
			}
		}
	}
}

void AFillainPlayerController::ActivateByTag(FGameplayTag Tag)
{
	if (!Tag.IsValid()) return;

	UAbilitySystemComponent* ASystemComponent = GetASC(); // your helper in this file
	if (!ASystemComponent || !IsValid(ASystemComponent)) return;

	APawn* P = GetPawn();
	if (!IsValid(P) || ASystemComponent->GetAvatarActor() != P) return;

	FGameplayTagContainer TagPot;
	TagPot.AddTag(Tag);

	ASystemComponent->TryActivateAbilitiesByTag(TagPot); 
}

void AFillainPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}

void AFillainPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AFillainPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;	
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AFillainPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AFillainPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AFillainPlayerController::OnMatchStateSet(FName NewState,  bool bTeamsMatch)
{
	MatchState = NewState;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AFillainPlayerController::OnRep_MatchState()
{

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AFillainPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if (HasAuthority()) bShowTeamScores = bTeamsMatch;
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	if (FillainHUD)
	{
		if (FillainHUD->CharacterOverlayWidgetFixed == nullptr) FillainHUD->AddCharacterOverlayFixed();
		if (FillainHUD->Announcement)
		{
			FillainHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (!HasAuthority()) return;
		if (bTeamsMatch)
		{
			InitTeamScores();
		}
		else
		{
			HideTeamScores();
		}
	}
}

void AFillainPlayerController::HandleCooldown()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	if (FillainHUD)
	{
		FillainHUD->CharacterOverlayWidgetFixed->RemoveFromParent();;
		bool bHUDValid = FillainHUD->Announcement && FillainHUD->Announcement->AnnouncementText && FillainHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			FillainHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = Announcement::NewMatchStartsIn;
			FillainHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AHAFGameState* HAFGameState = Cast<AHAFGameState>(UGameplayStatics::GetGameState(this));
			AHAFPlayerState* HAFPlayerState = GetPlayerState<AHAFPlayerState>();
			if (HAFGameState && HAFPlayerState)
			{
				TArray<AHAFPlayerState*> TopPlayers = HAFGameState->TopScoringPlayers;
				FString InfoTextString = bShowTeamScores ? GetTeamsInfoText(HAFGameState) : GetInfoText(TopPlayers);
				
				FillainHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	AFillainCharacter* FillainCharacter = Cast<AFillainCharacter>(GetPawn());
	if (FillainCharacter && FillainCharacter->GetCombatComponent())
	{
		FillainCharacter->bDisableGameplay = true;
		FillainCharacter->GetCombatComponent()->FireButtonPressed(false);
	}
}

FString AFillainPlayerController::GetInfoText(TArray<class AHAFPlayerState*>& Players) 
{
	AHAFPlayerState* HAFPlayerState = GetPlayerState<AHAFPlayerState>();
	if (HAFPlayerState == nullptr) return FString();

	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;

	}
	else if (Players.Num() == 1 && Players[0] == HAFPlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if (Players.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("The Winner Is: \n%s"), *Players[0]->GetPlayerName());
	}
	else if (Players.Num() > 1)
	{
		InfoTextString = Announcement::WeHaveATie;
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n Can't We Kill Both? No? Damn!"), *TiedPlayer->GetPlayerName()));
		}
	}
	else if (Players.Num() > 1 && Players.Contains(HAFPlayerState))
	{
		InfoTextString = FString("YOU Tied for the Win, Alongside: \n");
		Players.Remove(HAFPlayerState);
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n Fight to the Death!"), *TiedPlayer->GetPlayerName()));
		}
	}
	return InfoTextString;;
}

FString AFillainPlayerController::GetTeamsInfoText(AHAFGameState* HAFGameState)
{
	if (HAFGameState == nullptr) return FString();
	FString InfoTextString;

	const int32 RedTeamScore = HAFGameState->RedTeamScore;
	const int32 BlueTeamScore = HAFGameState->BlueTeamScore;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin);
		InfoTextString.Append(Announcement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Announcement::BlueTeam);
		InfoTextString.Append(TEXT("\n"));
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Announcement::RedTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d \n"), *Announcement::RedTeam, RedTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d \n"), *Announcement::BlueTeam, BlueTeamScore));
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoTextString = Announcement::BlueTeamWins;
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d \n"), *Announcement::BlueTeam, BlueTeamScore));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d \n"), *Announcement::RedTeam, RedTeamScore));
	}

	return InfoTextString;
}

void AFillainPlayerController::ToggleGameMode()
{
	if (AHybridGameMode* GM = GetWorld()->GetAuthGameMode<AHybridGameMode>())
	{
		// Flip between PvE and PvP
		if (GM->CurrentMode == EGameModeType::EGMT_PvE)
		{
			GM->SetGameMode(EGameModeType::EGMT_PvP);
		}
		else
		{
			GM->SetGameMode(EGameModeType::EGMT_PvE);
		}
	}
}

void AFillainPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FHAFGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AFillainPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FHAFGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if (!InputTag.MatchesTagExact(FHAFGameplayTags::Get().InputTag_LeftMouseButtonOrGamepadShoulder))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	
}


void AFillainPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FHAFGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}
	// --- Early return if not LMB/GamepadShoulder ---
	if (!InputTag.MatchesTagExact(FHAFGameplayTags::Get().InputTag_LeftMouseButtonOrGamepadShoulder))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	// --- If we're targeting or Shift key is down ---
	if (bShiftKeyDown)
	{
		if (GetASC())
		{
			// 🔒 Prevent spamming FireBolt (or any active ability)
			FGameplayTag ActiveTag = FGameplayTag::RequestGameplayTag(TEXT("Abilities.FireBolt.Active"));
			if (GetASC()->HasMatchingGameplayTag(ActiveTag))
			{
				// Optional debug log so you know it’s blocking correctly
				UE_LOG(LogTemp, Warning, TEXT("Blocked reactivation: %s already active"), *InputTag.ToString());
				return;
			}

			// ✅ If not active, try to activate normally
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
}

UHAFAbilitySystemComponent* AFillainPlayerController::GetASC()
{
	if (HAFAbilitySystemComponent == nullptr)
	{
		HAFAbilitySystemComponent = Cast<UHAFAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return HAFAbilitySystemComponent;
}

void AFillainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(FillainMappingContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}
void AFillainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UHAFInputComponent* HAFInputComponent = CastChecked<UHAFInputComponent>(InputComponent);
	HAFInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AFillainPlayerController::ShiftPressed);
	HAFInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AFillainPlayerController::ShiftReleased);
	HAFInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);;
	HAFInputComponent->BindAction(ToggleGameModeAction, ETriggerEvent::Triggered, this, &AFillainPlayerController::ToggleGameMode);

}

void AFillainPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}

/************************************************************************
**   I added the following functions to complete optional challenges   **
**   in the course, and they're proven to work correctly.			   **
************************************************************************/

void AFillainPlayerController::UpdateMatchCountdownColor()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText;
	if (bIsHUDValid)
	{
		if (MatchTimeElapsedTime >= (MatchTime - ThirtySecondsOnTheClock) && MatchTimeElapsedTime < MatchTime)
		{
			if (FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText)
			{
				FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetColorAndOpacity(MatchCountdownBlinkingColor);
			}
		}
		else if (MatchTimeElapsedTime < (MatchTime - ThirtySecondsOnTheClock))
		{
			if (FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText)
			{
				FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetColorAndOpacity(MatchCountdownColor);
			}
		}
	}
}

void AFillainPlayerController::ToggleMatchCountdownVisibility()
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText;
	if (bIsHUDValid) 
	{
		FTimerHandle MatchCountdownTimer;
		GetWorldTimerManager().SetTimer(MatchCountdownTimer, [&]()
			{FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetVisibility(ESlateVisibility::Hidden); }, .5f, false);
		FillainHUD->CharacterOverlayWidgetFixed->MatchCountdownText->SetVisibility(ESlateVisibility::Visible);
	}
}

FString AFillainPlayerController::GetWeaponTypeDisplayName(EWeaponType TypeOfWeapon)
{
	if (EquippedWeapon)
	{
		const UEnum* EnumPtr = StaticEnum<EWeaponType>();
		if (!EnumPtr) return FString("");

		return EnumPtr->GetDisplayNameTextByValue((int64)TypeOfWeapon).ToString();
	}
	else return FString("Unknown");
}

void AFillainPlayerController::SetHUDWeaponType(APawn* InPawn)
{
	if (!InPawn) return;

	if (!FillainHUD)
	{
		FillainHUD = Cast<AFillainHUD>(GetHUD());
	}

	AFillainCharacter* FCharacter = Cast<AFillainCharacter>(InPawn);
	if (!FCharacter) return;

	if (!EquippedWeapon)
	{
		EquippedWeapon = Cast<AWeaponBase>(FCharacter->GetEquippedWeapon());
	}

	// Now assign specific subclass pointers
	EquippedRangedWeapon = Cast<ARangedWeapon>(EquippedWeapon);
	EquippedMeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon);

	const bool bIsHUDValid =
		FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->WeaponTypeText;

	if (bIsHUDValid)
	{
		FString WeaponTypeName;

		if (EquippedWeapon)
		{
			EWeaponType NoMeleeType = EWeaponType::EWT_None;
			WeaponTypeName = GetWeaponTypeDisplayName(EquippedWeapon->GetWeaponType());
		}
		FillainHUD->CharacterOverlayWidgetFixed->WeaponTypeText->SetText(FText::FromString(WeaponTypeName));
	}
}

void AFillainPlayerController::ShowEliminationUI(FString Victim, FString Killer, FText Message)
{
	// UE_LOG(LogTemp, Warning, TEXT("ShowEliminationUI called for Victim: %s, Killer: %s"), *Victim, *Killer);

	if (!FillainHUD || !FillainHUD->CharacterOverlayWidgetFixed) return;

	UCharacterOverlayFixed* Overlay = FillainHUD->CharacterOverlayWidgetFixed;

	// Set text first
	Overlay->VictimNameText->SetText(FText::FromString(Victim));
	Overlay->KillerNameText->SetText(FText::FromString(Killer));
	Overlay->EliminationMessageText->SetText(Message);

	// Make visible
	Overlay->VictimNameText->SetVisibility(ESlateVisibility::Visible);
	Overlay->KillerNameText->SetVisibility(ESlateVisibility::Visible);
	Overlay->EliminationMessageText->SetVisibility(ESlateVisibility::Visible);

	// Delay animation slightly to ensure text is updated (optional test)
/*	FTimerHandle PlayAnimHandle;
	GetWorldTimerManager().SetTimer(PlayAnimHandle, [Overlay]() { */
		if (Overlay->EliminationAnimation)
		{
			Overlay->PlayAnimation(Overlay->EliminationAnimation);
		}
/*	}, 0.01f, false);  // Slight delay (~1 frame) */
	
/*	// Clear and hide after 3 seconds
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this, Overlay]() {
		Overlay->VictimNameText->SetText(FText::GetEmpty());
		Overlay->KillerNameText->SetText(FText::GetEmpty());
		Overlay->EliminationMessageText->SetText(FText::GetEmpty());

		Overlay->VictimNameText->SetVisibility(ESlateVisibility::Hidden);
		Overlay->KillerNameText->SetVisibility(ESlateVisibility::Hidden);
		Overlay->EliminationMessageText->SetVisibility(ESlateVisibility::Hidden);
	}, 4.f, false); */
}

void AFillainPlayerController::UpdateEliminationMessageForPvE(AFillainPlayerController* VictimPlayerController, AController* InstigatorController)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->EliminationMessageText && FillainHUD->CharacterOverlayWidgetFixed->VictimNameText && FillainHUD->CharacterOverlayWidgetFixed->KillerNameText && FillainHUD->CharacterOverlayWidgetFixed->EliminationAnimation;
	if (FillainHUD && bIsHUDValid && VictimPlayerController)
	{
		FString NameOfVictim = VictimPlayerController->PlayerState->GetPlayerName();
		AEnemyBase* KillerVillain = Cast<AEnemyBase>(InstigatorController->GetPawn());
		FString NameOfKiller = KillerVillain->GetEnemyDisplayName().ToString();
		FText EliminationMessage = FText::FromString(TEXT("Was Eliminated By"));
		FString VictimName = NameOfVictim;
		FString KillerName = NameOfKiller;
		ShowEliminationUI(VictimName, KillerName, EliminationMessage);
		
	}
}

void AFillainPlayerController::UpdateEliminationMessageForPvP(AFillainPlayerController* KillerPlayerController, AFillainPlayerController* VictimPlayerController)
{
	FillainHUD = FillainHUD == nullptr ? Cast<AFillainHUD>(GetHUD()) : FillainHUD;
	bool bIsHUDValid = FillainHUD && FillainHUD->CharacterOverlayWidgetFixed && FillainHUD->CharacterOverlayWidgetFixed->EliminationMessageText && FillainHUD->CharacterOverlayWidgetFixed->VictimNameText && FillainHUD->CharacterOverlayWidgetFixed->KillerNameText;

	if (FillainHUD && bIsHUDValid && VictimPlayerController && KillerPlayerController)
	{
		FString NameOfVictim = VictimPlayerController->PlayerState->GetPlayerName();
		FString NameOfKiller = KillerPlayerController->PlayerState->GetPlayerName();
		FText EliminationMessage = FText::FromString(TEXT("Was Eliminated By"));
		FString VictimName = FString::Printf(TEXT("%s"), *NameOfVictim);
		FString KillerName = FString::Printf(TEXT("%s"), *NameOfKiller);
		ShowEliminationUI(VictimName, KillerName, EliminationMessage);
	}
}

void AFillainPlayerController::InitializeHUDEliminationMessage(AFillainPlayerController* KillerPlayerController, AFillainPlayerController* VictimPlayerController, AController* InstigatorController)
{
	if (!KillerPlayerController && InstigatorController)
	{
		UpdateEliminationMessageForPvE(VictimPlayerController, InstigatorController);	
	}
	if (KillerPlayerController && VictimPlayerController)
	{
		UpdateEliminationMessageForPvP(KillerPlayerController, VictimPlayerController);
	}
}

void AFillainPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitOverlayIfNeeded();
}
void AFillainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	InitOverlayIfNeeded(); // covers timing where Pawn arrives after PS

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(InPawn))
	{
		AbilitySystemComponent = ASI->GetAbilitySystemComponent();
	}
	else
	{
		AbilitySystemComponent = nullptr;
	}
}

void AFillainPlayerController::InitOverlayIfNeeded()
{
	if (!IsLocalController() || bOverlayInitialized) return;

	AFillainCharacter* Char = Cast<AFillainCharacter>(GetPawn());
	if (!Char) { GetWorldTimerManager().SetTimerForNextTick(this, &AFillainPlayerController::InitOverlayIfNeeded); return; }

	// Ensure ASC is initialized on the client
	Char->InitASC();
	Char->InitializeAbilityActorInfo();

	if (AFillainHUD* HUD = Cast<AFillainHUD>(GetHUD()))
	{
		HUD->InitializeOverlay(
		this,
		GetPlayerState<APlayerState>(),
		Char->GetAbilitySystemComponent(),
		Char->GetAttributeSet() // your UHAFAttributeSet*
		);
		bOverlayInitialized = true;
	}
}
	