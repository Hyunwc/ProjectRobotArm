
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAType.h"
#include "RAFactoryManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateClassification, FClassificationMap, ClassificationData);

class ARARobotArm;

UCLASS()
class ROBOTARM_API ARAFactoryManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARAFactoryManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	TArray<ARARobotArm*> RobotArms;

	// 분류 개수를 나타낼 TMap<EProductType, int32>를 구조체로 래핑함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FClassificationMap ClassificationMap;

	UPROPERTY(BlueprintAssignable)
	FOnUpdateClassification OnUpdateClassification;

public:
	UFUNCTION()
	void RecordClassificationData(EProductType Type);

};
