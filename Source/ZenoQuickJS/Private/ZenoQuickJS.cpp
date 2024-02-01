// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZenoQuickJS.h"

#include "QuickJSBlueprintLibrary.h"
#include "QuickJSModule.h"
#include "QuickJSObjectBase.h"
#include "QuickJSSearchPath.h"
#include "QuickJSTrait.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FZenoQuickJSModule"

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

		InitContext(GlobalContext.ToSharedRef());
	}

	return GlobalContext.ToSharedRef();
}

const TArray<FQuickJSSearchPath>& FZenoQuickJSModule::GetScriptSearchPaths()
{
	return ScriptSourceSearchDirectory;
}

void FZenoQuickJSModule::AddScriptSearchPath(const FString& DirPath, int32 Priority)
{
	checkf(FPaths::DirectoryExists(DirPath), TEXT("Directory '%s' adding to script search paths isn't exist"),
	       *DirPath);
	for (const FQuickJSSearchPath& ExistPath : ScriptSourceSearchDirectory)
	{
		ensureMsgf(ExistPath.DirPath != DirPath, TEXT("Don't add same search directory twice"));
	}

	ScriptSourceSearchDirectory.Emplace(DirPath, Priority);
	ScriptSourceSearchDirectory.HeapSort(FQuickJSSearchPathPredicate{});
}

void FZenoQuickJSModule::InitContext(const TSharedRef<qjs::Context>& Context)
{
	// Setup zeno module
	auto& ZenoModule = Context->addModule("zeno");
	SetupLog(ZenoModule);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT && WITH_EDITOR
	// Add debug search path
	// Debug path has a higher priority
	const FString ContentDir = IPluginManager::Get().FindPlugin("ZenoQuickJS")->GetContentDir();
	AddScriptSearchPath(ContentDir / "Scripts" / "BuiltIn", 10);
#endif

	// Add default search path
	// AddScriptSearchPath("/ZenoQuickJS/Scripts/BuiltIn", 0);
	AddScriptSearchPath(FPaths::ProjectDir() / "Scripts" / "BuiltIn", 0);

	// Load debug module
	UQuickJSBlueprintLibrary::EvalFile("debug.js", EQuickJSEvalType::Module);

	// Setup unreal module
	FQuickJSModule::InitUnrealExportModule(Context);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FZenoQuickJSModule, ZenoQuickJS)

DEFINE_LOG_CATEGORY(LogQuickJS)
