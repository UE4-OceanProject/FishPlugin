// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

#include "FishState.h"
#include "FlockFish.h"

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Seek State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void SeekState::Update(float DeltaTime)
{	
	Fish->bIsFleeing = false;
	Fish->FleeTarget = nullptr;
	Fish->PreyTarget = nullptr;

	if (Fish->bIsLeader)
	{
		// Seek target
		SeekTarget(DeltaTime);
	}
	else
	{
		// School with buddies
		Flock(DeltaTime);
	}
}

void SeekState::SeekTarget(float DeltaTime)
{	
	// Set Speed
	Fish->CurrentSpeed = FMath::Lerp(Fish->CurrentSpeed, Fish->Speed, DeltaTime * Fish->SeekDecelerationMultiplier);

	// Set Rotation 
	FVector targetRotation = (Fish->GetSeekTarget() - Fish->GetActorLocation() + Fish->AvoidObstacle());
	FRotator LeaderRotation = FRotationMatrix::MakeFromX(targetRotation).Rotator();
	LeaderRotation = FMath::RInterpTo(Fish->GetActorRotation(), LeaderRotation, DeltaTime, Fish->TurnSpeed);
	Fish->SetRotation(LeaderRotation);

	// Set Velocity Vector
	FVector LeaderVelocity = Fish->GetActorForwardVector() * (DeltaTime * Fish->CurrentSpeed);
	Fish->SetVelocity(LeaderVelocity);
}

void SeekState::Flock(float DeltaTime)
{
	// Get a list of Fish neighbors and calculate seperation
	FVector seperation = FVector(0, 0, 0);
	if (Fish->NearbyFriends.IsValidIndex(0))
	{
		TArray<AActor*> neighborList = Fish->NearbyFriends;
		int neighborCount = 0;
		for (int i = 0; i < Fish->NearbyFriends.Num(); i++)
		{
			if (neighborList.IsValidIndex(i))
			{
				seperation += neighborList[i]->GetActorLocation() - Fish->GetActorLocation();
				neighborCount++;
			}

			if (i == Fish->NumNeighborsToEvaluate && i != 0)
			{
				break;
			}
		}
		seperation = ((seperation / neighborCount) * -1);
		seperation.Normalize();
		seperation *= Fish->NeighborSeparation;
	}

	// Maintain distance behind Leader
	FVector distBehind = (Cast<AFlockFish>(Fish->Leader)->GetFlockVelocity() * -1);
	distBehind.Normalize();
	distBehind *= Fish->FollowDist;

	// Calculate all seperation and distance behind Leader into one vector
	FVector LeaderLocation = Fish->Leader->GetActorLocation();
	FVector flockerVelocity = distBehind + LeaderLocation + seperation + Fish->AvoidObstacle();
	FRotator flockerRotation = FRotationMatrix::MakeFromX(flockerVelocity - Fish->GetActorLocation()).Rotator();

	// If fish is too far behind Leader, Speed up 
	float newSpeed = Fish->Speed;
	if (Fish->GetDistanceTo(Fish->Leader) > Fish->DistanceBehindSpeedUpRange)
	{
		// Set Speed
		Fish->CurrentSpeed = FMath::Lerp(Fish->CurrentSpeed, Fish->MaxSpeed, DeltaTime);
	}
	else
	{
		// Set Speed
		Fish->CurrentSpeed = FMath::Lerp(Fish->CurrentSpeed, Fish->Speed, DeltaTime);
	}


	// Set Velocity
	FVector flockVelocity = Fish->GetActorForwardVector() * (DeltaTime * Fish->CurrentSpeed);
	Fish->SetVelocity(flockVelocity);

	// Set Rotation
	FRotator flockRotation = FMath::RInterpTo(Fish->GetActorRotation(), flockerRotation, DeltaTime, Fish->TurnSpeed);
	Fish->SetRotation(flockRotation);
}


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Flee State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void FleeState::Update(float DeltaTime)
{
	Fish->bIsFleeing = true;
	Fish->FleeTarget = Enemy;
	Fish->PreyTarget = nullptr;
	if (Fish->GetDistanceTo(Enemy) >= Fish->FleeDistance)
	{
		Fish->SetState(new SeekState(Fish));
	}
	FleeFromEnemy(DeltaTime);
}

void FleeState::FleeFromEnemy(float DeltaTime)
{
	// Set Speed
	Fish->CurrentSpeed = FMath::Lerp(Fish->CurrentSpeed, Fish->MaxSpeed, (DeltaTime * Fish->FleeAccelerationMultiplier));
	
	// Set Velocity
	FVector fleeVelocity = Fish->GetActorForwardVector() * (DeltaTime * Fish->CurrentSpeed);
	Fish->SetVelocity(fleeVelocity);

	// Set Rotation
	FVector targetRotation = (Fish->GetActorLocation() - Enemy->GetActorLocation()) + Fish->AvoidObstacle();
	FRotator fleeRotation = FRotationMatrix::MakeFromX(targetRotation).Rotator();
	fleeRotation = FMath::RInterpTo(Fish->GetActorRotation(), fleeRotation, DeltaTime, Fish->TurnSpeed);
	Fish->SetRotation(fleeRotation);

}

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Chase State            /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void ChaseState::Update(float DeltaTime)
{
	Fish->bIsFleeing = false;
	Fish->FleeTarget = nullptr;
	Fish->PreyTarget = Prey;
	EatPrey();
	ChasePrey(DeltaTime);
}

void ChaseState::EatPrey()
{
	if (Fish->GetDistanceTo(Prey) < 1000)
	{
		float zLoc = Fish->minZ + FMath::Abs(0.25 * Fish->maxZ);
		Prey->SetActorLocation(FVector(FMath::FRandRange(Fish->minX, Fish->maxX), FMath::FRandRange(Fish->minY, Fish->maxX), zLoc));
		Fish->bIsFull = true;
		Fish->SetState(new SeekState(Fish));
	}
}

void ChaseState::ChasePrey(float DeltaTime)
{
	// Set Speed
	Fish->CurrentSpeed = FMath::Lerp(Fish->CurrentSpeed, Fish->MaxSpeed, (DeltaTime * Fish->ChaseAccelerationMultiplier));

	// Set Velocity
	FVector chaseVelocity = Fish->GetActorForwardVector() * (DeltaTime * Fish->CurrentSpeed);
	Fish->SetVelocity(chaseVelocity);

	// Set Rotation
	FVector seperation = FVector(0, 0, 0);
	if (Fish->NearbyFriends.IsValidIndex(0))
	{
		int neighborCount = 0;
		TArray<AActor*> neighborList = Fish->NearbyFriends;
		for (int i = 0; i < Fish->NumNeighborsToEvaluate; i++)
		{
			if (neighborList.IsValidIndex(i))
			{
				seperation += neighborList[i]->GetActorLocation() - Fish->GetActorLocation();
				neighborCount++;
			}
		}
		seperation = ((seperation / neighborCount) * -1);
		seperation.Normalize();
		seperation *= Fish->NeighborSeparation;
	}

	FVector preyLocation = Prey->GetActorLocation();
	FVector flockerVelocity = ((preyLocation + seperation) - Fish->GetActorLocation()) + Fish->AvoidObstacle();
	FRotator flockerRotation = FRotationMatrix::MakeFromX(flockerVelocity).Rotator();
	FRotator chaseRotation = FMath::RInterpTo(Fish->GetActorRotation(), flockerRotation, DeltaTime, Fish->TurnSpeed);
	Fish->SetRotation(chaseRotation);
}
