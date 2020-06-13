/*=================================================
* FileName: FishState.h
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
#include "FlockFish.h"


class AFlockFish;

/**
 * 
 */

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish State		          /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////
class FishState
{
protected:
	AFlockFish* Fish;

public:
	// Constructors
	FishState(){}
	FishState(AFlockFish* InFish)
	{
		Fish = InFish;
	};
	virtual void Update(float DeltaTime){};
	virtual void HandleOverlapEvent(AActor* OtherActor, UPrimitiveComponent* OtherComponent, FString ColliderString){};
};


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Seek State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

class SeekState : public FishState
{
public:

	SeekState(AFlockFish* InFish) : FishState(InFish){};
	virtual void Update(float DeltaTime) override;

protected:

	virtual void SeekTarget(float DeltaTime);
	virtual void Flock(float DeltaTime);
};


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Flee State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

class FleeState : public FishState
{
protected:
	AActor* Enemy;

public:

	FleeState(AFlockFish* InFish, AActor* InEnemy) : FishState(InFish)
	{
		Enemy = InEnemy;
	};
	virtual void Update(float DeltaTime) override;

protected:

	virtual void FleeFromEnemy(float DeltaTime);

};


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Chase State            /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

class ChaseState : public FishState
{
protected:
	AActor* Prey;

public:

	ChaseState(AFlockFish* InFish, AActor* InPrey) : FishState(InFish)
	{
		Prey = InPrey;
	};
	virtual void Update(float DeltaTime) override;

protected:

	virtual void ChasePrey(float DeltaTime);
	virtual void EatPrey();
};
