// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBarWidget.generated.h"

class UProgressBar;
class UWidget;
class ADeadHaulCharacter;

/**
 * Displays the player's stamina as a progress bar.
 */
UCLASS()
class DEADHAUL_API UStaminaBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Called by the HUD
	void InitializeStaminaBar(ADeadHaulCharacter* InCharacter);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// The progress bar bound in the Blueprint widget
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	// Normal stamina bar color
	UPROPERTY(EditAnywhere, Category = "Stamina|Style")
	FLinearColor NormalColor = FLinearColor(0.1f, 0.8f, 0.2f, 1.f);

	// Color shown while stamina is actively draining
	UPROPERTY(EditAnywhere, Category = "Stamina|Style")
	FLinearColor DrainingColor = FLinearColor(0.1f, 0.4f, 1.f, 1.f);

	// Color shown when the player is exhausted
	UPROPERTY(EditAnywhere, Category = "Stamina|Style")
	FLinearColor ExhaustedColor = FLinearColor(0.9f, 0.1f, 0.1f, 1.f);

	// How fast the bar fades in/out (higher = faster)
	UPROPERTY(EditAnywhere, Category = "Stamina|Style")
	float FadeSpeed = 4.f;

	// How fast the bar flashes when exhausted (pulses per second)
	UPROPERTY(EditAnywhere, Category = "Stamina|Style")
	float FlashSpeed = 6.f;

private:
	UPROPERTY()
	ADeadHaulCharacter* Character;

	// Tracks elapsed time for flash oscillation
	float FlashTimer = 0.f;

	// Updates bar fill, color, and opacity each tick
	void UpdateStaminaBar(float DeltaTime);
};
