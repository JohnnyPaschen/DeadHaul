// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.generated.h"

USTRUCT(BlueprintType)
struct DEADHAUL_API FPlayerInventoryItem
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    FName ItemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 Quantity = 0;

    bool IsEmpty() const { return ItemID == NAME_None || Quantity <= 0; }
};

