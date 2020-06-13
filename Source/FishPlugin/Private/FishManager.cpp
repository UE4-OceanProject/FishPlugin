/*=================================================
* FileName: FishManager.cpp
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

#include "FishManager.h"
#include "FlockFish.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


AFishManager::AFishManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFishManager::Tick(float val)
{
	Setup();

	if (AttachToPlayer)
	{
		MoveToPlayer();
	}
}

void AFishManager::MoveToPlayer()
{
	if (PlayerReference)
		this->SetActorLocation(PlayerReference->GetActorLocation());
}

float AFishManager::GetMinZ()
{
	return MinZ;
}

float AFishManager::GetMaxZ()
{
	return MaxZ;
}

void AFishManager::Setup()
{
	if (bIsSetup == false){
		if (!bAreFishSpawned)
		{
			MaxX = GetActorLocation().X + UnderwaterBoxLength;
			MaxY = GetActorLocation().Y + UnderwaterBoxLength;
			MinX = GetActorLocation().X - UnderwaterBoxLength;
			MinY = GetActorLocation().Y - UnderwaterBoxLength;

			UWorld* const world = GetWorld();
			int numFlocks = FlockTypes.Num();
			for (int i = 0; i < numFlocks; i++)
			{
				FVector spawnLoc = FVector(FMath::FRandRange(MinX, MaxX), FMath::FRandRange(MinY, MaxY), FMath::FRandRange(MinZ, MaxZ));
				AFlockFish *leaderFish = nullptr;
				for (int j = 0; j < NumInFlock[i]; j++)
				{
					AFlockFish *aFish = Cast<AFlockFish>(world->SpawnActor(FlockTypes[i]));
					aFish->bIsLeader = false;
					aFish->DebugMode = DebugMode;
					aFish->UnderwaterMax = FVector(MaxX, MaxY, MaxZ);
					aFish->UnderwaterMin = FVector(MinX, MinY, MinZ);
					aFish->underwaterBoxLength = UnderwaterBoxLength;
					spawnLoc = FVector(FMath::FRandRange(MinX, MaxX), FMath::FRandRange(MinY, MaxY), FMath::FRandRange(MinZ, MaxZ));
					if (j == 0)
					{
						aFish->bIsLeader = true;
						leaderFish = aFish;
					}
					else if (leaderFish != nullptr)
					{	
						aFish->Leader = leaderFish;
					}
					aFish->SetActorLocation(spawnLoc);
				}
			}
			bAreFishSpawned = true;
		}

		if (AttachToPlayer)
		{
			TArray<AActor*> aPlayerList;
			UGameplayStatics::GetAllActorsOfClass(this, PlayerType, aPlayerList);
			if (aPlayerList.Num() > 0)
			{	
				PlayerReference = aPlayerList[0];
				bIsSetup = true;
			}
		} 
		else
		{
			bIsSetup = true;
		}

	}
}
