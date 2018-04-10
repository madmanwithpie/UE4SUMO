// Fill out your copyright notice in the Description page of Project Settings.

#include "BTConeCheck_Service.h"
#include "PlayerCharacter.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

UBTConeCheck_Service::UBTConeCheck_Service(const FObjectInitializer & ObjectInitializer)
{
	NodeName = "Cone Check Pawn";
}

void UBTConeCheck_Service::TickNode(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	///Initializing all the nessecarry pointers we will be using later
	AEmployees_AI_Controller* AIController = Cast<AEmployees_AI_Controller>(OwnerComp.GetAIOwner());
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(AIController->GetPawn());
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComp();


	//Check to see if AIController, PlayerCharacter and EnemyCharacter return a valid pointer value
	if (AIController && PlayerCharacter && EnemyCharacter)
	{			

		//Get a vector line between player
		PlayerLocation = PlayerCharacter->GetActorLocation();
		EnemyLocation = EnemyCharacter->GetActorLocation();
		PlayerToEnemyLine = PlayerLocation - EnemyLocation;

		//Normalize vector in order to calculate the angle later
		PlayerToEnemyLine.Normalize();

		//Calculate the dotproduct and the angle between enemy forward vector and the line from player to enemy
		DotProduct = FVector::DotProduct(PlayerToEnemyLine, EnemyCharacter->GetActorForwardVector());
		Radian = FMath::Acos(DotProduct);
		Angle = FMath::RadiansToDegrees(Radian);

		  //Set a limit to the angle, in order to give the enemy a reasonable sight radius
		  if (Angle < 120.f)
		  {
			  const FName TraceTag("PawnTraceTag");
			  //Set up Ray Tracing parameters
			  FCollisionQueryParams PRT_TraceParams(FName(TEXT("")), false,EnemyCharacter);
			  PRT_TraceParams.TraceTag = TraceTag;
			  GetWorld()->DebugDrawTraceTag = TraceTag;
			  //Create a varible to store hit result values
			  FHitResult HitResult(ForceInit);
			  //Start tracing from enemy to player and store to the hit result variable
			  bool PawnRayTrace = GetWorld()->LineTraceSingleByChannel(
				  HitResult,
				  EnemyLocation,
				  PlayerLocation,
				  ECollisionChannel::ECC_WorldDynamic,	
				  PRT_TraceParams
			  );
			     //If the hit target is the player, set AI_State to Chase
			     if (HitResult.GetActor()==PlayerCharacter)
			     {				 
					 BlackboardComp->SetValueAsEnum(AI_State_BBKey.SelectedKeyName, static_cast<uint8>(EAI_State::AI_ChasePlayer));
					 BlackboardComp->SetValueAsObject(Target.SelectedKeyName, HitResult.GetActor());
					 LastSeenLocation_VECTOR = PlayerCharacter->GetActorLocation();
					 BlackboardComp->SetValueAsVector(LastSeenLocation.SelectedKeyName, LastSeenLocation_VECTOR);
					// UE_LOG(LogTemp, Warning, TEXT("IN_VISION_FIELD | RAY_CAST_SUCCESSFUL | Updating AIRemember = true"))
					 AIRemember = true;
				 }
				 //If not, set AI_State to Find Player
				 else if (AIRemember)
				 {
				   //UE_LOG(LogTemp, Warning, TEXT("IN_VISION_FIELD | RAY_CAST_LOST_VISION | AIRemember = true"))
				   //UE_LOG(LogTemp, Warning, TEXT("LastSeenLocation: %s | EnemyCharacter: %s"), *(LastSeenLocation_VECTOR.ToString()), *(EnemyCharacter->GetActorLocation().ToString()))
				   ///Get a distance between the Enemy and the Last Seen Location of Player and make sure that Enemy get's as close as possible to that location before it can forget Player location
					 BlackboardComp->SetValueAsEnum(AI_State_BBKey.SelectedKeyName, static_cast<uint8>(EAI_State::AI_PlayerLastSeen));
					 LastSeenDistance = LastSeenLocation_VECTOR - EnemyCharacter->GetActorLocation();
					// UE_LOG(LogTemp, Error, TEXT("LastSeenDistance: %f"), LastSeenDistance.Size())
					 if (LastSeenDistance.Size() < 15.f)
					 {
						 AIRemember = false;
					 }
				 } 
				 else if (AIRemember == false)
				 {
					// UE_LOG(LogTemp, Warning, TEXT("IN_VISION_FIELD | RAY_CAST_SUCCESSFUL_ATLEAST_ONCE | AIRemember = false"))
					 BlackboardComp->SetValueAsEnum(AI_State_BBKey.SelectedKeyName, static_cast<uint8>(EAI_State::AI_FindPlayer));
				 }		 
		  }
		  else
		  {
			  ///Perform same checks if Player is out of Cone vision
			  if (AIRemember)
			  {
				//  UE_LOG(LogTemp, Warning, TEXT("OUT_OF_VISION_FIELD | RAY_CAST_NEVER_HAPPENED | AIRemember = True"))
				//  UE_LOG(LogTemp, Warning, TEXT("LastSeenLocation: %s | EnemyCharacter: %s"),*(LastSeenLocation_VECTOR.ToString()),*(EnemyCharacter->GetActorLocation().ToString()))
				  BlackboardComp->SetValueAsEnum(AI_State_BBKey.SelectedKeyName, static_cast<uint8>(EAI_State::AI_PlayerLastSeen));
				  LastSeenDistance = LastSeenLocation_VECTOR - EnemyCharacter->GetActorLocation();
				//  UE_LOG(LogTemp, Error, TEXT("LastSeenDistance: %f"), LastSeenDistance.Size())
				  if (LastSeenDistance.Size() < 15.f)
				  {
					  AIRemember = false;
				  }
			  }
			  else if (AIRemember == false)
			  {
				//  UE_LOG(LogTemp, Warning, TEXT("OUT_OF_VISION_FIELD | RAY_CAST_NEVER_HAPPENED | AIRemember = False"))
				  BlackboardComp->SetValueAsEnum(AI_State_BBKey.SelectedKeyName, static_cast<uint8>(EAI_State::AI_FindPlayer));
			  }
		  }
	  ///When done, clean up PawnSensingComponent (Or maybe use later for OnHearNoise event?)
	}
}