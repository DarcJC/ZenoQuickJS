// Author: DarcJC
#include "QuickJSBlueprintLibrary.h"

#include "QuickJSErrors.h"

bool UQuickJSBlueprintLibrary::EvalFile(const FString& FilePath, EQuickJSEvalType EvalType)
{
	FZenoQuickJSModule& Module = FZenoQuickJSModule::GetChecked();
	const TSharedRef<qjs::Context> Context = Module.GetGlobalContext();
	try
	{
		const qjs::Value Ret = Context->evalFile(TCHAR_TO_ANSI(*FilePath), JS_EVAL_TYPE_MODULE);
		if (Ret.isError())
		{
			const JSValue Value = Ret.as<JSValue>();
			UE_LOG(LogQuickJS, Error, TEXT("Eval return an error %hs"), JS_ToCString(Context->ctx, Value));
		}
		else
		{
			return true;
		}
	}
	catch (qjs::exception& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to execute file %s : %hs"), *FilePath, Err.get().toJSON().c_str());
	}
	catch (std::exception& Err)
	{
		UE_LOG(LogQuickJS, Error, TEXT("Failed to execute file %s : %hs"), *FilePath, Err.what());
	}
	return false;
}
