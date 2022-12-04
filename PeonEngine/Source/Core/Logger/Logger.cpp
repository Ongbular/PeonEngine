#include "CorePCH.h"
#include "Logger.h"
#include <iostream>
#include <sstream>
#include <DbgHelp.h>
#include <sstream>
#include <iomanip>

#pragma warning( disable : 4819 26450 26451 26437 4804 26498 26800 26498 26495 4806 6285 )
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#pragma warning( default : 4819 26450 26451 26437 4804 26498 26800 26498 26495 4806 6285 )

#pragma comment(lib, "Dbghelp.lib")

#define PROJECT_NAME ("PeonEngine")

FILE* Logger::DummyFp = nullptr;

HRESULT Logger::InitializeLogger()
{
	const auto timer = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	tm tBuff{};
	localtime_s(&tBuff, &timer);  // NOLINT(cert-err33-c)

	std::stringstream ss;
	ss << sDefaultLogPath << "\\" << sDefaultLogPrefix << tBuff.tm_year << tBuff.tm_mon << tBuff.tm_yday << tBuff.tm_hour << tBuff.tm_min <<
		tBuff.tm_sec << sDefaultLogExtension;

	std::string logFilename = ss.str();

	bool bShowConsole = false;

#ifdef _DEBUG
	bShowConsole = true;
#endif
	std::vector<spdlog::sink_ptr> sinks;

	auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilename, true);
	sinks.emplace_back(fileSink);

	if (bShowConsole)
	{
		if (!AllocConsole())
		{
			return E_ABORT;
		}

		const errno_t errMsg = freopen_s(&DummyFp, "CONOUT$", "w", stdout);
		check(errMsg == 0 && DummyFp);

		std::cout.clear();

		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		sinks.emplace_back(consoleSink);
	}

	spdlog::set_default_logger(std::make_shared<spdlog::logger>("defaultLogger", begin(sinks), end(sinks)));
	spdlog::set_pattern("[%Y-%m-%d %T] [%^%l%$] %v");
	spdlog::set_level(spdlog::level::trace);
	
	return S_OK;
}

void Logger::PrintEngineLogo()
{
	spdlog::set_pattern("%v");

	Info("");
	Info("");
	Info(R"(  _______                                 ________                      __                     )");
	Info(R"( |       \                               |        \                    |  \                    )");
	Info(R"( | $$$$$$$\  ______    ______   _______  | $$$$$$$$ _______    ______   \$$ _______    ______  )");
	Info(R"( | $$__/ $$ /      \  /      \ |       \ | $$__    |       \  /      \ |  \|       \  /      \ )");
	Info(R"( | $$    $$|  $$$$$$\|  $$$$$$\| $$$$$$$\| $$  \   | $$$$$$$\|  $$$$$$\| $$| $$$$$$$\|  $$$$$$\)");
	Info(R"( | $$$$$$$ | $$    $$| $$  | $$| $$  | $$| $$$$$   | $$  | $$| $$  | $$| $$| $$  | $$| $$    $$)");
	Info(R"( | $$      | $$$$$$$$| $$__/ $$| $$  | $$| $$_____ | $$  | $$| $$__| $$| $$| $$  | $$| $$$$$$$$)");
	Info(R"( | $$       \$$     \ \$$    $$| $$  | $$| $$     \| $$  | $$ \$$    $$| $$| $$  | $$ \$$     \)");
	Info(R"(  \$$        \$$$$$$$  \$$$$$$  \$$   \$$ \$$$$$$$$ \$$   \$$ _\$$$$$$$ \$$ \$$   \$$  \$$$$$$$)");
	Info(R"(                                                             |  \__| $$                        )");
	Info(R"(                                                              \$$    $$                        )");
	Info(R"(                                                               \$$$$$$                         )");
	Info("");
	Info("");

	spdlog::set_pattern("[%Y-%m-%d %T] [%^%l%$] %v");
}

void Logger::PrintCallStack()
{
	Error("");
	Error("");
	Error("");

	spdlog::set_pattern("[CallStack] %v");

	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, TRUE);

	void* stack[128];
	int frames = (int)CaptureStackBackTrace(0, 128, stack, nullptr);

	SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(1, sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR));
	symbol->MaxNameLen = MAX_SYM_NAME;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (int i = 1; i < frames; ++i)
	{
		DWORD64 displacement = 0;
		DWORD lineDisplacement = 0;

		if (SymFromAddr(process, (DWORD64)stack[i], &displacement, symbol))
		{
			IMAGEHLP_MODULE64 moduleInfo;
			memset(&moduleInfo, 0, sizeof(IMAGEHLP_MODULE64));
			moduleInfo.SizeOfStruct = sizeof(moduleInfo);

			IMAGEHLP_LINE64 lineInfo;
			memset(&lineInfo, 0, sizeof(IMAGEHLP_LINE64));
			lineInfo.SizeOfStruct = sizeof(lineInfo);

			if (::SymGetModuleInfo64(process, symbol->ModBase, &moduleInfo) && ::SymGetLineFromAddr64(process, symbol->Address, &lineDisplacement, &lineInfo))
			{
				const char* pFileName = strstr(lineInfo.FileName, PROJECT_NAME);

				if (pFileName)
				{
					std::stringstream ss;
					ss << "[" << std::hex << std::uppercase << symbol->Address << "] ";
					ss << "[" << std::setw(64) << std::setfill(' ') << std::right << pFileName << ":";
					ss << std::setw(4) << std::right << std::dec << lineInfo.LineNumber << "] ";
					ss << "-> " << symbol->Name;
					spdlog::error("{}", ss.str());
				}				
			}
		}
	}
}

void Logger::ReleaseConsole()
{
	Info("Start to deinitialize Logger...");

	if (DummyFp != nullptr)
	{
		fclose(DummyFp);  // NOLINT(cert-err33-c)
		FreeConsole();
	}
}
