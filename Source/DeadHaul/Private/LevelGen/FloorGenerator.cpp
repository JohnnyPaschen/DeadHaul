// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGen/FloorGenerator.h"
#include "LevelGen/RoomBase.h"
#include "LevelGen/DoorConnector.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

// Sets default values
AFloorGenerator::AFloorGenerator()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AFloorGenerator::BeginPlay()
{
    Super::BeginPlay();

}

void AFloorGenerator::BeginDestroy()
{
    SpawnCounts.Empty();
    PlacedRooms.Empty();
    StartRoom = nullptr;
    EndRoom = nullptr;

    Super::BeginDestroy();
}

void AFloorGenerator::GenerateFloor()
{
    int32 MaxRetries = 10;

    for (int32 Retry = 0; Retry < MaxRetries; Retry++)
    {
        int32 RetryMinRooms = FMath::Max(3, MinMainPathLength + 2 - Retry);

        GenerateFloorInternal();
        if (PlacedRooms.Num() >= RetryMinRooms)
            break;

        UE_LOG(LogTemp, Warning,
            TEXT("[FloorGen] Only placed %d rooms (min %d), retrying... (%d/%d)"),
            PlacedRooms.Num(), RetryMinRooms, Retry + 1, MaxRetries);
    }
}

void AFloorGenerator::GenerateFloorInternal()
{
    ClearFloor();

    LastUsedSeed = (Seed == 0) ? FMath::Rand() : Seed;
    RandStream.Initialize(LastUsedSeed);
    SpawnCounts.Empty();

    UE_LOG(LogTemp, Log, TEXT("[FloorGen] Starting generation. Seed: %d"), LastUsedSeed);

    // Place start room
    StartRoom = SpawnRoom(StartRoomClass, GetActorLocation(), FRotator::ZeroRotator);
    if (!StartRoom)
    {
        UE_LOG(LogTemp, Error, TEXT("[FloorGen] Failed to spawn StartRoom!"));
        return;
    }
    StartRoom->FloorDepth = 0;
    PlacedRooms.Add(StartRoom);

    // --- Guaranteed main path ---
    // Pick one door at each step and force a placement until we hit MinMainPathLength
    ARoomBase* CurrentRoom = StartRoom;

    for (int32 i = 0; i < MinMainPathLength; i++)
    {
        TArray<ADoorConnector*> OpenDoors = CurrentRoom->GetOpenDoors();
        if (OpenDoors.IsEmpty()) break;

        // Shuffle doors so we try different directions
        for (int32 j = OpenDoors.Num() - 1; j > 0; j--)
        {
            int32 k = RandStream.RandRange(0, j);
            OpenDoors.Swap(j, k);
        }

        bool bExtended = false;
        for (ADoorConnector* ChosenDoor : OpenDoors)
        {
            // Main path should never place dead ends
            if (TryAttachRoom(ChosenDoor, /*bDeadEndOnly=*/false))
            {
                CurrentRoom = PlacedRooms.Last();
                bExtended = true;
                break;
            }
        }

        if (!bExtended)
        {
            // Couldn't extend from this room at all — back up to start room and try again
            CurrentRoom = StartRoom;
        }
    }

    // --- Fill remaining budget from all open doors ---
    TQueue<ADoorConnector*> OpenDoorQueue;
    for (ARoomBase* Room : PlacedRooms)
    {
        for (ADoorConnector* Door : Room->GetOpenDoors())
        {
            if (!Door->bIsConnected)
                OpenDoorQueue.Enqueue(Door);
        }
    }

    int32 RoomsPlaced = PlacedRooms.Num();
    while (!OpenDoorQueue.IsEmpty() && RoomsPlaced < TargetRoomCount)
    {
        ADoorConnector* CurrentDoor;
        OpenDoorQueue.Dequeue(CurrentDoor);

        if (!CurrentDoor || CurrentDoor->bIsConnected) continue;

        // Try a normal room first, fall back to a dead end
        bool bPlaced = TryAttachRoom(CurrentDoor, /*bDeadEndOnly=*/false);
        if (bPlaced)
        {
            RoomsPlaced++;
            ARoomBase* NewRoom = PlacedRooms.Last();
            for (ADoorConnector* NewDoor : NewRoom->GetOpenDoors())
            {
                if (!NewDoor->bIsConnected)
                    OpenDoorQueue.Enqueue(NewDoor);
            }
        }
        else
        {
            TryAttachRoom(CurrentDoor, /*bDeadEndOnly=*/true);
        }
    }

    // Seal any remaining open doors — try a dead end first, then hard-seal
    TArray<ADoorConnector*> DoorsToSeal;
    for (ARoomBase* Room : PlacedRooms)
    {
        for (ADoorConnector* Door : Room->GetOpenDoors())
        {
            if (!Door->bIsConnected)
                DoorsToSeal.Add(Door);
        }
    }

    for (ADoorConnector* Door : DoorsToSeal)
    {
		if (!Door || Door->bIsConnected) continue;
        TryAttachRoom(Door, true);
    }

    UE_LOG(LogTemp, Log, TEXT("[FloorGen] Placed %d rooms"), PlacedRooms.Num());

    PlaceEndRoom();
    SealAllOpenDoors();
    ValidateConnectivity();
    OnFloorGenerated();
}

void AFloorGenerator::ClearFloor()
{
    for (ARoomBase* Room : PlacedRooms)
    {
        if (IsValid(Room))
            Room->Destroy();
    }
    PlacedRooms.Empty();
    StartRoom = nullptr;
    EndRoom = nullptr;
}

// -------------------------------------------------------
// Room Spawning
// -------------------------------------------------------

ARoomBase* AFloorGenerator::SpawnRoom(TSubclassOf<ARoomBase> RoomClass,
    const FVector& Location,
    const FRotator& Rotation)
{
    if (!RoomClass) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    return GetWorld()->SpawnActor<ARoomBase>(RoomClass, Location, Rotation, Params);
}

bool AFloorGenerator::TryAttachRoom(ADoorConnector* OpenDoor, bool bDeadEndOnly)
{
    if (!OpenDoor || OpenDoor->bIsConnected) return false;

    // Build weighted candidate list, filtered by bDeadEndOnly if requested
    TArray<URoomDataAsset*> Candidates;
    int32 OwnerDepth = OpenDoor->OwningRoom ? OpenDoor->OwningRoom->FloorDepth : 0;

    for (URoomDataAsset* DA : RoomPool)
    {
        if (!DA || !DA->RoomClass) continue;

        // Filter to dead ends only when sealing, or exclude them on main path/fill
        if (bDeadEndOnly && DA->RoomType != ERoomType::DeadEnd) continue;
        if (!bDeadEndOnly && DA->RoomType == ERoomType::DeadEnd) continue;

        if (OwnerDepth < DA->MinDepth) continue;

        if (DA->MaxInstances >= 0)
        {
            int32 Count = SpawnCounts.Contains(DA) ? SpawnCounts[DA] : 0;
            if (Count >= DA->MaxInstances) continue;
        }

        // Add multiple entries proportional to weight
        int32 WeightedCount = FMath::Max(1, FMath::RoundToInt(DA->SpawnWeight * 10.f));
        for (int32 i = 0; i < WeightedCount; i++)
            Candidates.Add(DA);
    }

    if (Candidates.IsEmpty())
    {
        if (!bDeadEndOnly)
        {
            UE_LOG(LogTemp, Warning, TEXT("[FloorGen] No candidates available to attach to door on %s!"),
                *OpenDoor->OwningRoom->GetName());
        }
        return false;
    }

    // Shuffle candidates
    for (int32 i = Candidates.Num() - 1; i > 0; i--)
    {
        int32 j = RandStream.RandRange(0, i);
        Candidates.Swap(i, j);
    }

    // Try placing each candidate
    int32 AttemptsLeft = FMath::Min(MaxPlacementAttempts, Candidates.Num());
    for (int32 Attempt = 0; Attempt < AttemptsLeft; Attempt++)
    {
        URoomDataAsset* DA = Candidates[Attempt];

        // Spawn a test instance to read its door positions
        ARoomBase* TestRoom = SpawnRoom(DA->RoomClass,
            FVector::ZeroVector,
            FRotator::ZeroRotator);
        if (!TestRoom) continue;

        TArray<ADoorConnector*> IncomingOptions = TestRoom->GetOpenDoors();
        if (IncomingOptions.IsEmpty())
        {
            TestRoom->Destroy();
            continue;
        }

        // Pick a random incoming door
        int32 DoorIdx = RandStream.RandRange(0, IncomingOptions.Num() - 1);
        ADoorConnector* IncomingDoor = IncomingOptions[DoorIdx];

        FTransform PlacementTransform = ComputeRoomTransform(OpenDoor, IncomingDoor);
        TestRoom->Destroy();

        // Spawn at position and check for overlaps
        ARoomBase* NewRoom = SpawnRoom(DA->RoomClass,
            PlacementTransform.GetLocation(),
            PlacementTransform.Rotator());
        if (!NewRoom) continue;

        if (!bDeadEndOnly && !IsPlacementValid(PlacementTransform.GetLocation(),
            NewRoom->RoomExtent,
            PlacementTransform.Rotator()))
        {
            NewRoom->Destroy();
            continue;
        }

        // Success — link the connectors
        // Find the door in the new room closest to the open door's position
        ADoorConnector* MatchingDoor = nullptr;
        float MinDist = FLT_MAX;
        for (ADoorConnector* D : NewRoom->DoorConnectors)
        {
            if (!D) continue;
            float Dist = FVector::Dist(D->GetActorLocation(), OpenDoor->GetActorLocation());
            if (Dist < MinDist)
            {
                MinDist = Dist;
                MatchingDoor = D;
            }
        }

        if (MatchingDoor)
        {
            OpenDoor->bIsConnected = true;
            MatchingDoor->bIsConnected = true;
            OpenDoor->LinkedConnector = MatchingDoor;
            MatchingDoor->LinkedConnector = OpenDoor;
            MatchingDoor->OwningRoom = NewRoom;
        }

        NewRoom->FloorDepth = OwnerDepth + 1;
        NewRoom->OnRoomPlaced();
        SpawnCounts.FindOrAdd(DA)++;
        PlacedRooms.Add(NewRoom);

        return true;
    }

    return false;
}

// -------------------------------------------------------
// Placement Validation
// -------------------------------------------------------

bool AFloorGenerator::IsPlacementValid(const FVector& Center,
    const FVector& Extent, const FRotator& Rotation)
{
	DrawDebugBox(GetWorld(), Center, Extent * 0.9f, FQuat(Rotation), FColor::Red, true, 30.f); // visualize placement area

    FBox NewBox = FBox::BuildAABB(Center, Extent * 0.9f);

    for (ARoomBase* Room : PlacedRooms)
    {
        if (!IsValid(Room)) continue;
        FBox ExistingBox = FBox::BuildAABB(Room->GetActorLocation(), Room->RoomExtent * 0.9f);
        if (NewBox.Intersect(ExistingBox)) return false;
    }
    return true;
}

// -------------------------------------------------------
// Snapping Math
// -------------------------------------------------------

FTransform AFloorGenerator::ComputeRoomTransform(ADoorConnector* OpenDoor,
    ADoorConnector* IncomingDoor)
{
    // Only use Yaw — pitch and roll should never affect room placement
    float OpenDoorYaw = OpenDoor->GetActorRotation().Yaw;
    float TargetYaw = OpenDoorYaw + 180.f;
    float IncomingYaw = IncomingDoor->GetActorRotation().Yaw;
    FRotator RoomRotation = FRotator(0.f, TargetYaw - IncomingYaw, 0.f);

    FVector IncomingLocalOffset = IncomingDoor->GetActorLocation();
    FVector RotatedOffset = RoomRotation.RotateVector(IncomingLocalOffset);
    FVector RoomLocation = OpenDoor->GetActorLocation() - RotatedOffset;

    return FTransform(RoomRotation, RoomLocation);
}

// -------------------------------------------------------
// End Room & Sealing
// -------------------------------------------------------

void AFloorGenerator::PlaceEndRoom()
{
    if (!EndRoomClass) return;

    // Find deepest open door across all placed rooms
    ADoorConnector* DeepestDoor = nullptr;
    int32 MaxDepth = -1;

    for (ARoomBase* Room : PlacedRooms)
    {
        for (ADoorConnector* Door : Room->GetOpenDoors())
        {
            if (Room->FloorDepth > MaxDepth)
            {
                MaxDepth = Room->FloorDepth;
                DeepestDoor = Door;
            }
        }
    }

    if (!DeepestDoor) return;

    ARoomBase* TestEnd = SpawnRoom(EndRoomClass, FVector::ZeroVector, FRotator::ZeroRotator);
    if (!TestEnd) return;

    TArray<ADoorConnector*> EndDoors = TestEnd->GetOpenDoors();
    if (EndDoors.IsEmpty()) { TestEnd->Destroy(); return; }

    FTransform PlacementTransform = ComputeRoomTransform(DeepestDoor, EndDoors[0]);
    TestEnd->Destroy();

    EndRoom = SpawnRoom(EndRoomClass,
        PlacementTransform.GetLocation(),
        PlacementTransform.Rotator());
    if (!EndRoom) return;

    EndRoom->FloorDepth = MaxDepth + 1;
    EndRoom->OnRoomPlaced();
    PlacedRooms.Add(EndRoom);

    // Find the end room's door closest to DeepestDoor and link them
    ADoorConnector* MatchingDoor = nullptr;
    float MinDist = FLT_MAX;
    for (ADoorConnector* D : EndRoom->DoorConnectors)
    {
        if (!D) continue;
        float Dist = FVector::Dist(D->GetActorLocation(), DeepestDoor->GetActorLocation());
        if (Dist < MinDist)
        {
            MinDist = Dist;
            MatchingDoor = D;
        }
    }

    if (MatchingDoor)
    {
        DeepestDoor->bIsConnected = true;
        MatchingDoor->bIsConnected = true;
        DeepestDoor->LinkedConnector = MatchingDoor;
        MatchingDoor->LinkedConnector = DeepestDoor;
        MatchingDoor->OwningRoom = EndRoom;
    }

    // Seal any remaining open doors on the end room
    for (ADoorConnector* D : EndRoom->DoorConnectors)
    {
        if (D && !D->bIsConnected)
            D->bIsConnected = true;
    }

    UE_LOG(LogTemp, Log, TEXT("[FloorGen] End room placed at depth %d"), EndRoom->FloorDepth);
}

void AFloorGenerator::SealAllOpenDoors()
{
    for (ARoomBase* Room : PlacedRooms)
        if (IsValid(Room)) Room->SealOpenDoors();
}

// -------------------------------------------------------
// Connectivity Validation
// -------------------------------------------------------

bool AFloorGenerator::ValidateConnectivity()
{
    if (!StartRoom) return false;

    TSet<ARoomBase*> Visited;
    TQueue<ARoomBase*> Queue;
    Queue.Enqueue(StartRoom);
    Visited.Add(StartRoom);

    while (!Queue.IsEmpty())
    {
        ARoomBase* Current;
        Queue.Dequeue(Current);

        for (ADoorConnector* Door : Current->DoorConnectors)
        {
            if (Door && Door->LinkedConnector && Door->LinkedConnector->OwningRoom)
            {
                ARoomBase* Neighbor = Door->LinkedConnector->OwningRoom;
                if (!Visited.Contains(Neighbor))
                {
                    Visited.Add(Neighbor);
                    Queue.Enqueue(Neighbor);
                }
            }
        }
    }

    for (ARoomBase* Room : PlacedRooms)
    {
        if (!Visited.Contains(Room))
        {
            UE_LOG(LogTemp, Warning, TEXT("[FloorGen] Room %s is unreachable!"),
                *Room->GetName());
            return false;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[FloorGen] Connectivity check passed."));
    return true;
}