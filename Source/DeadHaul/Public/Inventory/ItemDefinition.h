// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "ItemDefinition.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Material    UMETA(DisplayName = "Material"),
    Consumable  UMETA(DisplayName = "Consumable"),
    Weapon      UMETA(DisplayName = "Weapon"),
    QuestItem   UMETA(DisplayName = "Quest Item")
};

USTRUCT(BlueprintType)
struct DEADHAUL_API FItemDefinitionRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText DisplayName = FText::GetEmpty();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    EItemType ItemType = EItemType::Material;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    int32 ScrapValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UTexture2D* Icon = nullptr;
};
