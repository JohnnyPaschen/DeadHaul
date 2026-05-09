// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotbarWidget.generated.h"

class UHorizontalBox;
class UHotbarSlotWidget;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class DEADHAUL_API UHotbarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void InitializeHotbar(UInventoryComponent* InInventoryComponent);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* SlotContainer;

    UPROPERTY(EditDefaultsOnly, Category = "Hotbar")
    TSubclassOf<UHotbarSlotWidget> SlotWidgetClass;

private:
    UPROPERTY()
    UInventoryComponent* InventoryComponent;

    UPROPERTY()
    TArray<UHotbarSlotWidget*> SlotWidgets;

    void BuildSlots();
    void RefreshAllSlots();
    void UpdateActiveHighlight(int32 ActiveIndex);

    UFUNCTION()
    void OnInventoryUpdated();

    UFUNCTION()
    void OnActiveSlotChanged(int32 NewSlotIndex);
	
};
