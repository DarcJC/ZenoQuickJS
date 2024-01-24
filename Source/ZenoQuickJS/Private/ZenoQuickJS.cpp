// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZenoQuickJS.h"

#define LOCTEXT_NAMESPACE "FZenoQuickJSModule"

void Println(const quickjs::args& Args)
{
	for (auto const& Arg : Args) UE_LOG(LogTemp, Display, TEXT("%hs"), Arg.as_cstring().c_str());
}

void FZenoQuickJSModule::StartupModule()
{
}

void FZenoQuickJSModule::ShutdownModule()
{
}

quickjs::runtime& FZenoQuickJSModule::GetManagedRuntime()
{
	return Runtime;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FZenoQuickJSModule, ZenoQuickJS)