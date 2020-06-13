/*=================================================
* FileName: FishManager.h
* 
* Created by: Komodoman
* Project name: OceanProject
* Unreal Engine version: 4.18.3
* Created on: 2015/03/17
*
* Last Edited on: 2018/03/15
* Last Edited by: Felipe "Zoc" Silveira
* 
* -------------------------------------------------
* For parts referencing UE4 code, the following copyright applies:
* Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
*
* Feel free to use this software in any commercial/free game.
* Selling this as a plugin/item, in whole or part, is not allowed.
* See "OceanProject\License.md" for full licensing details.
* =================================================*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishManager.generated.h"


/**
 * 
 */
UCLASS()
class AFishManager : public AActor
{
	GENERATED_BODY()

	
public:

	// Type of flocks to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	TArray<UClass*> FlockTypes;

	// # of fish in respective flock
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	TArray<float> NumInFlock;

	// min Z of volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float MinZ = -9000.0f;

	// max Z of volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float MaxZ = -950.0f;
	
	// Length of underwater box
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float UnderwaterBoxLength = 10000;

	// Attach fish manager to player?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	bool AttachToPlayer = false;

	// Player Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	bool DebugMode = false;

	// Player Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	UClass* PlayerType;

	// Player
	AActor* PlayerReference;

	// get min z
	float GetMinZ();

	// get min z
	float GetMaxZ();

	/** Constructor */
	AFishManager(const FObjectInitializer& ObjectInitializer);

protected:

	/** Overrided tick function */
	virtual void Tick(float val) override;

	/** setup the fish manager */
	void Setup();

	/** move fish manager to player */
	void MoveToPlayer();

	/* max bounds of water*/
	float MaxX;
	float MaxY;
	float MinX;
	float MinY;

	// is set up?
	bool bIsSetup = false;

	// Are fish spawned?
	bool bAreFishSpawned = false;
};
