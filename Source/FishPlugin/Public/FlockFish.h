// For copyright see LICENSE in EnvironmentProject root dir, or:
//https://github.com/UE4-OceanProject/OceanProject/blob/Master-Environment-Project/LICENSE

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FishManager.h"
#include "FlockFish.generated.h"


/**
 * 
 */
UCLASS()
class FISHPLUGIN_API AFlockFish : public APawn
{
	GENERATED_BODY()

public:
	AFlockFish(const class FObjectInitializer& ObjectInitializer);

	// Current state of the Fish
	class FishState* CurrentState;

	/** Fish interaction sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* FishInteractionSphere;

	// Is this fish a leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	bool bIsLeader;

	// Enemies that fish will flee from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	TArray<UClass*> EnemyTypes;

	// Prey that the fish will attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	TArray<UClass*> PreyTypes;

	// Neighbor type of the fish ( typically itself, but needed for code atm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	UClass* NeighborType;

	// Distance to that fish will try to remain behind leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float FollowDist;

	// Default Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float Speed;

	// Max Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float MaxSpeed;

	// Speed at which the fish turns (Try to have predators be slightly faster)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float TurnSpeed;

	// Frequency of turning ( turn every "turnFrequency" seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float TurnFrequency;

	// time after eating until it is hungry again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float HungerResetTime;

	// Go faster when fish is this distance behind leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float DistanceBehindSpeedUpRange;

	// Multiplies With Radius of Fish Interaction Sphere for neighbor seperation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float SeparationDistanceMultiplier;

	// Multiplies With Radius of Fish Interaction Sphere for Flee Distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float FleeDistanceMultiplier;

	// Multiplies with delta time since last tick when lerping to max speed (While Fleeing)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float FleeAccelerationMultiplier;

	// Multiplies with delta time since last tick when lerping to max speed (While Chasing)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float ChaseAccelerationMultiplier;

	// Multiplies with delta time since last tick when lerping to regular speed (While Seeking)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float SeekDecelerationMultiplier;

	// Avoid Distance Multiplier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float AvoidForceMultiplier;

	// Avoidance force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Settings")
	float AvoidanceForce;

	// Player the fish will avoid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	UClass* PlayerType;

	// volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	FVector UnderwaterMin;

	// volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	FVector UnderwaterMax;

	// Custom Z range for targeting (NULL will use full range of min/max)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	float CustomZSeekMin;

	// Custom Z range for targeting (NULL will use full range of min/max)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	float CustomZSeekMax;

	// Max number of neighbors to evaluate on a Tick (0 for all neighbors)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization")
	float NumNeighborsToEvaluate;

	// When set to 0, update every tick, otherwise update after specified time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Optimization")
	float UpdateEveryTick;

	// This is the target the fish will path to
	FVector TargetLocation;

	float InteractionSphereRadius;

	// Run fish in debug mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TESTING")
	bool DebugMode;

	// Flee distance
	// NOTE: CURRENTLY SET IN CODE
	float FleeDistance;

	// Weight to multiply with cohesion
	// NOTE: CURRENTLY SET IN CODE
	float NeighborSeparation;

	// current speed
	float CurrentSpeed;

	// This fish's leader
	AActor* Leader;

	// current prey in world
	//TArray<AActor*> prey;

	// Enemies that fish will flee from
	//TArray<AActor*> enemies;

	// These are the fish's flocking buddies
	TArray<AActor*> Neighbors;

	// Nearby Enemies
	TArray<AActor*> NearbyEnemies;

	// Nearby Prey
	TArray<AActor*> NearbyPrey;

	// Nearby Friends (non-threatening fish and neighbors)
	TArray<AActor*> NearbyFriends;

	// Flee target
	AActor* FleeTarget;

	// current prey
	AActor* PreyTarget;

	// is fish currently fleeing
	// Current state of the Fish
	bool bIsFleeing;

	// is the fish currently full?
	bool bIsFull;

	/* bounds of underwater box*/
	float underwaterBoxLength;

	FVector AvoidObstacle();

	float AvoidanceDistance;

	// Max bounds of box
	float maxX;
	float maxY;
	float maxZ;
	float minX;
	float minY;
	float minZ;

	// Getters and Setters
	FVector GetSeekTarget()
	{
		return TargetLocation;
	}

	void SetState(class FishState* NewState)
	{
		CurrentState = NewState;
	}

	void SetRotation(FRotator NewRotation)
	{
		CurrentRotation = NewRotation;
	};

	void SetVelocity(FVector NewVelocity)
	{
		CurrentVelocity = NewVelocity;
	};

	FRotator GetFlockRotation()
	{
		return CurrentRotation;
	};

	FVector GetFlockVelocity()
	{
		return CurrentVelocity;
	};

protected:

	/** Static mesh component */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Components")
    class UStaticMeshComponent* BaseMesh;
	
	/** Overrided tick function */
	virtual void Tick(float DeltaTime) override;

	/* setup the game on first tick */
	void Setup();

	/* Manage the various necessary timers*/
	void ManageTimers(float DeltaTime);

	/* Choose what state to be in*/
	void ChooseState();

	/* Move Bounds */
	void MoveBounds(float DeltaTime);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* ActivatedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* ActivatedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	/* update the current state*/
	void UpdateState(float DeltaTime);

	/* places the target randomly within the postprocessvolume*/
	void SpawnTarget();

	/* Does a bunch of debug stuff if debug mode is active*/
	void Debug();

	// current velocity
	FVector CurrentVelocity;

	// current rotation
	FRotator CurrentRotation;

	// turning timer
	float TurnTimer;

	// Are the array's setup?
	bool bIsSetup;

	// current hunger timer
	float HungerTimer;

	// Tick Timer
	float UpdateTimer;

	// player holder
	//AActor* player;

	// has fish manager?
	bool bHasFishManager;

	// fish manager
	AActor* FishManager;


};
