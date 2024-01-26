#pragma once

#include <quickjs/quickjspp.hpp>
#include "QuickJSTrait.h"
#include "quickjs/quickjs-libc.h"
#include "quickjs/quickjs.h"
#include "Templates/RemoveCV.h"

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

	/**
	 * Get the valid javascript extensions.
	 * Used to resolve module path without extension.
	 */
	static TArray<FString>& GetValidFileExtension();

	static void InitUnrealExportModule(const TSharedRef<qjs::Context>& InContext);
};

template <typename T, typename = TEnableIf< TIsDerivedFrom<TRemoveCV<T>, UClass>::IsDerived >>
class FQuickJSClassRegistrar
{
	FString ClassName;
	qjs::Context::Module& Module;
	TSharedRef<qjs::Context> Context;
	qjs::Value Ctor;
	qjs::Value Prototype;
public:
	FQuickJSClassRegistrar(FString InClassName, qjs::Context::Module& InModule, TSharedRef<qjs::Context> InContext)
		: ClassName(MoveTemp(InClassName))
		, Module(InModule)
		, Context(MoveTemp(InContext))
		, Ctor(JS_NULL)
		, Prototype(Context->newObject())
	{
	}

	// Forbidden Copy Ctor
	FQuickJSClassRegistrar(const FQuickJSClassRegistrar&) = delete;

	template <typename Func>
	FQuickJSClassRegistrar& AddFunction(const FString& Name, Func&& F)
	{
		Prototype[TCHAR_TO_ANSI(*Name)] = Forward<Func>(F);
		return *this;
	}

	template <typename Func>
	FQuickJSClassRegistrar& AddFunction(const FString& Name)
	{
		qjs::js_traits<TSharedPtr<T>>::template ensureCanCastToBase<Func>();
		Prototype.add<Func>(TCHAR_TO_ANSI(*Name));
		return *this;
	}
};


