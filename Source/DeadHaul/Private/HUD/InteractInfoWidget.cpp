// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InteractInfoWidget.h"
#include "Characters/DeadHaulCharacter.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemDefinition.h"
#include "Components/TextBlock.h"
#include "Items/PickupActor.h"

void UInteractInfoWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Hide the widget by default
    SetVisibility(ESlateVisibility::Hidden);
}

void UInteractInfoWidget::NativeDestruct()
{
    Super::NativeDestruct();

    // Clean up delegate bindings when the widget is destroyed
    if (Character)
    {
        Character->OnInteractableFocused.RemoveDynamic(this, &UInteractInfoWidget::OnInteractableFocused);
        Character->OnInteractableLost.RemoveDynamic(this, &UInteractInfoWidget::OnInteractableLost);
    }
}

void UInteractInfoWidget::InitializeInteractInfo(ADeadHaulCharacter* InCharacter, UInventoryComponent* InInventoryComponent)
{
    // Store references so we can use them in UpdateDisplay
    Character = InCharacter;
    InventoryComponent = InInventoryComponent;

    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractInfoWidget: Character reference is null!"));
        return;
    }

    // Bind to the character's interaction delegates so we react to reticle focus changes
    Character->OnInteractableFocused.AddDynamic(this, &UInteractInfoWidget::OnInteractableFocused);
    Character->OnInteractableLost.AddDynamic(this, &UInteractInfoWidget::OnInteractableLost);

    UE_LOG(LogTemp, Log, TEXT("InteractInfoWidget: Initialized and delegates bound."));
}

void UInteractInfoWidget::OnInteractableFocused(AActor* FocusedActor)
{
    if (!FocusedActor) return;

    // Try to cast to a PickupActor 
    APickupActor* Pickup = Cast<APickupActor>(FocusedActor);
    if (!Pickup)
    {
        // Focused actor is interactable but not a pickup — hide the info widget
        SetVisibility(ESlateVisibility::Hidden);
        return;
    }

    // Update the display with this item's data and show the widget
    UpdateDisplay(Pickup->GetItemID());
    SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UInteractInfoWidget::OnInteractableLost()
{
    // Nothing in focus — hide the info display
    SetVisibility(ESlateVisibility::Hidden);
}

void UInteractInfoWidget::UpdateDisplay(FName ItemID)
{
    if (!InventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractInfoWidget: InventoryComponent is null, cannot look up item definition."));
        return;
    }

    // Look up the item definition from the DataTable via the inventory component
    const FItemDefinitionRow* Definition = InventoryComponent->GetItemDefinition(ItemID);
    if (!Definition)
    {
        UE_LOG(LogTemp, Warning, TEXT("InteractInfoWidget: No definition found for ItemID: %s"), *ItemID.ToString());
        return;
    }

    // Set the item name text from the definition
    if (ItemNameText)
    {
        ItemNameText->SetText(Definition->DisplayName);
    }

    // Format and set the scrap value text
    if (ItemScrapText)
    {
        FText ScrapText = FText::Format(
            NSLOCTEXT("InteractInfo", "ScrapValue", "SCRAP: ${0}"),
            FText::AsNumber(Definition->ScrapValue)
        );
        ItemScrapText->SetText(ScrapText);
    }
}
