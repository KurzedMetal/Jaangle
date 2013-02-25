#include "stdafx.h"
#include "UINTtoStringMapping.h"

#define UINTtoStringMappingFILEHEADER "U2SM"
#define UINTtoStringMappingFILEVERSION 1

BOOL UINTtoStringMapping::Load(LPCTSTR pathName)
{
	m_bDirty = FALSE;
	HANDLE h = CreateFile(pathName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;
	BOOL bRet = FALSE;
	DWORD fileVersion = 0, nItems = 0, br = 0;
	CHAR fileHeader[sizeof(UINTtoStringMappingFILEHEADER)];
	ReadFile(h, &fileHeader, sizeof(UINTtoStringMappingFILEHEADER), &br, 0);
	if (memcmp(fileHeader, UINTtoStringMappingFILEHEADER, 4) != 0)
	{
		CloseHandle(h);
		return FALSE;
	}
	ReadFile(h, &fileVersion, sizeof(DWORD), &br, 0);
	if (fileVersion != UINTtoStringMappingFILEVERSION)
	{
		CloseHandle(h);
		return FALSE;
	}
	ReadFile(h, &nItems, sizeof(nItems), &br, 0);
	if (nItems > 1000000)
	{
		CloseHandle(h);
		return FALSE;
	}
	INT bufferSize = 1000;
	BYTE* buffer = new BYTE[bufferSize];
	bRet = TRUE;
	for (UINT i = 0; i < nItems; i++)
	{
		DWORD key, size;
		if (ReadFile(h, &key, sizeof(key), &br, 0) && br == sizeof(key))
		{
			if (ReadFile(h, &size, sizeof(size), &br, 0) && br == sizeof(size))
			{
				if (size >= bufferSize - sizeof(TCHAR))
				{
					delete [] buffer;
					bufferSize = size + sizeof(TCHAR);
					buffer = new BYTE[bufferSize];
				}
				if (ReadFile(h, buffer, size, &br, 0) && br == size)
				{
					LPTSTR str = (LPTSTR)buffer;
					str[size / sizeof(TCHAR)] = 0;
					Set(key, str);
				}
				else
				{
					bRet = FALSE;
					break;
				}
				
			}
			else
			{
				bRet = FALSE;
				break;
			}
		}
		else
		{
			bRet = FALSE;
			break;
		}
	}
	delete buffer;
	CloseHandle(h);
	m_bDirty = FALSE;
	if (!bRet)
		Clear();
	return bRet;
}

BOOL UINTtoStringMapping::Save(LPCTSTR pathName)
{
	m_bDirty = FALSE;

	HANDLE h = CreateFile(pathName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if (h == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD bw = 0;
	//=== Writing the file header
	WriteFile(h, &UINTtoStringMappingFILEHEADER, sizeof(UINTtoStringMappingFILEHEADER), &bw, 0);
	//=== Writing the file version
	DWORD fileVersion = 1;
	WriteFile(h, &fileVersion, sizeof(DWORD), &bw, 0);
	//=== Writing a DWORD for "number of items"
	DWORD nItems = m_mapping.size();
	WriteFile(h, &nItems, sizeof(DWORD), &bw, 0);

	//=== Writing the map in the format UINT-StringLen-String
	U2SMap::iterator it = m_mapping.begin();
	BOOL bRet = TRUE;
	for (; it != m_mapping.end(); it++)
	{
		DWORD key = it->first;
		if (!WriteFile(h, &key, sizeof(DWORD), &bw, 0))
		{
			bRet = FALSE;
			break;
		}
		DWORD stringSize = it->second.size() * sizeof(TCHAR);
		if (!WriteFile(h, &stringSize, sizeof(DWORD), &bw, 0))
		{
			bRet = FALSE;
			break;
		}
		LPCTSTR str = it->second.c_str();
		if (!WriteFile(h, str, stringSize, &bw, 0))
		{
			bRet = FALSE;
			break;
		}
	}
	CloseHandle(h);
	return bRet;
}

LPCTSTR UINTtoStringMapping::Get(UINT key) const
{
	U2SMap::const_iterator it = m_mapping.find(key);
	if (it != m_mapping.end())
		return it->second.c_str();
	return NULL;
}

void UINTtoStringMapping::Set(UINT key, LPCTSTR value)
{
	if (value != NULL)
	{
		m_bDirty = TRUE;
		m_mapping[key] = value;
	}
}

void UINTtoStringMapping::Remove(UINT key)
{
	U2SMap::iterator it = m_mapping.find(key);
	if (it != m_mapping.end())
	{
		m_bDirty = TRUE;
		m_mapping.erase(it);
	}
}

void UINTtoStringMapping::Clear()
{
	if (!m_mapping.empty())
	{
		m_mapping.clear();
		m_bDirty = TRUE;
	}
}
