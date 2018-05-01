// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// We want to use a spring arm to create a natual motion for our camera.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-50.f, 45.f, 0.f));
	SpringArm->TargetArmLength = 1000.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 8.0f;


	// Now we create a camera and attach to our spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	TeamId = FGenericTeamId(1);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (int i{}; i<10; i++)
	{
		DynMats[i] = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(i), this);
		DynMats[i]->SetScalarParameterValue(FName(TEXT("Value")), 0.f);
		GetMesh()->SetMaterial(i, DynMats[i]);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, FString::Printf(TEXT("STAMINA: %f"), Stamina), true);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("SCORE: %f"), Score), true);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("SIZE: %f"), Size), true);
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("SPEED: %f"), Speed), true);

	FrameTime = DeltaTime;
	LungeDirection = GetMesh()->GetComponentRotation();
	LungeDirection += FRotator(0.f, 90.f, 0.f);

	if (!bCanDash) CheckCooldownTimer -= 60.f* DeltaTime;
	if (CheckCooldownTimer<=0.f) {CheckCooldownTimer = 0.f; bCanDash = true;}

	GetMesh()->SetRelativeRotation(FMath::Lerp(FQuat(GetMesh()->GetComponentRotation()), FQuat(FRotator(0.0f, RotationValue, CaughtRotation)), 6.f * DeltaTime));
	GetMesh()->SetRelativeScale3D(FMath::Lerp(FVector(GetMesh()->GetComponentScale()), FVector(1.f, 1.f, 1.f+(Size-1.f)/4.f), 1.f * DeltaTime));

	for (int i{}; i<10; i++)
	{
		DynMats[i]->SetScalarParameterValue(FName(TEXT("Value")), FMath::Lerp((DynMats[i]->K2_GetScalarParameterValue(FName(TEXT("Value")))), ((Size-1.f)/3.f), 1.f*DeltaTime));
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("DeltaTime: %f"), DeltaTime), true);

	if (bDashing)
	{
		//USE OF DELTATIME HERE FOR SOME REASON PRODUCES INCONSISTENT RESULTS WHEN FRAMERATE CHANGES!
		//ONE OF THE THOSE INCONSISTENCIES IS THE DASH BEIGN LONGER THE HIGHER THE FRAMERATE OF THE ENGINE
		DashAlpha = FMath::Lerp(DashAlpha, 1.1f, 5.f * DeltaTime);
		if (DashAlpha >= 1.f)
		{
			bDashing = false;
			DashAlpha = 0.f;
			Speed = PrevSpeed;
			bEnableInput = true;
		}
		Speed += DashValue * DashCurve->GetFloatValue(DashAlpha);
		AddMovementInput(LungeDirection.Vector(), 50.f * DeltaTime);
	}

	if (bRunning == true && Size > 1.f && GetCharacterMovement()->Velocity.Size() != 0)
	{
		ChangeValues((DeltaTime / 10.f) * -1);
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	InputComponent->BindAxis("Run", this, &APlayerCharacter::Run);
	InputComponent->BindAction("Dash", IE_Released, this, &APlayerCharacter::Dash);
	InputComponent->BindAction("Punch", IE_Pressed, this, &APlayerCharacter::Punch);

}

void APlayerCharacter::MoveForward(float MoveAmount)
{
	if (Controller && bEnableInput)
	{
		if (MoveAmount > 0)
		{
			RotationValue = -90.f;
			GetMesh()->SetRelativeRotation(FMath::Lerp(FQuat(GetMesh()->GetComponentRotation()), FQuat(FRotator(0.0f, RotationValue, 0.0f)), 6.f * FrameTime));
			AddMovementInput(GetActorForwardVector(), MoveAmount);
			//PawnMakeNoise(1.f, GetActorLocation(), false);
		}
		else if (MoveAmount < 0)
		{
			RotationValue = 90.f;
			GetMesh()->SetRelativeRotation(FMath::Lerp(FQuat(GetMesh()->GetComponentRotation()), FQuat(FRotator(0.0f, RotationValue, 0.0f)), 6.f * FrameTime));
			AddMovementInput(GetActorForwardVector(), MoveAmount);
			//PawnMakeNoise(1.f, GetActorLocation(), false);
		}
	}
}

void APlayerCharacter::MoveRight(float MoveAmount)
{
	if (Controller && bEnableInput)
	{
		if (MoveAmount > 0)
		{
			RotationValue = 0.f;
			GetMesh()->SetRelativeRotation(FMath::Lerp(FQuat(GetMesh()->GetComponentRotation()), FQuat(FRotator(0.0f, RotationValue, 0.0f)), 6.f * FrameTime));
			AddMovementInput(GetActorRightVector(), MoveAmount);
			//PawnMakeNoise(1.f, GetActorLocation(), false);
		}
		else if (MoveAmount < 0)
		{
			RotationValue = -180.f;
			GetMesh()->SetRelativeRotation(FMath::Lerp(FQuat(GetMesh()->GetComponentRotation()), FQuat(FRotator(0.0f, RotationValue, 0.0f)), 6.f * FrameTime));
			AddMovementInput(GetActorRightVector(), MoveAmount);
			//PawnMakeNoise(1.f, GetActorLocation(), false);
		}
	}
}

void APlayerCharacter::Run(float RunSpeed)
{
	if (Controller && RunSpeed && !bDashing && Stamina >= 0.f  && GetCharacterMovement()->Velocity.Size() != 0)
	{
		Cast<UCharacterMovementComponent>(GetCharacterMovement())->MaxWalkSpeed = Speed * 1.6;
		bRunning = true;
		Stamina -= 80 * FrameTime;
		if (Stamina < 1.f && CheckRunCooldownTimer == 2)
		{
			GetWorldTimerManager().SetTimer(RunTimer, this, &APlayerCharacter::RunCooldown, 1.f, true, 0.f);
		}
	}
	else {
		Cast<UCharacterMovementComponent>(GetCharacterMovement())->MaxWalkSpeed = Speed;
		bRunning = false;
		if (Stamina <= 100.f && bRegainStamina)
		{
			Stamina += 25 * FrameTime;
			if (Stamina > 100.f) Stamina = 100.f;
		}
	}
}

void APlayerCharacter::Dash()
{

	if (DashAlpha == 0.f && !bDashing && !bCanDash)
	{
		PrevSpeed = Speed;
		bDashing = true;
		bCanDash = false;
		bEnableInput = false;
	}
}

void APlayerCharacter::Punch()
{
	//TEMP
}

void APlayerCharacter::ChangeValues(float Value)
{
	if (Size > 1.f && Value < 0.f)
	{
		Size += Value * SizeMultiplier;
		Speed -= Value * SpeedMultiplier;
		Score += Value * ScoreMultiplier;
	}
	else if (Size < 3.f && Value > 0.f)
	{
		Size += Value * SizeMultiplier;
		Speed -= Value * SpeedMultiplier;
		Score += Value * ScoreMultiplier;
	}

	if (Score > 100000.f) Score = 100000.f;
}

/*void APlayerCharacter::DashCooldown()
{
	if (CheckCooldownTimer > 1.f)
	{
		bCooldown = true;
		CheckCooldownTimer -= 1.f;
	}
	else if (CheckCooldownTimer <= 1.f)
	{
		bCooldown = false;
		CheckCooldownTimer = 6.f;
		//GetWorldTimerManager().ClearTimer(Timer);
	}
}

void APlayerCharacter::RunCooldown()
{
	if (CheckRunCooldownTimer > 0)
	{
		Stamina = 0.f;
		bRunning = false;
		CheckRunCooldownTimer--;
		bRegainStamina = false;
	}
	else if (CheckRunCooldownTimer <= 0)
	{
		bRegainStamina = true;
		CheckRunCooldownTimer = 2;
		GetWorldTimerManager().ClearTimer(RunTimer);
	}
}*/

void APlayerCharacter::Caught()
{
	if (CaughtCooldown > 1)
	{
		CaughtCooldown--;
		Lives--;
	}
	else if (CaughtCooldown <= 1)
	{
		GetWorldTimerManager().SetTimer(RespawnTimer, this, &APlayerCharacter::Respawn, 0.4f, true, 0.f);
		GetWorldTimerManager().ClearTimer(CaughtTimer);
		SetActorLocation(FVector(-450.f, -1000.f, 0.f));
		CaughtCooldown = 3;
		CaughtRotation = 0.f;
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void APlayerCharacter::Respawn()
{
	if (RespawnCooldown > 1)
	{
		GetMesh()->ToggleVisibility();
		RespawnCooldown--;
	}
	else if (RespawnCooldown <= 1)
	{
		GetWorldTimerManager().ClearTimer(RespawnTimer);
		RespawnCooldown = 8;
		GetMesh()->SetVisibility(true);
	}
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return TeamId;
}
