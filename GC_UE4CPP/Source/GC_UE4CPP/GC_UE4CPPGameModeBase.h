#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GC_UE4CPPGameModeBase.generated.h"

class AEnemy;
class AInGameHUD;

UCLASS()
class GC_UE4CPP_API AGC_UE4CPPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AGC_UE4CPPGameModeBase();

	void WinLoseCondition();
	
	// variable for food counting (use in Chest.cpp and Widget.cpp)
	float FoodCounter = 0;
	bool bIsGameFinished = false;
	bool bIsTouched = false;

	int NPCCount;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> Enemy;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> FoodToSpawn;

	AActor* EnemySpawn;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle TimerHandle;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnNPC();

	void DelayedSpawn();

};
