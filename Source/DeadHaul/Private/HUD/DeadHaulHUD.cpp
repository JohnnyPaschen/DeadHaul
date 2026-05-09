// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/DeadHaulHUD.h"
#include "HUD/HotbarWidget.h"
#include "Characters/DeadHaulCharacter.h"
#include "Inventory/InventoryComponent.h"

void ADeadHaulHUD::BeginPlay()
{
    Super::BeginPlay();

    if (!HotbarWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: HotbarWidgetClass is null!"));
        return;
    }

    HotbarWidget = CreateWidget<UHotbarWidget>(GetWorld(), HotbarWidgetClass);
    if (!HotbarWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Failed to create HotbarWidget!"));
        return;
    }

    HotbarWidget->AddToViewport();
    UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: HotbarWidget added to viewport!"));

    ADeadHaulCharacter* Character = Cast<ADeadHaulCharacter>(GetOwningPawn());
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Failed to get Character!"));
        return;
    }

    if (!Character->GetInventoryComponent())
    {
        UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Failed to get InventoryComponent!"));
        return;
    }

    HotbarWidget->InitializeHotbar(Character->GetInventoryComponent());
    UE_LOG(LogTemp, Warning, TEXT("DeadHaulHUD: Hotbar initialized successfully!"));
}