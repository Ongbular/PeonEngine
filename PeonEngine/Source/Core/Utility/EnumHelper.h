#pragma once

#include "magic_enum/magic_enum.h"
#include "CoreTypes.h"

class EnumHelper
{
public:
	template <typename E>
	static constexpr auto GetName(E e);

	template <typename E>
	static constexpr auto FromString(const std::string& str);

	template <typename E>
	static constexpr auto FromInteger(int32 num);

	template <typename E>
	static constexpr auto ToInteger(E e);

	template <typename E>
	static constexpr auto GetRange();

	template <typename E>
	static constexpr auto GetCount();
};

template <typename E>
constexpr auto EnumHelper::GetName(E e)
{
	return magic_enum::enum_name(e);
}

template <typename E>
constexpr auto EnumHelper::FromString(const std::string& str)
{
	auto result = magic_enum::enum_cast<E>(str);
	static_assert(result.has_value());

	return result.value();
}

template <typename E>
constexpr auto EnumHelper::FromInteger(const int32 num)
{
	auto result = magic_enum::enum_cast<E>(num);
	static_assert(result.has_value());

	return result.value();
}

template <typename E>
constexpr auto EnumHelper::ToInteger(E e)
{
	return magic_enum::enum_integer(e);
}

template <typename E>
constexpr auto EnumHelper::GetRange()
{
	return magic_enum::enum_values<E>();
}

template <typename E>
constexpr auto EnumHelper::GetCount()
{
	return magic_enum::enum_count<E>();
}

struct EnumHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};