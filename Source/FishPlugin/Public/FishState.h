// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

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
