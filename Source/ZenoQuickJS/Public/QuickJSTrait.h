﻿#pragma once

#include "QuickJSObjectBase.h"
#include "quickjs/quickjspp.hpp"
#include "Templates/RemoveCV.h"
#include "UObject/GCObjectScopeGuard.h"

static JSValue GetJSValueFromProperty(JSContext* Context, void const* Object, FProperty* Property);
static JSValue GetJSValueFromFunction(JSContext* Context, const UObject* Object, UFunction* Function);
static int SetJSValueToProperty(JSContext* Context, const JSValueConst& Value, void* Object, FProperty* Property);
static JSValue GetJSValueFromArrayProperty(JSContext* Context, const UObject* Object, FArrayProperty* Property);
static JSValue ConvertReturnParamToJSValue(JSContext* Context, void* ReturnParamAddress, FProperty* Property);

namespace qjs
{
	template <typename T, typename /*_SFINAE*/ = void>
	struct TUnrealType
	{
		static inline JSClassID GetJSClassId() = delete;
	};

	template <class T>
	struct TUnrealType<TArray<T>>
	{
		static inline JSClassID InnerJSClassId = 0;
		static inline JSClassExoticMethods ExoticMethods{
			.get_own_property = [](JSContext* Context, JSPropertyDescriptor* Descriptor, JSValueConst Value,
			                       JSAtom Prop) -> int
			{
				JSValue PropValue = JS_AtomToValue(Context, Prop);
				int32_t Index;
				// Convert PropValue to valid index
				if (JS_ToInt32(Context, &Index, PropValue) == 0 && Index >= 0)
				{
					JS_FreeValue(Context, PropValue);

					TArray<T>* ArrayRef = static_cast<TArray<T>*>(JS_GetOpaque(Value, InnerJSClassId));
					if (!ArrayRef)
					{
						JS_ThrowInternalError(Context, "Invalid TArray");
						return -1;
					}
					if (Index >= ArrayRef->Num())
					{
						// Index out of range
						JS_ThrowRangeError(Context, "Trying to index '%d' in a TArray with size '%d'", Index,
						                   ArrayRef->Num());
						return -1;
					}

					// Retrieve Element from array
					T& Element = (*ArrayRef)[Index];
					JSValue ElementValue = js_traits<T>::wrap(Context, Element);

					Descriptor->flags = JS_PROP_C_W_E;
					Descriptor->value = ElementValue;
					return 0;
				}

				JS_FreeValue(Context, PropValue); // 如果转换失败或不是整数，也需要释放PropValue
				return -1; // 不是有效的索引，返回错误
			},
			.get_own_property_names = nullptr,
			.delete_property = nullptr,
			.define_own_property = nullptr,
			.has_property = nullptr,
			.get_property = nullptr,
			.set_property = nullptr
		};

		static inline JSClassID GetJSClassId(JSContext* Context)
		{
			if (0 == InnerJSClassId)
			{
				JS_NewClassID(&InnerJSClassId);
			}
			auto Runtime = JS_GetRuntime(Context);
			if (!JS_IsRegisteredClass(Runtime, InnerJSClassId))
			{
				FString Name;
				if constexpr (TIsPointer<T>::Value && TIsDerivedFrom<
					typename TRemovePointer<T>::Type, UObject>::IsDerived)
				{
					const UClass* Class = T::StaticClass();
					Name = Class->GetName();
				}
				else
				{
					Name = "UnknownStruct";;
				}
				FString ClassName = FString::Printf(TEXT("TArray<%s>"), *Name);
				JSClassDef ClassDef{
					.class_name = TCHAR_TO_ANSI(*ClassName),
					.finalizer = nullptr, // We aren't tracing lifetime of TArray for now
					.gc_mark = [](JSRuntime* Runtime, JSValueConst Value, JS_MarkFunc* MarkerFunc)
					{
						if constexpr (TIsSame<typename TRemovePointer<T>::Type, UJSValueContainer>::Value)
						{
							TArray<UJSValueContainer*>* ArrRef = static_cast<TArray<UJSValueContainer*>*>(JS_GetOpaque(
								Value, InnerJSClassId));
							if (nullptr != ArrRef)
							{
								for (UJSValueContainer* Container : *ArrRef)
								{
									if (IsValid(Container))
									{
										JS_MarkValue(Runtime, **Container, MarkerFunc);
									}
								}
							}
						}
					},
					.call = nullptr,
					.exotic = &ExoticMethods,
				};
				const int Err = JS_NewClass(Runtime, InnerJSClassId, &ClassDef);
				if (Err < 0)
				{
					JS_ThrowInternalError(Context, "Can't register class %s", TCHAR_TO_ANSI(*ClassName));
					throw exception{Context};
				}
			}
			return InnerJSClassId;
		}

		/**
		 * Convert a TArray to JS object.
		 * Caller must trace lifetime of TArray to avoid dandling pointer.
		 * @param Context JSContext
		 * @param InRef A valid pointer to TArray<T>
		 * @return A JSValue with InRef as opaque data
		 */
		static inline JSValue From(JSContext* Context, TArray<T>* InRef)
		{
			check(nullptr != Context);
			check(nullptr != InRef);

			JSValue NewValue = JS_NewObjectClass(Context, GetJSClassId());
			JS_SetOpaque(NewValue, InRef);
			return NewValue;
		}
	};

	/** Conversions for non-owning pointers to class T. nullptr corresponds to JS_NULL.
	 * @tparam T class type
	 */
	template <class T>
	struct js_traits<T*, std::enable_if_t<std::is_class_v<T> && !TIsDerivedFrom<T, UObject>::IsDerived>>
	{
		static JSValue wrap(JSContext* ctx, T* ptr)
		{
			if (ptr == nullptr)
			{
				return JS_NULL;
			}
			if (js_traits<std::shared_ptr<T>>::QJSClassId == 0) // not registered
			{
#if defined(__cpp_rtti)
				// If you have an error here with T=JSValueConst
				// it probably means you are passing JSValueConst to where JSValue is expected
				js_traits<std::shared_ptr<T>>::register_class(ctx, typeid(T).name());
#else
            return JS_ThrowTypeError(ctx, "quickjspp js_traits<T *>::wrap: Class is not registered");
#endif
			}
			auto jsobj = JS_NewObjectClass(ctx, js_traits<std::shared_ptr<T>>::QJSClassId);
			if (JS_IsException(jsobj))
				return jsobj;

			// shared_ptr with empty deleter since we don't own T*
			auto pptr = new std::shared_ptr<T>(ptr, [](T*)
			{
			});
			JS_SetOpaque(jsobj, pptr);
			return jsobj;
		}

		static T* unwrap(JSContext* ctx, JSValueConst v)
		{
			if (JS_IsNull(v))
			{
				return nullptr;
			}
			auto ptr = js_traits<std::shared_ptr<T>>::unwrap(ctx, v);
			return ptr.get();
		}
	};

	/** Conversions for TSharedPtr<T>. Empty TSharedPtr corresponds to JS_NULL.
	 * T should be registered to a context before conversions.
	 * T should not be an UObject.
	 * @tparam T class type
	 */
	template <class T>
	struct js_traits<TSharedPtr<T>, typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived>::type>
	{
		/// Registered class id in QuickJS.
		inline static JSClassID QJSClassId = 0;

		/// Signature of the function to obtain the TSharedPtr from the JSValue.
		using ptr_cast_fcn_t = std::function<TSharedPtr<T>(JSContext*, JSValueConst)>;

		/// Used by registerDerivedClass to register new derived classes with this class' base type.
		inline static std::function<void(JSClassID, ptr_cast_fcn_t)> registerWithBase;

		/// Mapping between derived class' JSClassID and function to obtain the TSharedPtr from the JSValue.
		inline static std::unordered_map<JSClassID, ptr_cast_fcn_t> ptrCastFcnMap;

		/** Register a class as a derived class.
		 * 
		 * @tparam D type of the derived class
		 * @param derived_class_id class id of the derived class
		 * @param ptr_cast_fcn function to obtain a TSharedPtr from the JSValue
		 */
		template <typename D>
		static void registerDerivedClass(JSClassID derived_class_id, ptr_cast_fcn_t ptr_cast_fcn)
		{
			static_assert(std::is_base_of<T, D>::value && !std::is_same<T, D>::value, "Type is not a derived class");
			using derived_ptr_cast_fcn_t = typename js_traits<TSharedPtr<D>>::ptr_cast_fcn_t;

			// Register how to obtain the TSharedPtr from the derived class.
			ptrCastFcnMap[derived_class_id] = ptr_cast_fcn;

			// Propagate the registration to our base class (if any).
			if (registerWithBase) registerWithBase(derived_class_id, ptr_cast_fcn);

			// Instrument the derived class so that it can propagate new derived classes to us.
			auto old_registerWithBase = js_traits<TSharedPtr<D>>::registerWithBase;
			js_traits<TSharedPtr<D>>::registerWithBase =
				[old_registerWithBase = std::move(old_registerWithBase)]
			(JSClassID derived_class_id, derived_ptr_cast_fcn_t derived_ptr_cast_fcn)
				{
					if (old_registerWithBase) old_registerWithBase(derived_class_id, derived_ptr_cast_fcn);
					registerDerivedClass<D>(derived_class_id,
					                        [derived_cast_fcn = std::move(derived_ptr_cast_fcn)](
					                        JSContext* ctx, JSValueConst v)
					                        {
						                        return TSharedPtr<T>(derived_cast_fcn(ctx, v));
					                        });
				};
		}

		template <typename B>
		static
		std::enable_if_t<std::is_same_v<B, T> || std::is_same_v<B, void>>
		ensureCanCastToBase()
		{
		}

		template <typename B>
		static
		std::enable_if_t<!std::is_same_v<B, T> && !std::is_same_v<B, void>>
		ensureCanCastToBase()
		{
			static_assert(std::is_base_of_v<B, T>, "Type is not a derived class");

			if (js_traits<TSharedPtr<T>>::QJSClassId == 0)
				JS_NewClassID(&js_traits<TSharedPtr<T>>::QJSClassId);

			js_traits<TSharedPtr<B>>::template registerDerivedClass<T>(QJSClassId, unwrap);
		}

		template <auto M>
		static void ensureCanCastToBase()
		{
			ensureCanCastToBase<detail::class_from_member_pointer_t<decltype(M)>>();
		}

		/** Stores offsets to qjs::Value members of T.
		 * These values should be marked by class_registrar::mark for QuickJS garbage collector
		 * so that the cycle removal algorithm can find the other objects referenced by this object.
		 */
		static inline std::vector<Value T::*> markOffsets;

		/** Register class in QuickJS context.
		 *
		 * @param ctx context
		 * @param name class name
		 * @param proto class prototype or JS_NULL
		 * @param call QJS call function. see quickjs doc
		 * @param exotic pointer to QJS exotic methods(static lifetime) which allow custom property handling. see quickjs doc
		 * @throws exception
		 */
		static void register_class(JSContext* ctx, const char* name, JSValue proto = JS_NULL,
		                           JSClassCall* call = nullptr, JSClassExoticMethods* exotic = nullptr)
		{
			if (QJSClassId == 0)
			{
				JS_NewClassID(&QJSClassId);
			}
			auto rt = JS_GetRuntime(ctx);
			if (!JS_IsRegisteredClass(rt, QJSClassId))
			{
				JSClassGCMark* marker = nullptr;
				if (!markOffsets.empty())
				{
					marker = [](JSRuntime* rt, JSValueConst val, JS_MarkFunc* mark_func)
					{
						auto pptr = static_cast<TSharedPtr<T>*>(JS_GetOpaque(val, QJSClassId));
						assert(pptr);
						const T* ptr = pptr->get();
						assert(ptr);
						for (Value T::* member : markOffsets)
						{
							JS_MarkValue(rt, (*ptr.*member).v, mark_func);
						}
					};
				}
				JSClassDef def{
					name,
					// destructor (finalizer)
					[](JSRuntime* rt, JSValue obj) noexcept
					{
						auto pptr = static_cast<TSharedPtr<T>*>(JS_GetOpaque(obj, QJSClassId));
						delete pptr;
					},
					// mark
					marker,
					// call
					call,
					// exotic
					exotic
				};
				int e = JS_NewClass(rt, QJSClassId, &def);
				if (e < 0)
				{
					JS_ThrowInternalError(ctx, "Can't register class %s", name);
					throw exception{ctx};
				}
			}
			JS_SetClassProto(ctx, QJSClassId, proto);
		}

		/** Create a JSValue from TSharedPtr<T>.
		 * Creates an object with class if #QJSClassId and sets its opaque pointer to a new copy of #ptr.
		 */
		static JSValue wrap(JSContext* ctx, TSharedPtr<T> ptr)
		{
			if (!ptr)
				return JS_NULL;
			if (QJSClassId == 0) // not registered
			{
#if defined(__cpp_rtti)
				// automatically register class on first use (no prototype)
				register_class(ctx, typeid(T).name());
#else
                return JS_ThrowTypeError(ctx, "quickjspp TSharedPtr<T>::wrap: Class is not registered");
#endif
			}
			auto jsobj = JS_NewObjectClass(ctx, QJSClassId);
			if (JS_IsException(jsobj))
				return jsobj;

			auto pptr = new TSharedPtr<T>(std::move(ptr));
			JS_SetOpaque(jsobj, pptr);
			return jsobj;
		}

		/// @throws exception if #v doesn't have the correct class id
		static TSharedPtr<T> unwrap(JSContext* ctx, JSValueConst v)
		{
			TSharedPtr<T> ptr = nullptr;
			if (JS_IsNull(v))
			{
				return ptr;
			}
			auto obj_class_id = JS_GetClassID(v);

			if (obj_class_id == QJSClassId)
			{
				// The JS object is of class T
				void* opaque = JS_GetOpaque2(ctx, v, obj_class_id);
				assert(opaque && "No opaque pointer in object");
				ptr = *static_cast<TSharedPtr<T>*>(opaque);
			}
			else if (ptrCastFcnMap.count(obj_class_id))
			{
				// The JS object is of a class derived from T
				ptr = ptrCastFcnMap[obj_class_id](ctx, v);
			}
			else
			{
				// The JS object does not derives from T
				JS_ThrowTypeError(ctx, "Expected type %s, got object with classid %d",
				                  QJSPP_TYPENAME(T), obj_class_id);
				throw exception{ctx};
			}
			if (!ptr)
			{
				JS_ThrowInternalError(ctx, "Object's opaque pointer is NULL");
				throw exception{ctx};
			}
			return ptr;
		}
	};

	/** Conversions for std::shared_ptr<T>. Empty shared_ptr corresponds to JS_NULL.
	 * T should be registered to a context before conversions.
	 * @tparam T class type
	 */
	template <class T>
	struct js_traits<std::shared_ptr<T>, typename TEnableIf<!TIsDerivedFrom<T, UObject>::IsDerived>::type>
	{
		/// Registered class id in QuickJS.
		inline static JSClassID QJSClassId = 0;

		/// Signature of the function to obtain the std::shared_ptr from the JSValue.
		using ptr_cast_fcn_t = std::function<std::shared_ptr<T>(JSContext*, JSValueConst)>;

		/// Used by registerDerivedClass to register new derived classes with this class' base type.
		inline static std::function<void(JSClassID, ptr_cast_fcn_t)> registerWithBase;

		/// Mapping between derived class' JSClassID and function to obtain the std::shared_ptr from the JSValue.
		inline static std::unordered_map<JSClassID, ptr_cast_fcn_t> ptrCastFcnMap;

		/** Register a class as a derived class.
		 * 
		 * @tparam D type of the derived class
		 * @param derived_class_id class id of the derived class
		 * @param ptr_cast_fcn function to obtain a std::shared_ptr from the JSValue
		 */
		template <typename D>
		static void registerDerivedClass(JSClassID derived_class_id, ptr_cast_fcn_t ptr_cast_fcn)
		{
			static_assert(std::is_base_of<T, D>::value && !std::is_same<T, D>::value, "Type is not a derived class");
			using derived_ptr_cast_fcn_t = typename js_traits<std::shared_ptr<D>>::ptr_cast_fcn_t;

			// Register how to obtain the std::shared_ptr from the derived class.
			ptrCastFcnMap[derived_class_id] = ptr_cast_fcn;

			// Propagate the registration to our base class (if any).
			if (registerWithBase) registerWithBase(derived_class_id, ptr_cast_fcn);

			// Instrument the derived class so that it can propagate new derived classes to us.
			auto old_registerWithBase = js_traits<std::shared_ptr<D>>::registerWithBase;
			js_traits<std::shared_ptr<D>>::registerWithBase =
				[old_registerWithBase = std::move(old_registerWithBase)]
			(JSClassID derived_class_id, derived_ptr_cast_fcn_t derived_ptr_cast_fcn)
				{
					if (old_registerWithBase) old_registerWithBase(derived_class_id, derived_ptr_cast_fcn);
					registerDerivedClass<D>(derived_class_id,
					                        [derived_cast_fcn = std::move(derived_ptr_cast_fcn)](
					                        JSContext* ctx, JSValueConst v)
					                        {
						                        return std::shared_ptr<T>(derived_cast_fcn(ctx, v));
					                        });
				};
		}

		template <typename B>
		static
		std::enable_if_t<std::is_same_v<B, T> || std::is_same_v<B, void>>
		ensureCanCastToBase()
		{
		}

		template <typename B>
		static
		std::enable_if_t<!std::is_same_v<B, T> && !std::is_same_v<B, void>>
		ensureCanCastToBase()
		{
			static_assert(std::is_base_of_v<B, T>, "Type is not a derived class");

			if (js_traits<std::shared_ptr<T>>::QJSClassId == 0)
				JS_NewClassID(&js_traits<std::shared_ptr<T>>::QJSClassId);

			js_traits<std::shared_ptr<B>>::template registerDerivedClass<T>(QJSClassId, unwrap);
		}

		template <auto M>
		static void ensureCanCastToBase()
		{
			ensureCanCastToBase<detail::class_from_member_pointer_t<decltype(M)>>();
		}

		/** Stores offsets to qjs::Value members of T.
		 * These values should be marked by class_registrar::mark for QuickJS garbage collector
		 * so that the cycle removal algorithm can find the other objects referenced by this object.
		 */
		static inline std::vector<Value T::*> markOffsets;

		/** Register class in QuickJS context.
		 *
		 * @param ctx context
		 * @param name class name
		 * @param proto class prototype or JS_NULL
		 * @param call QJS call function. see quickjs doc
		 * @param exotic pointer to QJS exotic methods(static lifetime) which allow custom property handling. see quickjs doc
		 * @throws exception
		 */
		static void register_class(JSContext* ctx, const char* name, JSValue proto = JS_NULL,
		                           JSClassCall* call = nullptr, JSClassExoticMethods* exotic = nullptr)
		{
			if (QJSClassId == 0)
			{
				JS_NewClassID(&QJSClassId);
			}
			auto rt = JS_GetRuntime(ctx);
			if (!JS_IsRegisteredClass(rt, QJSClassId))
			{
				JSClassGCMark* marker = nullptr;
				if (!markOffsets.empty())
				{
					marker = [](JSRuntime* rt, JSValueConst val, JS_MarkFunc* mark_func)
					{
						auto pptr = static_cast<std::shared_ptr<T>*>(JS_GetOpaque(val, QJSClassId));
						assert(pptr);
						const T* ptr = pptr->get();
						assert(ptr);
						for (Value T::* member : markOffsets)
						{
							JS_MarkValue(rt, (*ptr.*member).v, mark_func);
						}
					};
				}
				JSClassDef def{
					name,
					// destructor (finalizer)
					[](JSRuntime* rt, JSValue obj) noexcept
					{
						auto pptr = static_cast<std::shared_ptr<T>*>(JS_GetOpaque(obj, QJSClassId));
						delete pptr;
					},
					// mark
					marker,
					// call
					call,
					// exotic
					exotic
				};
				int e = JS_NewClass(rt, QJSClassId, &def);
				if (e < 0)
				{
					JS_ThrowInternalError(ctx, "Can't register class %s", name);
					throw exception{ctx};
				}
			}
			JS_SetClassProto(ctx, QJSClassId, proto);
		}

		/** Create a JSValue from std::shared_ptr<T>.
		 * Creates an object with class if #QJSClassId and sets its opaque pointer to a new copy of #ptr.
		 */
		static JSValue wrap(JSContext* ctx, std::shared_ptr<T> ptr)
		{
			if (!ptr)
				return JS_NULL;
			if (QJSClassId == 0) // not registered
			{
#if defined(__cpp_rtti)
				// automatically register class on first use (no prototype)
				register_class(ctx, typeid(T).name());
#else
            JS_ThrowTypeError(ctx, "quickjspp std::shared_ptr<T>::wrap: Class is not registered");
            return JS_EXCEPTION;
#endif
			}
			auto jsobj = JS_NewObjectClass(ctx, QJSClassId);
			if (JS_IsException(jsobj))
				return jsobj;

			auto pptr = new std::shared_ptr<T>(std::move(ptr));
			JS_SetOpaque(jsobj, pptr);
			return jsobj;
		}

		/// @throws exception if #v doesn't have the correct class id
		static std::shared_ptr<T> unwrap(JSContext* ctx, JSValueConst v)
		{
			std::shared_ptr<T> ptr = nullptr;
			if (JS_IsNull(v))
			{
				return ptr;
			}
			auto obj_class_id = JS_GetClassID(v);

			if (obj_class_id == QJSClassId)
			{
				// The JS object is of class T
				void* opaque = JS_GetOpaque2(ctx, v, obj_class_id);
				assert(opaque && "No opaque pointer in object");
				ptr = *static_cast<std::shared_ptr<T>*>(opaque);
			}
			else if (ptrCastFcnMap.count(obj_class_id))
			{
				// The JS object is of a class derived from T
				ptr = ptrCastFcnMap[obj_class_id](ctx, v);
			}
			else
			{
				// The JS object does not derives from T
				JS_ThrowTypeError(ctx, "Expected type %s, got object with classid %d",
				                  QJSPP_TYPENAME(T), obj_class_id);
				throw exception{ctx};
			}
			if (!ptr)
			{
				JS_ThrowInternalError(ctx, "Object's opaque pointer is NULL");
				throw exception{ctx};
			}
			return ptr;
		}
	};

	template <class T>
	struct js_traits<T*, std::enable_if_t<TIsDerivedFrom<T, UObject>::IsDerived>>
	{
		struct FJSClassData
		{
			FGCObjectScopeGuard* Guard = nullptr;
			UClass* Class;

			explicit FJSClassData(T* InObject)
			{
				if (nullptr != InObject)
				{
					Guard = new FGCObjectScopeGuard(InObject);
				}
				Class = T::StaticClass();
			}
			
			explicit FJSClassData(T* InObject, UClass* InClass)
				: Class(InClass)
			{
				check(nullptr != InClass);
				if (nullptr != InObject)
				{
					Guard = new FGCObjectScopeGuard(InObject);
				}
			}

			~FJSClassData() noexcept
			{
				delete Guard;
			}
		};

		static JSValue wrap(JSContext* ctx, T* ptr, UClass* Class = T::StaticClass());

		inline static JSClassID QJSClassId = 0;
		static JSClassExoticMethods ExoticMethods;

		static void JSClassGCMark(JSRuntime* Runtime, JSValueConst Value,
		                          JS_MarkFunc* MarkerFunc)
		{
			if (const FJSClassData* JSData = static_cast<FJSClassData*>(JS_GetOpaque(Value, QJSClassId));
				nullptr != JSData && nullptr != JSData->Guard)
			{
				const UObject* Object = Cast<UObject>(JSData->Guard->Get());
				if (IsValid(Object) && !IsEngineExitRequested())
				{
					// Find all fields in object
					// If there is any UJSValueContainer* field, add reference to it
					// TODO: cache object field might helpful to improve performance
					for (TFieldIterator<FProperty> PropIter(Object->GetClass()); PropIter; ++PropIter)
					{
						FProperty* Property = *PropIter;
						if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
						{
							const UJSValueContainer* JSValueContainer = Cast<UJSValueContainer>(
								ObjectProperty->GetObjectPropertyValue_InContainer(Object));
							// We won't mark reachability for weak value ref
							if (IsValid(JSValueContainer) && JSValueContainer->IsValid() && !JSValueContainer->
								CheckIsWeakRef())
							{
								JS_MarkValue(Runtime, **JSValueContainer, MarkerFunc);
							}
						}
					}
				}
			}
		}

		static void RegisterClass(
			JSContext* Context, const char* ClassName, JSValue Proto = JS_NULL,
			JSClassCall* ClassCall = nullptr, JSClassExoticMethods* Exotic = nullptr)
		{
			if (QJSClassId == 0)
			{
				JS_NewClassID(&QJSClassId);
			}
			auto Runtime = JS_GetRuntime(Context);
			if (!JS_IsRegisteredClass(Runtime, QJSClassId))
			{
				JSClassDef ClassDef{
					.class_name = ClassName,
					.finalizer = [](JSRuntime* Runtime, JSValue Object) noexcept
					{
						FJSClassData* JSData = static_cast<FJSClassData*>(
							JS_GetOpaque(Object, QJSClassId));
						if (nullptr != JSData)
						{
							// Release referencer
							delete JSData;
						}
					},
					.gc_mark = &JSClassGCMark, // TODO darc: Impl gc mark for UPROPERTY-Wrapped JSValue
					.call = ClassCall,
					.exotic = Exotic,
				};
				if (JS_NewClass(Runtime, QJSClassId, &ClassDef))
				{
					JS_ThrowInternalError(Context, "Failed to register class %s", ClassName);
					throw exception{Context};
				}
			}
			JS_SetClassProto(Context, QJSClassId, Proto);
		}

		static T* unwrap(JSContext* ctx, JSValueConst v)
		{
			if (JS_IsNull(v))
			{
				return nullptr;
			}
			const JSClassID ObjectClassID = JS_GetClassID(v);
			FJSClassData* JSData = static_cast<FJSClassData*>(JS_GetOpaque2(ctx, v, ObjectClassID));
			if (nullptr != JSData && nullptr != JSData->Guard)
			{
				const FGCObjectScopeGuard* ScopeGuard = JSData->Guard;
				const T* Object = Cast<T>(ScopeGuard->Get());
				if (IsValid(Object))
				{
					return const_cast<T*>(Object);
				}
			}

			return nullptr;
		}

		static UClass* UnwrapClass(JSContext* Context, JSValueConst Value)
		{
			if (JS_IsNull(Value))
			{
				return nullptr;
			}
			const JSClassID ObjectClassID = JS_GetClassID(Value);
			FJSClassData* JSData = static_cast<FJSClassData*>(JS_GetOpaque2(Context, Value, ObjectClassID));
			if (nullptr != JSData)
			{
				if (IsValid(JSData->Class))
				{
					return JSData->Class;
				}
			}
			return nullptr;
		}
	};

	template <class T>
	JSValue js_traits<T*, std::enable_if_t<TIsDerivedFrom<T, UObject>::IsDerived, void>>::wrap(JSContext* ctx, T* ptr,
		UClass* Class)
	{
		if (QJSClassId == 0) // not registered
		{
			// If you have an error here with T=JSValueConst
			// it probably means you are passing JSValueConst to where JSValue is expected
			RegisterClass(ctx, TCHAR_TO_ANSI(*Class->GetName()), JS_NULL, nullptr, &ExoticMethods);
		}
		auto JSObject = JS_NewObjectClass(ctx, QJSClassId);
		if (JS_IsException(JSObject))
			return JSObject;
		FJSClassData* OpaqueData = new FJSClassData(ptr, Class);
		JS_SetOpaque(JSObject, OpaqueData);
		return JSObject;
	}

	template <class T>
	JSClassExoticMethods js_traits<T*, std::enable_if_t<TIsDerivedFrom<T, UObject>::IsDerived, void>>::ExoticMethods = {
		.get_own_property = nullptr,
		.get_own_property_names = nullptr,
		.delete_property = nullptr,
		.define_own_property = nullptr,
		.has_property = nullptr,
		.get_property = [](JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst receiver) -> JSValue
		{
			FJSClassData* JSData = static_cast<FJSClassData*>(JS_GetOpaque(obj, QJSClassId));
			if (!JSData || (!JSData->Guard && !JSData->Class))
			{
				return JS_NULL;
			}

			// Cast JSAtom to FString
			const char* PropNameCStr = JS_AtomToCString(ctx, atom);
			FString PropName(PropNameCStr);
			JS_FreeCString(ctx, PropNameCStr);

			if (PropName == "UnrealClass")
			{
				return js_traits<UClass*>::wrap(ctx, JSData->Class, UClass::StaticClass());
			}
			if (PropName == "UnrealClassName")
			{
				UClass* Class = JSData->Class;
				FString Name = Class->GetName();
				return JS_NewStringLen(ctx, TCHAR_TO_ANSI(*Name), Name.Len());
			}
			if (PropName == "IsValid")
			{
				return JS_NewBool(ctx, !!JSData->Guard);
			}

			if (JSData->Guard != nullptr)
			{
				const UObject* TmpObj = JSData->Guard->Get();
				const T* UnrealObject = Cast<T>(TmpObj);
				if (!IsValid(UnrealObject))
				{
					return JS_ThrowReferenceError(ctx, "Trying to visit an invalid UObject");
				}

				// Lookup properties
				UClass* Class = UnrealObject->GetClass();
				if (FProperty* Property = Class->FindPropertyByName(FName(*PropName)); nullptr != Property)
				{
					return GetJSValueFromProperty(ctx, UnrealObject, Property);
				}
				UFunction* Function = Class->FindFunctionByName(FName(*PropName), EIncludeSuperFlag::IncludeSuper);
				return GetJSValueFromFunction(ctx, UnrealObject, Function);
			}
			else // JSData->Class != nullptr
			{
				UClass* Class = JSData->Class;
				UFunction* Function = Class->FindFunctionByName(FName(*PropName), EIncludeSuperFlag::IncludeSuper);
				return GetJSValueFromFunction(ctx, nullptr, Function);
			}
		},
		.set_property = [](JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst value, JSValueConst receiver,
		                   int flags) -> int
		{
			FJSClassData* JSData = static_cast<FJSClassData*>(JS_GetOpaque(obj, QJSClassId));
			if (!JSData || !JSData->Guard)
			{
				return false;
			}

			T* UnrealObject = Cast<T>(const_cast<UObject*>(JSData->Guard->Get()));
			if (!IsValid(UnrealObject))
			{
				return false;
			}

			// Cast JSAtom to FString
			const char* PropNameCStr = JS_AtomToCString(ctx, atom);
			FString PropName(PropNameCStr);
			JS_FreeCString(ctx, PropNameCStr);

			// Lookup property
			FProperty* Property = UnrealObject->GetClass()->FindPropertyByName(FName(*PropName));
			return SetJSValueToProperty(ctx, value, UnrealObject, Property);
		},
	};


	namespace detail
	{
		struct unreal_function
		{
			static inline JSValue InvokeFunction(JSContext* Context, JSValueConst FuncObj, JSValueConst ThisObj,
			                                     int Argc,
			                                     JSValueConst* Argv, int Flags)
			{
				UFunction* FunctionToCall = js_traits<UFunction*>::unwrap(Context, FuncObj);
				UObject* Self = js_traits<UObject*>::unwrap(Context, ThisObj);
				UClass* SelfClass = js_traits<UObject*>::UnwrapClass(Context, ThisObj);
				if (!IsValid(FunctionToCall) || !IsValid(SelfClass))
				{
					return JS_ThrowReferenceError(Context, "Invalid UFunction to called with");
				}

				// Check this context
				const bool bIsStaticFunc = !!(FunctionToCall->FunctionFlags & FUNC_Static);
				if (!bIsStaticFunc && !IsValid(Self))
				{
					return JS_ThrowInternalError(Context, "Invalid 'this' context to call 'UFunction'");
				}

				// Check arguments count
				if (Argc + 1 < FunctionToCall->NumParms)
				{
					return JS_ThrowInternalError(Context, "Not enough arguments to call 'UFunction'");
				}

				UObject* CDO = SelfClass->ClassDefaultObject;

				// Try to fill function params
				FStructOnScope FuncParams(FunctionToCall);
				if (FunctionToCall->ParmsSize > 0)
				{
					int32 CurrentParamCount = 0;
					void* EndAddress = FuncParams.GetStructMemory() + FunctionToCall->ParmsSize;
					for (TFieldIterator<FProperty> Iterator(FunctionToCall); Iterator && Iterator->
					     HasAnyPropertyFlags(CPF_Parm); ++Iterator)
					{
						const bool bIsReturnParam = Iterator->HasAnyPropertyFlags(CPF_ReturnParm);
						const bool bIsOutParam = Iterator->HasAnyPropertyFlags(CPF_OutParm);
						if (bIsReturnParam || bIsOutParam)
							continue;

						if (CurrentParamCount > FunctionToCall->NumParms)
						{
							break;
						}

						FProperty* Param = *Iterator;

						// void* ParamAddress = FuncParams.GetStructMemory() + Param->GetOffset_ForUFunction();
						JSValueConst ArgValue = Argv[CurrentParamCount];
						SetJSValueToProperty(Context, ArgValue, FuncParams.GetStructMemory(), Param);

						++CurrentParamCount;
					}
				}

				if (bIsStaticFunc)
					CDO->ProcessEvent(FunctionToCall, FuncParams.GetStructMemory());
				else
					Self->ProcessEvent(FunctionToCall, FuncParams.GetStructMemory());

				const bool bHasReturnParam = FunctionToCall->ReturnValueOffset != MAX_uint16;
				void* ReturnValueAddress = bHasReturnParam
					                           ? reinterpret_cast<void*>(FuncParams.GetStructMemory() + FunctionToCall->
						                           ReturnValueOffset)
					                           : nullptr;
				FProperty* ReturnProperty = FunctionToCall->GetReturnProperty();
				if (nullptr != ReturnValueAddress && nullptr != ReturnProperty)
				{
					return ConvertReturnParamToJSValue(Context, ReturnValueAddress, ReturnProperty);
				}

				return JS_UNDEFINED;
			}

			static inline void RegisterClass(JSContext* Context)
			{
				if (0 == js_traits<UFunction*>::QJSClassId)
				{
					js_traits<UFunction*>::RegisterClass(Context, "UnrealFunction", JS_NULL, &InvokeFunction, nullptr);
				}
			}

			static inline JSValue WrapFunction(JSContext* Context, UFunction* InFunction)
			{
				RegisterClass(Context);
				return js_traits<UFunction*>::wrap(Context, InFunction);
			}
		};
	}
}
