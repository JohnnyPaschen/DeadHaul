// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HotbarWidget.h"
#include "HUD/HotbarSlotWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

void UHotbarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UHotbarWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (InventoryComponent)
    {
        InventoryComponent->OnInventoryUpdated.RemoveDynamic(this, &UHotbarWidget::OnInventoryUpdated);
        InventoryComponent->OnActiveSlotChanged.RemoveDynamic(this, &UHotbarWidget::OnActiveSlotChanged);
    }
}

void UHotbarWidget::InitializeHotbar(UInventoryComponent* InInventoryComponent)
{
    if (!InInventoryComponent || !SlotWidgetClass) return;

    InventoryComponent = InInventoryComponent;

    InventoryComponent->OnInventoryUpdated.AddDynamic(this, &UHotbarWidget::OnInventoryUpdated);
    InventoryComponent->OnActiveSlotChanged.AddDynamic(this, &UHotbarWidget::OnActiveSlotChanged);

    BuildSlots();
    RefreshAllSlots();
    UpdateActiveHighlight(InventoryComponent->GetActiveSlotIndex());
}

void UHotbarWidget::BuildSlots()
{
    if (!SlotContainer || !SlotWidgetClass || !InventoryComponent) return;

    SlotContainer->ClearChildren();
    SlotWidgets.Empty();

    const int32 HotbarCount = InventoryComponent->GetHotbarSlots();

    for (int32 i = 0; i < HotbarCount; ++i)
    {
        UHotbarSlotWidget* SlotWidget = CreateWidget<UHotbarSlotWidget>(this, SlotWidgetClass);
        if (!SlotWidget) continue;

        UHorizontalBoxSlot* BoxSlot = SlotContainer->AddChildToHorizontalBox(SlotWidget);
        if (BoxSlot)
        {
            BoxSlot->SetPadding(FMargin(4.f, 0.f));
            BoxSlot->SetHorizontalAlignment(HAlign_Fill);
            BoxSlot->SetVerticalAlignment(VAlign_Fill);
        }

        SlotWidgets.Add(SlotWidget);
        SlotWidget->SetSlotNumber(i + 1);
    }
}

void UHotbarWidget::RefreshAllSlots()
{
    if (!InventoryComponent) return;

    const TArray<FPlayerInventoryItem>& Slots = InventoryComponent->GetSlots();
    const int32 HotbarCount = InventoryComponent->GetHotbarSlots();

    for (int32 i = 0; i < SlotWidgets.Num() && i < HotbarCount && i < Slots.Num(); ++i)
    {
        const FItemDefinitionRow* Definition = InventoryComponent->GetItemDefinition(Slots[i].ItemID);
        SlotWidgets[i]->UpdateSlot(Slots[i], Definition);
    }
}

void UHotbarWidget::UpdateActiveHighlight(int32 ActiveIndex)
{
    for (int32 i = 0; i < SlotWidgets.Num(); ++i)
    {
        SlotWidgets[i]->SetActive(i == ActiveIndex);
    }
}

void UHotbarWidget::OnInventoryUpdated()
{
    RefreshAllSlots();
}

void UHotbarWidget::OnActiveSlotChanged(int32 NewSlotIndex)
{
    UpdateActiveHighlight(NewSlotIndex);
}

