// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractionInterface.h"
#include "PickupActor.generated.h"

class UStaticMeshComponent;
class UDataTable;

UCLASS()
class DEADHAUL_API APickupActor : public AActor, public IInteractionInterface
{
    GENERATED_BODY()

public:
    APickupActor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    FName ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    int32 Quantity = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    UDataTable* ItemDatabase;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* MeshComponent;

public:
    virtual void Interact_Implementation(ACharacter* Interactor) override;
    virtual FText GetInteractPrompt_Implementation() const override;
};
