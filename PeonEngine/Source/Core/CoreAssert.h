#pragma once

#include <sstream>
#include <Windows.h>

#pragma warning( disable : 4819 26450 26451 26437 4804 26498 26800 26498 26495 4806 6285 )
#include "Logger/Logger.h"
#pragma warning( default : 4819 26450 26451 26437 4804 26498 26800 26498 26495 4806 6285 )

#ifdef _DEBUG
#define check(expr) \
		do \
		{ \
			if (!(expr)) \
			{ \
				Logger::Error("***** Error Occured *****"); \
				Logger::Error(""); \
				Logger::Error("Assertion Failed: {} [{}]:[{}]", #expr, __FILE__, __LINE__); \
				Logger::Error(""); \
				Logger::PrintCallStack(); \
				assert(false); \
			} \
		} while(0)

#define checkf(expr, ...) \
		do \
		{ \
			if (!(expr)) \
			{ \
				Logger::Error("***** Error Occured *****"); \
				Logger::Error(""); \
				Logger::Error("Assertion Failed: {} [{}]:[{}]", #expr, __FILE__, __LINE__); \
				Logger::Error(__VA_ARGS__); \
				Logger::Error(""); \
				Logger::PrintCallStack(); \
				assert(false); \
			} \
		} while(0)

#define UNIMPLEMENTED() checkf(false, "Unimplemented function");

#else
#define CHECK(expr) (void(0))
#define CHECK_MSG(expr, format, ...) (void(0))
#define UNIMPLEMENTED() (void(0))
#endif

#define EXIT_WITH_ERROR_MESSAGE_BOX(Message) \
			do { \
			MessageBox(nullptr, TEXT(Message), TEXT("Error!"), MB_ICONERROR | MB_OK); \
			exit(1); \
			} while(0)

namespace Assertion
{
	inline void Verify(const HRESULT hr, const char* file, const uint32_t line)
	{
		if (FAILED(hr))
		{
			std::stringstream ss;
			ss << "Error Code: 0x" << std::hex << hr;

			Logger::Error("***** Failed to verify *****");
			Logger::Error("");
			const std::string errorMsg = fmt::format("[{}] [{}]:[{}]", ss.str(), file, line);
			Logger::Error("{}", errorMsg);
			Logger::Error("");
			Logger::PrintCallStack();
			EXIT_WITH_ERROR_MESSAGE_BOX(errorMsg.c_str());
		}
	}
}

#define verifyResult(Result) Assertion::Verify((Result), __FILE__, __LINE__)
