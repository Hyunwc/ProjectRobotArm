// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RARobotArm.h"
#include "ControlRigComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"


ARARobotArm::ARARobotArm()
{
	PrimaryActorTick.bCanEverTick = true;

	EndEffectorName = "Robot1_End_ctrl";
	GrabSpeed = 0.5f;

	ControlRigComponent = CreateDefaultSubobject<UControlRigComponent>(TEXT("ControlRig"));
	ControlRigComponent->SetupAttachment(Root);

	Skeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal"));
	Skeletal->SetupAttachment(Root);

	EndEffectorScene = CreateDefaultSubobject<USceneComponent>(TEXT("EndEffectorScene"));
	EndEffectorScene->SetupAttachment(Root);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(Root);

	CurrentState = EActionState::Idle;

}

void ARARobotArm::BeginPlay()
{
	Super::BeginPlay();

	StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	ReturnTransform = StartTransform;
}

void ARARobotArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	FTransform EffectorTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

	EndEffectorScene->SetWorldTransform(EffectorTransform);

	UpdateRobotState(DeltaTime);

	
}

void ARARobotArm::StartRobotAction()
{
	if (CurrentState != EActionState::Idle) return;

	TArray<AActor*> OverlappingActors;
	BoxComp->GetOverlappingActors(OverlappingActors);

	if (OverlappingActors.Num() > 0)
	{
		CurrentTargetMesh = OverlappingActors[0];
		if (CurrentTargetMesh)
		{
			// 목표 트랜스폼은 타겟 메시의 월드 트랜스폼으로 설정
			GrabTransform = CurrentTargetMesh->GetActorTransform();

			// 현재 End_Ctrl의 트랜스폼을 가져옴
			
			Alpha = 0.0f;
			CurrentState = EActionState::MovingToTarget;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No meshes detected in the box."));
	}
}

void ARARobotArm::UpdateRobotState(float DeltaTime)
{
	switch (CurrentState)
	{
	case EActionState::MovingToTarget:
		if (Alpha == 0.0f)
		{
			StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
		}
		MoveToTransform(GrabTransform, DeltaTime);
		if (Alpha >= 1.0f)
		{
			CurrentState = EActionState::Grabbing;
		}
		break;

	case EActionState::Grabbing:
		if (CurrentTargetMesh)
		{
			// 메시를 End_Ctrl에 Attach
			//FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, false);
			CurrentTargetMesh->AttachToComponent(EndEffectorScene, FAttachmentTransformRules::KeepWorldTransform);

			//StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

			Alpha = 0.0f;
			CurrentState = EActionState::MovingToDestination;
		}
		break;

	case EActionState::MovingToDestination:
		MoveToTransform(TargetTransform, DeltaTime);
		if (Alpha >= 1.0f)
		{
			CurrentState = EActionState::Dropping;
		}
		break;

	case EActionState::Dropping:
		if (CurrentTargetMesh)
		{
			// 메시 분리
			CurrentTargetMesh->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentTargetMesh = nullptr;

			//StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

			Alpha = 0.0f;
			CurrentState = EActionState::Returning;
		}
		break;

	case EActionState::Returning:
		MoveToTransform(ReturnTransform, DeltaTime);
		if (Alpha >= 1.0f)
		{
			CurrentState = EActionState::Idle;
			UE_LOG(LogTemp, Warning, TEXT("Robot arm returned to idle state."));
		}
		break;

	case EActionState::Idle:
		break;
	}
}

void ARARobotArm::MoveToTransform(const FTransform& Destination, float DeltaTime)
{
	Alpha = FMath::Clamp(Alpha + DeltaTime * GrabSpeed, 0.0f, 1.0f);

	const FVector L = FMath::Lerp(StartTransform.GetLocation(), Destination.GetLocation(), Alpha);
	const FQuat R = FQuat::Slerp(StartTransform.GetRotation(), Destination.GetRotation(), Alpha).GetNormalized();
	const FVector S = FVector(1.f);

	// 새로운 트랜스폼으로 컨트롤릭 업데이트
	FTransform NewTransform(R, L, S);

	// SetControlTransform 함수가 ControlRigComponent에 있다고 가정합니다.
	ControlRigComponent->SetControlTransform(EndEffectorName, NewTransform, EControlRigComponentSpace::WorldSpace);
}
