// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//Custom headers


//Required headers
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE4SUMOGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UE4SUMO_API AUE4SUMOGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:


};
