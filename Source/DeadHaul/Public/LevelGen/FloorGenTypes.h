#pragma once
#include "CoreMinimal.h"
#include "FloorGenTypes.generated.h"

USTRUCT()
struct FFloorGenTypes
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Start		UMETA(DisplayName = "Start Room"),
	Normal		UMETA(DisplayName = "Normal Room"),
	Arena 		UMETA(DisplayName = "Arena Room"),
	Corridor	UMETA(DisplayName = "Corridor"),
	End 		UMETA(DisplayName = "End Room"),
	DeadEnd 	UMETA(DisplayName = "Dead End Room"),
};

UENUM(BlueprintType)
enum class EDoorDirection : uint8
{
	North	UMETA(DisplayName = "North"),
	East	UMETA(DisplayName = "East"),
	South	UMETA(DisplayName = "South"),
	West	UMETA(DisplayName = "West"),
};