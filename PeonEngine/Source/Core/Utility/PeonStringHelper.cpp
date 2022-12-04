#include "CorePCH.h"
#include "PeonStringHelper.h"
#include <codecvt>

std::wstring PeonStringHelper::StringToWString(const std::string& str)
{
	std::wstring ret(str.cbegin(), str.cend());
	return ret;
}

std::string PeonStringHelper::WStringToString(const std::wstring& wStr)
{
	std::string ret;

	std::transform(wStr.begin(), wStr.end(), std::back_inserter(ret),
	               [](const wchar_t c) { return static_cast<char>(c); });
	return ret;
}
