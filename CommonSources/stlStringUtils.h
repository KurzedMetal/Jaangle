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
#ifndef _stlStringUtils_h_
#define _stlStringUtils_h_

#ifndef _STRING_
#include <string>
#endif


namespace std
{
	typedef basic_string<TCHAR> tstring;
}

#ifdef _UNITTESTING
BOOL TestStlStringUtils();
#endif

void trimLeft(std::basic_string<char> &str, const char* delims = " \t\r\n");
void trimLeft(std::basic_string<wchar_t> &str, const wchar_t* delims = L" \t\r\n");
void trimRight(std::basic_string<char> &str, const char* delims = " \t\r\n");
void trimRight(std::basic_string<wchar_t> &str, const wchar_t* delims = L" \t\r\n");
void trim(std::basic_string<char> &str, const char* delims = " \t\r\n");
void trim(std::basic_string<wchar_t> &str, const wchar_t* delims = L" \t\r\n");


//"replace" replaces maxtimes the replacewhat with the replaceWithWhat "target" string
//if maxtimes = 0 (default) replaces All 'replacewhat'
template<class T>
UINT replace(std::basic_string<T>& target, 
			 const std::basic_string<T>& replaceWhat, 
			 const std::basic_string<T>& replaceWithWhat,
			 UINT maxTimes)
{
	UINT count = 0;
	while(1)
	{
		const int pos = (const int) target.find(replaceWhat);
		if (pos==-1) 
			break;
		target.replace(pos,replaceWhat.size(),replaceWithWhat);
		count++;
		if (maxTimes == count)
			break;
	}
	return count;
}

template<class T>
UINT replace(std::basic_string<T>& target, 
			 const std::basic_string<T>& replaceWhat, 
			 const std::basic_string<T>& replaceWithWhat)
{
	return replace(target, replaceWhat, replaceWithWhat, 0);
}

template<class T>
UINT replace(std::basic_string<T>& target, 
			 const T* replaceWhat, 
			 const T* replaceWithWhat,
			 UINT maxTimes)
{
	UINT replaces = 0;
	size_t pos = target.find(replaceWhat);

	const T *eos = replaceWhat;
	while( *eos++ ) ;
	size_t lenWhat = (size_t)(eos - replaceWhat - 1);

	eos = replaceWithWhat;
	while( *eos++ ) ;
	size_t lenWithWhat = (size_t)(eos - replaceWithWhat - 1);

	UINT count = 0;
	while (pos != std::tstring::npos)
	{
		replaces++;
		target.replace(pos, lenWhat, replaceWithWhat);
		pos = target.find(replaceWhat, pos + lenWithWhat);
		count++;
		if (maxTimes == count)
			break;

	}
	return replaces;
}

template<class T>
UINT replace(std::basic_string<T>& target, 
			 const T* replaceWhat, 
			 const T* replaceWithWhat)
{
	return replace(target, replaceWhat, replaceWithWhat, 0);
}




template<class T>
INT RemoveEnclosedString(std::basic_string<T>& text, 
						 const T* fndStart, 
						 const T* fndEnd);



BOOL Ansi2Unicode(std::wstring& unicode, 
				  const std::string& ansi, 
				  UINT winCodePage = CP_ACP);

template<class _Elem>
INT getToken(std::basic_string<_Elem>& input,
			 INT startPos,
			 std::basic_string<_Elem>& delimiter,
			 std::basic_string<_Elem>& token)
{
	size_t endPos = input.find(delimiter, startPos);
	if (endPos == std::basic_string<_Elem>::npos)
		return -1;
	token = input.substr(startPos, endPos - startPos);
	return INT (endPos + delimiter.size());
}

#ifdef _VECTOR_

//Appends the splits of the "input" param to the results
//If the delimiter is not found it appends the whole string
//returns the number of splits that it has found
//results is not cleared from any previous items
template<class _Elem>
INT splitString(const std::basic_string<_Elem>& input, 
				const std::basic_string<_Elem>& delimiter, 
				std::vector<std::basic_string<_Elem> >& results, 
				bool includeEmpties = true)
{
	if (input.empty() || delimiter.empty())
		return 0;
	size_t delimSize = delimiter.size();
	INT startPos = 0;
	INT endPos = input.find (delimiter, 0);
	INT vecSize = results.size();
	while(endPos != std::basic_string<_Elem>::npos)
	{
		if (startPos == endPos && includeEmpties)
			results.push_back(std::basic_string<_Elem>());
		else
			results.push_back(input.substr(startPos, endPos - startPos));
		startPos = endPos + delimSize;
		endPos = input.find (delimiter, startPos);
	}
	results.push_back(input.substr(startPos));
	return results.size() - 1 - vecSize;//Splits
}

//std::vector<std::tstring> splitString(
//									  std::tstring& input, 
//									  const TCHAR seperator);
//
//std::vector<std::tstring> splitString(
//									  std::tstring& input, 
//									  const std::tstring& seperator);

#endif

#endif

