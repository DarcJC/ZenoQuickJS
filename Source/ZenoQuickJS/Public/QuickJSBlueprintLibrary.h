// Author: DarcJC

#pragma once

#include "CoreMinimal.h"
#include "QuickJSTypes.h"
#include "ZenoQuickJS.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuickJSBlueprintLibrary.generated.h"

UCLASS()
class ZENOQUICKJS_API UQuickJSBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = ( Keywords = "Debug" ))
	static bool EvalFile(const FString& FilePath, EQuickJSEvalType EvalType = EQuickJSEvalType::Global);
};
