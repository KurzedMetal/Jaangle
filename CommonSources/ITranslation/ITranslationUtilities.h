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
#ifndef _ITranslationUtilities_H
#define _ITranslationUtilities_H

#include "ITranslation.h"
#include "EditableTranslation.h"

struct TranslationComparison
{
	TranslationComparison():
		firstCount(0),
		secondCount(0),
		notInSecond(0),
		notInFirst(0),
		same(0)
		{}
	INT firstCount;		//String count of the first
	INT secondCount;	//String count of the second
	INT notInSecond;	//Exists in 1st but not in 2nd
	INT notInFirst;		//Exists in 2nd but not in 1st
	INT same;			//Same string in both Translations
};

class ITranslationUtilities
{
public:
	static BOOL CompareTranslations(ITranslation& first, ITranslation& second, 
		TranslationComparison& stats, std::basic_string<TCHAR>& log);

	static BOOL ImportTranslationFile(LPCTSTR pathName, EditableTranslation& destTranslation, BOOL bInfoOnly = FALSE);
	static BOOL ExportTranslationFile(LPCTSTR pathName, ITranslation* pSrcTranslation, ITranslation* pReferenceTranslation);

private:
	static LPTSTR GetTranslationFile(LPCTSTR pathName);
	static BOOL GetTranslationStrings(LPCTSTR langFileData, EditableTranslation& destTranslation);
	static BOOL GetTranslationInfo(LPCTSTR langFileData, EditableTranslation& TranslationInfo);
	static void Replace(std::tstring& str, LPCTSTR oldString, LPCTSTR newString);
	static void FixString(std::tstring& str);
	static void SaveHeader(CFile& f, LPCTSTR header, LPCTSTR info);


};

#endif
