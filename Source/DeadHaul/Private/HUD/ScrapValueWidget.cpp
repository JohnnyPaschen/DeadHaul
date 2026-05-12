// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/ScrapValueWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Components/TextBlock.h"

void UScrapValueWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UScrapValueWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (InventoryComponent)
    {
        InventoryComponent->OnInventoryUpdated.RemoveDynamic(this, &UScrapValueWidget::OnInventoryUpdated);
    }
}

void UScrapValueWidget::InitializeScrapWidget(UInventoryComponent* InInventoryComponent)
{
    if (!InInventoryComponent) return;

    InventoryComponent = InInventoryComponent;
    InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UScrapValueWidget::OnInventoryUpdated);

    RefreshScrapValue();
}

void UScrapValueWidget::RefreshScrapValue()
{
    if (!InventoryComponent || !ScrapValueText) return;

    int32 TotalScrap = InventoryComponent->GetTotalScrapValue();
    FString DisplayString = FString::Printf(TEXT("SCRAP VALUE: $%d"), TotalScrap);
    ScrapValueText->SetText(FText::FromString(DisplayString));
}

void UScrapValueWidget::OnInventoryUpdated()
{
    RefreshScrapValue();
}