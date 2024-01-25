// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "quickjs/quickjs-libc.h"
#include "quickjs/quickjspp.hpp"

class FZenoQuickJSModule : public IModuleInterface
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

private:
	qjs::Runtime Runtime;
	TSharedPtr<qjs::Context> GlobalContext;

	bool bIsLiving = false;
};

DECLARE_LOG_CATEGORY_EXTERN(LogQuickJS, Display, Display);
