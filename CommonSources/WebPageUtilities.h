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
#ifndef _WebPageUtilities_h_
#define _WebPageUtilities_h_

#include <string>
#include <Wininet.h>

#ifdef _UNITTESTING
BOOL TestWebPageUtilities();
#endif


//=== CAREFULL* DO NOT USE URLS LARGER THAN 1000 BYTES!!! USE POST INSTEAD

BOOL DownloadWebPageA(std::string& page, HINTERNET hInternet, LPCSTR url);
BOOL DownloadWebPageW(std::string& page, HINTERNET hInternet, LPCWSTR url);

BOOL DownloadWebPagePostA(std::string& page, HINTERNET hInternet, LPCSTR url, LPCSTR postData);
BOOL DownloadWebPagePostW(std::string& page, HINTERNET hInternet, LPCWSTR url, LPCSTR postData);

#ifdef _UNICODE
#define DownloadWebPage DownloadWebPageW
#define DownloadWebPagePost DownloadWebPagePostW
#else
#define DownloadWebPage DownloadWebPageA
#define DownloadWebPagePost DownloadWebPagePostA
#endif

//Retrieves and checks the HTTP Headers
BOOL DownloadWebPageUnicode(std::wstring& page, HINTERNET hInternet, LPCTSTR url);
BOOL DownloadToFile(LPCTSTR fileName, HINTERNET hInternet, LPCTSTR url);

INT ReplaceHtmlEntities(std::wstring& htmlText);
//The default code page should be 1252 - Latin1 (OR 28591)
UINT GetWindowsCodePageA(LPCSTR html);		//Latin1
UINT GetWindowsCodePageW(LPCWSTR html);		//Latin1

void GetTextFromHtmlFragment(std::wstring& text, LPCTSTR startFragment, LPCTSTR endFragment);


//===Converts a url parameter to a UTF-8 based URL encoded string.
//===source: the url parameter.
//===encoded: the output. UrlEncode APPENDS the characters in that string
BOOL URLEncodeA(std::string& encoded, LPCSTR source);
BOOL URLEncodeW(std::wstring& encoded, LPCWSTR source);


#ifdef _UNICODE
#define URLEncode URLEncodeW
#else
#define URLEncode URLEncodeA
#endif







#endif

