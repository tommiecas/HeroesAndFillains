#include "UI/Widgets/EnemyAttributeMenuWidget.h"
#include "Interfaces/EnemyAttributeMenuWidgetControllerInterface.h"
#include "TimerManager.h"

void UEnemyAttributeMenuWidget::SetWidgetController(UObject* InController)
{
	// ✅ Just call Super — it handles setting + propagation + event firing
	Super::SetWidgetController(InController);
}

void UEnemyAttributeMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Start hidden
	SetVisibility(ESlateVisibility::Hidden);
	SetRenderOpacity(0.f);
}

void UEnemyAttributeMenuWidget::ShowTemporarily(float Duration)
{
	SetVisibility(ESlateVisibility::Visible);
	SetRenderOpacity(1.f);

	GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);

	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this]()
	{
		const float FadeDuration = 0.5f;
		const float Step = 0.05f;
		float TimeElapsed = 0.f;

		TSharedPtr<FTimerHandle> SafeHandle = MakeShared<FTimerHandle>();
		GetWorld()->GetTimerManager().SetTimer(*SafeHandle,
			FTimerDelegate::CreateLambda([this, SafeHandle, &TimeElapsed, FadeDuration, Step]()
			{
				TimeElapsed += Step;
				float Alpha = 1.f - FMath::Clamp(TimeElapsed / FadeDuration, 0.f, 1.f);
				SetRenderOpacity(Alpha);

				if (Alpha <= 0.f)
				{
					SetVisibility(ESlateVisibility::Hidden);
					if (UWorld* World = GetWorld())
					{
						World->GetTimerManager().ClearTimer(*SafeHandle);
					}
				}
			}),
			Step, true);
	});

	GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, TimerDel, Duration, false);
}

void UEnemyAttributeMenuWidget::FadeIn(float Duration)
{
	SetVisibility(ESlateVisibility::Visible);
	bIsFading = true;
	bFadeIn = true;
	FadeTime = 0.f;
	FadeDuration = Duration;
	SetRenderOpacity(0.f);
}

void UEnemyAttributeMenuWidget::FadeOut(float Duration)
{
	bIsFading = true;
	bFadeIn = false;
	FadeTime = 0.f;
	FadeDuration = Duration;
}

void UEnemyAttributeMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Super::NativeTick(MyGeometry, InDeltaTime);

	if (GetVisibility() == ESlateVisibility::Visible)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			FVector2D MousePos;
			if (PC->GetMousePosition(MousePos.X, MousePos.Y))
			{
				const FVector2D Offset(20.f, 20.f);
				SetPositionInViewport(MousePos + Offset, true);
			}
		}
	}
	
	if (bIsFading)
	{
		FadeTime += InDeltaTime;
		const float Alpha = FMath::Clamp(FadeTime / FadeDuration, 0.f, 1.f);

		if (bFadeIn)
		{
			SetRenderOpacity(Alpha);
			if (Alpha >= 1.f)
			{
				bIsFading = false;
			}
		}
		else
		{
			SetRenderOpacity(1.f - Alpha);
			if (Alpha >= 1.f)
			{
				bIsFading = false;
				SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UEnemyAttributeMenuWidget::WidgetControllerSet_Implementation()
{
	// Optional: you can add logging or setup logic here
	UE_LOG(LogTemp, Log, TEXT("[%s] WidgetControllerSet_Implementation called"), *GetName());
}
