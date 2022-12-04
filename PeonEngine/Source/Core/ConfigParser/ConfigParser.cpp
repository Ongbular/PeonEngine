#include "CorePCH.h"
#include "ConfigParser.h"

const string ConfigParser::sConfigDir = ENGINE_CONFIG_PATH;
ParserPtr ConfigParser::sEditorConfigParser = std::make_shared<inipp::Ini<char>>();

HRESULT ConfigParser::ParseConfigurations()
{
	HRESULT hr;
	bool ret = parseIni(sEditorConfigParser, sConfigDir + EDITOR_CONFIG_FILE_NAME);

	hr = ret ? S_OK : E_FAIL;

	return hr;
}

bool ConfigParser::parseIni(const ParserPtr parser, const string& fileName)
{
	std::ifstream fs(fileName);

	if (!fs.is_open())
	{
		return false;
	}

	parser->parse(fs);
	return true;
}
