// Author: DarcJC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuickJSTypes.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum EQuickJSEvalType : uint8
{
	Global = 0 UMETA(DisplayName = "Global Code"),
	Module = 1 << 0 UMETA(DisplayName = "Module Code"),
	Direct = 2 << 0 UMETA(DisplayName = "Direct Call (Internal Use)", Hidden),
	Indirect = 3 << 0 UMETA(DisplayName = "Indirect Call (Internal Use)", Hidden),
};
ENUM_CLASS_FLAGS(EQuickJSEvalType)
