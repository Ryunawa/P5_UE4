// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_StoreFood.h"
#include "AIController.h"
#include "Enemy.h"
#include "FoodSpot.h"
#include "BehaviorTree/BlackboardComponent.h"



EBTNodeResult::Type UBTTask_StoreFood::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemy* NPC = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	UBlackboardComponent* BB = OwnerComp.GetAIOwner()->GetBlackboardComponent();
	AFoodSpot* Spot = Cast<AFoodSpot>(BB->GetValueAsObject("SelectedSpot"));	
	if(Spot->FoodSnapped) return EBTNodeResult::Failed;
	BB->SetValueAsObject("SelectedSpot", nullptr);
	if(!NPC->bIsCarryingFood) return EBTNodeResult::Succeeded;
	NPC->StorePlate(Spot);
	BB->SetValueAsBool("bIsFoodStored",true);
	return EBTNodeResult::Succeeded;
}