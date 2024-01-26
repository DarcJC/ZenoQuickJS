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
	UFUNCTION(BlueprintCallable, meta = ( Keywords = "Eval File Exec" ))
	static bool EvalFile(const FString& FilePath, EQuickJSEvalType EvalType = EQuickJSEvalType::Global);

	UFUNCTION(BlueprintCallable, meta = ( Keywords = "Eval Text Exec" ))
	static bool Eval(const FString& InputString, EQuickJSEvalType EvalType = EQuickJSEvalType::Global);
};
