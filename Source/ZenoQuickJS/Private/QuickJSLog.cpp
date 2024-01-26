#include "QuickJSObjectBase.h"
#include "ZenoQuickJS.h"
#include "QuickJSTrait.h"

using FLogFuncDelegate = TFunction<void(const qjs::rest<std::string>&)>;

#define MAKE_LOG_FUNCTION(FunctionName, LogLevel) \
	void FunctionName(const qjs::rest<std::string>& Args) { \
		FString Output; \
		for (auto const& Arg : Args) \
		{ \
			Output.Append(Arg.c_str()); \
		} \
		UE_LOG(LogQuickJS, LogLevel, TEXT("%s"), *Output); \
	}

MAKE_LOG_FUNCTION(PrintToLogFile, Log);
MAKE_LOG_FUNCTION(PrintInfo, Display);
MAKE_LOG_FUNCTION(PrintWarning, Warning);
MAKE_LOG_FUNCTION(PrintError, Error);
MAKE_LOG_FUNCTION(PrintFatal, Fatal);

void SetupLog(qjs::Context::Module& InModule)
{
	InModule.function<&PrintToLogFile>("LogFile");
	InModule.function<&PrintInfo>("PrintInfo");
	InModule.function<&PrintWarning>("PrintWarning");
	InModule.function<&PrintError>("PrintError");
	InModule.function<&PrintFatal>("PrintFatal");

#if WITH_EDITOR
	UQuickJSTestObject* TestObject = NewObject<UQuickJSTestObject>();
	JSValue Wrapper = qjs::js_traits<UQuickJSTestObject*>::wrap(InModule.ctx, TestObject);
	InModule.add("Test", MoveTemp(Wrapper));
#endif
}
