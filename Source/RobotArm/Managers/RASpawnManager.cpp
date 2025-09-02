// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RASpawnManager.h"
#include "Machines/RAConveyor.h"
#include "RATestActor.h"

ARASpawnManager::ARASpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ARASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &ARASpawnManager::SpawnProduct, 3.0f, true);
}

void ARASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARASpawnManager::SpawnProduct()
{
	if (TargetConveyor)
	{
		TargetConveyor->ProductSpawn(ProductClass);
	}
}

