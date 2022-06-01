#pragma once

#include <string>
#include <stdint.h>
#include <map>
#include <algorithm>

/***********************************************************************
		参数间以空格分隔，如果参数本身含有空格，则需要用双引号包围
***********************************************************************/

class Param
{
public:
	Param(const std::string& value)
		: m_value(value)
	{
	}

	~Param()
	{
	}

	std::string toString()
	{
		return m_value;
	}

	int32_t toInt()
	{
		return atoi(m_value.c_str());
	}

	int64_t toInt64()
	{
		return _atoi64(m_value.c_str());
	}

	uint32_t toUInt()
	{
		char* end_ptr = nullptr;
		return strtoul(m_value.c_str(), &end_ptr, 0);
	}

	uint64_t toUInt64()
	{
		char* end_ptr = nullptr;
		return _strtoui64(m_value.c_str(), &end_ptr, 0);
	}

private:
	std::string m_value;
};

class CmdlineParser
{
public:
	CmdlineParser(const std::string& cmdline)
	{
		analyze(cmdline);
	}

	~CmdlineParser()
	{
	}

	std::string getFullCmdLine()
	{
		return m_strCmdLine;
	}

	bool analyze(const std::string& cmdline)
	{
		bool bResult = false;
		int nPos = 0, nParamNamePos = 0, nParamValuePos = 0;
		int nStatus = 0;

		std::string strParamName;
		bool bInComma = false;
		char chPos = '\0';

		if (cmdline.empty())
			goto Exit0;

		m_strCmdLine = cmdline;

		for (int nPos = 0; 0 == nPos || '\0' != cmdline[nPos - 1]; nPos++)
		{
			chPos = cmdline[nPos];

			switch (nStatus)
			{
			case 0:

				if (bInComma)
				{
					if ('\"' == chPos)
						bInComma = false;
				}
				else if ('-' == chPos || '/' == chPos)
					nStatus = 1;
				else if ('\"' == chPos)
					bInComma = true;

				break;

			case 1:
				nParamNamePos = nPos;

				nStatus = 2;
				break;

			case 2:
				if (' ' == chPos || '\0' == chPos)
				{
					strParamName = m_strCmdLine.substr(nParamNamePos, nPos - nParamNamePos);
					setParam(strParamName, (uint32_t)1);

					nStatus = 0;
				}
				else if ('=' == chPos)
				{
					strParamName = m_strCmdLine.substr(nParamNamePos, nPos - nParamNamePos);
					nStatus = 3;
				}

				break;

			case 3:
				if ('\"' == chPos)
				{
					nPos++;
					bInComma = true;
				}
				else if (' ' == chPos || '\0' == chPos)
				{
					setParam(strParamName, "");
					nStatus = 0;

					break;
				}

				nParamValuePos = nPos;
				nStatus = 4;

				break;

			case 4:
				if ((' ' == chPos && !bInComma) || '\0' == chPos || ('\"' == chPos && bInComma))
				{
					setParam(strParamName, m_strCmdLine.substr(nParamValuePos, nPos - nParamValuePos));
					nStatus = 0;
					bInComma = false;
				}

				break;
			}
		}

		bResult = true;

	Exit0:

		return bResult;
	}

	bool hasParam(const std::string& param_name)
	{
		std::string param = param_name;
		std::transform(param.begin(), param.end(), param.begin(), tolower);

		CmdParamMap::iterator iter = m_mapParams.find(param);
		if (iter != m_mapParams.end())
		{
			return true;
		}

		return false;
	}

	bool setParam(const std::string& param_name, const std::string& param_value)
	{
		std::string param = param_name;
		std::transform(param.begin(), param.end(), param.begin(), tolower);
		m_mapParams[param] = param_value;

		return true;
	}

	bool setParam(const std::string& param_name, uint32_t param_value)
	{
		std::string param = param_name;
		std::transform(param.begin(), param.end(), param.begin(), tolower);

		char value[128] = { 0 };
		sprintf_s(value, "%d", param_value);

		m_mapParams[param] = value;

		return true;
	}

	bool setParam(const std::string& param_name, uint64_t param_value)
	{
		std::string param = param_name;
		std::transform(param.begin(), param.end(), param.begin(), tolower);

		char value[128] = { 0 };
		sprintf_s(value, "%I64d", param_value);

		m_mapParams[param] = value;

		return true;
	}

	Param operator[](const std::string& param_name)
	{
		std::string param = param_name;
		std::transform(param.begin(), param.end(), param.begin(), tolower);

		std::string value = "";

		CmdParamMap::iterator iter = m_mapParams.find(param);
		if (iter != m_mapParams.end())
		{
			value = iter->second;
		}

		return Param(value);
	}


protected:
	typedef std::map<std::string, std::string> CmdParamMap;

	std::string m_strCmdLine;
	CmdParamMap m_mapParams;
};
