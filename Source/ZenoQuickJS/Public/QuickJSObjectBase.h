// Author: DarcJC

#pragma once

#include <quickjs/quickjs.h>

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuickJSObjectBase.generated.h"

UCLASS()
class ZENOQUICKJS_API UQuickJSObjectBase : public UObject
{
	GENERATED_BODY()

protected:
};

UCLASS()
class ZENOQUICKJS_API UJSValueContainer : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * Retrieve the value held in container.
	 *
	 * Borrower must ensure the value is GC reachable.
	 */
	TSharedRef<JSValue> BorrowChecked() const;

	JSValue operator*() const;

	bool IsValid() const;

	

protected:
	TSharedPtr<JSValue> Inner;
	
};

#if WITH_EDITORONLY_DATA
UCLASS()
class UQuickJSTestObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Float;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Integer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Boolean;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UQuickJSTestObject* Object;
};
#endif
