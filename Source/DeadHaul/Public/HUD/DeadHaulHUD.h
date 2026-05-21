// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DeadHaulHUD.generated.h"

class UHotbarWidget;
class UScrapValueWidget;
class UReticleWidget;
class UInteractInfoWidget;
class UStaminaBarWidget;

/**
 * 
 */
UCLASS()
class DEADHAUL_API ADeadHaulHUD : public AHUD
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UHotbarWidget> HotbarWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UScrapValueWidget> ScrapValueWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UReticleWidget> ReticleWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UInteractInfoWidget> InteractInfoWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<UStaminaBarWidget> StaminaBarWidgetClass;

private:
    UPROPERTY()
    UHotbarWidget* HotbarWidget;

    UPROPERTY()
    UScrapValueWidget* ScrapValueWidget;

    UPROPERTY()
    UReticleWidget* ReticleWidget;

    UPROPERTY()
    UInteractInfoWidget* InteractInfoWidget;

    UPROPERTY()
    UStaminaBarWidget* StaminaBarWidget;
};
