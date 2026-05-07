// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DeadHaulCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
//class UInventoryComponent;

UCLASS()
class DEADHAUL_API ADeadHaulCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADeadHaulCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	//----------------
	//CAMERA
	//----------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* PlayerCamera;

	//----------------
	//COMPONENTS
	//----------------

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	//UInventoryComponent* InventoryComponent;

	//----------------
	//STATS
	//----------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentStamina = 100.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//----------------
	//GETTERS
	//----------------

	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	UCameraComponent* GetPlayerCamera() const;

};
