#pragma once

#include <string>

using namespace std;

string ReadFileIntoString(const string& path) {
	ifstream input_file(path);
	if (!input_file.is_open()) {
		return "";
	}
	return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

std::string Subreplace(std::string resource_str, std::string sub_str, std::string new_str)
{
	std::string dst_str = resource_str;
	std::string::size_type pos = 0;
	while ((pos = dst_str.find(sub_str)) != std::string::npos)
	{
		dst_str.replace(pos, sub_str.length(), new_str);
	}
	return dst_str;
}

int CreateFullDirectory(std::string path)
{
	int len = path.length();
	char tmpDirPath[256] = { 0 };
	for (int i = 0; i < len; i++)
	{
		tmpDirPath[i] = path[i];
		if (tmpDirPath[i] == '\\')
		{
			if (_access(tmpDirPath, 0) == -1)
			{
				int ret = _mkdir(tmpDirPath);
				if (ret == -1) return ret;
			}
		}
	}
	return 0;
}

bool IsValidFileName(const std::string& strFileName)
{
	// Windows下文件名中不能包含\/:*?"<>|这些字符
	std::regex regExpress("[\\/:*?\"<>|]");
	bool bValid = !std::regex_search(strFileName, regExpress);
	return bValid;
}

BOOL CanAccessFile(string strPath, DWORD dwGenericAccessMask)
{
	DWORD dwSize = 0;
	PSECURITY_DESCRIPTOR psd = NULL;
	SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;

	// 获取文件权限信息结构体大小  
	BOOL bRet = GetFileSecurityA(strPath.c_str(), si, psd, 0, &dwSize);
	if (bRet || GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return FALSE;
	}


	char* pBuf = new char[dwSize];
	ZeroMemory(pBuf, dwSize);
	psd = (PSECURITY_DESCRIPTOR)pBuf;

	// 获取文件权限信息结构体大小  
	bRet = GetFileSecurityA(strPath.c_str(), si, psd, dwSize, &dwSize);
	if (!bRet)
	{
		delete[]pBuf;
		return FALSE;
	}

	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		delete[]pBuf;
		return FALSE;
	}

	// 模拟令牌  
	HANDLE hImpersonatedToken = NULL;
	if (!DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken))
	{
		delete[]pBuf;
		CloseHandle(hToken);
		return FALSE;
	}

	// 在检测是否有某个权限时，将GENERIC_WRITE等值传给本函数的第二个参数dwGenericAccessMask
	// GENERIC_WRITE等参数在调用CreateFile创建文件时会使用到，下面调用MapGenericMask将
	// GENERIC_WRITE等转换成FILE_GENERIC_WRITE等
	// 将GENERIC_XXXXXX转换成FILE_GENERIC_XXXXXX
	GENERIC_MAPPING genMap;
	genMap.GenericRead = FILE_GENERIC_READ;
	genMap.GenericWrite = FILE_GENERIC_WRITE;
	genMap.GenericExecute = FILE_GENERIC_EXECUTE;
	genMap.GenericAll = FILE_ALL_ACCESS;
	MapGenericMask(&dwGenericAccessMask, &genMap);

	// 调用AccessCheck来检测是否有指定的权限
	PRIVILEGE_SET privileges = { 0 };
	DWORD dwGrantedAccess = 0;
	DWORD privLength = sizeof(privileges);
	BOOL bGrantedAccess = FALSE;
	if (AccessCheck(psd, hImpersonatedToken, dwGenericAccessMask,
		&genMap, &privileges, &privLength, &dwGrantedAccess, &bGrantedAccess))
	{
		if (bGrantedAccess)
		{
			if (dwGenericAccessMask == dwGrantedAccess)
			{
				bGrantedAccess = TRUE;
			}
			else
			{
				bGrantedAccess = FALSE;
			}
		}
		else
		{
			bGrantedAccess = FALSE;
		}
	}
	else
	{
		bGrantedAccess = FALSE;
	}

	delete[]pBuf;
	CloseHandle(hImpersonatedToken);
	CloseHandle(hToken);
	return bGrantedAccess;
}