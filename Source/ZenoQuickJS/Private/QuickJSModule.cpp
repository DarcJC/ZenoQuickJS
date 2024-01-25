#include "QuickJSModule.h"

JSModuleDef* FQuickJSModule::DefaultModuleLoaderFunction(JSContext* Context, const char* ModuleName, void* Opaque)
{
	check(Context != nullptr);

	const FString ModulePath = ResolveModulePath(ModuleName);
	JSModuleDef* NewModule = nullptr;
	if (FString ModuleContent; FFileHelper::LoadFileToString(ModuleContent, *ModulePath))
	{
		const FString NormalizedName = FString(ModulePath).Replace(TEXT(":"), TEXT("//")).Replace(TEXT("\\"), TEXT("/"));
		const JSValue FunctionEvalResult = JS_Eval(Context, TCHAR_TO_ANSI(*ModuleContent), ModuleContent.Len(), TCHAR_TO_ANSI(*NormalizedName), JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
		if (!JS_IsException(FunctionEvalResult))
		{
			js_module_set_import_meta(Context, FunctionEvalResult, true, false);
			NewModule = static_cast<JSModuleDef*>(JS_VALUE_GET_PTR(FunctionEvalResult));
			// set import.meta
			JSValue Meta = JS_GetImportMeta(Context, NewModule);
			if (!JS_IsException(Meta))
			{
				JS_DefinePropertyValueStr(Context, Meta, "url", JS_NewString(Context, TCHAR_TO_ANSI(*NormalizedName)), JS_PROP_C_W_E);
				JS_DefinePropertyValueStr(Context, Meta, "main", JS_NewBool(Context, false), JS_PROP_C_W_E);
			}
		}
		JS_FreeValue(Context, FunctionEvalResult);
	}

	return NewModule;
}

FString FQuickJSModule::ResolveModulePath(const char* ModuleName)
{
	FString ModulePath { ModuleName };
	FString Extension = FPaths::GetExtension(ModulePath, false);
	if (FPaths::IsRelative(ModulePath))
	{
	}
	else if (Extension.IsEmpty())
	{
		// If absolute path without extension name, try with known extensions
	}
	else
	{
		ModulePath.Reset();
	}

	return ModulePath;
}

TArray<FString>& FQuickJSModule::GetValidFileExtension()
{
	static TArray<FString> ValidTextExtensions {
		TEXT(".js"),
		TEXT(".mjs"),
		TEXT(".zjs"),
	};

	return ValidTextExtensions;
}
