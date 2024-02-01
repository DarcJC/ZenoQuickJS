// Author: DarcJC

#pragma once

#include <quickjs/quickjs.h>

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/WeakObjectPtr.h"
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

	bool CheckIsWeakRef() const;

protected:
	TSharedPtr<JSValue> Inner;

	// If weak flags is set, JSValue held by container might GC by JS VM.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsWeak = false;
	
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

	UFUNCTION(BlueprintCallable)
	UQuickJSTestObject* TestFunc(UQuickJSTestObject* InObject);

	UFUNCTION(BlueprintCallable)
	void SetInteger(int32 NewValue);
};
#endif
