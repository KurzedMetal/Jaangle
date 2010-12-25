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
#ifndef _SKINMANAGER_H
#define _SKINMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <set>

#include "TSSkin.h"

//*********************************************************************
// SkinManager
//*********************************************************************

class AppSettings;

class SkinManager : public ISkinManager
{
public:
	SkinManager();
	virtual ~SkinManager();

public:
	virtual void RegisterSkinnable(ISkinnable& skinnable);
	virtual void UnRegisterSkinnable(ISkinnable& skinnable);
	virtual void UpdateSkinnables(ISkin& skin);

private:
	std::set<ISkinnable*> m_skinables;


public:
	TSSkin& GetSkin();



public:	//=== Skins Folder Management
	BOOL SetSkinsFolder(LPCTSTR translationPath);
	LPCTSTR GetSkinsFolder()	{return m_skinFolder.c_str();}
	void RefreshSkinsFolder();
	UINT GetSkinsCount();


	//=== Info retrieval
	LPCTSTR GetSkinInfo(INT idx, ISkin::SkinInfoEnum infoType);	
	LPCTSTR GetSkinPath(INT idx);	

	//=== MACROS
	void IncreaseRelativeFontSize();
	void DecreaseRelativeFontSize();
	void ResetRelativeFontSize();

	void UpdateSkinnables();

	BOOL LoadSkin(LPCTSTR skinPath);

private:
	void InitDefaultSkin();

private:
	std::tstring m_skinFolder;
	std::tstring m_tmpBuffer;
	TSSkin m_skin;

	struct FullSkinInfo
	{
		TSSkinInfo skinInfo;
		std::basic_string<TCHAR> path;
	};
	std::vector<FullSkinInfo> m_skinsInfo;


};
#endif
