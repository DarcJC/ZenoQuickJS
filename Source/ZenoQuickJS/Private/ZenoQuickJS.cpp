// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZenoQuickJS.h"

#include "QuickJSModule.h"
#include "QuickJSSearchPath.h"

#define LOCTEXT_NAMESPACE "FZenoQuickJSModule"


void Println(const qjs::rest<std::string>& Args)
{
	FString Output;
	for (auto const& Arg : Args)
	{
		Output.Append(Arg.c_str());
	}
	UE_LOG(LogTemp, Display, TEXT("%s"), *Output);
}

void FZenoQuickJSModule::StartupModule()
{
	bIsLiving = true;
	
	// Set module loader
	JS_SetModuleLoaderFunc(Runtime.rt, nullptr, FQuickJSModule::DefaultModuleLoaderFunction, nullptr);

	const TSharedRef<qjs::Context> Context = GetGlobalContext();

	/** Register Unreal Module */
	{
	}

	// Set flag
}

void FZenoQuickJSModule::ShutdownModule()
{
	bIsLiving = false;
	// TODO: notify scripts
	GlobalContext.Reset();
}

qjs::Runtime& FZenoQuickJSModule::GetManagedRuntime()
{
	checkf(bIsLiving, TEXT("ZenoQuickJS Module is invalid, it is not initialized or shutdown."));
	
	return Runtime;
}

TSharedRef<qjs::Context> FZenoQuickJSModule::GetGlobalContext()
{
	checkf(bIsLiving, TEXT("ZenoQuickJS Module is invalid, it is not initialized or shutdown."));
	
	if (!GlobalContext)
	{
		GlobalContext = MakeShared<qjs::Context>(Runtime);

		// Setup zeno module
		auto& ZenoModule = GlobalContext->addModule("zeno");
		ZenoModule.function<&Println>("println");
	}
	
	return GlobalContext.ToSharedRef();
}

const TArray<FQuickJSSearchPath>& FZenoQuickJSModule::GetScriptSearchPaths()
{
	return ScriptSourceSearchDirectory;
}

void FZenoQuickJSModule::AddScriptSearchPath(const FString& DirPath, int32 Priority)
{
	checkf(FPaths::DirectoryExists(DirPath), TEXT("Directory '%s' adding to script search paths isn't exist"), *DirPath);
	for (const FQuickJSSearchPath& ExistPath : ScriptSourceSearchDirectory)
	{
		ensureMsgf(ExistPath.DirPath != DirPath, TEXT("Don't add same search directory twice"));
	}

	ScriptSourceSearchDirectory.Emplace(DirPath, Priority);
	ScriptSourceSearchDirectory.HeapSort(FQuickJSSearchPathPredicate {});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FZenoQuickJSModule, ZenoQuickJS)

DEFINE_LOG_CATEGORY(LogQuickJS)