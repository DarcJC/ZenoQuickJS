// Author: DarcJC

#pragma once

#include "CoreMinimal.h"
#include "QuickJSTypes.h"
#include "ZenoQuickJS.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuickJSBlueprintLibrary.generated.h"

class UQuickJSTestObject;

UCLASS()
class ZENOQUICKJS_API UQuickJSBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "JS", meta = ( Keywords = "Eval File Exec" ))
	static bool EvalFile(const FString& FilePath, EQuickJSEvalType EvalType = EQuickJSEvalType::Global);

	UFUNCTION(BlueprintCallable, Category = "JS", meta = ( Keywords = "Eval Text Exec" ))
	static bool Eval(const FString& InputString, EQuickJSEvalType EvalType = EQuickJSEvalType::Global);

#if WITH_EDITORONLY_DATA
	UFUNCTION(BlueprintCallable, Category = "JS", meta = ( Keywords = "Create Test Object" ))
	static UQuickJSTestObject* CreateTestObject();
#endif
};
