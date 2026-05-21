// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/StaminaBarWidget.h"
#include "Characters/DeadHaulCharacter.h"
#include "Components/ProgressBar.h"

void UStaminaBarWidget::InitializeStaminaBar(ADeadHaulCharacter* InCharacter)
{
	Character = InCharacter;
}

void UStaminaBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Start fully transparent — fades in when stamina drops
	SetRenderOpacity(0.f);
}

void UStaminaBarWidget::NativeDestruct()
{
	Super::NativeDestruct();
	Character = nullptr;
}

void UStaminaBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateStaminaBar(InDeltaTime);
}

void UStaminaBarWidget::UpdateStaminaBar(float DeltaTime)
{
	if (!Character || !StaminaBar) return;

	float MaxStamina = Character->GetMaxStamina();
	float CurrentStamina = Character->GetCurrentStamina();
	bool bExhausted = Character->GetIsExhausted();

	// Update the bar fill (0.0 - 1.0)
	float StaminaPercent = (MaxStamina > 0.f) ? (CurrentStamina / MaxStamina) : 0.f;
	StaminaBar->SetPercent(StaminaPercent);

	// --- Color / Flash ---
	if (bExhausted)
	{
		// Flash red while exhausted
		FlashTimer += DeltaTime * FlashSpeed;
		float FlashAlpha = (FMath::Sin(FlashTimer) + 1.f) * 0.5f;
		StaminaBar->SetFillColorAndOpacity(FLinearColor::LerpUsingHSV(ExhaustedColor, FLinearColor::White, FlashAlpha * 0.3f));
	}
	else if (Character->GetIsSprinting())
	{
		// Blue while actively draining
		FlashTimer = 0.f;
		StaminaBar->SetFillColorAndOpacity(DrainingColor);
	}
	else
	{
		// Lerp from blue to green as stamina refills back to full
		FlashTimer = 0.f;
		FLinearColor CurrentColor = FLinearColor::LerpUsingHSV(DrainingColor, NormalColor, StaminaPercent);
		StaminaBar->SetFillColorAndOpacity(CurrentColor);
	}

	// --- Fade In / Out ---
	float CurrentOpacity = GetRenderOpacity();
	float TargetOpacity = (StaminaPercent < 1.f || bExhausted) ? 1.f : 0.f;
	float NewOpacity = FMath::FInterpTo(CurrentOpacity, TargetOpacity, DeltaTime, FadeSpeed);
	SetRenderOpacity(NewOpacity);
}

