// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//Custom headers
#include "PlayerCharacter.h"

//Engine specific headers
#include "Runtime/Engine/Classes/Components/ShapeComponent.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

//Required headers
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WalkSpeedBoost.generated.h"

class APlayerCharacter;
UCLASS()
class UE4SUMO_API AWalkSpeedBoost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWalkSpeedBoost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere)
		UShapeComponent* Box;

	UPROPERTY(EditDefaultsOnly, Category = Curve)
		UCurveFloat* Curve;

	UPROPERTY(EditDefaultsOnly, Category = Curve)
		UCurveFloat* CurveFloating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundBase* SpeedBoostSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundBase* SodaSound;

	APlayerCharacter* Player = nullptr;

	float Alpha = 0.f;

	bool bPickup = false;

	FTimerHandle TimerHandle;

	FTimerHandle DelayTimer;

	unsigned int Repeats = 5;

	bool bUp = true;

	float Beta = 0.f;
private:
	UFUNCTION()
		void OnPlayerEnterPickupBox(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	void SpeedBoost();

	void DashDelay();
};
