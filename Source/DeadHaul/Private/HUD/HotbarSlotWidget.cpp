// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HotbarSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UHotbarSlotWidget::UpdateSlot(const FPlayerInventoryItem& Item, const FItemDefinitionRow* Definition)
{
    if (!ItemIcon || !QuantityText) return;

    if (Item.IsEmpty() || !Definition)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        QuantityText->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (Definition->Icon)
    {
        ItemIcon->SetBrushFromTexture(Definition->Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    //show quantity text if stackable and more than 1
    if (Definition->MaxStackSize > 1 && Item.Quantity > 1)
    {
        QuantityText->SetText(FText::AsNumber(Item.Quantity));
        QuantityText->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        QuantityText->SetVisibility(ESlateVisibility::Hidden);
    }
}


void UHotbarSlotWidget::SetActive(bool bIsActive)
{
    if (!SlotBorder) return;
    SlotBorder->SetBrushColor(bIsActive ? ActiveBorderColor : InactiveBorderColor);
}

void UHotbarSlotWidget::SetSlotNumber(int32 SlotNumber)
{
    if (!SlotNumberText) return;
    SlotNumberText->SetText(FText::AsNumber(SlotNumber));
}