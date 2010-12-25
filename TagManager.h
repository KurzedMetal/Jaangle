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
#ifndef _TagManager_h_
#define _TagManager_h_

#include "TracksViewSchema.h"
class CDBEngine;
class InformationManager;
class AppSettings;
class FileTagger;

//This Manager is responsible to 
//1. Sync files with the database
//2. Handle the InSync Flag in The TracksTable Field
class TagManager
{

public:
	TagManager();
	~TagManager();
	enum OPTIONS
	{
		O_First = -1,
		O_Enabled,
		O_WriteComments,
		O_WriteLyrics,
		O_WritePersonal,
		O_WriteRating,
		O_Last
	};

public:
	//Init
	void Init(CDBEngine* eng, InformationManager* im, AppSettings* settings);
	BOOL SyncTrack(UINT itemID);
	BOOL SyncTrack(TracksViewSchema& tvs);
	//Settings
	BOOL GetOption(OPTIONS opt)
	{
		ASSERT(opt>O_First && opt<O_Last);
		return m_Options[opt];
	}
	void SetOption(OPTIONS opt, BOOL val)
	{
		ASSERT(opt>O_First && opt<O_Last);
		m_Options[opt] = val;
	}
	BOOL SetTrackInSync(UINT itemID, BOOL bInSync);

private:
	void ReadOptions();
	FileTagger* m_pFileTagger;
	CDBEngine* m_eng;
	InformationManager* m_im;
	AppSettings* m_pSettings;
	BOOL m_Options[O_Last];
	static LPCTSTR SettingsCategory;
};

#endif
