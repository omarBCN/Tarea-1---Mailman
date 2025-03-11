// Fill out your copyright notice in the Description page of Project Settings.

#include "Pathfinding/NodeComponent.h"

UNodeComponent::UNodeComponent()
{
	// Sets collisions to detect clicks (for raycasting)
	SetCollisionProfileName(TEXT("BlockAll"));
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
