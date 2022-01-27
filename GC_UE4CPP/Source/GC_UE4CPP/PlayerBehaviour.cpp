#include "PlayerBehaviour.h"
#include "FoodBehaviour.h"
#include "PhysXInterfaceWrapperCore.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"


// Sets default values
APlayerBehaviour::APlayerBehaviour()
{
	Speed = 400.0f;
	IsHandEmpty = true;
	
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 700; // Set the distance between the camera and the character	
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 540, 0);

	// Create a Sphere
	SphereDetection = CreateDefaultSubobject<USphereComponent>(TEXT("SphereDetection"));
}

// Called when the game starts or when spawned
void APlayerBehaviour::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void APlayerBehaviour::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!CurrentVelocity.IsZero())
    {
        const FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
        SetActorLocation(NewLocation);
    }
}

// Called to bind functionality to input
void APlayerBehaviour::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Respond every frame to the values of our two movement axes, MoveX and MoveY.


    PlayerInputComponent->BindAxis("MoveX", this, &APlayerBehaviour::Move_XAxis);
    PlayerInputComponent->BindAxis("MoveY", this, &APlayerBehaviour::Move_YAxis);


	//camera input
	check(PlayerInputComponent);

	//Bind the key with the methods
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerBehaviour::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerBehaviour::LookUpAtRate);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerBehaviour::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Zoom", this, &APlayerBehaviour::Zoom);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerBehaviour::InteractFood);
}

void APlayerBehaviour::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerBehaviour::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerBehaviour::Move_XAxis(float Rate)
{
	AddMovementInput(GetFollowCamera()->GetForwardVector(), Rate * Speed);
}

void APlayerBehaviour::Move_YAxis(float Rate)
{
	AddMovementInput(GetFollowCamera()->GetRightVector(), Rate * Speed);
}

//Set the distance between the player and the camera
void APlayerBehaviour::Zoom(float Rate)
{
	CameraBoom->TargetArmLength -= (GetWorld()->DeltaTimeSeconds * Rate * 4000);
}

//Allow to interact with the food
void APlayerBehaviour::InteractFood()
{
	const FVector Start = GetActorLocation();
	const FVector End = GetActorLocation() * 200;
	ActorsToIgnore.Add(this);

	// Create a sphere trace around the player and add inside an array all actors hit by the sphere trace
	Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, SphereRange,
		UEngineTypes::ConvertToTraceType(ECC_Camera), true, ActorsToIgnore,
		EDrawDebugTrace::None,HitArray, true, FLinearColor::Gray,FLinearColor::Blue, 60.0f);

	if(Hit)
	{
		for(const FHitResult HitResult : HitArray)
		{
			//To see the actors hit (debug only)
			/*GEngine->AddOnScreenDebugMessage(-1, 60.0f, FColor::Orange,
			FString::Printf(TEXT("Hit: %s"), *HitResult.Actor->GetName()));*/
			
			Result = Cast<AFoodBehaviour>(HitResult.Actor); // Check if the the actor hit is a FoodBehaviour actor

			if(Result != nullptr && IsHandEmpty == true)
			{
				USkeletalMeshComponent* PlayerMesh = GetMesh(); // Get the SkeletalMesh of the Player
				HitResult.Actor->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Fist_RSocket")); // Attach the food to the right hand
				HitResult.Actor->SetActorRelativeScale3D(FVector(0.025f, 0.025f, 0.025f)); // Set a smaller size to the food
				Speed = Speed / 2.0f;
				IsHandEmpty = false;
			}
		}
	}
}

