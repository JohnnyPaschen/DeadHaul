// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ReticleWidget.h"
#include "Characters/DeadHaulCharacter.h"
#include "Components/TextBlock.h"

void UReticleWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UReticleWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (Character)
    {
        Character->OnInteractableFocused.RemoveDynamic(this, &UReticleWidget::OnInteractableFocused);
        Character->OnInteractableLost.RemoveDynamic(this, &UReticleWidget::OnInteractableLost);
    }
}

void UReticleWidget::InitializeReticle(ADeadHaulCharacter* InCharacter)
{
    if (!InCharacter) return;

    Character = InCharacter;
    Character->OnInteractableFocused.AddDynamic(this, &UReticleWidget::OnInteractableFocused);
    Character->OnInteractableLost.AddDynamic(this, &UReticleWidget::OnInteractableLost);

    if (ReticleText)
        ReticleText->SetColorAndOpacity(FSlateColor(DefaultColor));

    if (ReticleDot)
        ReticleDot->SetColorAndOpacity(FSlateColor(DefaultColor));
}

void UReticleWidget::OnInteractableFocused(AActor* FocusedActor)
{
    if (ReticleText)
        ReticleText->SetColorAndOpacity(FSlateColor(HighlightColor));

    if (ReticleDot)
        ReticleDot->SetColorAndOpacity(FSlateColor(HighlightColor));
}

void UReticleWidget::OnInteractableLost()
{
    if (ReticleText)
        ReticleText->SetColorAndOpacity(FSlateColor(DefaultColor));

    if (ReticleDot)
        ReticleDot->SetColorAndOpacity(FSlateColor(DefaultColor));
}

