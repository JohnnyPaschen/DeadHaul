// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/ItemDefinition.h"
#include "HotbarSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;

/**
 * 
 */
UCLASS()
class DEADHAUL_API UHotbarSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdateSlot(const FPlayerInventoryItem& Item, const FItemDefinitionRow* Definition);
    void SetActive(bool bIsActive);
    void SetSlotNumber(int32 SlotNumber);

protected:
    UPROPERTY(meta = (BindWidget))
    UBorder* SlotBorder;

    UPROPERTY(meta = (BindWidget))
    UImage* ItemIcon;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuantityText;

    UPROPERTY(EditAnywhere, Category = "Hotbar|Style")
    FLinearColor ActiveBorderColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

    UPROPERTY(EditAnywhere, Category = "Hotbar|Style")
    FLinearColor InactiveBorderColor = FLinearColor(0.1f, 0.1f, 0.1f, 0.7f);

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SlotNumberText;
	
};
