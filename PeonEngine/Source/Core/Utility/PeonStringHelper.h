#pragma once

class PeonStringHelper
{
public:
	static std::wstring StringToWString(const std::string& str);

	static std::string WStringToString(const std::wstring& wStr);
};