// Author: DarcJC
#include "QuickJSBlueprintLibrary.h"

#include "QuickJSErrors.h"
#include "QuickJSModule.h"

static void DumpJSObject(JSContext* Context, JSValueConst Value, const FString& FilePath = "<unknown>")
{
	const char* CStr = JS_ToCString(Context, Value);
	UE_LOG(LogQuickJS, Error, TEXT("Error occurred '%s' : %hs"), *FilePath, nullptr != CStr ? CStr : "[exception]");
	JS_FreeCString(Context, CStr);
}

bool UQuickJSBlueprintLibrary::EvalFile(const FString& FilePath, EQuickJSEvalType EvalType)
{
	FZenoQuickJSModule& Module = FZenoQuickJSModule::GetChecked();
	const TSharedRef<qjs::Context> Context = Module.GetGlobalContext();
	try
	{
		const FString ResolvedPath = FQuickJSModule::ResolveModulePath(TCHAR_TO_ANSI(*FilePath));
		FString ModuleFileContext;
		if (!ResolvedPath.IsEmpty() && FFileHelper::LoadFileToString(ModuleFileContext, *ResolvedPath))
		{
			const qjs::Value Ret = Context->eval(
				TCHAR_TO_ANSI(*ModuleFileContext), TCHAR_TO_ANSI(*ResolvedPath), EvalType);
			if (Ret.isError())
			{
				const JSValue Value = Ret.as<JSValue>();
				DumpJSObject(Context->ctx, Value, FilePath);
				JS_FreeValue(Context->ctx, Value);
			}
			else
			{
				return true;
			}
		}
		else
		{
			UE_LOG(LogQuickJS, Error, TEXT("Failed to load file %s : File not found / insufficient permission."),
			       *FilePath);
		}
	}
	catch (qjs::exception& Err)
	{
		const JSValue ExceptionValue = Err.get();
		DumpJSObject(Context->ctx, ExceptionValue, FilePath);
		if (JS_IsError(Context->ctx, ExceptionValue))
		{
			const JSValue Val = JS_GetPropertyStr(Context->ctx, ExceptionValue, "stack");
			if (!JS_IsUndefined(Val))
			{
				DumpJSObject(Context->ctx, Val, FilePath);
			}
			JS_FreeValue(Context->ctx, Val);
		}
		JS_FreeValue(Context->ctx, ExceptionValue);
	}
	catch (std::exception& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to execute file %s : %hs"), *FilePath, Err.what());
	}
	return false;
}

bool UQuickJSBlueprintLibrary::Eval(const FString& InputString, EQuickJSEvalType EvalType)
{
	FZenoQuickJSModule& Module = FZenoQuickJSModule::GetChecked();
	const TSharedRef<qjs::Context> Context = Module.GetGlobalContext();

	try
	{
		const qjs::Value Ret = Context->eval(TCHAR_TO_ANSI(*InputString), "<Blueprint>", EvalType);
		if (Ret.isError())
		{
			const JSValue Value = Ret.as<JSValue>();
			UE_LOG(LogQuickJS, Error, TEXT("Eval return an error %hs"), JS_ToCString(Context->ctx, Value));
			return false;
		}
	}
	catch (qjs::exception& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to eval expr : %hs"), Err.get().toJSON().c_str());
	}
	catch (std::exception& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to eval expr : %hs"), Err.what());
	}
	return true;
}

#if WITH_EDITOR
UQuickJSTestObject* UQuickJSBlueprintLibrary::CreateTestObject()
{
	UQuickJSTestObject* Object = NewObject<UQuickJSTestObject>();
	Object->Object = Object;
	// Object->LazyObject = Object;
	// Object->SoftObject = Object;
	// Object->WeakObject = Object;
	return Object;
}
#endif
