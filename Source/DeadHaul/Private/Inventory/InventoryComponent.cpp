// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemDefinition.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Items/PickupActor.h"
#include "Components/StaticMeshComponent.h"

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

const FItemDefinitionRow* UInventoryComponent::GetItemDefinition(FName InItemID) const
{
    if (!ItemDatabase || InItemID == NAME_None) return nullptr;
    return ItemDatabase->FindRow<FItemDefinitionRow>(InItemID, TEXT("UInventoryComponent::GetItemDefinition"));
}

bool UInventoryComponent::AddItem(FPlayerInventoryItem Item)
{
    if (Item.IsEmpty()) return false;

    const FItemDefinitionRow* Definition = GetItemDefinition(Item.ItemID);
    if (!Definition)
    {
        UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: No item definition found for ID [%s]"), *Item.ItemID.ToString());
        return false;
    }

    int32 MaxStack = Definition->MaxStackSize;
    int32 QuantityToAdd = Item.Quantity;

    // try to stack into existing slots first
    if (MaxStack > 1)
    {
        int32 StackableIndex = FindStackableSlot(Item.ItemID, MaxStack);
        while (StackableIndex != -1 && QuantityToAdd > 0)
        {
            FPlayerInventoryItem& ExistingSlot = Slots[StackableIndex];
            int32 SpaceInSlot = MaxStack - ExistingSlot.Quantity;
            int32 AmountToStack = FMath::Min(QuantityToAdd, SpaceInSlot);

            ExistingSlot.Quantity += AmountToStack;
            QuantityToAdd -= AmountToStack;

            StackableIndex = FindStackableSlot(Item.ItemID, MaxStack);
        }
    }

    // fill empty slots with any remainder
    while (QuantityToAdd > 0)
    {
        int32 EmptyIndex = FindEmptySlot();
        if (EmptyIndex == -1)
        {
            OnInventoryUpdated.Broadcast();
            return false;
        }

        FPlayerInventoryItem NewSlotItem = Item;
        NewSlotItem.Quantity = FMath::Min(QuantityToAdd, MaxStack);
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

void UInventoryComponent::DropActiveItem(ACharacter* Character)
{
    if (!Character || !Slots.IsValidIndex(ActiveSlotIndex)) return;

    FPlayerInventoryItem& ActiveSlot = Slots[ActiveSlotIndex];
    if (ActiveSlot.IsEmpty()) return;

    const FItemDefinitionRow* Definition = GetItemDefinition(ActiveSlot.ItemID);
    if (!Definition || !Definition->PickupActorClass) return;

    FVector DropLocation = Character->GetActorLocation() + Character->GetActorForwardVector() * 100.f;
    DropLocation.Z += 50.f;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APickupActor* DroppedItem = Character->GetWorld()->SpawnActor<APickupActor>(
        Definition->PickupActorClass,
        DropLocation,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (DroppedItem)
    {
        DroppedItem->InitializeFromDrop(ActiveSlot.ItemID, 1, ItemDatabase);

        UStaticMeshComponent* Mesh = DroppedItem->FindComponentByClass<UStaticMeshComponent>();
        if (Mesh && Mesh->IsSimulatingPhysics())
        {
            FVector LaunchDirection = Character->GetActorForwardVector() + FVector(0.f, 0.f, 0.3f);
            LaunchDirection.Normalize();
            Mesh->AddImpulse(LaunchDirection * 300.f, NAME_None, true);
        }

        ActiveSlot.Quantity -= 1;
        if (ActiveSlot.Quantity <= 0)
        {
            ActiveSlot = FPlayerInventoryItem();
        }

        OnInventoryUpdated.Broadcast();
    }
}

int32 UInventoryComponent::GetTotalScrapValue() const
{
    int32 Total = 0;

    for (const FPlayerInventoryItem& Slot : Slots)
    {
        if (Slot.IsEmpty()) continue;

        const FItemDefinitionRow* Definition = GetItemDefinition(Slot.ItemID);
        if (!Definition) continue;

        Total += Definition->ScrapValue * Slot.Quantity;
    }

    return Total;
}