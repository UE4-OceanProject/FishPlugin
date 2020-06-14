// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

#include "FlockFish.h"
#include "FishState.h"
#include "FishManager.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


#define COLLISION_TRACE ECC_GameTraceChannel4

AFlockFish::AFlockFish(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("FishMesh"));
	RootComponent = BaseMesh;

	// Fish interaction sphere
	FishInteractionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("FishInteractionSphere"));
	FishInteractionSphere->SetSphereRadius(10);
	FishInteractionSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	FishInteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlockFish::OnBeginOverlap);
	FishInteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AFlockFish::OnEndOverlap);

	if (bIsLeader == true)
	{
		SpawnTarget();
	}

	//Defaults
	FollowDist = 50.0;
	Speed = 1200.0;
	MaxSpeed = 2400.0;
	TurnSpeed = 3.0;
	TurnFrequency = 1.0;
	TurnFrequency = 1.0;
	HungerResetTime = 20.0;
	DistanceBehindSpeedUpRange = 3000.0;
	SeparationDistanceMultiplier = 0.75;
	FleeDistanceMultiplier = 5.0;
	FleeAccelerationMultiplier = 2.0;
	ChaseAccelerationMultiplier = 2.0;
	SeekDecelerationMultiplier = 1.0;
	AvoidForceMultiplier = 1.0;
	AvoidanceForce = 20000.0;
	UnderwaterMin = FVector(-40000, -40000, -9000);
	UnderwaterMax = FVector(40000, 40000, -950);
	CustomZSeekMin = 0.0;
	CustomZSeekMax = 0.0;
	NumNeighborsToEvaluate = 5.0;
	UpdateEveryTick = 0.0;
	DebugMode = true;
	FleeDistance = 0.0;
	NeighborSeparation = 300.0;
	CurrentSpeed = Speed;
	bIsFleeing = false;
	bIsFull = false;
	underwaterBoxLength = 10000.0;
	AvoidanceDistance = 5000.0;
	CurrentVelocity = FVector(0, 0, 0);
	CurrentRotation = FRotator(0, 0, 0);
	TurnTimer = 0.0;
	bIsSetup = false;
	HungerTimer = 0.0;
	UpdateTimer = 0.0;
	bHasFishManager = false;
}

void AFlockFish::Tick(float DeltaTime)
{

	// Setup the fish (happens on first tick only)
	// NOTE TO SELF: consider creating a beginplay event that does this stuff (although beginplay is buggy as hell x.x)
	Setup();

	// If debug mode true, draw interaction sphere and avoiddistance
	Debug();

	// Move Bounds based on location of FishManager (if applicable)
	MoveBounds(DeltaTime);

	// Manage Timers (hungerTimer, updateTimer, and turnTimer)
	ManageTimers(DeltaTime);

	// Decide what state to be in
	ChooseState();

	// Update curVelocity and curRotation through current state
	UpdateState(DeltaTime);

	// Update world rotation and velocity
	this->SetActorRotation(CurrentRotation);
	this->AddActorWorldOffset(CurrentVelocity);

	Super::Tick(DeltaTime);
}

void AFlockFish::Debug()
{
	if (DebugMode)
	{
		FVector actorLocation = this->GetActorLocation();
		FVector forwardVector = (this->GetActorForwardVector() * AvoidanceDistance) + actorLocation;
		FVector forwardVector2 = (this->GetActorForwardVector() * (AvoidanceDistance * 0.1)) + actorLocation;

		DrawDebugLine(
			GetWorld(),
			actorLocation,
			forwardVector,
			FColor::Magenta,
			false, -1, 0,
			10
			);

		FColor indicatorColor = FColor::Cyan;
		if (NearbyEnemies.IsValidIndex(0))
		{
			indicatorColor = FColor::Red;
		}
		else if (NearbyPrey.IsValidIndex(0) && bIsFull == false)
		{
			indicatorColor = FColor::Green;
		}
		DrawDebugSphere(
			GetWorld(),
			actorLocation,
			FishInteractionSphere->GetScaledSphereRadius(),
			20,
			indicatorColor
			);
		DrawDebugLine(
			GetWorld(),
			actorLocation,
			forwardVector2,
			indicatorColor,
			true, 10, 0,
			20
			);
	}

}

FVector AFlockFish::AvoidObstacle()
{
	FVector actorLocation = this->GetActorLocation();
	FVector forwardVector = (this->GetActorForwardVector() * AvoidanceDistance) + actorLocation;

	FHitResult OutHitResult;
	FCollisionQueryParams Line(FName("Collision param"), true);
	bool const bHadBlockingHit = GetWorld()->LineTraceSingleByChannel(OutHitResult, actorLocation, forwardVector, COLLISION_TRACE, Line);
	FVector returnVector = FVector(0, 0, 0);
	float distanceToBound = distanceToBound = (this->GetActorLocation() - OutHitResult.ImpactPoint).Size();
	if (bHadBlockingHit)
	{
		if (OutHitResult.ImpactPoint.Z > this->GetActorLocation().Z + FishInteractionSphere->GetScaledSphereRadius())
		{	
			returnVector.Z += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * -1;
		}
		else if (OutHitResult.ImpactPoint.Z < this->GetActorLocation().Z - FishInteractionSphere->GetScaledSphereRadius())
		{
			returnVector.Z += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * 1;
		}

		if (OutHitResult.ImpactPoint.X > this->GetActorLocation().X)
		{
			returnVector.X += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * -1;
		}
		else if (OutHitResult.ImpactPoint.X < this->GetActorLocation().X)
		{
			
			returnVector.X += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * 1;
		}

		if (OutHitResult.ImpactPoint.Y > this->GetActorLocation().Y)
		{
			returnVector.Y += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * -1;
		}
		else if (OutHitResult.ImpactPoint.Y < this->GetActorLocation().Y)
		{

			returnVector.Y  += (1 / (distanceToBound * (1 / AvoidForceMultiplier))) * 1;
		}

		returnVector.Normalize();
		FVector avoidance = returnVector * AvoidanceForce;
		return avoidance;
	}
	return FVector(0, 0, 0);
}

void AFlockFish::UpdateState(float DeltaTime)
{
	if (UpdateEveryTick == 0)
	{
		CurrentState->Update(DeltaTime);
	}
	else if (UpdateTimer >= UpdateEveryTick)
	{
		CurrentState->Update(DeltaTime);
		UpdateTimer = 0;
	}
}


void AFlockFish::OnBeginOverlap(UPrimitiveComponent* ActivatedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Is overlapping with enemy?
	if (EnemyTypes.Find(OtherActor->GetClass()) >= 0)
	{	
		NearbyEnemies.Add(OtherActor);
	}
	else if (PreyTypes.Find(OtherActor->GetClass()) >= 0)
	{	
		if (OtherActor->GetClass() == this->GetClass())
		{
			if (!Cast<AFlockFish>(OtherActor)->bIsLeader)
			{
				NearbyPrey.Add(OtherActor);
			}
		}
		else
		{
			NearbyPrey.Add(OtherActor);
		}
	}
	else if (OtherActor->GetClass() == this->GetClass())
	{
		NearbyFriends.Add(OtherActor);
	}
}

void AFlockFish::OnEndOverlap(UPrimitiveComponent* ActivatedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (NearbyEnemies.Find(OtherActor) >= 0)
	{
		NearbyEnemies.Remove(OtherActor);
	}
	else if (NearbyPrey.Find(OtherActor) >= 0)
	{
		NearbyPrey.Remove(OtherActor);
	}
	else if (NearbyFriends.Find(OtherActor) >= 0)
	{
		NearbyFriends.Remove(OtherActor);
	}
}

void AFlockFish::ChooseState()
{
	if (NearbyEnemies.IsValidIndex(0))
	{
		CurrentState = new FleeState(this, NearbyEnemies[0]);
	}
	else if (NearbyPrey.IsValidIndex(0) && bIsFull == false)
	{
		CurrentState = new ChaseState(this, NearbyPrey[0]);
	}
	else
	{
		CurrentState = new SeekState(this);
	}
}

void AFlockFish::ManageTimers(float DeltaTime)
{
	// Check if the fish is full or not
	if (bIsFull)
	{
		HungerTimer += DeltaTime;

		if (HungerTimer >= HungerResetTime)
		{
			HungerTimer = 0.0f;
			bIsFull = false;
		}
	}

	// decide on wether to move target this tick
	if (TurnTimer >= TurnFrequency && bIsLeader == true)
	{
		SpawnTarget();
		TurnTimer = 0.0;
	}

	UpdateTimer += DeltaTime;
	TurnTimer += DeltaTime;
}



void AFlockFish::MoveBounds(float DeltaTime)
{
	if (bHasFishManager)
	{
		FVector fishManagerPosition = FishManager->GetActorLocation();
		maxX = fishManagerPosition.X + underwaterBoxLength;
		minX = fishManagerPosition.X - underwaterBoxLength;
		maxY = fishManagerPosition.Y + underwaterBoxLength;
		minY = fishManagerPosition.Y - underwaterBoxLength;

		FVector ActorLocation = this->GetActorLocation();
		if (ActorLocation.Z > UnderwaterMax.Z)
		{	
			ActorLocation.Z = UnderwaterMin.Z + FMath::Abs((0.999 * UnderwaterMax.Z));
		}
		else if (ActorLocation.Z < UnderwaterMin.Z)
		{
			ActorLocation.Z = UnderwaterMin.Z + FMath::Abs((0.001 * UnderwaterMax.Z));
		}

		if (ActorLocation.X > maxX)
		{
			ActorLocation.X = minX + FMath::Abs((0.1 * maxX));
		}
		else if (ActorLocation.X < minX)
		{
			ActorLocation.X = maxX - FMath::Abs((0.1 * maxX));
		}

		if (ActorLocation.Y > maxY)
		{
			ActorLocation.Y = minY + FMath::Abs((0.1 * maxY));
		}
		else if (ActorLocation.Y < minY)
		{
			ActorLocation.Y = maxY - FMath::Abs((0.1 * maxY));
		}

		this->SetActorLocation(ActorLocation);
	}
}

void AFlockFish::SpawnTarget()
{
	TargetLocation = FVector(FMath::FRandRange(minX, maxX), FMath::FRandRange(minY, maxY), FMath::FRandRange(minZ, maxZ));
}


void AFlockFish::Setup()
{
	// Setup the enemies list on first tick
	if (bIsSetup == false)
	{
		maxX = UnderwaterMax.X;
		maxY = UnderwaterMax.Y;
		minX = UnderwaterMin.X;
		minY = UnderwaterMin.Y;

		InteractionSphereRadius = FishInteractionSphere->GetScaledSphereRadius();

		if (CustomZSeekMax == 0.0)
		{
			minZ = UnderwaterMin.Z;
			maxZ = UnderwaterMax.Z;
		}
		else
		{
			minZ = CustomZSeekMin;
			maxZ = CustomZSeekMax;
		}

		FleeDistance = FishInteractionSphere->GetScaledSphereRadius() * FleeDistanceMultiplier;
		NeighborSeparation = FishInteractionSphere->GetScaledSphereRadius() * SeparationDistanceMultiplier;
		AvoidanceDistance = FishInteractionSphere->GetScaledSphereRadius() * 2;

		CurrentState = new SeekState(this);

		TArray<AActor*> aFishManagerList;
		UGameplayStatics::GetAllActorsOfClass(this, AFishManager::StaticClass(), aFishManagerList);
		if (aFishManagerList.Num() > 0)
		{
			bHasFishManager = true;
			FishManager = aFishManagerList[0];
		}

		// Setup Neighbors
		if (!FishManager)
		{
			TArray<AActor*> aNeighborList;
			UGameplayStatics::GetAllActorsOfClass(this, NeighborType, aNeighborList);
			Neighbors.Append(aNeighborList);
			for (int i = 0; i < Neighbors.Num(); i++)
			{
				if (Cast<AFlockFish>(Neighbors[i])->bIsLeader)
				{
					Leader = Neighbors[i];
					break;
				}
			}
		}
		//nearbyFriends.Append(neighbors);


		bIsSetup = true;
	}
}
