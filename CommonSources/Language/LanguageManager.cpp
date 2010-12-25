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
#include "StdAfx.h"
#include "LanguageManager.h"

LPCTSTR cLangName[] =
{
	_T("Neutral"),
	_T("Arabic"),
	_T("Bulgarian"),
	_T("Catalan"),
	_T("Chinese"),
	_T("Czech"),
	_T("Danish"),
	_T("German"),
	_T("Greek"),
	_T("English"),
	_T("Spanish"),
	_T("Finnish"),
	_T("French"),
	_T("Hebrew"),
	_T("Hungarian"),
	_T("Icelandic"),
	_T("Italian"),
	_T("Japanese"),
	_T("Korean"),
	_T("Dutch"),
	_T("Norwegian"),
	_T("Polish"),
	_T("Portuguese"),
	_T("Romanian"),
	_T("Russian"),
	_T("Croatian"),
	_T("Slovak"),
	_T("Albanian"),
	_T("Swedish"),
	_T("Thai"),
	_T("Turkish")
};

LanguageManager::LanguageManager(ILanguage& systemDefaultLanguage):
	m_systemDefaultLanguage(systemDefaultLanguage),
		m_bLanguageIsLoaded(FALSE)
{
	//m_pActiveLanguage = new FallBackLanguage(pDefLang);
	//LangStorage* pLS = new LangStorage;
	//pLS->info = pDefLang->GetLanguageInfo();
	//m_languages.push_back(pLS);
}
LanguageManager::~LanguageManager()
{
}

//Actions
void LanguageManager::RefreshLanguageFolder()
{
	ClearLanguages();
	TCHAR searchPath[MAX_PATH];
	_sntprintf(searchPath, MAX_PATH, _T("%s*.lng"), GetLanguageFolder());
	CFileFind ff;
	BOOL bCont = ff.FindFile(searchPath);
	while (bCont)
	{
		bCont = ff.FindNextFile();
		FullLanguageInfo lInfo;
		lInfo.langPath = ff.GetFilePath();
		lInfo.bIsValid = TRUE;
		m_languages.push_back(lInfo);
		//MappedLanguage lng;
		//if (LanguageSerialization::ImportLngFile(ff.GetFilePath(), lng, TRUE))
		//{
		//	pLS->info = lng.GetLanguageInfo();
		//	m_languages.push_back(pLS);
		//}
		//else
		//	TRACE(_T("@1LanguageManager::ScanLanguagesFolder. Invalide lng File '%s'\r\n"), ff.GetFilePath());
	}
	return;

}
void LanguageManager::ClearLanguages()
{
	m_languages.clear();
}

LPCTSTR LanguageManager::GetLanguagePath(UINT idx)
{
	if (idx < GetLanguageCount())
	{
		FullLanguageInfo& li = m_languages[idx];
		if (li.bIsValid == FALSE)
			return NULL;
		return li.langPath.c_str();
	}
	return NULL;
}


LanguageInfo* LanguageManager::GetLanguageInfo(UINT idx)
{
	if (idx < GetLanguageCount())
	{
		FullLanguageInfo& li = m_languages[idx];
		if (li.bIsValid == FALSE)
			return NULL;
		if (li.bLangInfoValid == FALSE)
		{
			MappedLanguage lng;
			if (!ILanguageUtilities::ImportLanguageFile(li.langPath.c_str(), lng, TRUE))
			{
				li.bIsValid = FALSE;
				return NULL;
			}
			li.bLangInfoValid = TRUE;
			li.langInfo = lng.GetLanguageInfo();
		}
		return &li.langInfo;
	}
	return NULL;
}

LanguageComparison* LanguageManager::GetLanguageComparison(UINT idx, LPCTSTR* log)
{
	if (idx < GetLanguageCount())
	{
		FullLanguageInfo& li = m_languages[idx];
		if (li.bIsValid == FALSE)
			return NULL;
		if (li.bLangCompValid == FALSE)
		{
			MappedLanguage lng;
			if (!ILanguageUtilities::ImportLanguageFile(li.langPath.c_str(), lng, FALSE))
			{
				li.bIsValid = FALSE;
				return NULL;
			}
			if (li.bLangInfoValid == FALSE)
			{
				li.bLangInfoValid = TRUE;
				li.langInfo = lng.GetLanguageInfo();
			}
			if (!ILanguageUtilities::CompareLanguages(m_systemDefaultLanguage, lng, li.langComp, li.langCompLog))
			{
				li.bLangCompValid = FALSE;
				return NULL;
			}
			li.bLangCompValid = TRUE;
		}
		*log = li.langCompLog.c_str();
		return &li.langComp;
	}
	return NULL;
}

BOOL LanguageManager::IsLanguageLoaded(UINT idx)
{
	if (IsLanguageLoaded() == FALSE)
		return FALSE;
	if (idx >= GetLanguageCount())
		return FALSE;
	if (m_languages[idx].bIsValid == FALSE)
		return FALSE;
	return _tcsicmp(m_languages[idx].langPath.c_str(), m_loadedLanguagePath.c_str()) == 0;
}



UINT LanguageManager::GetLanguageCount()
{
	return m_languages.size();//plus the default
}

BOOL LanguageManager::LoadLanguage(LPCTSTR languagePath)
{
	ASSERT(languagePath!=NULL);
	if (_taccess(languagePath, 02) != 0)
		return FALSE;
	//if (_tcsicmp(languagePath, m_loadedLanguagePath.c_str()) == 0)
	//{
	//	m_bLanguageIsLoaded = TRUE;
	//	return TRUE;
	//}
	m_loadedLanguage.ClearStrings();
	m_loadedLanguage.GetLanguageInfo() = LanguageInfo();
	m_loadedLanguagePath.clear();
	if (!ILanguageUtilities::ImportLanguageFile(languagePath, m_loadedLanguage))
		return FALSE;
	m_bLanguageIsLoaded = TRUE;
	m_loadedLanguagePath = languagePath;
	return TRUE;
}

BOOL LanguageManager::LoadLanguage(UINT idx)
{
	if (idx >= GetLanguageCount())
		return FALSE;
	if (IsLanguageLoaded(idx))
		return TRUE;
	return LoadLanguage(m_languages[idx].langPath.c_str());
}

BOOL LanguageManager::LoadLanguageByLANGID(LANGID lID)
{
	LPCTSTR langName = GetLangNameByLangID(lID);
	if (langName != NULL)
	{
		TCHAR path[MAX_PATH];
		_sntprintf(path, MAX_PATH, _T("%s%s.lng"), GetLanguageFolder(), langName);
		return LoadLanguage(path);
	}
	return FALSE;
}

void LanguageManager::UnloadLanguage()
{
	m_bLanguageIsLoaded = FALSE;
}

LPCTSTR LanguageManager::GetLangNameByLangID(LANGID id)
{
	BYTE index = LOBYTE(id);
	if (index<0x20)
		return cLangName[index];
	return NULL;
}

BOOL LanguageManager::ExportLanguageFile(INT idx, LPCTSTR path)
{
	ASSERT(idx < (INT)GetLanguageCount());
	if (idx >= (INT)GetLanguageCount())
		return FALSE;
	if (idx >= 0)
	{
		MappedLanguage lng;
		if (!ILanguageUtilities::ImportLanguageFile(m_languages[idx].langPath.c_str(), lng, FALSE))
			return FALSE;
		if (!ILanguageUtilities::ExportLanguageFile(path, &lng, &m_systemDefaultLanguage))
			return FALSE;
	}
	else
	{
		if (!ILanguageUtilities::ExportLanguageFile(path, NULL, &m_systemDefaultLanguage))
			return FALSE;
	}
	return TRUE;
}
