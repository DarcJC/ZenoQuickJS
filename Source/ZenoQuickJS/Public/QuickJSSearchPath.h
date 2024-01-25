#pragma once

struct FQuickJSSearchPath
{
	FString DirPath;
	int32 Priority = 0;
};

struct FQuickJSSearchPathPredicate
{
	bool operator()(const FQuickJSSearchPath& Lhs, const FQuickJSSearchPath& Rhs) const
	{
		// Higher at top
		return Lhs.Priority > Rhs.Priority;
	}
};
