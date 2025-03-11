// Fill out your copyright notice in the Description page of Project Settings.


#include "AICore/AIControllerBase.h"

#include "AICore/AIDataComponent.h"
#include "Navigation/PathFollowingComponent.h"


// Sets default values
AAIControllerBase::AAIControllerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAIControllerBase::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	//Reset CurrentMoveID
	CurrentMoveID = FAIRequestID::InvalidRequest;

	switch (Result.Code)
	{
	case EPathFollowingResult::Success:
		UE_LOG(LogTemp, Display, TEXT("Target Reached"));
		break;
	case EPathFollowingResult::Blocked:
	case EPathFollowingResult::OffPath:
	case EPathFollowingResult::Aborted:
	case EPathFollowingResult::Invalid:
		UE_LOG(LogTemp, Display, TEXT("I failed following path"));
		break;
	default: ;
	}
}


void AAIControllerBase::MoveToInternal()
{
	if(UAIDataComponent* AIDataComponent = GetPawn()->FindComponentByClass<UAIDataComponent>())
	{
		if(AActor* Target = AIDataComponent->GetNextTarget())
		{
			if (!CurrentMoveID.IsValid())
			{
				FAIMoveRequest Request;
				Request.SetAcceptanceRadius(50.f)
				.SetCanStrafe((true))
				.SetUsePathfinding(true)
				.SetAllowPartialPath(true)
				.SetNavigationFilter(AIDataComponent->GetNavQueryFilterClass())
				.SetGoalActor(AIDataComponent->GetNextTarget());

				const FPathFollowingRequestResult MoveRequest = MoveTo(Request);
				if (MoveRequest.Code == EPathFollowingRequestResult::Failed || MoveRequest.Code == EPathFollowingRequestResult::AlreadyAtGoal)
					return;
				CurrentMoveID = MoveRequest.MoveId;	
			}
		}
	}
}

void AAIControllerBase::PauseActiveMovement()
{
	PauseMove(CurrentMoveID);
}

void AAIControllerBase::ResumeActiveMovement()
{
	ResumeMove(CurrentMoveID);
}

void AAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AttachToPawn(InPawn);	//Attaches controller to possessed Pawn

	// if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this,0))
	//MoveToInternal();
}	

// Called every frame
void AAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

