#include "QuickJSModule.h"

#include "QuickJSBlueprintLibrary.h"
#include "QuickJSErrors.h"
#include "QuickJSSearchPath.h"
#include "ZenoQuickJS.h"

JSModuleDef* FQuickJSModule::DefaultModuleLoaderFunction(JSContext* Context, const char* ModuleName, void* Opaque)
{
	check(Context != nullptr);

	const FString ModulePath = ResolveModulePath(ModuleName);
	if (ModulePath.IsEmpty())
	{
		return nullptr;
	}
	
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
			// Meta is helpful to resolve relative import
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
    const FString Extension = FPaths::GetExtension(ModulePath, false);
    const TArray<FString>& ValidExtensions = GetValidFileExtension();

    if (FPaths::IsRelative(ModulePath))
    {
        const TArray<FQuickJSSearchPath>& SearchPaths = FZenoQuickJSModule::GetScriptSearchPaths();
        // Try relative path in search paths
        for (const auto& [DirPath, Priority] : SearchPaths)
        {
            if (Extension.IsEmpty())
            {
                // Try with known extensions
                for (const FString& Ext : ValidExtensions)
                {
                    FString TestPath = FPaths::Combine(DirPath, ModulePath + TEXT(".") + Ext);
                    if (FPaths::FileExists(TestPath))
                    {
                        return TestPath;
                    }
                }
            }
            else
            {
                FString TestPath = FPaths::Combine(DirPath, ModulePath);
                if (FPaths::FileExists(TestPath))
                {
                    return TestPath;
                }
            }
        }
    }
    else if (Extension.IsEmpty())
    {
        // If absolute path without extension, try with known extensions
        for (const FString& Ext : ValidExtensions)
        {
            FString TestPath = ModulePath + TEXT(".") + Ext;
            if (FPaths::FileExists(TestPath))
            {
                return TestPath;
            }
        }
    }
    else
    {
        // If absolute path with extension, check if file exists
        if (FPaths::FileExists(ModulePath))
        {
            return ModulePath;
        }
    }

    // If no valid path found, return empty string
    return FString{};
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

void FQuickJSModule::InitUnrealExportModule(const TSharedRef<qjs::Context>& InContext)
{
	qjs::Context::Module& UnrealModule = InContext->addModule("unreal");
	
	JSValue Value = qjs::js_traits<UObject*>::wrap(InContext->ctx, nullptr, UQuickJSBlueprintLibrary::StaticClass());
	UnrealModule.add("QuickJSBlueprintLibrary", MoveTemp(Value));
	
	for (TObjectIterator<UStruct> It; It; ++It)
	{
		UStruct* Struct = *It;
		if (Struct->IsA<UClass>())
		{
			// UClass* Class = Cast<UClass>(Struct);
			// JSValue Value = qjs::js_traits<UObject*>::wrap(InContext->ctx, nullptr, Class);
			// UnrealModule.add(TCHAR_TO_ANSI(*Class->GetName()), MoveTemp(Value));
		}
		else if (Struct->IsA<UScriptStruct>())
		{
		}
		else
		{
		}
	}
}
