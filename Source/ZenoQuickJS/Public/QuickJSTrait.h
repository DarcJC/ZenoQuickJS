#pragma once

#include "QuickJSObjectBase.h"
#include "quickjs/quickjspp.hpp"
#include "UObject/GCObjectScopeGuard.h"

static inline JSValue GetJSValueFromProperty(JSContext* Context, const UObject* Object, FProperty* Property)
{
	if (!Property)
	{
		return JS_UNDEFINED;
	}

	// handling string property
	if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
	{
		FString Value = StrProperty->GetPropertyValue_InContainer(Object);
		return JS_NewString(Context, TCHAR_TO_UTF8(*Value));
	}

	// handling boolean property
	if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		bool Value = BoolProperty->GetPropertyValue_InContainer(Object);
		return JS_NewBool(Context, Value);
	}

	// handling integer property
	if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
	{
		uint8 Value = ByteProperty->GetPropertyValue_InContainer(Object);
		return JS_NewUint32(Context, Value);
	}
	if (FUInt16Property* IntProperty = CastField<FUInt16Property>(Property))
	{
		uint16 Value = IntProperty->GetPropertyValue_InContainer(Object);
		return JS_NewUint32(Context, Value);
	}
	if (FUInt32Property* IntProperty = CastField<FUInt32Property>(Property))
	{
		uint32 Value = IntProperty->GetPropertyValue_InContainer(Object);
		return JS_NewUint32(Context, Value);
	}
	if (FUInt64Property* IntProperty = CastField<FUInt64Property>(Property))
	{
		uint64 Value = IntProperty->GetPropertyValue_InContainer(Object);
		return JS_NewInt64(Context, Value);
	}
	if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
	{
		int32 Value = IntProperty->GetPropertyValue_InContainer(Object);
		return JS_NewInt32(Context, Value);
	}
	if (FInt8Property* Int8Property = CastField<FInt8Property>(Property))
	{
		int8 Value = Int8Property->GetPropertyValue_InContainer(Object);
		return JS_NewInt32(Context, Value);
	}
	if (FInt16Property* IntProperty = CastField<FInt16Property>(Property))
	{
		int16 Value = IntProperty->GetPropertyValue_InContainer(Object);
		return JS_NewInt32(Context, Value);
	}
	if (FInt64Property* IntProperty = CastField<FInt64Property>(Property))
	{
		int64 Value = IntProperty->GetPropertyValue_InContainer(Object);
		return JS_NewInt64(Context, Value);
	}

	// handling float property
	if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		float Value = FloatProperty->GetPropertyValue_InContainer(Object);
		return JS_NewFloat64(Context, Value);
	}
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
	{
		double Value = DoubleProperty->GetPropertyValue_InContainer(Object);
		return JS_NewFloat64(Context, Value);
	}

	// return undefined if can't handle
	return JS_UNDEFINED;
}

static int SetJSValueToProperty(JSContext* Context, const JSValueConst& Value, UObject* Object, FProperty* Property)
{
	if (!Property)
	{
		JS_ThrowReferenceError(Context, "Failed to find property from object");
		return -1;
	}

	// Handling string property
	if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
	{
		// Cast JSValue to string
		size_t Len;
		const char* Str = JS_ToCStringLen(Context, &Len, Value);
		if (!Str)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to CString");
			return -2;
		}

		// Set string property
		FString UnrealStr = FString(UTF8_TO_TCHAR(Str));
		StrProperty->SetPropertyValue_InContainer(Object, UnrealStr);
		JS_FreeCString(Context, Str);
		return true;
	}
	// Handling boolean property
	if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		int32 BoolValue;
		if (JS_ToInt32(Context, &BoolValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to int32(bool)");
			return -2;
		}

		BoolProperty->SetPropertyValue_InContainer(Object, BoolValue != 0);
		return true;
	}

	// Handling integer property
	if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
	{
		uint32 ByteValue;
		if (JS_ToUint32(Context, &ByteValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to uint32");
			return -2;
		}

		ByteProperty->SetPropertyValue_InContainer(Object, ByteValue);
	}
	if (FUInt16Property* IntProperty = CastField<FUInt16Property>(Property))
	{
		uint32 IntValue;
		if (JS_ToUint32(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to uint32");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, IntValue);
	}
	if (FUInt32Property* IntProperty = CastField<FUInt32Property>(Property))
	{
		uint32 IntValue;
		if (JS_ToUint32(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to uint32");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, IntValue);
	}
	if (FUInt64Property* IntProperty = CastField<FUInt64Property>(Property))
	{
		int64 IntValue;
		if (JS_ToInt64(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to int64");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, FMath::Abs(IntValue));
	}
	if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
	{
		int32 IntValue;
		if (JS_ToInt32(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to int32");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, IntValue);
		return true;
	}
	if (FInt8Property* IntProperty = CastField<FInt8Property>(Property))
	{
		int32 IntValue;
		if (JS_ToInt32(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to int32");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, IntValue);
		return true;
	}
	if (FInt16Property* IntProperty = CastField<FInt16Property>(Property))
	{
		int32 IntValue;
		if (JS_ToInt32(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to int32");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, IntValue);
		return true;
	}
	if (FInt64Property* IntProperty = CastField<FInt64Property>(Property))
	{
		int64 IntValue;
		if (JS_ToInt64(Context, &IntValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to int64");
			return -2;
		}

		IntProperty->SetPropertyValue_InContainer(Object, IntValue);
	}

	// Handling float property
	if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		double FloatValue;
		if (JS_ToFloat64(Context, &FloatValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to float64");
			return -2;
		}

		FloatProperty->SetPropertyValue_InContainer(Object, static_cast<float>(FloatValue));
		return true;
	}
	if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
	{
		double DoubleValue;
		if (JS_ToFloat64(Context, &DoubleValue, Value) < 0)
		{
			JS_ThrowTypeError(Context, "Value failed to cast to float64");
			return -2;
		}

		DoubleProperty->SetPropertyValue_InContainer(Object, DoubleValue);
		return true;
	}

	// Types we are not handled
	return false;
}

namespace qjs
{
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
            JS_ThrowTypeError(ctx, "quickjspp js_traits<T *>::wrap: Class is not registered");
            return JS_EXCEPTION;
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
                JS_ThrowTypeError(ctx, "quickjspp TSharedPtr<T>::wrap: Class is not registered");
                return JS_EXCEPTION;
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
		inline static JSClassID QJSClassId = 0;
		inline static JSClassExoticMethods ExoticMethods{
			.get_own_property = nullptr,
			.get_own_property_names = nullptr,
			.delete_property = nullptr,
			.define_own_property = nullptr,
			.has_property = nullptr,
			.get_property = [](JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst receiver) -> JSValue
			{
				auto* Guard = static_cast<FGCObjectScopeGuard*>(JS_GetOpaque(obj, QJSClassId));
				if (!Guard)
				{
					return JS_UNDEFINED;
				}

				const T* UnrealObject = Cast<T>(Guard->Get());
				if (!IsValid(UnrealObject))
				{
					return JS_UNDEFINED;
				}

				// Cast JSAtom to FString
				const char* PropNameCStr = JS_AtomToCString(ctx, atom);
				FString PropName(PropNameCStr);
				JS_FreeCString(ctx, PropNameCStr);

				// Lookup properties
				FProperty* Property = UnrealObject->GetClass()->FindPropertyByName(FName(*PropName));
				return GetJSValueFromProperty(ctx, UnrealObject, Property);
			},
			.set_property = [](JSContext* ctx, JSValueConst obj, JSAtom atom, JSValueConst value, JSValueConst receiver,
			                   int flags) -> int
			{
				auto* Guard = static_cast<FGCObjectScopeGuard*>(JS_GetOpaque(obj, QJSClassId));
				if (!Guard)
				{
					return false;
				}

				T* UnrealObject = Cast<T>( const_cast<UObject*>(Guard->Get()) );
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

		static void JSClassGCMark(JSRuntime* Runtime, JSValueConst Value,
		                          JS_MarkFunc* MarkerFunc)
		{
			if (const FGCObjectScopeGuard* Guard = static_cast<FGCObjectScopeGuard*>(JS_GetOpaque(Value, QJSClassId));
				nullptr != Guard)
			{
				const UObject* Object = Cast<UObject>(Guard->Get());
				if (IsValid(Object))
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
							if (IsValid(JSValueContainer) && JSValueContainer->IsValid())
							{
								JS_MarkValue(Runtime, Value, MarkerFunc);
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
			}
			JSClassDef ClassDef{
				.class_name = ClassName,
				.finalizer = [](JSRuntime* Runtime, JSValue Object) noexcept
				{
					FGCObjectScopeGuard* Guard = static_cast<FGCObjectScopeGuard*>(JS_GetOpaque(Object, QJSClassId));
					if (nullptr != Guard)
					{
						// Release referencer
						delete Guard;
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

		static JSValue wrap(JSContext* ctx, T* ptr)
		{
			UClass* Class = T::StaticClass();
			if (QJSClassId == 0) // not registered
			{
				// If you have an error here with T=JSValueConst
				// it probably means you are passing JSValueConst to where JSValue is expected
				RegisterClass(ctx, TCHAR_TO_ANSI(*Class->GetName()), JS_NULL, nullptr, &ExoticMethods);
			}
			auto JSObject = JS_NewObjectClass(ctx, QJSClassId);
			if (JS_IsException(JSObject))
				return JSObject;
			if (nullptr != ptr)
			{
				FGCObjectScopeGuard* ScopeGuard = new FGCObjectScopeGuard(ptr);
				JS_SetOpaque(JSObject, ScopeGuard);
			}
			return JSObject;
		}

		static T* unwrap(JSContext* ctx, JSValueConst v)
		{
			if (JS_IsNull(v))
			{
				return nullptr;
			}
			auto ObjectClassID = JS_GetClassID(v);
			auto* ScopeGuard = static_cast<FGCObjectScopeGuard*>(JS_GetOpaque2(ctx, v, ObjectClassID));
			if (nullptr != ScopeGuard)
			{
				const T* Object = Cast<T>(ScopeGuard->Get());
				if (IsValid(Object))
				{
					return const_cast<T*>(Object);
				}
			}

			return nullptr;
		}
	};
}
