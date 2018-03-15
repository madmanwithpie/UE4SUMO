// Fill out your copyright notice in the Description page of Project Settings.

#include "Employees_AI_Controller.h"


// Sets default values
AEmployees_AI_Controller::AEmployees_AI_Controller()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEmployees_AI_Controller::BeginPlay()
{
	Super::BeginPlay();
	AEnemyCharacter* GetControlledEnemy = Cast<AEnemyCharacter>(GetPawn());
	if (GetControlledEnemy != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("C++ controller has now possesed: %s"), (*GetControlledEnemy->GetName()))
	} else UE_LOG(LogTemp, Warning, TEXT("C++ controller has not possesed anything!"), (*GetControlledEnemy->GetName()))
}

// Called every frame
void AEmployees_AI_Controller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
