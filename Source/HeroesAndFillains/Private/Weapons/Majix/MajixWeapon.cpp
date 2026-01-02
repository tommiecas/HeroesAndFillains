// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/MajixWeapon.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


AMajixWeapon::AMajixWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	
}

void AMajixWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMajixWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

