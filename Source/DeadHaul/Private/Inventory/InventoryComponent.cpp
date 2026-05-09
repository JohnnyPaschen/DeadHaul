// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Slots.SetNum(HotbarSlots);
}

bool UInventoryComponent::AddItem(FPlayerInventoryItem Item)
{
    if (Item.IsEmpty()) return false;

    int32 QuantityToAdd = Item.Quantity;

    //try to stack into existing slots first
    if (Item.MaxStackSize > 1)
    {
        int32 StackableIndex = FindStackableSlot(Item.ItemID, Item.MaxStackSize);
        while (StackableIndex != -1 && QuantityToAdd > 0)
        {
            FPlayerInventoryItem& ExistingSlot = Slots[StackableIndex];
            int32 SpaceInSlot = ExistingSlot.MaxStackSize - ExistingSlot.Quantity;
            int32 AmountToStack = FMath::Min(QuantityToAdd, SpaceInSlot);

            ExistingSlot.Quantity += AmountToStack;
            QuantityToAdd -= AmountToStack;

            StackableIndex = FindStackableSlot(Item.ItemID, Item.MaxStackSize);
        }
    }

    //fill empty slots with any remainder of an item
    while (QuantityToAdd > 0)
    {
        int32 EmptyIndex = FindEmptySlot();
        if (EmptyIndex == -1)
        {
            OnInventoryUpdated.Broadcast();
            return false;
        }

        FPlayerInventoryItem NewSlotItem = Item;
        NewSlotItem.Quantity = FMath::Min(QuantityToAdd, Item.MaxStackSize);
        Slots[EmptyIndex] = NewSlotItem;
        QuantityToAdd -= NewSlotItem.Quantity;
    }

    OnInventoryUpdated.Broadcast();
    return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
    if (ItemID == NAME_None || Quantity <= 0) return false;

    // check if we have enough before removing anything
    if (GetItemCount(ItemID) < Quantity) return false;

    int32 QuantityToRemove = Quantity;

    for (FPlayerInventoryItem& Slot : Slots)
    {
        if (Slot.ItemID == ItemID && QuantityToRemove > 0)
        {
            int32 AmountToRemove = FMath::Min(QuantityToRemove, Slot.Quantity);
            Slot.Quantity -= AmountToRemove;
            QuantityToRemove -= AmountToRemove;

            if (Slot.Quantity <= 0)
            {
                Slot = FPlayerInventoryItem();
            }
        }
    }

    OnInventoryUpdated.Broadcast();
    return true;
}

void UInventoryComponent::ClearSlot(int32 SlotIndex)
{
    if (!Slots.IsValidIndex(SlotIndex)) return;

    Slots[SlotIndex] = FPlayerInventoryItem();
    OnInventoryUpdated.Broadcast();
}

void UInventoryComponent::SwapSlots(int32 SlotIndexA, int32 SlotIndexB)
{
    if (!Slots.IsValidIndex(SlotIndexA) || !Slots.IsValidIndex(SlotIndexB)) return;
    if (SlotIndexA == SlotIndexB) return;

    Slots.Swap(SlotIndexA, SlotIndexB);
    OnInventoryUpdated.Broadcast();
}

bool UInventoryComponent::HasItem(FName ItemID, int32 Quantity) const
{
    return GetItemCount(ItemID) >= Quantity;
}

int32 UInventoryComponent::GetItemCount(FName ItemID) const
{
    int32 Total = 0;
    for (const FPlayerInventoryItem& Slot : Slots)
    {
        if (Slot.ItemID == ItemID)
        {
            Total += Slot.Quantity;
        }
    }
    return Total;
}

void UInventoryComponent::SetActiveSlot(int32 SlotIndex)
{
    int32 ClampedIndex = FMath::Clamp(SlotIndex, 0, FMath::Min(HotbarSlots, Slots.Num()) - 1);

    if (ClampedIndex != ActiveSlotIndex)
    {
        ActiveSlotIndex = ClampedIndex;
        OnActiveSlotChanged.Broadcast(ActiveSlotIndex);
    }
}

void UInventoryComponent::CycleActiveSlot(int32 Direction)
{
    int32 HotbarSize = FMath::Min(HotbarSlots, Slots.Num());
    if (HotbarSize <= 0) return;

    int32 NewIndex = (ActiveSlotIndex + Direction + HotbarSize) % HotbarSize;
    SetActiveSlot(NewIndex);
}

FPlayerInventoryItem UInventoryComponent::GetActiveItem() const
{
    if (Slots.IsValidIndex(ActiveSlotIndex))
    {
        return Slots[ActiveSlotIndex];
    }
    return FPlayerInventoryItem();
}

bool UInventoryComponent::UpgradeInventory()
{
    if (Slots.Num() >= MaxSlots) return false;

    int32 NewSize = FMath::Min(Slots.Num() + SlotsPerUpgrade, MaxSlots);
    Slots.SetNum(NewSize);

    OnInventoryUpdated.Broadcast();
    return true;
}

int32 UInventoryComponent::GetRemainingUpgrades() const
{
    int32 SlotsDifference = MaxSlots - Slots.Num();
    return FMath::CeilToInt((float)SlotsDifference / (float)SlotsPerUpgrade);
}

int32 UInventoryComponent::FindStackableSlot(FName ItemID, int32 MaxStack) const
{
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        const FPlayerInventoryItem& Slot = Slots[i];
        if (Slot.ItemID == ItemID && Slot.Quantity < MaxStack)
        {
            return i;
        }
    }
    return -1;
}

int32 UInventoryComponent::FindEmptySlot() const
{
    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i].IsEmpty())
        {
            return i;
        }
    }
    return -1;
}