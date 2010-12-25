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
#ifndef _FolderMultiLanguage_H
#define _FolderMultiLanguage_H

#include "MappedLanguage.h"
#include "ILanguageUtilities.h"

//It...
//- Loads / Keep track of / Releases all the available languages of a folder
//- Calculates Language Statistics
//- Have some helper functions for easy Language Loading
//- It is the owner of the active language

//Considerations
//- It should load quickly.
//- Load Language only on demand



class LanguageManager
{
public:
	LanguageManager(ILanguage& systemDefaultLanguage);
	~LanguageManager();

	BOOL LoadLanguage(LPCTSTR langPath);
	BOOL LoadLanguage(UINT idx);
	BOOL LoadLanguageByLANGID(LANGID langID);
	void UnloadLanguage();
	BOOL IsLanguageLoaded()						{return m_bLanguageIsLoaded;}
	ILanguage& GetLanguage()					{return m_loadedLanguage;}
	LPCTSTR GetLanguagePath()					{return m_loadedLanguagePath.c_str();}

	void SetLanguageFolder(LPCTSTR langPath)	{ASSERT(langPath != NULL); m_languageFolder = langPath;}
	LPCTSTR GetLanguageFolder()					{return m_languageFolder.c_str();}
	void RefreshLanguageFolder();

	ILanguage& GetSystemDefaultLanguage();

	UINT GetLanguageCount();
	LPCTSTR GetLanguagePath(UINT idx);
	LanguageInfo* GetLanguageInfo(UINT idx);
	LanguageComparison* GetLanguageComparison(UINT idx, LPCTSTR* log);
	BOOL IsLanguageLoaded(UINT idx);


	BOOL ExportLanguageFile(INT idx, LPCTSTR pathName);

private:
	static LPCTSTR GetLangNameByLangID(LANGID id);
	void ClearLanguages();
	class FullLanguageInfo
	{
	public:
		FullLanguageInfo():bLangInfoValid(FALSE),bLangCompValid(FALSE),bIsValid(FALSE)	{}
		BOOL bIsValid;
		std::tstring langPath;
		BOOL bLangInfoValid;
		LanguageInfo langInfo;
		BOOL bLangCompValid;
		LanguageComparison langComp;
		std::tstring langCompLog;
	};
	std::vector<FullLanguageInfo> m_languages;
	std::tstring m_languageFolder;
	std::tstring m_loadedLanguagePath;
	ILanguage& m_systemDefaultLanguage;
	MappedLanguage m_loadedLanguage;
	BOOL m_bLanguageIsLoaded;
};

#endif
