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
#include "stlStringUtils.h"

#ifdef _UNITTESTING
BOOL TestStlStringUtils()
{
	UNITTESTSTART;
	std::tstring tst1(_T("     \tThis is a test\r\n"));
	trimLeft(tst1, (LPCTSTR) NULL);
	UNITTEST(tst1 == _T("This is a test\r\n"));
	trimRight(tst1, (LPCTSTR) NULL);
	UNITTEST(tst1 == _T("This is a test"));
	UNITTEST(replace(tst1, _T("is"), _T("was")) == 2);
	UNITTEST(tst1 == _T("Thwas was a test"));
	UNITTEST(replace(tst1, _T("was"), _T("is"), 1) == 1);
	UNITTEST(tst1 == _T("This was a test"));

	std::string ansi = "Ελληνικά όπως ΠάντΆ. English";
	UNITTEST(Ansi2Unicode(tst1, ansi));
	UNITTEST(tst1 == _T("Ελληνικά όπως ΠάντΆ. English"));


#ifdef _VECTOR_
	std::vector<std::tstring> vec;
	UNITTEST(splitString(tst1, std::tstring(_T("|")), vec, FALSE) == 0);
	UNITTEST(vec[0] == tst1);
	vec.clear();
	UNITTEST(splitString(tst1, std::tstring(_T(". ")), vec, FALSE) == 1);
	UNITTEST(vec[0] == _T("�������� ���� ����"));
	UNITTEST(vec[1] == _T("English"));
#endif
	return TRUE;
}
#endif

void trimLeft(std::basic_string<char> &str, const char* delims/* = " \t\r\n"*/)
{
	ASSERT(delims != NULL);
	str.erase(0, str.find_first_not_of(delims));
}
void trimLeft(std::basic_string<wchar_t> &str, const wchar_t* delims/* = L" \t\r\n"*/)
{
	ASSERT(delims != NULL);
	str.erase(0, str.find_first_not_of(delims));
}
void trimRight(std::basic_string<char> &str, const char* delims/* = " \t\r\n"*/)
{
	ASSERT(delims != NULL);
	str.erase(str.find_last_not_of(delims) + 1);
}
void trimRight(std::basic_string<wchar_t> &str, const wchar_t* delims/* = L" \t\r\n"*/)
{
	ASSERT(delims != NULL);
	str.erase(str.find_last_not_of(delims) + 1);
}
void trim(std::basic_string<char> &str, const char* delims /*= " \t\r\n"*/)
{
	trimLeft(str, delims);
	trimRight(str, delims);
}
void trim(std::basic_string<wchar_t> &str, const wchar_t* delims/* = L" \t\r\n"*/)
{
	trimLeft(str, delims);
	trimRight(str, delims);
}


BOOL Ansi2Unicode(std::wstring& unicode, const std::string& ansi, UINT winCodePage /*= CP_OEMCP*/)
{
	UINT wLen = MultiByteToWideChar(winCodePage, 0, ansi.c_str(), (INT)ansi.size(), NULL, 0);
	if (wLen > 0)
	{
		LPWSTR ret = new wchar_t[wLen + 1];
		MultiByteToWideChar(winCodePage, 0, ansi.c_str(), (INT)ansi.size(), ret, wLen);
		ret[wLen] = 0;
		unicode = ret;
		delete ret;
		return TRUE;
	}
	return FALSE;
}

INT RemoveEnclosedString(std::wstring& text, LPCTSTR fndStart, LPCTSTR fndEnd)
{
	INT removals = 0;
	size_t start = text.find(fndStart);
	while (start != std::string::npos)
	{
		size_t end = text.find(fndEnd, start);
		if (end != std::string::npos)
		{
			end += _tcslen(fndEnd);
			removals++;
			text.erase(start, end - start);
		}
		else
			break;
		start = text.find(fndStart);
	}
	return removals;
}



#ifdef _VECTOR_



//std::vector<std::tstring> splitString(std::tstring& input, const TCHAR seperator) 
//{ 
//	ASSERT(input.empty()==false); 
//	ASSERT(input[0]!=seperator); 
//	ASSERT(input[input.size()-1]!=seperator);
//	std::vector<std::tstring> result; 
//	int pos = 0; 
//	while(pos<static_cast<int>(input.size())) 
//	{ 
//		if (input[pos]==seperator) 
//		{ 
//			const std::tstring found = input.substr(0,pos); 
//			result.push_back(found); 
//			input = input.substr(pos+1,input.size()-pos); pos = 0; 
//		} 
//		++pos; 
//	} 
//	result.push_back(input); 
//	return result; 
//}
//
//std::vector<std::tstring> splitString(std::tstring& input, const std::tstring& seperator) 
//{ 
//	ASSERT(input.empty()==false); 
//	ASSERT(input.substr(0,seperator.size()) != seperator); 
//	ASSERT(input.substr(input.size()-seperator.size(),seperator.size())!=seperator); 
//	std::vector<std::tstring> result; 
//	int pos = 0; 
//	while(pos<static_cast<int>(input.size())) 
//	{ 
//		if (input.substr(pos,seperator.size())==seperator) 
//		{ 
//			const std::tstring found = input.substr(0,pos); 
//			result.push_back(found); 
//			input = input.substr(pos+seperator.size(),input.size()-pos); 
//			pos = 0; 
//		} 
//		++pos; 
//	} 
//	result.push_back(input); 
//	return result; 
//} 
#endif
