// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DeadHaulHUD.h"
#include "HUD/HotbarWidget.h"
#include "HUD/ScrapValueWidget.h"
#include "HUD/ReticleWidget.h"
#include "Characters/DeadHaulCharacter.h"
#include "Inventory/InventoryComponent.h"

void ADeadHaulHUD::BeginPlay()
{
    Super::BeginPlay();

    ADeadHaulCharacter* Character = Cast<ADeadHaulCharacter>(GetOwningPawn());
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Failed to get Character!"));
        return;
    }

    UInventoryComponent* Inventory = Character->GetInventoryComponent();
    if (!Inventory)
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Failed to get InventoryComponent!"));
        return;
    }

    //hotbar
    if (HotbarWidgetClass)
    {
        HotbarWidget = CreateWidget<UHotbarWidget>(GetWorld(), HotbarWidgetClass);
        if (HotbarWidget)
        {
            HotbarWidget->AddToViewport();
            HotbarWidget->InitializeHotbar(Inventory);
            UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Hotbar initialized successfully!"));
        }
    }

    //scrap value
    if (ScrapValueWidgetClass)
    {
        ScrapValueWidget = CreateWidget<UScrapValueWidget>(GetWorld(), ScrapValueWidgetClass);
        if (ScrapValueWidget)
        {
            ScrapValueWidget->AddToViewport();
            ScrapValueWidget->InitializeScrapWidget(Inventory);
            UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: ScrapValueWidget initialized successfully!"));
        }
    }

    // reticle
    if (ReticleWidgetClass)
    {
        ReticleWidget = CreateWidget<UReticleWidget>(GetWorld(), ReticleWidgetClass);
        if (ReticleWidget)
        {
            ReticleWidget->AddToViewport();
            ReticleWidget->InitializeReticle(Character);
            UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: ReticleWidget initialized successfully!"));
        }
    }
}