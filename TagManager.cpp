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
#include "TagManager.h"
#include "DBEngine.h"
#include "ObjFactory.h"
#include "DBManager.h"
#include "TracksTableSchema.h"
#include "InformationManager.h"
#include "TracksViewSchema.h"
#include "AppSettings/AppSettings.h"
#include "PrgAPI.h"
#include "Taggers/FileTagger.h"


LPCTSTR TagManager::SettingsCategory = _T("TAGMANAGER");

TagManager::TagManager():
m_eng(NULL),
m_pSettings(NULL),
m_im(NULL),
m_pFileTagger(NULL)
{
	for (int i = O_First + 1; i < O_Last; i++)
		SetOption((OPTIONS)i, TRUE);
}

TagManager::~TagManager()
{
	TRACEST(_T("~TagManager\r\n"));
	ASSERT(m_pSettings != NULL);
	m_pSettings->Write(SettingsCategory, _T("Enabled"), (INT) GetOption(O_Enabled));
	m_pSettings->Write(SettingsCategory, _T("WriteComments"), (INT) GetOption(O_WriteComments));
	m_pSettings->Write(SettingsCategory, _T("WriteLyrics"), (INT) GetOption(O_WriteLyrics));
	m_pSettings->Write(SettingsCategory, _T("WritePersonal"), (INT) GetOption(O_WritePersonal));
	m_pSettings->Write(SettingsCategory, _T("WriteRating"), (INT) GetOption(O_WriteRating));
	delete m_pFileTagger;
}

void TagManager::Init(CDBEngine* eng, InformationManager* im, AppSettings* settings)
{	
	ASSERT(eng != NULL);
	ASSERT(im != NULL);
	ASSERT(settings != NULL);
	m_eng = eng;
	m_im = im;
	m_pSettings = settings;
	ReadOptions();
	m_pFileTagger = ObjFactory::Instance()->CreateFileTagger();
}

void TagManager::ReadOptions()
{
	INT val;
	m_pSettings->Read(SettingsCategory, _T("Enabled"), val, 1);
	SetOption(O_Enabled, val);
	m_pSettings->Read(SettingsCategory, _T("WriteComments"), val, 1);
	SetOption(O_WriteComments, val);
	m_pSettings->Read(SettingsCategory, _T("WriteLyrics"), val, 1);
	SetOption(O_WriteLyrics, val);
	m_pSettings->Read(SettingsCategory, _T("WritePersonal"), val, 0);
	SetOption(O_WritePersonal, val);
	m_pSettings->Read(SettingsCategory, _T("WriteRating"), val, 0);
	SetOption(O_WriteRating, val);
}

BOOL TagManager::SyncTrack(UINT itemID)
{
	BOOL bFileInSync = FALSE;
	if (GetOption(O_Enabled))
	{
		TracksViewSchema tvs;
		if (ObjFactory::Instance()->GetDbManager()->GetTracksViewByTrackID(itemID, tvs))
		{
			bFileInSync = SyncTrack(tvs);
		}
	}
	else
		SetTrackInSync(itemID, FALSE);
	return bFileInSync;
}

BOOL TagManager::SetTrackInSync(UINT itemID, BOOL bInSync)
{
	BOOL ret = FALSE;
	TracksTableSchema tts;
	if (tts.Edit(itemID))
	{
		tts.SetSyncState(bInSync ? TSS_InSync : TSS_NotInSync);
		ret = tts.Update();
	}
	else
		ASSERT(0);
	return ret;
}
//Will handle the whole recordset
BOOL TagManager::SyncTrack(TracksViewSchema& tvs)
{
	if (!GetOption(O_Enabled))	return TRUE;//Job not done but this is due to the settings
	TCHAR buffer[MAX_PATH];
	INT val = 0;
	BOOL bFileInSync = TRUE;
	TagInfo info;
	while (!tvs.IsEOF())
	{
		if (tvs.GetFileIsWriteble())
		{
			tvs.GetName(buffer, 70);
			info.Title = buffer;
			//Write Album -----------------------------------
			tvs.GetAlbumName(buffer, 70);
			info.Album = buffer;
			//Write Artist -----------------------------------
			val = tvs.GetArtID();
			if (val > 1)
			{
				tvs.GetArtistName(buffer, 70);
				info.Artist = buffer;
			}
			else
				info.Artist = _T("");
			//Write Genre -----------------------------------
			val = tvs.GetGenreID();
			if (val > 1)
			{
				tvs.GetGenreName(buffer, 70);
				info.Genre = buffer;
			}
			else
				info.Genre = _T("");
			//Write Comments -----------------------------------
			CString sText;
			if (GetOption(O_WriteComments))
			{
				val = tvs.GetCommentID();
				if (val > 0)
				{
					if (!m_im->GetText(IIT_TrackComment, tvs.GetTrackID(), sText))
						ASSERT(0);//Why???
				}
				info.Comment = sText;
			}
			if (GetOption(O_WriteLyrics))
			{
				val = tvs.GetLyricsID();
				sText.Empty();
				if (val > 0)
				{
					if (!m_im->GetText(IIT_TrackLyrics, tvs.GetTrackID(), sText))
						ASSERT(0);//Why???
				}
				info.Lyrics = sText;
			}
			//if (GetOption(O_WritePersonal))
			//{
			//	val = tvs.GetPersonalID();
			//	sText.Empty();
			//	if (val > 0)
			//	{
			//		if (!m_im->GetText(IIT_TrackPersonal, tvs.GetTrackID(), sText))
			//			ASSERT(0);//Why???
			//	}
			//	if (!pWriter->SetPersonal(sText))
			//		bFileInSync = FALSE;
			//}

			info.TrackNo = tvs.GetTrackNo();
			info.Year = tvs.GetYear();
			info.Rating = tvs.GetRating();
			
			tvs.GetLocation(buffer, MAX_PATH);

			if (!m_pFileTagger->Write(buffer, info))
			{
				bFileInSync = FALSE;
				TCHAR msg[300];
				_sntprintf(msg, 300, _T("File '%s' did not affected."), buffer);
				PRGAPI()->NotifyUser(msg, NULL, SEV_Warning);
			}
			else
			{
				TCHAR msg[300];
				_sntprintf(msg, 300, _T("File '%s' modified."), buffer);
				PRGAPI()->NotifyUser(msg, NULL, SEV_InfoL1);
			}
		}
		if (bFileInSync != tvs.GetFileIsInSync())
			SetTrackInSync(tvs.GetTrackID(), bFileInSync);
		tvs.MoveNext();
	}
	return TRUE;

}

//BOOL TagManager::TotalTag(UINT itemID, TagInfo& ti, UINT TagInfoFlags)
//{
//	ASSERT(0);
//	return FALSE;
//}
//
//BOOL TagManager::UpdateDB(UINT itemID, TagInfo& ti, UINT TagInfoFlags)
//{
//	CollectionManager::ProcessCollection()
//	return FALSE;
//
//}
//BOOL TagManager::UpdateFile(LPCTSTR path, TagInfo& ti, UINT TagInfoFlags)
//{
//	return m_pFileTagger->Write(path, ti, TagInfoFlags, FALSE) > 0;
//}
