#include "QuickJSTrait.h"

static JSValue GetJSValueFromProperty(JSContext* Context, void const* Object, FProperty* Property)
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

	// handling object property
	if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
	{
		// We check validation of UObject when the script trying to visit it
		// Not here
		UObject* ObjectValue = ObjectProperty->GetObjectPropertyValue_InContainer(Object);
		return qjs::js_traits<UObject*>::wrap(Context, ObjectValue, ObjectProperty->PropertyClass);
	}

	// return undefined if can't handle
	return JS_UNDEFINED;
}

JSValue GetJSValueFromFunction(JSContext* Context, const UObject* Object, UFunction* Function)
{
	return qjs::detail::unreal_function::WrapFunction(Context, Function);
}

static int SetJSValueToProperty(JSContext* Context, const JSValueConst& Value, void* Object, FProperty* Property)
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

	// Object properties
	if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
	{
		UObject* UnrealObject = qjs::js_traits<UObject*>::unwrap(Context, Value);
		if (!IsValid(UnrealObject))
		{
			JS_ThrowReferenceError(Context, "Trying to access an invalid unreal object");
			return -3;
		}
		UClass* PropertyClass = ObjectProperty->PropertyClass;
		if (UnrealObject->IsA(PropertyClass))
		{
			ObjectProperty->SetObjectPropertyValue_InContainer(Object, UnrealObject);
			return true;
		}
		UClass* ValueClass = UnrealObject->GetClass();
		JS_ThrowTypeError(
			Context,
			"Excepted '%s' class, found '%s'",
			TCHAR_TO_ANSI(*PropertyClass->GetName()),
			TCHAR_TO_ANSI(*ValueClass->GetName()));
		return -3;
	}

	// Types we are not handled
	return false;
}

static JSValue GetJSValueFromArrayProperty(JSContext* Context, const UObject* Object, FArrayProperty* Property)
{
	if (!Context || !IsValid(Object) || !Property) {
        return JS_UNDEFINED;
    }

	// Property->
	// void* ArrayDataPtr = Property->ContainsObjectReference()
	
	return JS_UNDEFINED;
}

JSValue ConvertReturnParamToJSValue(JSContext* Context, void* ReturnParamAddress, FProperty* Property)
{
	if (!Property)
    {
        return JS_UNDEFINED;
    }

    // handling string property
    if (FStrProperty* StrProperty = CastField<FStrProperty>(Property))
    {
        FString Value = StrProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewString(Context, TCHAR_TO_UTF8(*Value));
    }

    // handling boolean property
    if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
    {
        bool Value = BoolProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewBool(Context, Value);
    }

    // handling integer property
    if (FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
    {
        uint8 Value = ByteProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewUint32(Context, Value);
    }
    if (FUInt16Property* IntProperty = CastField<FUInt16Property>(Property))
    {
        uint16 Value = IntProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewUint32(Context, Value);
    }
    if (FUInt32Property* IntProperty = CastField<FUInt32Property>(Property))
    {
        uint32 Value = IntProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewUint32(Context, Value);
    }
    if (FUInt64Property* IntProperty = CastField<FUInt64Property>(Property))
    {
        uint64 Value = IntProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewInt64(Context, Value);
    }
    if (FIntProperty* IntProperty = CastField<FIntProperty>(Property))
    {
        int32 Value = IntProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewInt32(Context, Value);
    }
    if (FInt8Property* Int8Property = CastField<FInt8Property>(Property))
    {
        int8 Value = Int8Property->GetPropertyValue(ReturnParamAddress);
        return JS_NewInt32(Context, Value);
    }
    if (FInt16Property* IntProperty = CastField<FInt16Property>(Property))
    {
        int16 Value = IntProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewInt32(Context, Value);
    }
    if (FInt64Property* IntProperty = CastField<FInt64Property>(Property))
    {
        int64 Value = IntProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewInt64(Context, Value);
    }

    // handling float property
    if (FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
    {
        float Value = FloatProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewFloat64(Context, Value);
    }
    if (FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
    {
        double Value = DoubleProperty->GetPropertyValue(ReturnParamAddress);
        return JS_NewFloat64(Context, Value);
    }
	
	// handling object property
	if (FObjectPropertyBase* ObjectProperty = CastField<FObjectPropertyBase>(Property))
	{
		// We check validation of UObject when the script trying to visit it
		// Not here
		UObject* ObjectValue = *static_cast<UObject**>(ReturnParamAddress);
		return qjs::js_traits<UObject*>::wrap(Context, ObjectValue, ObjectProperty->PropertyClass);
	}

	return JS_UNDEFINED;
}
