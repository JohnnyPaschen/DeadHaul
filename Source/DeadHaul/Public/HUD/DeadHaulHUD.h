// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DeadHaulHUD.generated.h"

class UHotbarWidget;

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

private:
    UPROPERTY()
    UHotbarWidget* HotbarWidget;
	
};
