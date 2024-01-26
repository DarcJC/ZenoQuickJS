#pragma once

class FQuickJSFileNotFoundError : public std::exception
{
public:
	explicit inline FQuickJSFileNotFoundError(FString InFilePath)
		: FilePath(MoveTemp(InFilePath))
	{
		Info = FString::Printf(TEXT("File '%s' not found."), *FilePath);
	}

	char const* what() const noexcept
	{
		return TCHAR_TO_ANSI(*Info);
	}

	FString FilePath;
private:
	FString Info;
};
