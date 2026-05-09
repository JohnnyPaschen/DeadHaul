// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HotbarSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void UHotbarSlotWidget::UpdateSlot(const FPlayerInventoryItem& Item)
{
    if (!ItemIcon || !QuantityText) return;

    if (Item.IsEmpty())
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        QuantityText->SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    if (Item.Icon)
    {
        ItemIcon->SetBrushFromTexture(Item.Icon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }

    //show quantity text if stackable and more than 1
    if (Item.MaxStackSize > 1 && Item.Quantity > 1)
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

