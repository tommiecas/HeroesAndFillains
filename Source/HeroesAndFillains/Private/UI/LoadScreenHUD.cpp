// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModels/MVVM_LoadScreen.h"
#include "UI/Widgets/LoadScreenWidget.h"

void ALoadScreenHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!LoadScreenViewModel && LoadScreenViewModelClass)
	{
		LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	}
}

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel->InitializeLoadSlots();	
	UE_LOG(LogTemp, Warning, TEXT("VM valid pre-widget: %s"),
		LoadScreenViewModel ? TEXT("YES") : TEXT("NO"));

	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	LoadScreenWidget->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
}
