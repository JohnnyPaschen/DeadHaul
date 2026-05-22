// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/DeadHaulCharacter.h"
#include "Camera/CameraComponent.h"
#include "Inventory/InventoryComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ADeadHaulCharacter::ADeadHaulCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// enable crouching on the character movement component
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	//slow the player down while crouching
	GetCharacterMovement()->MaxWalkSpeedCrouched = 175.f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	bUseControllerRotationYaw = true; // character body rotates with player look direction

	//----------------
	// CAMERA
	//----------------

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->bUsePawnControlRotation = true; // camera follows controller look direction
	PlayerCamera->SetRelativeLocation(FVector(20.f, 0.f, BaseEyeHeight));
	BaseEyeHeight = 80.f;
	CrouchedEyeHeight = 65.f;

	ShadowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShadowMesh"));
	ShadowMesh->SetupAttachment(GetMesh());
	ShadowMesh->SetHiddenInGame(true);
	ShadowMesh->bCastHiddenShadow = true;
	ShadowMesh->SetCastShadow(true);

	//----------------
	// INVENTORY
	//----------------
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

// Called when the game starts or when spawned
void ADeadHaulCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
		GetMesh()->HideBoneByName(FName("neck"), EPhysBodyOp::PBO_None);
	}

	if (ShadowMesh && GetMesh())
	{
		ShadowMesh->SetSkeletalMesh(GetMesh()->GetSkeletalMeshAsset());
		ShadowMesh->SetAnimInstanceClass(GetMesh()->GetAnimClass());
	}
}

void ADeadHaulCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	PlayerCamera->SetRelativeLocation(FVector(60.f, 0.f, CrouchedEyeHeight));
}

void ADeadHaulCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	PlayerCamera->SetRelativeLocation(FVector(20.f, 0.f, BaseEyeHeight));
}

// Called every frame
void ADeadHaulCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	HandleStaminaTick(DeltaTime);

	AActor* CurrentFocused = GetFocusedInteractable();

	if (CurrentFocused != LastFocusedActor)
	{
		if (CurrentFocused)
		{
			OnInteractableFocused.Broadcast(CurrentFocused);
		}
		else
		{
			OnInteractableLost.Broadcast();
		}

			LastFocusedActor = CurrentFocused;
	}

	// Move camera to crouched or standing height
	//float TargetCameraZ = bIsCrouching ? CrouchingCameraZ : StandingCameraZ;
	//FVector CurrentCameraLocation = PlayerCamera->GetRelativeLocation();
	//float NewZ = FMath::FInterpTo(CurrentCameraLocation.Z, TargetCameraZ, DeltaTime, CrouchCameraInterpSpeed);
	//PlayerCamera->SetRelativeLocation(FVector(15.f, 0.f, 80.f));
}

// Called to bind functionality to input
void ADeadHaulCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//----------------
	// MOVEMENT
	//----------------

	PlayerInputComponent->BindAxis("MoveForward", this, &ADeadHaulCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADeadHaulCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookRight", this, &ADeadHaulCharacter::LookRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ADeadHaulCharacter::LookUp);

	//----------------
	// INVENTORY HOTBAR BUTTONS
	//----------------

	PlayerInputComponent->BindAction("HotbarScrollUp", IE_Pressed, this, &ADeadHaulCharacter::ScrollHotbarUp);
	PlayerInputComponent->BindAction("HotbarScrollDown", IE_Pressed, this, &ADeadHaulCharacter::ScrollHotbarDown);

	PlayerInputComponent->BindAction("HotbarSlot1", IE_Pressed, this, &ADeadHaulCharacter::SelectSlot1);
	PlayerInputComponent->BindAction("HotbarSlot2", IE_Pressed, this, &ADeadHaulCharacter::SelectSlot2);
	PlayerInputComponent->BindAction("HotbarSlot3", IE_Pressed, this, &ADeadHaulCharacter::SelectSlot3);
	PlayerInputComponent->BindAction("HotbarSlot4", IE_Pressed, this, &ADeadHaulCharacter::SelectSlot4);
	PlayerInputComponent->BindAction("HotbarSlot5", IE_Pressed, this, &ADeadHaulCharacter::SelectSlot5);
	PlayerInputComponent->BindAction("HotbarSlot6", IE_Pressed, this, &ADeadHaulCharacter::SelectSlot6);

	//----------------
	// INTERACTION
	//----------------

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADeadHaulCharacter::Interact);
	PlayerInputComponent->BindAction("DropItem", IE_Pressed, this, &ADeadHaulCharacter::DropItem);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ADeadHaulCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ADeadHaulCharacter::StopCrouch);

	// Jump input
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	// Sprint input
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ADeadHaulCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ADeadHaulCharacter::StopSprint);

}

//----------------
// GETTERS
//----------------

UInventoryComponent* ADeadHaulCharacter::GetInventoryComponent() const
{
	return InventoryComponent;
}

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


//----------------
// HOTBAR INPUT
//----------------

void ADeadHaulCharacter::ScrollHotbarUp()
{
	if (InventoryComponent)
		InventoryComponent->CycleActiveSlot(-1);
}

void ADeadHaulCharacter::ScrollHotbarDown()
{
	if (InventoryComponent)
		InventoryComponent->CycleActiveSlot(1);
}

void ADeadHaulCharacter::SelectSlot1() { if (InventoryComponent) InventoryComponent->SetActiveSlot(0); }
void ADeadHaulCharacter::SelectSlot2() { if (InventoryComponent) InventoryComponent->SetActiveSlot(1); }
void ADeadHaulCharacter::SelectSlot3() { if (InventoryComponent) InventoryComponent->SetActiveSlot(2); }
void ADeadHaulCharacter::SelectSlot4() { if (InventoryComponent) InventoryComponent->SetActiveSlot(3); }
void ADeadHaulCharacter::SelectSlot5() { if (InventoryComponent) InventoryComponent->SetActiveSlot(4); }
void ADeadHaulCharacter::SelectSlot6() { if (InventoryComponent) InventoryComponent->SetActiveSlot(5); }

//----------------
// INTERACTION
//----------------

AActor* ADeadHaulCharacter::GetFocusedInteractable() const
{
	if (!PlayerCamera) return nullptr;

	const FVector Start = PlayerCamera->GetComponentLocation();
	const FVector End = Start + PlayerCamera->GetForwardVector() * InteractRange;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	//DrawDebugLine(
	//	GetWorld(),
	//	Start,
	//	End,
	//	FColor::Red,
	//	false,
	//	2.0f,
	//	0,
	//	2.0f
	//);

	if (bHit && HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit: %s"), *HitResult.GetActor()->GetName());

		AActor* HitActor = HitResult.GetActor();
		if (HitActor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			return HitActor;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor does not implement IInteractionInterface"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace hit nothing"));
	}

	return nullptr;
}

void ADeadHaulCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("Interact() called"));

	AActor* FocusedActor = GetFocusedInteractable();
	if (FocusedActor)
	{
		IInteractionInterface::Execute_Interact(FocusedActor, this);
	}
}

void ADeadHaulCharacter::DropItem()
{
	if (InventoryComponent)
		InventoryComponent->DropActiveItem(this);
}

void ADeadHaulCharacter::StartCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("StartCrouch called"));
	bIsCrouching = true;
	Crouch();
}

void ADeadHaulCharacter::StopCrouch()
{
	bIsCrouching = false;
	UnCrouch();
}

//----------------
// SPRINT
//----------------

void ADeadHaulCharacter::StartSprint()
{
	// Only allow sprint if 
	if (!bIsExhausted && !bIsCrouching)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ADeadHaulCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

//----------------
// STAMINA TICK
//----------------

void ADeadHaulCharacter::HandleStaminaTick(float DeltaTime)
{
	if (bIsSprinting && !bIsExhausted)
	{
		// Drain stamina while actively sprinting
		CurrentStamina = FMath::Max(0.f, CurrentStamina - StaminaDrainRate * DeltaTime);

		// Hit zero — trigger exhaustion
		if (CurrentStamina <= 0.f)
		{
			bIsExhausted = true;
			StopSprint();
		}
	}
	else
	{
		// Regen stamina when not sprinting
		CurrentStamina = FMath::Min(MaxStamina, CurrentStamina + StaminaRegenRate * DeltaTime);

		// Count down exhaustion cooldown once stamina starts recovering
		if (bIsExhausted)
		{
			ExhaustionTimer += DeltaTime;
			if (ExhaustionTimer >= ExhaustionCooldown)
			{
				bIsExhausted = false;
				ExhaustionTimer = 0.f;
			}
		}
	}
}

//----------------
// GETTERS
//----------------

float ADeadHaulCharacter::GetMaxStamina() const
{
	return MaxStamina;
}

bool ADeadHaulCharacter::GetIsExhausted() const
{
	return bIsExhausted;
}

