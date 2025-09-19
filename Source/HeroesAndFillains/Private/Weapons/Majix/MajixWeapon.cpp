// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Majix/MajixWeapon.h"


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

