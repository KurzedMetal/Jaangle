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
#include "ITranslationUtilities.h"

BOOL ITranslationUtilities::CompareTranslations(ITranslation& first, ITranslation& second, TranslationComparison& stats, std::basic_string<TCHAR>& log)
{
	BOOL bTranslated = FALSE;
	stats = TranslationComparison();
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
LPCTSTR const LNG_AppVersion = _T("APPVERSION");
LPCTSTR const LNG_AutoUpdateURL = _T("AUTOUPDATEURL");


BOOL ITranslationUtilities::ImportTranslationFile(LPCTSTR pathName, EditableTranslation& destTranslation, BOOL bInfoOnly/* = FALSE*/)
{
	ASSERT(pathName != NULL);
	if (_taccess(pathName, 2) == 0)
	{
		std::auto_ptr<TCHAR> dta(GetTranslationFile(pathName));
		if (dta.get() != NULL)
		{
			destTranslation.ClearStrings();
			if (!bInfoOnly)
			{
				if (!GetTranslationStrings(dta.get(), destTranslation))
					return FALSE;
			}
			TCHAR fileTitle[MAX_PATH];
			GetFileTitle(pathName, fileTitle, MAX_PATH);
			TCHAR* dot = _tcsrchr(fileTitle, '.');
			if (dot != NULL)
				*dot = 0;
			destTranslation.SetTranslationInfo(ITranslation::TI_Name, fileTitle);
			//li.Path = pathName;
			return GetTranslationInfo(dta.get(), destTranslation);
		}
	}
	return FALSE;
}

BOOL ITranslationUtilities::ExportTranslationFile(LPCTSTR pathName, ITranslation* pSrcTranslation, ITranslation* pReferenceLanguage/* = NULL*/)
{
	ASSERT(pathName != NULL);
	ASSERT(pSrcTranslation!=NULL ||pReferenceLanguage!=NULL);
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
		_T("#   @WWW=Author's Website\r\n")
		_T("#   @VERSION=Language File Version\r\n")
		_T("#   @APPVERSION=Application version\r\n")
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

	ITranslation* pTrans = pSrcTranslation != NULL ? pSrcTranslation : pReferenceLanguage;
	SaveHeader(f, LNG_Name, pTrans->GetTranslationInfo(ITranslation::TI_Name));
	SaveHeader(f, LNG_LocalizedName, pTrans->GetTranslationInfo(ITranslation::TI_LocalizedName));
	SaveHeader(f, LNG_AuthorName, pTrans->GetTranslationInfo(ITranslation::TI_Author));
	SaveHeader(f, LNG_Email, pTrans->GetTranslationInfo(ITranslation::TI_Email));
	SaveHeader(f, LNG_Description, pTrans->GetTranslationInfo(ITranslation::TI_Description));
	SaveHeader(f, LNG_WebSite, pTrans->GetTranslationInfo(ITranslation::TI_WebSite));
	SaveHeader(f, LNG_Version, pTrans->GetTranslationInfo(ITranslation::TI_Version));
	SaveHeader(f, LNG_AppVersion, pTrans->GetTranslationInfo(ITranslation::TI_AppVersion));
	SaveHeader(f, LNG_AutoUpdateURL, pTrans->GetTranslationInfo(ITranslation::TI_AutoUpdateURL));
	f.Write(_T("\r\n"), 2 * sizeof(TCHAR));


	BOOL bTranslated = FALSE;
	std::tstring strLoader;
	for (int i = 0; i < 1000; i++)
	{
		LPCTSTR srcStr = pSrcTranslation ? pSrcTranslation->GetString(i) : NULL;
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

LPTSTR ITranslationUtilities::GetTranslationFile(LPCTSTR pathName)
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

void ITranslationUtilities::SaveHeader(CFile& f, LPCTSTR header, LPCTSTR info)
{
	ASSERT(header != NULL);
	ASSERT(_tcslen(header) < 20);
	f.Write(_T("@"), sizeof(TCHAR));
	f.Write(header, _tcslen(header) * sizeof(TCHAR));
	f.Write(_T("="), sizeof(TCHAR));
	if (info != NULL)
		f.Write(info, _tcslen(info) * sizeof(TCHAR));
	f.Write(_T("\r\n"), 2 * sizeof(TCHAR));
}

BOOL ITranslationUtilities::GetTranslationStrings(LPCTSTR translationFileData, EditableTranslation& destLanguage)
{
	LPCTSTR startPos = translationFileData;
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
void ITranslationUtilities::FixString(std::tstring& str)
{
	Replace(str, _T("\\r"), _T("\r"));
	Replace(str, _T("\\n"), _T("\n"));
	Replace(str, _T("\\t"), _T("\t"));
}

void ITranslationUtilities::Replace(std::tstring& str, LPCTSTR oldString, LPCTSTR newString)
{
	int pos = str.find(oldString);
	while (pos != -1)
	{
		str.replace(pos, _tcslen(oldString), newString);
		pos = str.find(oldString);
	}
}

BOOL ITranslationUtilities::GetTranslationInfo(LPCTSTR translationFileData, EditableTranslation& translation)
{
	LPCTSTR startPos = translationFileData;
	while (TRUE)
	{
		if (*startPos == '@')
		{
			LPCTSTR eqPos = _tcschr(startPos, '=');
			LPCTSTR lineEndPos = _tcschr(startPos, 13);
			if (eqPos > 0 && (eqPos < lineEndPos || lineEndPos == 0))
			{
				if (startPos[1] == 0 || eqPos[1] == 0)
					TRACE(_T("@1 ITranslationUtilities::GetTranslationInfo War: (Unknown info field). Near:'%.20s'\r"), startPos);
				else
				{
					TCHAR key[20];
					if (eqPos - startPos - 1 < 20)
					{
						_tcsncpy(key, &startPos[1],eqPos - startPos - 1);
						key[eqPos - startPos - 1] = 0;
						ITranslation::TranslationInfoEnum ti = ITranslation::TI_Last;// refString = NULL;
						if (_tcscmp(key, LNG_Name) == 0)
							ti = ITranslation::TI_Name;
						else if (_tcscmp(key, LNG_LocalizedName) == 0)
							ti = ITranslation::TI_LocalizedName;
						else if (_tcscmp(key, LNG_AuthorName) == 0)
							ti = ITranslation::TI_Author;
						else if (_tcscmp(key, LNG_Email) == 0)
							ti = ITranslation::TI_Email;
						else if (_tcscmp(key, LNG_Description) == 0)
							ti = ITranslation::TI_Description;
						else if (_tcscmp(key, LNG_WebSite) == 0)
							ti = ITranslation::TI_WebSite;
						else if (_tcscmp(key, LNG_Version) == 0)
							ti = ITranslation::TI_Version;
						else if (_tcscmp(key, LNG_AutoUpdateURL) == 0)
							ti = ITranslation::TI_AutoUpdateURL;
						else if (_tcscmp(key, LNG_AppVersion) == 0)
							ti = ITranslation::TI_AppVersion;

						if (ti != ITranslation::TI_Last)
							translation.SetTranslationInfo(ti, &eqPos[1], lineEndPos - eqPos - 1);
						else
							TRACE(_T("@2 ITranslationUtilities::GetTranslationInfo. Unknown Key: '%s'\r\n"), key);
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


