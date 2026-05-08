// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DeadHaulCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
ADeadHaulCharacter::ADeadHaulCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//----------------
	// SPRING ARM
	//----------------

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	//----------------
	// CAMERA
	//----------------

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PlayerCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ADeadHaulCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeadHaulCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADeadHaulCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADeadHaulCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADeadHaulCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &ADeadHaulCharacter::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ADeadHaulCharacter::LookUp);

}

//----------------
// GETTERS
//----------------

//UInventoryComponent* ADeadHaulCharacter::GetInventoryComponent() const
//{
//	return InventoryComponent;
//}

float ADeadHaulCharacter::GetCurrentHealth() const
{
	return CurrentHealth;
}

float ADeadHaulCharacter::GetCurrentStamina() const
{
	return CurrentStamina;
}

UCameraComponent* ADeadHaulCharacter::GetPlayerCamera() const
{
	return PlayerCamera;
}

void ADeadHaulCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
		AddMovementInput(GetActorForwardVector(), Value);
}

void ADeadHaulCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
		AddMovementInput(GetActorRightVector(), Value);
}

void ADeadHaulCharacter::LookRight(float Value)
{
	AddControllerYawInput(Value);
}

void ADeadHaulCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}
