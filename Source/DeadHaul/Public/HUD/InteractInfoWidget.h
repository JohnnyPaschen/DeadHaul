// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractInfoWidget.generated.h"

class UTextBlock;
class ADeadHaulCharacter;
class UInventoryComponent;

/**
 * Displays Item info (name & Scrap Value) when player
 * is looking at an interactable object in the world.
 * Listens to character delegates to know when to show & hide
 */
UCLASS()
class DEADHAUL_API UInteractInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    /**
     * Sets up the widget by binding to the character's interaction delegates
     * and storing a reference to the inventory component for DataTable lookups.
     */
    void InitializeInteractInfo(ADeadHaulCharacter* InCharacter, UInventoryComponent* InInventoryComponent);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//Text displaying the item's display name
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;

	//Text displaying the item's scrap value
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemScrapText;

private:
    //Reference to the player character for delegate binding
    UPROPERTY()
    ADeadHaulCharacter* Character;

    //Reference to the inventory component used for DataTable lookups
    UPROPERTY()
    UInventoryComponent* InventoryComponent;

    /**
     * Called when the player looks at an interactable actor.
     * Looks up the item definition and updates the display text.
     */
    UFUNCTION()
    void OnInteractableFocused(AActor* FocusedActor);

    /**
     * Called when the player looks away from an interactable actor.
     * Hides the widget from view.
     */
    UFUNCTION()
    void OnInteractableLost();

    /**
     * Updates the item name and scrap value text blocks
     * based on the provided item ID looked up from the DataTable.
     */
    void UpdateDisplay(FName ItemID);
};
