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
#include "objfactory.h"
#include "AppSettings/IniAppSettings.h"
#include "ActionManager.h"
#include "ServicesRegistrationSrv.h"

//Media Players Include



ObjFactory* ObjFactory::g_Inst = 0;

ObjFactory::ObjFactory(void)
{
}

LPCTSTR const cPlayerSettings = _T("PLAYER");

ObjFactory::~ObjFactory(void)
{
	TRACEST(_T("ObjFactory::~ObjFactory"));
}

ObjFactory* ObjFactory::Instance()
{
	if (g_Inst == 0)
		g_Inst = new ObjFactory;
	return g_Inst;
}

#include "AppSettings/IniAppSettings.h"
AppSettings* ObjFactory::CreateAppSettings(LPCTSTR conString)
{
	if (conString != NULL && !PathFileExists(conString))
	{
		TCHAR bf[MAX_PATH];
		_tcsncpy(bf, conString, MAX_PATH);
		TCHAR* ch = _tcsrchr(bf, '\\');
		if (ch != NULL)
			*ch = 0;
		SHCreateDirectoryEx(NULL, bf, NULL);
	}
	AppSettings* pSet = new IniAppSettings();
	pSet->Load(conString);
	return pSet;
}


#include "EnhTagLibTagger.h"

FileTagger* ObjFactory::CreateFileTagger()
{
	return new EnhTagLibTagger();
}
