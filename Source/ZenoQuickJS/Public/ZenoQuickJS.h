// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "quickjs/quickjspp.hpp"

class FZenoQuickJSModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	quickjs::runtime& GetManagedRuntime();

private:
	quickjs::runtime Runtime;
};
