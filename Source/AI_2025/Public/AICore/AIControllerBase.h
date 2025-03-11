// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "AIControllerBase.generated.h"

UCLASS()
class AI_2025_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAIControllerBase();	
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void MoveToInternal();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void PauseActiveMovement();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void ResumeActiveMovement();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	
	virtual void OnPossess(APawn* InPawn) override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FAIRequestID CurrentMoveID = FAIRequestID::InvalidRequest;
};

