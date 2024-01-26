﻿// Author: DarcJC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuickJSObjectBase.generated.h"

UCLASS()
class ZENOQUICKJS_API UQuickJSObjectBase : public UObject
{
	GENERATED_BODY()

protected:
};

#if WITH_EDITORONLY_DATA
UCLASS()
class UQuickJSTestObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text;
};
#endif
