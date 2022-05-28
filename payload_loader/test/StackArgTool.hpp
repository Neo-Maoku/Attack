#pragma once

#include <string>
#include <fstream>
#include <vector>
#include<sstream>

using namespace std;

class StackArg
{
public:
	bool m_status = false;
	int argSum, stackTotal;
	vector <vector<T>> m_values;
#ifdef _M_IX86
	int typeLength = 4, fillCount = 830;
#else
	int typeLength = 8, fillCount = 300;
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
		vector <vector<T>> valuesArray(argSum);
		int byteTotal = int(content.length() / 2);
		int temp = byteTotal % (argSum * typeLength);

		int zeroCount = 0;
		if (temp != 0) {
			zeroCount = (argSum * typeLength - byteTotal % (argSum * typeLength)) * 2;
		}

		for (int i = 0; i < zeroCount; i++)
		{
			content += "0";
		}
		reverse(content.begin(), content.end());
		int strLength = content.length();

		for (int i = 0; i < strLength; i += bytes * argSum)
		{
			for (int j = 0; j < argSum; j++)
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
	int index[8] = { 1,0,3,2,5,4,7,6 };
#else
	int bytes = 16, stackCount = 12;
	int index[16] = { 1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14 };
#endif

	T numChange(string num)
	{
		stringstream ss;
		string s;
		
		for (size_t i = 0; i < bytes; i++)
		{
			s += num.substr(index[i], 1);
		}

		T value = 0;

		ss << std::hex << s.c_str();
		ss >> value;

		return value;
	}

	string readFileIntoString(const string& path) {
		ifstream input_file(path);
		if (!input_file.is_open()) {
			return "";
		}
		return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}
};
