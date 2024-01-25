#pragma once

class FQuickJSFileNotFoundError : public std::exception
{
public:
	explicit inline FQuickJSFileNotFoundError(FString InFilePath)
		: FilePath(MoveTemp(InFilePath))
	{
		Info = FString::Printf(TEXT("File '%s' not found."), *FilePath);
	}

	virtual inline char const* what() const override
	{
		return TCHAR_TO_ANSI(*Info);
	}

	FString FilePath;
private:
	FString Info;
};
