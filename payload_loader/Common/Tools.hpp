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
	// Windows���ļ����в��ܰ���\/:*?"<>|��Щ�ַ�
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

	// ��ȡ�ļ�Ȩ����Ϣ�ṹ���С  
	BOOL bRet = GetFileSecurityA(strPath.c_str(), si, psd, 0, &dwSize);
	if (bRet || GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return FALSE;
	}


	char* pBuf = new char[dwSize];
	ZeroMemory(pBuf, dwSize);
	psd = (PSECURITY_DESCRIPTOR)pBuf;

	// ��ȡ�ļ�Ȩ����Ϣ�ṹ���С  
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

	// ģ������  
	HANDLE hImpersonatedToken = NULL;
	if (!DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken))
	{
		delete[]pBuf;
		CloseHandle(hToken);
		return FALSE;
	}

	// �ڼ���Ƿ���ĳ��Ȩ��ʱ����GENERIC_WRITE��ֵ�����������ĵڶ�������dwGenericAccessMask
	// GENERIC_WRITE�Ȳ����ڵ���CreateFile�����ļ�ʱ��ʹ�õ����������MapGenericMask��
	// GENERIC_WRITE��ת����FILE_GENERIC_WRITE��
	// ��GENERIC_XXXXXXת����FILE_GENERIC_XXXXXX
	GENERIC_MAPPING genMap;
	genMap.GenericRead = FILE_GENERIC_READ;
	genMap.GenericWrite = FILE_GENERIC_WRITE;
	genMap.GenericExecute = FILE_GENERIC_EXECUTE;
	genMap.GenericAll = FILE_ALL_ACCESS;
	MapGenericMask(&dwGenericAccessMask, &genMap);

	// ����AccessCheck������Ƿ���ָ����Ȩ��
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