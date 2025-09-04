// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RARobotArm.h"
#include "ControlRigComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Components/RARobotArmFSM.h"
#include "Components/WidgetComponent.h"
#include "RAConveyor.h"
#include "RATestActor.h"
#include "RASensor.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RARobotArmStateWidget.h"

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

	StateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StateWidget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> StateWidgetRef(TEXT("/Game/UI/WBP_RobotArmState.WBP_RobotArmState_C"));
	if (StateWidgetRef.Class)
	{
		StateWidget->SetWidgetClass(StateWidgetRef.Class);
		StateWidget->SetupAttachment(Root);
		StateWidget->SetRelativeLocation(FVector(0.f, 0.f, 400.f));
		StateWidget->SetDrawSize(FVector2D(100.f, 30.f));
		StateWidget->SetWidgetSpace(EWidgetSpace::Screen);
		StateWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	

	FSM = CreateDefaultSubobject<URARobotArmFSM>(TEXT("FSM"));

	Type = EProductType::Default;
}

void ARARobotArm::BeginPlay()
{
	Super::BeginPlay();

	// TODO : 에디터에서 할당하는 것으로 수정해봅니다.
	TArray<AActor*> FoundConveyor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARAConveyor::StaticClass(), FoundConveyor);
	if (FoundConveyor.Num() > 0)
	{
		Conveyor = Cast<ARAConveyor>(FoundConveyor[0]);
		UE_LOG(LogTemp, Log, TEXT("RobotArm : 컨베이어 찾았습니다"));
	}
	
	TArray<AActor*> FoundSensor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARASensor::StaticClass(), FoundSensor);
	if (FoundSensor.Num() > 0)
	{
		Sensor = Cast<ARASensor>(FoundSensor[0]);
		UE_LOG(LogTemp, Log, TEXT("RobotArm : 센서 찾았습니다"));
	}

	Sensor->OnStateChangeSearch.AddDynamic(this, &ARARobotArm::StartSearch);

	StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	ReturnTransform = StartTransform;

	TargetTransform = TargetConveyor->DettachTransform;
}

void ARARobotArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Delta = DeltaTime;

	FTransform EffectorTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

	EndEffectorScene->SetWorldTransform(EffectorTransform);

	switch (FSM->CurrentState)
	{
	case ERobotArmState::Idle:
		IdleState();
		break;
	case ERobotArmState::Search:
		SearchState();
		break;
	case ERobotArmState::Attach:
		AttachState();
		break;
	case ERobotArmState::Carry:
		CarryState();
		break;
	case ERobotArmState::Dettach:
		DettachState();
		break;
	case ERobotArmState::Return:
		ReturnState();
		break;
	}

	OnRobotArmStateChanged.Broadcast(FSM->CurrentState);

	if (StateWidget)
	{
		if (URARobotArmStateWidget* ArmWidget = Cast<URARobotArmStateWidget>(StateWidget->GetUserWidgetObject()))
		{
			ArmWidget->UpdateRobotArmState(FSM->CurrentState);
		}
	}
}

// 이 함수 들어오면 Search상태로
void ARARobotArm::StartSearch(EProductType SearchType)
{
	// 자신과 다른 타입이라면
	if (Type != SearchType)
	{
		return;
	}

	TargetType = SearchType;

	FSM->ChangeState(ERobotArmState::Search);
}

void ARARobotArm::IdleState()
{
	//GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Cyan, TEXT("명령 대기 중..."));
}

void ARARobotArm::SearchState()
{
	if (Conveyor->Products.Num() > 0)
	{
		GrabActor = Conveyor->Products[0].TestActor;

		FString GrabMsg = GrabActor->GetActorLabel();
		//GEngine->AddOnScreenDebugMessage(5, 5.0f, FColor::Orange, GrabMsg);

		// 감지한 액터가 유효하면서 자신이 집어야할 타입이라면
		if (GrabActor && (GrabActor->GetProductType() == Type))
		{
			GrabTransform = GrabActor->GetActorTransform();

			Alpha = 0.0f;

			// 물건 좌표 갱신했으니 Attach 상태로
			FSM->ChangeState(ERobotArmState::Attach);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No meshes searched"));
		FSM->ChangeState(ERobotArmState::Idle);
	}
}

void ARARobotArm::AttachState()
{
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 물건집으러 이동
	MoveToTransform(GrabTransform, Delta);

	if (Alpha >= 1.0f)
	{
		if (GrabActor)
		{
			//GEngine->AddOnScreenDebugMessage(8, 2.f, FColor::Red, TEXT("유효"));
			GrabTransform = GrabActor->GetActorTransform();
		
			// 어태치 이 후에도 액터가 스플라인을 따라가지 않게 하기 위해 배열에서 제거
			if (Conveyor)
			{
				Conveyor->RemoveProduct(GrabActor);
			}

			MoveToTransform(GrabTransform, Delta);

			GrabActor->AttachToComponent(EndEffectorScene, FAttachmentTransformRules::KeepWorldTransform);
		
			Alpha = 0.0f;
		
			FSM->ChangeState(ERobotArmState::Carry);
		}
	}
}

void ARARobotArm::CarryState()
{
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 목표 지점으로 이동
	MoveToTransform(TargetTransform, Delta);

	if (Alpha >= 1.0f)
	{
		FSM->ChangeState(ERobotArmState::Dettach);
	}
}

void ARARobotArm::DettachState()
{
	if (GrabActor)
	{
		// 메시 분리
		GrabActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 타겟 컨베이어에서 물품이 다시 이동할 수 있도록 컨베이어의 배열에 더해줌.
		TargetConveyor->AddProduct(GrabActor);

		OnClassficationFinished.Broadcast(GrabActor->GetProductType());

		GrabActor = nullptr;

		//StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

		Alpha = 0.0f;
		FSM->ChangeState(ERobotArmState::Return);
	}
}

void ARARobotArm::ReturnState()
{
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 복귀 지점으로 이동
	MoveToTransform(ReturnTransform, Delta);

	if (Alpha >= 1.0f)
	{
		Alpha = 0.0f;

		FSM->ChangeState(ERobotArmState::Idle);
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

	ControlRigComponent->SetControlTransform(EndEffectorName, NewTransform, EControlRigComponentSpace::WorldSpace);
}
