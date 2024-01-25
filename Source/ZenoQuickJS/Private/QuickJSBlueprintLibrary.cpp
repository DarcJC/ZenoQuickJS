// Author: DarcJC
#include "QuickJSBlueprintLibrary.h"

void UQuickJSBlueprintLibrary::EvalFile(const FString& FilePath)
{
	if (!FPaths::FileExists(FilePath))
	{
		UE_LOG(LogQuickJS, Error, TEXT("File %s doesn't exist!"), *FilePath);
		return;
	}
		
	FZenoQuickJSModule& Module = FZenoQuickJSModule::GetChecked();
	const TSharedRef<qjs::Context> Context = Module.GetGlobalContext();
	try
	{
		qjs::Value Ret = Context->evalFile(TCHAR_TO_ANSI(*FilePath), JS_EVAL_TYPE_MODULE);
		if (!Ret.isError())
		{
			JSValue Value = Ret.as<JSValue>();
			UE_LOG(LogQuickJS, Display, TEXT("%hs"), JS_ToCString(Context->ctx, Value));
		}
	}
	catch (qjs::exception& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to execute file %s : %hs"), *FilePath, Err.get().toJSON().c_str());
	}
	catch (std::runtime_error& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to execute file %s : %hs"), *FilePath, Err.what());
	}
	// JSValue Value2 = js_std_await(Context->ctx, Value);
	// UE_LOG(LogTemp, Display, TEXT("%hs"), JS_ToCString(Context->ctx, Value2));
	// UE_LOG(LogTemp, Display, TEXT("%hs"), Ret.toJSON().c_str());
}
