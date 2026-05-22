// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractionInterface.h"
#include "DeadHaulCharacter.generated.h"


class UCameraComponent;
class UInventoryComponent; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFocused, AActor*, FocusedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractableLost);

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
	UCameraComponent* PlayerCamera;

	//----------------
	// MOVEMENT
	//----------------

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRight(float Value);
	void LookUp(float Value);

	//----------------
	//COMPONENTS
	//----------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventoryComponent* InventoryComponent;

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

	//----------------
	// SPRINT
	//----------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 960.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaDrainRate = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float StaminaRegenRate = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float ExhaustionCooldown = 2.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	bool bIsExhausted = false;

	void StartSprint();
	void StopSprint();

	//----------------
	// INVENTORY HOTBAR INPUTS
	//----------------

	void ScrollHotbarUp();
	void ScrollHotbarDown();

	void SelectSlot1();
	void SelectSlot2();
	void SelectSlot3();
	void SelectSlot4();
	void SelectSlot5();
	void SelectSlot6();

	//----------------
	// INTERACTION
	//----------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractRange = 450.f;

	void Interact();

	void DropItem();

	UPROPERTY()
	AActor* LastFocusedActor = nullptr;

	//----------------
	// CROUCH
	//----------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* ShadowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StandingCameraZ = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchingCameraZ = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CrouchCameraInterpSpeed = 2.5f;

	void StartCrouch();
	void StopCrouch();
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouching = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//----------------
	//GETTERS
	//----------------

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetCurrentStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	UCameraComponent* GetPlayerCamera() const;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetFocusedInteractable() const;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractableFocused OnInteractableFocused;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractableLost OnInteractableLost;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool GetIsExhausted() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool GetIsSprinting() const;

private:
	// Tracks how long we've been in exhaustion cooldown
	float ExhaustionTimer = 0.f;

	void HandleStaminaTick(float DeltaTime);
};
