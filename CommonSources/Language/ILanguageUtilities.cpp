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
#include "ILanguageUtilities.h"

BOOL ILanguageUtilities::CompareLanguages(ILanguage& first, ILanguage& second, LanguageComparison& stats, std::basic_string<TCHAR>& log)
{
	BOOL bTranslated = FALSE;
	stats = LanguageComparison();
	TCHAR bf[100];
	for (int i = 0; i < 1000; i++)
	{
		LPCTSTR firstRet = first.GetString(i);
		LPCTSTR secRet = second.GetString(i);
		if (firstRet != NULL)
		{
			stats.firstCount++;
			if (secRet != NULL)
			{
				stats.secondCount++;
				if (_tcscmp(firstRet, secRet) == 0)
				{
					log += _T("Same string found: ");
					log += _itot(i, bf, 10);
					log += _T(" = ");
					log += firstRet;
					log += _T("\r\n");
					stats.same++;
				}
			}
			else
			{
				stats.notInSecond++;
				log += _T("Untranslated string found: ");
				log += _itot(i, bf, 10);
				log += _T(" = ");
				log += firstRet;
				log += _T("\r\n");
			}
		}
		else
		{
			if (secRet != NULL)
			{
				stats.secondCount++;
				stats.notInFirst++;
				log += _T("Unused string found: ");
				log += _itot(i, bf, 10);
				log += _T(" = ");
				log += secRet;
				log += _T("\r\n");
			}
		}
	}
	return TRUE;
}


LPCTSTR const LNG_Name = _T("NAME");
LPCTSTR const LNG_LocalizedName = _T("LOCALIZEDNAME");
LPCTSTR const LNG_AuthorName = _T("AUTH");
LPCTSTR const LNG_Email = _T("EMAIL");
LPCTSTR const LNG_Description = _T("DESC");
LPCTSTR const LNG_WebSite = _T("WWW");
LPCTSTR const LNG_Version = _T("VERSION");
LPCTSTR const LNG_Build = _T("BUILD");
LPCTSTR const LNG_AutoUpdateURL = _T("AUTOUPDATEURL");


BOOL ILanguageUtilities::ImportLanguageFile(LPCTSTR pathName, ILanguage& destLanguage, BOOL bInfoOnly/* = FALSE*/)
{
	ASSERT(pathName != NULL);
	if (_taccess(pathName, 2) == 0)
	{
		std::auto_ptr<TCHAR> dta(GetLanguageFile(pathName));
		if (dta.get() != NULL)
		{
			destLanguage.ClearStrings();
			if (!bInfoOnly)
			{
				if (!GetLanguageStrings(dta.get(), destLanguage))
					return FALSE;
			}
			LanguageInfo& li = destLanguage.GetLanguageInfo();
			li = LanguageInfo();
			TCHAR fileTitle[MAX_PATH];
			GetFileTitle(pathName, fileTitle, MAX_PATH);
			TCHAR* dot = _tcsrchr(fileTitle, '.');
			if (dot != NULL)
				*dot = 0;
			li.Name = fileTitle;
			//li.Path = pathName;
			return GetLanguageInfo(dta.get(), li);
		}
	}
	return FALSE;
}

BOOL ILanguageUtilities::ExportLanguageFile(LPCTSTR pathName, ILanguage* pSrcLanguage, ILanguage* pReferenceLanguage/* = NULL*/)
{
	ASSERT(pathName != NULL);
	ASSERT(pSrcLanguage!=NULL ||pReferenceLanguage!=NULL);
	CFile f;
	if (!f.Open(pathName, CFile::modeCreate | CFile::modeWrite))
		return FALSE;
#ifdef _UNICODE
	//Write Unicode header
	BYTE b = 0xff;
	f.Write(&b, 1);
	b = 0xfe;
	f.Write(&b, 1);
#endif
	const TCHAR* const Info = _T("#**************How To Translate**************\r\n")
		_T("#1. All the lines that start with '#' are comments and will be ignored\r\n")
		_T("#2. The lines that starts with @ store info about the tranlation file. Specifically:\r\n")
		_T("#   @NAME=Language Name in English.\r\n")
		_T("#   @LOCALIZEDNAME=Native Language Name.\r\n")
		_T("#   @AUTH=Author Name.\r\n")
		_T("#   @EMAIL=Author's Email\r\n")
		_T("#   @DESC=Description of this language file\r\n")
		_T("#   @WWW=Author's WebSite\r\n")
		_T("#   @VERSION=Language File Version\r\n")
		_T("#   @BUILD=Program's build that this translation is based on\r\n")
		_T("#   @AUTOUPDATEURL= (future feature that will support live lang pack update)\r\n")
		_T("#3. All The following lines which start with (number)= are the ones that you\r\n")
		_T("#   must translate. Untranslated strings are initially commented.\r\n")
		_T("#   Already translated strings are (optionally) kept for reference as a comment\r\n")
		_T("#   CAUTION!!!\r\n")
		_T("#   Translation strings are ONE-LINERS.\r\n")
		_T("#   DO NOT CHANGE %s (strings) %d (integers) and other %_ as they placeholders.\r\n")
		_T("#   DO NOT CHANGE the numbers before the'='.\r\n")
		_T("#   Watch for special characters like '\\r\\n' or '\\n' which means new line.\r\n")
		_T("#   Take care of the HTML Tags.\r\n")
		_T("\r\n");
	f.Write(Info, _tcslen(Info) * sizeof(TCHAR));

	const LanguageInfo& li = pSrcLanguage ? pSrcLanguage->GetLanguageInfo() : LanguageInfo();
	SaveHeader(f, LNG_Name, li.Name.c_str());
	SaveHeader(f, LNG_LocalizedName, li.LocalizedName.c_str());
	SaveHeader(f, LNG_AuthorName, li.Author.c_str());
	SaveHeader(f, LNG_Email, li.Email.c_str());
	SaveHeader(f, LNG_Description, li.Description.c_str());
	SaveHeader(f, LNG_WebSite, li.WebSite.c_str());
	SaveHeader(f, LNG_Version, li.Version.c_str());
	TCHAR bf[30];
	_itot(li.BuildNum, bf, 10);
	SaveHeader(f, LNG_Build, bf);
	SaveHeader(f, LNG_AutoUpdateURL, li.AutoUpdateURL.c_str());
	f.Write(_T("\r\n"), 2 * sizeof(TCHAR));


	BOOL bTranslated = FALSE;
	std::tstring strLoader;
	for (int i = 0; i < 1000; i++)
	{
		LPCTSTR srcStr = pSrcLanguage ? pSrcLanguage->GetString(i) : NULL;
		LPCTSTR refStr = pReferenceLanguage ? pReferenceLanguage->GetString(i) : NULL;
		if (refStr == NULL)
			continue;
		TCHAR header[10];
		_sntprintf(header, 10, _T("%d="), i);
		header[9] = 0;
		strLoader = refStr;
		Replace(strLoader, _T("\r"), _T("\\r"));
		Replace(strLoader, _T("\n"), _T("\\n"));
		Replace(strLoader, _T("\t"), _T("\\t"));
		f.Write(_T("#"), sizeof(TCHAR));
		f.Write(header, _tcslen(header) * sizeof(TCHAR));
		f.Write(strLoader.c_str(), strLoader.size() * sizeof(TCHAR));
		f.Write(_T("\r\n"), 2 * sizeof(TCHAR));
		if (srcStr != NULL)
		{
			strLoader = srcStr;
			Replace(strLoader, _T("\r"), _T("\\r"));
			Replace(strLoader, _T("\n"), _T("\\n"));
			Replace(strLoader, _T("\t"), _T("\\t"));
			f.Write(header, _tcslen(header) * sizeof(TCHAR));
			f.Write(strLoader.c_str(), strLoader.size() * sizeof(TCHAR));
			f.Write(_T("\r\n"), 2 * sizeof(TCHAR));
		}
	}
	f.Close();
	return TRUE;
}

LPTSTR ILanguageUtilities::GetLanguageFile(LPCTSTR pathName)
{
	ASSERT(pathName != NULL);
	CFile f;
	if (!f.Open(pathName, CFile::modeRead))
		return FALSE;
	UINT fLen = (UINT) f.GetLength();
	if (fLen < 5)
	{
		f.Close();
		return FALSE;
	}
	BYTE* fileData = new BYTE[fLen + sizeof(TCHAR)];
	f.Read(fileData, fLen);
	fileData[fLen] = 0;
	f.Close();
	memset(&fileData[fLen], 0, sizeof(TCHAR));//Make it null terminated
	LPTSTR ret = new TCHAR[fLen - 2];
#ifdef _UNICODE
	if (fileData[0] == 0xff && fileData[1] == 0xfe)//The file is in UNICODE
		_tcsncpy(ret, (LPCTSTR)CW2CT((LPCTSTR)&fileData[2]), fLen - 2);
	else//The file is in ANSI
		_tcsncpy(ret, (LPCTSTR)CA2CT((LPCSTR)fileData), fLen - 2);
#else
	//if (fileData[0] == 0xff && fileData[1] == 0xfe)//The file is in UNICODE
	//	stringData = CT2A(&fileData[2]);
	//else//The file is in ANSI
	//	stringData = fileData;
#endif
	delete [] fileData;
	return ret;
}

void ILanguageUtilities::SaveHeader(CFile& f, LPCTSTR header, LPCTSTR info)
{
	ASSERT(header != NULL);
	ASSERT(_tcslen(header) < 20);
	f.Write(_T("@"), sizeof(TCHAR));
	f.Write(header, _tcslen(header) * sizeof(TCHAR));
	f.Write(_T("="), sizeof(TCHAR));
	if (_tcslen(info) > 0)
		f.Write(info, _tcslen(info) * sizeof(TCHAR));
	f.Write(_T("\r\n"), 2 * sizeof(TCHAR));
}

BOOL ILanguageUtilities::GetLanguageStrings(LPCTSTR langFileData, ILanguage& destLanguage)
{
	LPCTSTR startPos = langFileData;
	std::tstring strBuffer;
	while (TRUE)
	{
		if (*startPos != '@' && *startPos != '#')
		{
			LPCTSTR eqPos = _tcschr(startPos, '=');
			if (eqPos == 0)	
				break;						//EOF FOUND
			if (eqPos - startPos <= 5)					//Starting number cant'be larger than this. Usually '234='
			{
				TCHAR bf[20];
				_tcsncpy(bf, startPos, eqPos - startPos);
				bf[eqPos - startPos] = 0;
				int resNum = _ttoi(startPos);
				if (resNum > 0)							//Invalid ID before equality
				{
					LPCTSTR lineEndPos = _tcschr(eqPos, 13);
					strBuffer.clear();
					if (lineEndPos != 0)				//EOF
						strBuffer.insert(0, &eqPos[1], lineEndPos - eqPos - 1);
					else
						strBuffer.insert(0, &eqPos[1]);
					FixString(strBuffer);
					destLanguage.SetString(resNum, strBuffer.c_str());
					startPos = lineEndPos;
				}
				else
				{
					TRACE(_T("@1ILanguageUtilities::GetLanguageStrings. LngFile error. Near:'%.20s'\r"), startPos);
					startPos = _tcschr(eqPos, 13);
				}
			}
			else
			{
				TRACE(_T("@1ILanguageUtilities::GetLanguageStrings. LngFile error. (eqPos - startPos) Near:'%.20s'\r"), startPos);
				startPos = _tcschr(eqPos, 13);
			}
		}
		else
		{
			startPos = _tcschr(startPos, 13);
		}
		if (startPos == NULL)
			break;
		while (*startPos == 13 || *startPos == 10)
			startPos++;
		if (*startPos == 0)
			break;
	}
	return TRUE;
}
void ILanguageUtilities::FixString(std::tstring& str)
{
	Replace(str, _T("\\r"), _T("\r"));
	Replace(str, _T("\\n"), _T("\n"));
	Replace(str, _T("\\t"), _T("\t"));
}

void ILanguageUtilities::Replace(std::tstring& str, LPCTSTR oldString, LPCTSTR newString)
{
	int pos = str.find(oldString);
	while (pos != -1)
	{
		str.replace(pos, _tcslen(oldString), newString);
		pos = str.find(oldString);
	}
}

BOOL ILanguageUtilities::GetLanguageInfo(LPCTSTR langFileData, LanguageInfo& languageInfo)
{
	LPCTSTR startPos = langFileData;
	while (TRUE)
	{
		if (*startPos == '@')
		{
			LPCTSTR eqPos = _tcschr(startPos, '=');
			LPCTSTR lineEndPos = _tcschr(startPos, 13);
			if (eqPos > 0 && (eqPos < lineEndPos || lineEndPos == 0))
			{
				if (startPos[1] == 0 || eqPos[1] == 0)
					TRACE(_T("@1CLanguage::LoadLanguageFile War: (Unknown info field). Near:'%.20s'\r"), startPos);
				else
				{
					TCHAR key[20];
					if (eqPos - startPos - 1 < 20)
					{
						_tcsncpy(key, &startPos[1],eqPos - startPos - 1);
						key[eqPos - startPos - 1] = 0;
						std::tstring* refString = 0;
						if (_tcscmp(key, LNG_Name) == 0)
							refString = &languageInfo.Name;
						else if (_tcscmp(key, LNG_LocalizedName) == 0)
							refString = &languageInfo.LocalizedName;
						else if (_tcscmp(key, LNG_AuthorName) == 0)
							refString = &languageInfo.Author;
						else if (_tcscmp(key, LNG_Email) == 0)
							refString = &languageInfo.Email;
						else if (_tcscmp(key, LNG_Description) == 0)
							refString = &languageInfo.Description;
						else if (_tcscmp(key, LNG_WebSite) == 0)
							refString = &languageInfo.WebSite;
						else if (_tcscmp(key, LNG_Version) == 0)
							refString = &languageInfo.Version;
						else if (_tcscmp(key, LNG_AutoUpdateURL) == 0)
							refString = &languageInfo.AutoUpdateURL;

						if (_tcscmp(key, LNG_Build) == 0)
						{
							if (lineEndPos - eqPos - 1 < 10)
							{
								TCHAR build[20];
								_tcsncpy(build, &eqPos[1], lineEndPos - eqPos - 1);
								build[lineEndPos - eqPos - 1] = 0;
							}
						}
						else if (refString != NULL)
						{
							refString->clear();
							refString->insert(0, &eqPos[1], lineEndPos - eqPos - 1);
							FixString(*refString);
						}
						else
							TRACE(_T("LanguageManager::LoadLanguageInfoPriv. Unknown Key: '%s'\r\n"), key);
					}



				}
				startPos = lineEndPos;
			}
		}
		else
			startPos = _tcschr(startPos, 13);
		if (startPos == NULL)
			break;
		while (*startPos == 13 || *startPos == 10)
			startPos++;
		if (*startPos == 0)
			break;
	}
	return TRUE;
}


