//	/*
// 	*
// 	* Copyright (C) 2003-2010 Alexandros Economou
//	*
//	* This file is part of Jaangle (http://www.jaangle.com)
// 	*
// 	* This Program is free software; you can redistribute it and/or modify
// 	* it under the terms of the GNU General Public License as published by
// 	* the Free Software Foundation; either version 2, or (at your option)
// 	* any later version.
// 	*
// 	* This Program is distributed in the hope that it will be useful,
// 	* but WITHOUT ANY WARRANTY; without even the implied warranty of
// 	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// 	* GNU General Public License for more details.
// 	*
// 	* You should have received a copy of the GNU General Public License
// 	* along with GNU Make; see the file COPYING. If not, write to
// 	* the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
// 	* http://www.gnu.org/copyleft/gpl.html
// 	*
//	*/ 
#ifndef _CSTRINGUTILS_H
#define _CSTRINGUTILS_H

///////////////////////////////////////////////////////////////////////////////
//
// _tcsltrim()
//
// Purpose:     Removes (trims) leading whitespace characters from a string
//
// Parameters:  pszSource - Pointer to the null-terminated string to be trimmed. 
//                          On return, pszSource will hold the trimmed string
//
// Returns:     TCHAR *   - pointer to trimmed string
//
TCHAR* _tcsltrim(TCHAR * pszSource);
TCHAR* _tcsrtrim(TCHAR * pszSource);
TCHAR* _tcstrim(TCHAR * pszSource);

TCHAR* _tcsproper(TCHAR* str);

const char*  __fastcall stristr(const char* pszMain, const char* pszSub);
const WCHAR* __fastcall wcsistr(const WCHAR* pszMain, const WCHAR* pszSub);

#ifdef UNICODE
#define _tcsistr wcsistr
#else
#define _tcsistr stristr
#endif

const char* stristrex(const char* pszMain, const char* pszSub, int range);
const WCHAR* wcsistrex(const WCHAR* pszMain, const WCHAR* pszSub, int range);
#ifdef UNICODE
#define _tcsistrex wcsistrex
#else
#define _tcsistrex stristrex
#endif


//Replaces chars or array if chars to 'replace' from str
//int _tchrrep(TCHAR find, TCHAR replace, LPTSTR str);
//int _tchrrep(LPCTSTR findOne, TCHAR replace, LPTSTR str);


//------------------------------------------------------------------
//-----------Copied by the normal c source - Generic implementations
//------------------------------------------------------------------
//Templated implementation of strchr (Used by templates)
//T* is a null terminated array of char/wchar_t
template <class T>
T* __cdecl genstrchr(T* string, T ch)
{
	while (*string && *string != (T)ch)
		string++;

	if (*string == (T)ch)
		return((T *)string);
	return(NULL);
}
//Templated implementation of strlen (Used by templates)
//T* is a null terminated array of char/wchar_t
template <class T>
size_t __cdecl genstrlen (const T * string)
{
	const T *eos = string;
	while( *eos++ ) ;
	return( (size_t)(eos - string - 1) );
}

//Templated implementation of strstr (Used by templates)
template <class T>
T* __cdecl genstrstr (const T * string1, const T * string2)
{
	T *cp = (T *) string1;
	T *s1, *s2;
	if ( !*string2)
		return (T *)string1;
	while (*cp)
	{
		s1 = cp;
		s2 = (T *) string2;
		while ( *s1 && *s2 && !(*s1-*s2) )
			s1++, s2++;
		if (!*s2)
			return(cp);
		cp++;
	}
	return(NULL);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------

//genstrchrrep:
//Replace all 'find' / 'findone' chars with the 'replace' char in a 'str' string
//returns the number of replaces
template <class T>
int genstrchrrep(T find, T replace, T* str)
{
	int ret = 0;
	T* f = genstrchr(str, find);
	while (f != NULL)
	{
		ret++;
		*f = replace;
		f = genstrchr(f, find);
	}
	return ret;
}

template <class T>
int genstrchrrep(const T* findOne, T replace, T* str)
{
	int ret = 0;
	const T* f = findOne;
	while (*f != NULL)
	{
		ret += genstrchrrep(*f, replace, str);
		f++;
	}
	return ret;
}

#define	_tcschrrep genstrchrrep<TCHAR>
#define	strchrrep genstrchrrep<char>
#define	wcschrrep genstrchrrep<wchar_t>

#define INVALID_CHARS_FOR_FILENAME _T("/\\:?*\"<>|")


//Templated implementation of strrchrex
//This function tries to reverse find a char/wchar_t int a string from starting pos
//The difference with strrchr is that the function can search from anywhere in the 
//string and not from just the last char of the string.
//PARAMS:
//	searchableString: The string that will be searched
//	startPos: From which character should we start searching?
//	charToSearch: The char to search
//RETURNS:
//	NULL if charToSearch cannot be found. Otherwise the position
template <class T>
const T* genstrrchrex(const T* searchableString, const T* startPos, T charToSearch)
{
	ASSERT(searchableString != NULL && startPos != NULL);
	ASSERT(startPos > searchableString);
	while (*startPos != charToSearch)
	{
		startPos--;
		if (startPos <= searchableString)
			return NULL;
	}
	return startPos;
}

#define	_tcsrchrex genstrrchrex<TCHAR>
#define	strrchrex genstrrchrex<char>
#define	wcsrchrex genstrrchrex<wchar_t>



//Templated implementation of genstrReplaceInl
//This function tries to replace every instance of fndStr with repStr
//searching in a srcStr. If the repStr is larger than fndStr then the
//srcStr will get larger until reaching the maxcapacity value
//PARAMS:
//	srcStr: The string that will be read/modified
//	maxCapacity: The maximum size that srcStr can be resized to fit the result. If NULL it is the size of the input string
//	fndStr: The string that will be replaced
//	repStr: The string that will replace fndStr (NOTE: can be "" in order to delete all instances of fndStr)
//RETURNS:
//	-1 on error. Onterwise the number of successful replaces
template <class T>
INT genstrReplaceInl(T* srcStr, size_t maxCapacity, const T* fndStr, const T* repStr)
{
	ASSERT(srcStr != NULL && fndStr != NULL && repStr != NULL);
	UINT fndStrLen = (UINT)genstrlen(fndStr);
	UINT repStrLen = (UINT)genstrlen(repStr);
	UINT strLen = (UINT)genstrlen(srcStr);
	if (fndStrLen == 0)	return -1;
	if (strLen == 0)	return -1;
	if (maxCapacity < strLen)	maxCapacity = strLen;
	T* pos;
	pos = genstrstr(srcStr, fndStr);
	INT repCount = 0;
	while (pos != NULL)
	{
		if (maxCapacity < strLen +  (repCount + 1) * (repStrLen - fndStrLen))
			return -1;
		if (repStrLen != fndStrLen)
			memmove(&pos[repStrLen], &pos[fndStrLen], (strLen - (pos - srcStr) - fndStrLen + 1) * sizeof(T));
		memcpy(pos, repStr, repStrLen * sizeof(T));
		pos+=repStrLen;
		strLen += (repStrLen - fndStrLen);
		pos = genstrstr(pos, fndStr);
		repCount++;
	}
	return repCount;
}

#define strReplaceInl genstrReplaceInl<char>
#define wcsReplaceInl genstrReplaceInl<wchar_t>
#define _tcsReplaceInl genstrReplaceInl<TCHAR>

//Converts an HTML codepage (html) to Unicode
//using the "charset" tag in the code page.
//Use (size) if html is not null-terminated
//returns a buffer that the client must free
LPWSTR HTMLText2Unicode(LPCSTR html, UINT size = -1);

BOOL InlineHTML2Text(LPTSTR htmlText);



//=== Usage like CA2CT() for String Change Cases
enum ChangeCaseTypeEnum
{
	CCT_First,
	CCT_Lower,
	CCT_Upper,
	CCT_Proper,
	CCT_FirstCapital,
	CCT_Last
};
//=== As with CA2CT use with (LPCTSTR) cast in _sntprinf and general in fuction that does not
//=== have explicit (LPCTSTR) || (LPTSTR) castings
class CChangeCase
{
	static const INT s_nBufferLength = 128;
public:
	CChangeCase(LPCTSTR psz, ChangeCaseTypeEnum type):
	  m_psz(NULL),
		  m_heapBuffer(NULL)
	  {
		  Init(psz, type);
	  }
	  ~CChangeCase()
	  {
		  delete m_heapBuffer;
	  }

	  operator LPTSTR()
	  {
		  return (m_psz);
	  }
	  operator LPCTSTR()
	  {
		  return (m_psz);
	  }

	  LPCTSTR GetBuffer()	{return m_psz;}

private:
	void Init(LPCTSTR psz, ChangeCaseTypeEnum type)
	{
		if (psz == NULL || psz[0] == 0)
		{
			m_psz = m_stackBuffer;
			m_psz[0] = 0;
			return;
		}
		int nLength = _tcslen( psz )+1;
		if (nLength < s_nBufferLength)
			m_psz = m_stackBuffer;
		else
		{
			delete m_heapBuffer;
			m_heapBuffer = new TCHAR[nLength];
			m_psz = m_heapBuffer;
		}
		_tcscpy(m_psz, psz);
		switch (type)
		{
		case CCT_Proper:
			_tcsproper(m_psz);
			break;
		case CCT_Lower:
			_tcslwr(m_psz);
			break;
		case CCT_Upper:
			_tcsupr(m_psz);
			break;
		case CCT_FirstCapital:
			_tcslwr(m_psz);
			m_psz[0] = _totupper(m_psz[0]);
			break;
		}
	}


public:
	LPTSTR m_psz;
	TCHAR m_stackBuffer[s_nBufferLength];
	LPTSTR m_heapBuffer;

private:
	CChangeCase( const CChangeCase& )				{}
	CChangeCase& operator=( const CChangeCase& )	{return *this;}
};




#endif 
