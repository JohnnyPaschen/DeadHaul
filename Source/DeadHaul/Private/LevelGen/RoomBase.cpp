// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGen/RoomBase.h"
#include "DrawDebugHelpers.h"
#include "LevelGen/RoomDataAsset.h"
#include "LevelGen/DoorConnector.h"

// Sets default values
ARoomBase::ARoomBase()
{
    PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	CollectDoorConnectors();
}

void ARoomBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bVisualizeBounds)
	{
		// Draw the room extent box in the editor
		//DrawDebugBox(GetWorld(), GetActorLocation(), RoomExtent, BoundsVisualizationColor, true, -1.0f, 0);
	}
}

void ARoomBase::CollectDoorConnectors()
{
    DoorConnectors.Empty();

    // Method 1: directly attached actors
    TArray<AActor*> AttachedActors;
    GetAttachedActors(AttachedActors);
    for (AActor* Actor : AttachedActors)
    {
        if (ADoorConnector* Door = Cast<ADoorConnector>(Actor))
        {
            Door->OwningRoom = this;
            DoorConnectors.Add(Door);
        }
    }

    // Method 2: child actor components
    TArray<UChildActorComponent*> ChildActorComps;
    GetComponents<UChildActorComponent>(ChildActorComps);
    for (UChildActorComponent* CAC : ChildActorComps)
    {
        if (ADoorConnector* Door = Cast<ADoorConnector>(CAC->GetChildActor()))
        {
            if (!DoorConnectors.Contains(Door))
            {
                Door->OwningRoom = this;
                DoorConnectors.Add(Door);
            }
        }
    }
}

TArray<ADoorConnector*> ARoomBase::GetOpenDoors() const
{
    TArray<ADoorConnector*> Open;
    for (ADoorConnector* Door : DoorConnectors)
    {
        if (Door && !Door->bIsConnected)
            Open.Add(Door);
    }
    return Open;
}

void ARoomBase::SealOpenDoors_Implementation()
{
    
}

void ARoomBase::OnRoomPlaced_Implementation()
{
    ApplyMaterials();
}

void ARoomBase::ApplyMaterials()
{
    if (!RoomData) return;

    TArray<UStaticMeshComponent*> Meshes;
    GetComponents<UStaticMeshComponent>(Meshes);

    for (UStaticMeshComponent* Mesh : Meshes)
    {
        if (Mesh->ComponentHasTag("Floor") && RoomData->FloorMaterial)
            Mesh->SetMaterial(0, RoomData->FloorMaterial);

        if (Mesh->ComponentHasTag("Wall") && RoomData->WallMaterial)
            Mesh->SetMaterial(0, RoomData->WallMaterial);
    }
}
