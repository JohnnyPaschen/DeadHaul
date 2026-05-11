// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/ItemDefinition.h"
#include "InventoryComponent.generated.h"

class UDataTable;
class APickupActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveSlotChanged, int32, NewSlotIndex);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEADHAUL_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FPlayerInventoryItem> Slots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 HotbarSlots = 6;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 MaxSlots = 24;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 SlotsPerUpgrade = 6;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    int32 ActiveSlotIndex = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    UDataTable* ItemDatabase;

public:

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnInventoryUpdated OnInventoryUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
    FOnActiveSlotChanged OnActiveSlotChanged;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(FPlayerInventoryItem Item);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveItem(FName ItemID, int32 Quantity = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ClearSlot(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SwapSlots(int32 SlotIndexA, int32 SlotIndexB);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasItem(FName ItemID, int32 Quantity = 1) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetItemCount(FName ItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetActiveSlot(int32 SlotIndex);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void CycleActiveSlot(int32 Direction);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FPlayerInventoryItem GetActiveItem() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory|Upgrades")
    bool UpgradeInventory();

    UFUNCTION(BlueprintCallable, Category = "Inventory|Upgrades")
    int32 GetRemainingUpgrades() const;

    const FItemDefinitionRow* GetItemDefinition(FName InItemID) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void DropActiveItem(ACharacter* Character);

    //----------------
    // GETTERS
    //----------------

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    const TArray<FPlayerInventoryItem>& GetSlots() const { return Slots; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetSlotCount() const { return Slots.Num(); }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetHotbarSlots() const { return HotbarSlots; }

private:
    int32 FindStackableSlot(FName ItemID, int32 MaxStack) const;
    int32 FindEmptySlot() const;
};
