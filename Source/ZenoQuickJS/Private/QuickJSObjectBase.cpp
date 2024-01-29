// Author: DarcJC


#include "QuickJSObjectBase.h"

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
