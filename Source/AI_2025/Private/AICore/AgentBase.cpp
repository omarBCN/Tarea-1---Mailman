// Fill out your copyright notice in the Description page of Project Settings.


#include "AICore/AgentBase.h"
#include "AICore/AIDataComponent.h"
#include "AICore/AIControllerBase.h"


// Sets default values
AAgentBase::AAgentBase()
{
	PrimaryActorTick.bCanEverTick = true;
	 
	//Assigns AI Controller
	AIControllerClass = AAIControllerBase::StaticClass();
	//Sets AutoPossess mode
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	AIDataComponent = CreateDefaultSubobject<UAIDataComponent>(TEXT("AIDataComponent"));
}



