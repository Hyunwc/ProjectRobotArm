// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RAMachineBase.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

ARAMachineBase::ARAMachineBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

}

void ARAMachineBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARAMachineBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

