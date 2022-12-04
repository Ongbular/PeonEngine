#pragma once

#include "inipp/inipp.h"
#include "Logger/Logger.h"
#include "ConfigDefines.h"

enum class eConfigType : uint8
{
	Editor,
};

using ParserPtr = shared_ptr<inipp::Ini<char>>;

class ConfigParser final{
	
public:
	static HRESULT ParseConfigurations();
	
	template <typename T>
	static T GetValue(eConfigType configType, const std::string& sectionName, const std::string& propName);

private:
	static bool parseIni(ParserPtr parser, const string& fileName);


	ConfigParser() = default;
	~ConfigParser() = default;

	static const string sConfigDir;
	static ParserPtr sEditorConfigParser;
};

template <typename T>
T ConfigParser::GetValue(const eConfigType configType, const std::string& sectionName, const std::string& propName)
{
	ParserPtr parser{ nullptr };

	switch (configType)
	{
	case eConfigType::Editor:
		parser = sEditorConfigParser;
		break;
	default:
		check(false);
	}

	T ret;
	
	if (!inipp::get_value(parser->sections[sectionName.c_str()], propName, ret))
	{
		Logger::Error("Can't get {} of section {}.", propName, sectionName);
		Logger::Error("You need to check {} configuration file.", EnumHelper::GetName(configType));
		check(false);
	}

	return ret;
}
