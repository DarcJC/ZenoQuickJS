// Author: DarcJC

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuickJSSettings.generated.h"

/**
 * 
 */
UCLASS(Config=QuickJS, DefaultConfig)
class ZENOQUICKJS_API UQuickJSSettings : public UObject
{
	GENERATED_BODY()
};

#if WITH_EDITORONLY_DATA
UCLASS(Config=QuickJS, meta = (DisplayName = "QuickJS"), MinimalAPI)
class UQuickJSExportSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="QuickJS")
	FFilePath DeclarationFileSavePath;
};
#endif

