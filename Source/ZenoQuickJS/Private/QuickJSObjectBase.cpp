// Author: DarcJC


#include "QuickJSObjectBase.h"

#include "ZenoQuickJS.h"

TSharedRef<JSValue> UJSValueContainer::BorrowChecked() const
{
	return Inner.ToSharedRef();
}

JSValue UJSValueContainer::operator*() const
{
	check( Inner.IsValid() );
	return *Inner;
}

bool UJSValueContainer::IsValid() const
{
	return Inner.IsValid();
}

bool UJSValueContainer::CheckIsWeakRef() const
{
	return bIsWeak;
}

#if WITH_EDITOR
UQuickJSTestObject* UQuickJSTestObject::TestFunc(UQuickJSTestObject* InObject)
{
	if (IsValid(InObject))
	{
		UE_LOG(LogQuickJS, Display, TEXT("Float: %.2f"), InObject->Float);
	}
	else
	{
		UE_LOG(LogQuickJS, Warning, TEXT("Passing invalid object to test func"));
	}
	return InObject;
}

void UQuickJSTestObject::SetInteger(int32 NewValue)
{
	Integer = NewValue;
}
#endif
