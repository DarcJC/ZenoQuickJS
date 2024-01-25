// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "quickjs/quickjs-libc.h"
#include "quickjs/quickjspp.hpp"

struct FQuickJSSearchPath;

class ZENOQUICKJS_API FZenoQuickJSModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static FORCEINLINE FZenoQuickJSModule& GetChecked()
	{
		return FModuleManager::GetModuleChecked<FZenoQuickJSModule>("ZenoQuickJS");
	}

	qjs::Runtime& GetManagedRuntime();
	TSharedRef<qjs::Context> GetGlobalContext();

	static const TArray<FQuickJSSearchPath>& GetScriptSearchPaths();
	static void AddScriptSearchPath(const FString& DirPath, int32 Priority = 0);

private:
	qjs::Runtime Runtime;
	TSharedPtr<qjs::Context> GlobalContext;

	bool bIsLiving = false;
	
	static inline TArray<FQuickJSSearchPath> ScriptSourceSearchDirectory {};
};

DECLARE_LOG_CATEGORY_EXTERN(LogQuickJS, Display, Display);
