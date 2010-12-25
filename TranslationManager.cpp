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
#include "TranslationManager.h"

TranslationManager::TranslationManager()
{
	m_fallBackTranslation.SetDefaultTranslation(&m_resourceTranslation);
}
TranslationManager::~TranslationManager()
{
	ASSERT(m_translatables.empty());
}

//=== ITranslationManager
void TranslationManager::RegisterTranslatable(ITranslatable& translatable)
{
	m_translatables.insert(&translatable);
}
void TranslationManager::UnRegisterTranslatable(ITranslatable& translatable)
{
	std::set<ITranslatable*>::iterator it = m_translatables.find(&translatable);
	if (it != m_translatables.end())
		m_translatables.erase(it);
	else
		ASSERT(0);
}
void TranslationManager::UpdateTranslatables(ITranslation& translation)
{
	std::set<ITranslatable*>::iterator it = m_translatables.begin();
	for (;it != m_translatables.end(); it++)
		(*it)->ApplyTranslation(translation);
}

//=== ITranslation
LPCTSTR TranslationManager::GetTranslationInfo(TranslationInfoEnum infoType)
{
	return m_fallBackTranslation.GetTranslationInfo(infoType);
}
LPCTSTR TranslationManager::GetString(UINT stringID)
{
	return m_fallBackTranslation.GetString(stringID);
}

//=== Xtra Interface

void TranslationManager::SetTranslationsFolder(LPCTSTR translationPath)
{
	ASSERT(translationPath != NULL);
	m_translationFolder = translationPath;
}

LPCTSTR TranslationManager::GetTranslationsFolder()
{
	return m_translationFolder.c_str();
}
void TranslationManager::RefreshTranslationsFolder()
{
	m_translations.clear();
	TCHAR searchPath[MAX_PATH];
	_sntprintf(searchPath, MAX_PATH, _T("%s*.lng"), m_translationFolder.c_str());
	CFileFind ff;
	BOOL bCont = ff.FindFile(searchPath);
	while (bCont)
	{
		bCont = ff.FindNextFile();
		FullTranslationInfo lInfo;
		lInfo.path = ff.GetFilePath();
		m_translations.push_back(lInfo);
	}
}


UINT TranslationManager::GetTranslationsCount()
{
	return m_translations.size();
}


//=== Info retrieval
LPCTSTR TranslationManager::GetTranslationInfo(INT idx, TranslationInfoEnum infoType)
{
	if (idx >= (INT)m_translations.size())
	{
		ASSERT(0);
		return NULL;
	}
	if (m_translations[idx].info[ITranslation::TI_Name].empty())
		ReadTranslationInfo(m_translations[idx]);
	return m_translations[idx].info[infoType].c_str();
}

LPCTSTR TranslationManager::GetTranslationPath(INT idx)
{
	if (idx >= (INT)m_translations.size())
	{
		ASSERT(0);
		return NULL;
	}
	return m_translations[idx].path.c_str();
}
const TranslationComparison* TranslationManager::GetTranslationComparison(INT idx)
{
	if (idx >= (INT)m_translations.size())
	{
		ASSERT(0);
		return NULL;
	}
	if (m_translations[idx].comparison.firstCount == 0)
		ReadComparisonInfo(m_translations[idx]);
	return &m_translations[idx].comparison;
}
LPCTSTR TranslationManager::GetTranslationComparisonLog(INT idx)
{
	if (idx >= (INT)m_translations.size())
	{
		ASSERT(0);
		return NULL;
	}
	if (m_translations[idx].comparison.firstCount == 0)
		ReadComparisonInfo(m_translations[idx]);
	return m_translations[idx].comparisonLog.c_str();
}

BOOL TranslationManager::ReadTranslationInfo(FullTranslationInfo& info)
{
	EditableTranslation t;
	if (ITranslationUtilities::ImportTranslationFile(info.path.c_str(), t, TRUE))
	{
		for (INT i = 0; i < ITranslation::TI_Last; i++)
		{
			ITranslation::TranslationInfoEnum it = ITranslation::TranslationInfoEnum(i);
			LPCTSTR text = t.GetTranslationInfo(it);
			info.info[i] = text == NULL ? _T("") : text;
		}
		if (info.info[TI_Name].empty())
		{
			LPCTSTR pos = _tcsrchr(info.path.c_str(), '\\');
			if (pos != NULL)
			{
				pos++;
				info.info[TI_Name].insert(0, pos, info.path.size() - (pos - info.path.c_str()) - 4);
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL TranslationManager::ReadComparisonInfo(FullTranslationInfo& info)
{
	EditableTranslation translation;
	if (ITranslationUtilities::ImportTranslationFile(info.path.c_str(), translation, FALSE))
	{

		if (ITranslationUtilities::CompareTranslations(m_resourceTranslation, translation, info.comparison, info.comparisonLog))
		{
			return TRUE;
			//==Comparison successs
		}
	}
	return FALSE;
}


//=== Other

ITranslation& TranslationManager::GetDefaultTranslation()		
{
	return m_resourceTranslation;
}

LPCTSTR TranslationManager::GetTranslationPath()
{
	if (m_loadedTranslationPath.empty())
		return _T("@RES@");
	return m_loadedTranslationPath.c_str();
}


BOOL TranslationManager::LoadTranslation(LPCTSTR langPath)
{
	ASSERT(langPath != NULL);
	if (m_loadedTranslationPath != langPath)
	{
		UnloadTranslation();
		m_loadedTranslationPath = langPath;
		if (ITranslationUtilities::ImportTranslationFile(langPath, m_fileTranslation, FALSE))
		{
			m_fallBackTranslation.SetTranslation(&m_fileTranslation);
			return TRUE;
		}
		else
		{
			TRACE(_T("@1 TranslationManager::LoadTranslation Import failed\r\n"));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL TranslationManager::LoadTranslation(INT idx)
{
	if (idx >= (INT)m_translations.size())
	{
		ASSERT(0);
		return FALSE;
	}
	return LoadTranslation(m_translations[idx].path.c_str());
}

BOOL TranslationManager::LoadTranslationByLANGID(LANGID langID)
{
	LPCTSTR langName = GetLangNameByLangID(langID);
	if (langName != NULL)
	{
		TCHAR path[MAX_PATH];
		_sntprintf(path, MAX_PATH, _T("%s%s.lng"), GetTranslationsFolder(), langName);
		return LoadTranslation(path);
	}
	return FALSE;
}

void TranslationManager::UnloadTranslation()
{
	m_fallBackTranslation.SetTranslation(NULL);
	m_fileTranslation.ClearStrings();
	m_loadedTranslationPath.clear();
}

LPCTSTR TranslationManager::GetLangNameByLangID(LANGID id)
{
	LPCTSTR cLangName[] =
	{
		_T("Neutral"),//0x00
		_T("Arabic"),
		_T("Bulgarian"),
		_T("Catalan"),
		_T("Chinese"),
		_T("Czech"),
		_T("Danish"),
		_T("German"),
		_T("Greek"),//0x08
		_T("English"),
		_T("Spanish"),
		_T("Finnish"),
		_T("French"),
		_T("Hebrew"),
		_T("Hungarian"),
		_T("Icelandic"),
		_T("Italian"),//0x10
		_T("Japanese"),
		_T("Korean"),
		_T("Dutch"),
		_T("Norwegian"),
		_T("Polish"),
		_T("Portuguese"),
		_T("Romansh"),
		_T("Romanian"),//0x18
		_T("Russian"),
		_T("Serbian"),
		_T("Slovak"),
		_T("Albanian"),
		_T("Swedish"),
		_T("Thai"),
		_T("Turkish"),
		_T("Urdu"),//0x20
		_T("Indonesian"),
		_T("Ukrainian"),
		_T("Belarusian"),
		_T("Slovenian"),
		_T("Estonian"),
		_T("Latvian"),
		_T("Lithuanian"),
		_T("Tajic"),//0x28
		_T("Persian"),
		_T("Vietnamese"),
		_T("Armenian"),
		_T("Azeri"),
		_T("Basque")//0x2d

	};
	BYTE index = LOBYTE(id);
	if (index<=0x2d)
		return cLangName[index];
	return NULL;
}

BOOL TranslationManager::ExportTranslationFile(INT idx, LPCTSTR path)
{
	ASSERT(idx < (INT)GetTranslationsCount());
	if (idx >= (INT)GetTranslationsCount())
		return FALSE;
	if (idx >= 0)
	{
		EditableTranslation translation;
		if (!ITranslationUtilities::ImportTranslationFile(m_translations[idx].path.c_str(), translation, FALSE))
			return FALSE;
		if (!ITranslationUtilities::ExportTranslationFile(path, &translation, &m_resourceTranslation))
			return FALSE;
	}
	else
	{
		if (!ITranslationUtilities::ExportTranslationFile(path, NULL, &m_resourceTranslation))
			return FALSE;
	}
	return TRUE;
}
