
// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/Melee/Sword.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/FillainCharacter.h"
#include "UI/PickupWidgetComponent.h"
#include "Components/WidgetComponent.h"  
#include "HeroesAndFillains/HeroesAndFillains.h"
#include "UI/PickupGearWidget.h"

ASword::ASword()  
{  
    // Create and setup SwordMesh with null check
    SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
    SetRootComponent(SwordMesh);
    SwordMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  
    SwordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  
}

void ASword::WeaponDropped()
{
    SetWeaponState(EWeaponState::EWS_Dropped);
    
    if (SwordMesh)
    {
        FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
        SwordMesh->DetachFromComponent(DetachRules);
    }
    
    SetOwner(nullptr);
    FillainOwnerCharacter = nullptr;
    FillainOwnerController = nullptr;
}

void ASword::ResetSword()
{
    AFillainCharacter* SwordWielder = Cast<AFillainCharacter>(GetOwner());
    if (SwordWielder)
    {
        SwordWielder->SetOverlappingWeapon(nullptr);
        SwordWielder->UnCrouch();
    }

    if (!HasAuthority()) return;

    if (SwordMesh)
    {
        FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
        SwordMesh->DetachFromComponent(DetachRules);
    }

    SetWeaponState(EWeaponState::EWS_Unclaimed);
    
    USphereComponent* AreaSpherePtr = GetAreaSphere();
    if (AreaSpherePtr)
    {
        AreaSpherePtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        AreaSpherePtr->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
    }

    SetOwner(nullptr);
    FillainOwnerCharacter = nullptr;
    FillainOwnerController = nullptr;
    SetActorTransform(InitialTransform);
}


void ASword::OnEquippedOneHanded()
{
    ShowPickupAndInfoWidgets(false);
    
    USphereComponent* AreaSpherePtr = GetAreaSphere();
    if (AreaSpherePtr)
    {
        AreaSpherePtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (SwordMesh)
    {
        SwordMesh->SetSimulatePhysics(false);
        SwordMesh->SetEnableGravity(false);
        SwordMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        SwordMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
    }

    EnableCustomDepth(false);
}

void ASword::OnEquippedTwoHanded()
{
    ShowPickupAndInfoWidgets(false);
    
    USphereComponent* AreaSpherePtr = GetAreaSphere();
    if (AreaSpherePtr)
    {
        AreaSpherePtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (SwordMesh)
    {
        SwordMesh->SetSimulatePhysics(false);
        SwordMesh->SetEnableGravity(false);
        SwordMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        SwordMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
    }

    EnableCustomDepth(false);
}

void ASword::OnDropped()
{
    if (HasAuthority())
    {
        if (AreaSphere)
        {
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            AreaSphere->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Overlap);
        }
        else
        {
            // UE_LOG(LogTemp, Warning, TEXT("AreaSphere is null in OnDropped"));
        }
    }

    if (SwordMesh)
    {
        SwordMesh->SetSimulatePhysics(true);
        SwordMesh->SetEnableGravity(true);
        SwordMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        SwordMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
        SwordMesh->SetCollisionResponseToChannel(ECC_PlayerCharacter, ECollisionResponse::ECR_Ignore);
        SwordMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
        
        SwordMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
        SwordMesh->MarkRenderStateDirty();
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("SwordMesh is null in OnDropped"));
    }
    
    EnableCustomDepth(true);
}


void ASword::BeginPlay()
{
    Super::BeginPlay();
    
    if (!IsValid(this))
    {
        // UE_LOG(LogTemp, Error, TEXT("Invalid Sword instance in BeginPlay"));
        return;
    }
    
    InitialTransform = GetActorTransform();

    // Cache and validate widget component
    UWidgetComponent* WidgetComponent = GetPickupGearWidgetComponent();
    if (!WidgetComponent)
    {
        // UE_LOG(LogTemp, Warning, TEXT("PickupGearWidgetComponent is null"));
        return;
    }

    // Get and validate user widget
    UUserWidget* UserWidget = WidgetComponent->GetUserWidgetObject();
    if (!UserWidget)
    {
        // UE_LOG(LogTemp, Warning, TEXT("UserWidget is null"));
        return;
    }

    // Cast and set visibility
    if (UPickupGearWidget* PickupWidget = Cast<UPickupGearWidget>(UserWidget))
    {
        PickupWidget->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("Failed to cast to PickupGearWidget"));
    }
    
    // Verify critical components
    if (!SwordMesh)
    {
        // UE_LOG(LogTemp, Error, TEXT("SwordMesh is null in BeginPlay"));
    }
    
    if (!RootComponent)
    {
        // UE_LOG(LogTemp, Error, TEXT("RootComponent is null in BeginPlay"));
    }
}
