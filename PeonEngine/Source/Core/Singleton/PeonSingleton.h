#pragma once

template <typename T>
class PeonSingleton
{
public:
	~PeonSingleton() = default;
	PeonSingleton(const PeonSingleton& other) = delete;
	PeonSingleton(PeonSingleton&& other) = delete;
	PeonSingleton& operator=(const PeonSingleton& rhs) = delete;
	PeonSingleton& operator=(PeonSingleton&& rhs) = delete;

	static T& Get();

protected:
	PeonSingleton() = default;
	struct Token {};
};

template <typename T>
T& PeonSingleton<T>::Get()
{
	static T instance(Token{});
	return instance;
}
