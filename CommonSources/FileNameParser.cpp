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
#include "stdafx.h"
#include "FileNameParser.h"
#include "cStringUtils.h"

#define PLACEHOLDER_ARTIST	_T("$a")
#define PLACEHOLDER_ALBUM	_T("$l")
#define PLACEHOLDER_TRACKNO _T("$n")
#define PLACEHOLDER_TITLE	_T("$t")
#define PLACEHOLDER_YEAR	_T("$y")
#define PLACEHOLDER_DISCARD	_T("$d")

#ifdef _UNITTESTING
BOOL FileNameParser::UnitTest()
{
	//UNITTESTSTART;
	INT i = 0;
	while (TRUE)
	{
		LPCTSTR curFormat = FileNameParser::GetReadPredefinedFormat(i);
		if (curFormat == NULL)
			break;
		TRACE(_T("%d: %s\r\n"), i, curFormat);
		i++;
	}
	return TRUE;
}
#endif


LPCTSTR FileNameParser::GetReadPredefinedFormat(INT idx)
{
	//Must be in the form "$[id][div]...[$][id]" == >
	static LPCTSTR const predPattern[] = {
		_T("$a-$l-$n-$t"),		//D:\Music\00s\Fall Of Math\65 Days of Static - The Fall of Math - 01 - Another Code Against The Gone.mp3
		_T("$a-$l-$n.$t"),		
		_T("$n-$a-$l-$t"),
		_T("$a-$l-$t"),
		_T("$a-$n-$t"),
		_T("$n-$a-$t"),
		_T("$a-$t"),			//D:\Music\- Reggae\Abyssinians\Arise\The Abyssinians - Dem a Come.mp3
		_T("$a-$l\\$n-$t"),
		_T("$a\\$l\\$n-$t"),	
		_T("$l\\$n.$t"),	
		_T("$n-$t"),
		_T("$n.$t"),
		_T("$n $t"),
		_T("$a-$l\\$n $t"),		//...\Active Member- FIERA\01 Ligo Na Skefteis.mp3
		_T("$a-$l-$d\\$n $t"),	//...\Nine Inch Nails - The Slip - Flac\06 Head Down.flac
		_T("$t"),				//...\Gazing Down.mp3
		NULL
	};
	if (idx < (sizeof(predPattern) / sizeof(LPCTSTR)))
		return predPattern[idx];
	return NULL;
}

LPCTSTR FileNameParser::GetWritePredefinedFormat(INT idx)
{
	static LPCTSTR const predPattern[] = 
	{
		_T("$a - $l - $n - $t"),
		_T("$a - $l - $t"),
		_T("$a - $t"),
		_T("$l - $n - $t"),
		_T("$l - $t"),
		_T("$n - $a - $t"),
		_T("$n - $t"),
		_T("$n $t"),
		NULL
	};
	if (idx < (sizeof(predPattern) / sizeof(LPCTSTR)))
		return predPattern[idx];
	return NULL;
}


FileNameParser::FileNameParser():
m_TrackNo(0),
m_Year(0)
{
	m_Artist[0] = 0;
	m_Album[0] = 0;
	m_Title[0] = 0;
}

BOOL FileNameParser::ParseFileName(LPCTSTR fileName, LPCTSTR pattern/* = NULL*/)
{
	ASSERT(fileName != NULL);
	BOOL ret = FALSE;
	//06-04-2007 BugFix
	if (_tcsrchr(fileName, '.') == 0) 
		return FALSE;//Don't accept files without extension
	INT i = 0;
	if (pattern == NULL)
	{
		LPCTSTR curPattern = GetReadPredefinedFormat(i);
		while (curPattern != NULL)
		{
			if (DecodeAs(fileName, curPattern))
			{
				TRACE(_T("@4FileNameParser::ParseFileName [Automatic matched pattern: %s] %s\r\n"),
					curPattern,
					fileName);
				ret = TRUE;
				break;
			}
			i++;
			curPattern = GetReadPredefinedFormat(i);
		}
	}
	else
	{
		ret = DecodeAs(fileName, pattern);
	}
	return ret;
}

BOOL FileNameParser::DecodeAs(LPCTSTR fileName, LPCTSTR pattern)
{
	ASSERT(fileName != NULL);
	ASSERT(pattern != NULL);
	//Reset Values
	m_Artist[0] = 0;
	m_Album[0] = 0;
	m_Title[0] = 0;
	m_TrackNo = 0;
	m_Year = 0;

	//===Pattern Validity Check
	INT patternLen = _tcslen(pattern);
	if (((patternLen - 2) % 3) != 0)
	{
		//Expecting form: "$[id][div]...[$][id]" === 3 . N + 2
		TRACE(_T("@1 FileNameParser::DecodeAs pattern is not valid\r\n"));
		return FALSE;
	}

	//===Calculate the number of dividers "\" in the pattern
	INT dirDividers = 1;
	LPCTSTR patPos = _tcschr(pattern, '\\');
	while (patPos != 0)
	{
		dirDividers++;
		patPos = _tcschr(patPos + 1, '\\');
	}
	//===Calculate the Filename's starting parsing point
	LPCTSTR fnPosC = &fileName[_tcslen(fileName)];
	while (dirDividers > 0)
	{
		fnPosC = _tcsrchrex(fileName, fnPosC - 1, '\\');
		if (fnPosC == NULL)
		{
			//TRACE(_T("@2 FileNameParser::DecodeAs Not matching (dirDividers)\r\n"));
			return FALSE;
		}
		dirDividers--;
	}
	fnPosC++;
	TCHAR locFN[MAX_PATH];
	_tcsncpy(locFN, fnPosC, MAX_PATH);
	LPTSTR fnPos = (LPTSTR) _tcsrchr(locFN, '.');
	if (fnPos == 0)	
	{
		//TRACE(_T("@2 FileNameParser::DecodeAs Filename does not have an extension (proceeding)\r\n"));
	}
	else
		*fnPos = 0;
	INT curFnPos = 0;
	INT curPatPos = 0;

	while (pattern[curPatPos] == '$')
	{
		if (pattern[curPatPos + 1] == NULL)
		{
			//TRACE(_T("@1 FileNameParser::DecodeAs pattern has an illegal syntax (missing id)\r\n"));
			return FALSE;
		}
		INT moveForward = ExtractAttribute(pattern[curPatPos + 1], pattern[curPatPos + 2], &locFN[curFnPos]);
		if (moveForward == 0)
		{
			//TRACE(_T("@1 FileNameParser::DecodeAs Pattern:'%s' Filename:'%s' failed\r\n"), pattern ,fileName);
			return FALSE;
		}
		if (pattern[curPatPos + 2] == 0)//last character
			return TRUE;
		curFnPos += moveForward;
		curPatPos += 3;
	}
	if (pattern[curPatPos] != 0)
	{
		//TRACE(_T("@1 FileNameParser::DecodeAs pattern has an illegal syntax (missing $)\r\n"));
		return FALSE;
	}
	return TRUE;
}


BOOL FileNameParser::IsInteger(LPCTSTR str)
{
	int num = _ttoi(str);
	if (num > 0)
	{
		TCHAR strnum[20];
		_sntprintf(strnum, 20, _T("%d"), num);
		if (abs(int(_tcslen(strnum) - _tcslen(str))) <=1)
			return TRUE;
	}
	return FALSE;
}

INT FileNameParser::ExtractAttribute(TCHAR code, TCHAR divider, LPCTSTR str)
{
	ASSERT(str != NULL);
	if (str == NULL || str[0] == 0)
		return 0;
	TCHAR attributeString[MAX_STRING];
	if (divider != 0)
	{
		LPCTSTR fndPos = _tcschr(str, divider);
		if (fndPos == NULL || (fndPos - str >= MAX_STRING))
		{
			TRACE(_T("@3 FileNameParser::ExtractAttribute. Divider '%c' not found\r\n"), divider);
			return 0;
		}
		_tcsncpy(attributeString, str, fndPos - str);
		attributeString[fndPos - str] = 0; 
	}
	else
	{
		_tcsncpy(attributeString, str, MAX_STRING);
		attributeString[MAX_STRING - 1] = 0; 
	}
	INT retval = _tcslen(attributeString) + 1;//plus the divider
	_tcsrtrim(_tcsltrim(attributeString));


	switch (code)
	{
	case 'a'://Artist
		_tcscpy(m_Artist, attributeString);
		break;
	case 'l'://Album
		_tcscpy(m_Album, attributeString);
		break;
	case 't'://Title
		_tcscpy(m_Title, attributeString);
		break;
	case 'n'://TrackNo
		m_TrackNo = _ttoi(attributeString);
		if (m_TrackNo == 0)
			return 0;
		break;
	case 'y'://TrackNo
		m_Year = _ttoi(attributeString);
		if (m_Year == 0)
			return 0;
		break;
	case 'd'://Discard
		break;
	default:
		TRACE(_T("@1 FileNameParser::ExtractAttribute. Invalid code '%c'\r\n"), code);
		return 0;
	}
	return retval;
}


BOOL FileNameParser::TranslatePattern(LPTSTR newName, UINT bfLen,
		  LPCTSTR pattern, 
		  LPCTSTR artist, LPCTSTR album, LPCTSTR trackNo, LPCTSTR title, LPCTSTR year, LPCTSTR discard)
{
	ASSERT(pattern != NULL);
	if (pattern == NULL)
		return FALSE;
	if (artist == NULL)		
		artist = _T("<artist>");
	if (album == NULL)		
		album = _T("<album>");
	if (trackNo == NULL)	
		trackNo = _T("<track no>");
	if (title == NULL)		
		title= _T("<title>");
	if (year == NULL)		
		year = _T("<year>");
	if (discard == NULL)	
		discard = _T("<discard>");

	_tcsncpy(newName, pattern, bfLen);
	_tcsReplaceInl(newName, bfLen, PLACEHOLDER_ARTIST, artist);
	_tcsReplaceInl(newName, bfLen, PLACEHOLDER_ALBUM, album);
	_tcsReplaceInl(newName, bfLen, PLACEHOLDER_TRACKNO, trackNo);
	_tcsReplaceInl(newName, bfLen, PLACEHOLDER_TITLE, title);
	_tcsReplaceInl(newName, bfLen, PLACEHOLDER_YEAR, year);
	_tcsReplaceInl(newName, bfLen, PLACEHOLDER_DISCARD, discard);
	return TRUE;
}


