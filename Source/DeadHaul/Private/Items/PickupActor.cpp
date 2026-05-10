// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickupActor.h"
#include "Characters/DeadHaulCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/ItemDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"

APickupActor::APickupActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;
}

void APickupActor::BeginPlay()
{
    Super::BeginPlay();
}

void APickupActor::Interact_Implementation(ACharacter* Interactor)
{
    if (!Interactor || !ItemDatabase || ItemID == NAME_None) return;

    ADeadHaulCharacter* DHCharacter = Cast<ADeadHaulCharacter>(Interactor);
    if (!DHCharacter) return;

    UInventoryComponent* Inventory = DHCharacter->GetInventoryComponent();
    if (!Inventory) return;

    FPlayerInventoryItem NewItem;
    NewItem.ItemID = ItemID;
    NewItem.Quantity = Quantity;

    if (Inventory->AddItem(NewItem))
    {
        UE_LOG(LogTemp, Log, TEXT("PickupActor: Picked up [%s] x%d"), *ItemID.ToString(), Quantity);
        Destroy();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PickupActor: Inventory full. Could not pick up [%s]."), *ItemID.ToString());
    }
}

FText APickupActor::GetInteractPrompt_Implementation() const
{
    if (!ItemDatabase || ItemID == NAME_None)
        return FText::FromString(TEXT("Pick up"));

    const FItemDefinitionRow* Row = ItemDatabase->FindRow<FItemDefinitionRow>(ItemID, TEXT("PickupActor::GetInteractPrompt"));
    if (!Row)
        return FText::FromString(TEXT("Pick up"));

    return FText::Format(FText::FromString(TEXT("Pick up {0}")), Row->DisplayName);
}
