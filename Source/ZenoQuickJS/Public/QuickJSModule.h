#pragma once

#include "quickjs/quickjs-libc.h"
#include "quickjs/quickjs.h"

class FQuickJSModule
{
public:
	static JSModuleDef* DefaultModuleLoaderFunction(JSContext* Context, const char* ModuleName, void* Opaque);

	/**
	 * Resolve the path of input module name
	 * @param ModuleName Module name in syntax such as import 'module_name.js'
	 * @return Resolved file path of the module
	 */
	static FString ResolveModulePath(const char* ModuleName);

	static TArray<FString>& GetValidFileExtension();
};
