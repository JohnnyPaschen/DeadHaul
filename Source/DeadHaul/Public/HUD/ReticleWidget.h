// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

class UTextBlock;
class ADeadHaulCharacter;

/**
 * 
 */
UCLASS()
class DEADHAUL_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void InitializeReticle(ADeadHaulCharacter* InCharacter);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReticleText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ReticleDot;

    UPROPERTY(EditAnywhere, Category = "Reticle")
    FLinearColor DefaultColor = FLinearColor(1.f, 1.f, 1.f, 0.8f);

    UPROPERTY(EditAnywhere, Category = "Reticle")
    FLinearColor HighlightColor = FLinearColor(1.f, 0.8f, 0.f, 1.f);


private:
    UPROPERTY()
    ADeadHaulCharacter* Character;

    UFUNCTION()
    void OnInteractableFocused(AActor* FocusedActor);

    UFUNCTION()
    void OnInteractableLost();
};
