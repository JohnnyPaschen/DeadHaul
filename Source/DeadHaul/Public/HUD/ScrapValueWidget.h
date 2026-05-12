// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScrapValueWidget.generated.h"

class UTextBlock;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class DEADHAUL_API UScrapValueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void InitializeScrapWidget(UInventoryComponent* InInventoryComponent);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ScrapValueText;

private:
    UPROPERTY()
    UInventoryComponent* InventoryComponent;

    void RefreshScrapValue();

    UFUNCTION()
    void OnInventoryUpdated();
	
};
