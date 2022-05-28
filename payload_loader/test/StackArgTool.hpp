#pragma once

#include <string>
#include <fstream>
#include <vector>
#include<sstream>

#pragma warning (disable: 4996)
#define QWORD unsigned long long

using namespace std;

class StackArg
{
public:
	bool m_status = false;
	int argSum, stackTotal;
#ifdef _M_IX86
	int typeLength = 4, fillCount = 830;
	vector <vector<DWORD>> m_values;
#else
	int typeLength = 8, fillCount = 300;
	vector <vector<QWORD>> m_values;
#endif

	StackArg(const std::string& shellcodePath)
	{
		content = readFileIntoString(shellcodePath);

		if (content != "") {
			m_status = true;

			argDealWith();
		}
	}

	~StackArg()
	{
	}

	void argDealWith()
	{
		argSum = stackCount + m_argCount;
#ifdef _M_IX86
		vector <vector<DWORD>> valuesArray(argSum);
#else
		vector <vector<QWORD>> valuesArray(argSum);
#endif
		int byteTotal = int(content.length() / 2);
		int temp = byteTotal % (argSum * typeLength);

		int zeroCount = 0;
		if (temp != 0) {
			zeroCount = (argSum * typeLength - byteTotal % (argSum * typeLength)) * 2;
		}

		for (size_t i = 0; i < zeroCount; i++)
		{
			content += "0";
		}
		reverse(content.begin(), content.end());
		int strLength = content.length();

		for (size_t i = 0; i < strLength; i += bytes * argSum)
		{
			for (size_t j = 0; j < argSum; j++)
			{
				int strIndex = i + j * bytes;
				valuesArray[j].push_back(numChange(content.substr(strIndex, bytes)));
			}
			actualCount++;
		}

		m_values = valuesArray;
		stackTotal = actualCount + fillCount;
	}

protected:
	string content;
	int actualCount = 0;
	int m_argCount = 4;

#ifdef _M_IX86
	int bytes = 8, stackCount = 2;

	DWORD numChange(string num)
	{
		stringstream ss;
		string s;
		int index[] = { 1,0,3,2,5,4,7,6 };
		for (size_t i = 0; i < bytes; i++)
		{
			s += num.substr(index[i], 1);
		}
		
		DWORD value = 0;

		ss << std::hex << s.c_str();
		ss >> value;

		return value;
	}
#else
	int bytes = 16, stackCount = 12;

	QWORD numChange(string num)
	{
		stringstream ss;
		string s;
		int index[] = { 1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14 };
		for (size_t i = 0; i < bytes; i++)
		{
			s += num.substr(index[i], 1);
		}
		
		QWORD value = 0;

		ss << std::hex << s.c_str();
		ss >> value;

		return value;
	}
#endif
	

	//从文件读入到string里
	string readFileIntoString(const string& path) {
		ifstream input_file(path);
		if (!input_file.is_open()) {
			return "";
		}
		return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}
};
