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
#include "TeenSpirit.h"
#include "ActionManager.h"
//#include "DBManager.h"

#include "MainTrackPage.h"
#include "InfoPage.h"
#include "PropertiesSheet.h"
#include "MultipleTracksPage.h"
#include "InfoDownloadManager.h"
#include "MainArtistPage.h"
#include "VariousPage.h"
#include "MultiPicturePage.h"
#include "MainAlbumsPage.h"
//#include "TracksTableSchema.h"
#include "XFolderDialog.h"
#include "ShellFileOp/CShellFileOp.h"
#include "MediaPlayer.h"
#include "PrgAPI.h"
#include "ServicesRegistrationSrv.h"

#include "FileTagger.h"

#include "cStringUtils.h"
//#include "TextFileDocument/textfile.h"
#include "MediaPlayer.h"
#include "MiniPlayerDlg.h"
#include "SQLManager.h"
//#include "FullTracksPlaylist.h"
//#include "ImplPlaylist.h"
#include "CollectionManager.h"
#include "LocalPictureManager.h"
#include "InputBox.h"

#include "CollectionsDlg.h"
#include "HistoryDlg.h"
#include "AboutDlg.h"
#include "OptionsSheet.h"
#include "QuizRunningDlg.h"
#include "MainOptionsPage.h"
#include "InfoRetrievalPP.h"
#include "MediaPlayerPP.h"
#include "GlobalHotkeysPP.h"
//#include "TrackListPP.h"
//#include "PlayListPP.h"
#include "TaggingPP.h"
#include "LanguagesPP.h"
#include "SkinsPP.h"
#include "MiniPlayerPP.h"
//#include "InfoControlPP.h"
#include "GenericInfoProviderPP.h"
#include "InformationProviders/InfoProviderFactory.h"
#include "Trackers/ITracker.h"
#include "SystemTimeFunctions.h"
#include "TSOnlineServices.h"
#include "MediaPlayerUtilities.h"
#include "UserProgressDlg.h"
#include "NewVersionChecker.h"
#include "StateManager.h"
#include "QuickSearchDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


ActionManager::ActionManager():
m_LogTrackStatus(-1),
m_LastTrackLength(0.0),
m_logListPos(0),
m_rndGenerator((boost::int32_t)GetTickCount())
//m_pMiniPlayerDlg(NULL),
//m_pQuickSearchDlg(NULL)
{

}

ActionManager::~ActionManager()	
{
	//if (m_pQuickSearchDlg)
	//{
	//	m_pQuickSearchDlg->DestroyWindow();
	//	delete m_pQuickSearchDlg;
	//}
	//DestroyMiniPlayer();
	//DestroyAdvancedSearchDlg();
}

BOOL ActionManager::OpenFolderToItem(LPCTSTR fileName, CollectionTypesEnum type)
{
	TRACEST(_T("ActionManager::OpenFolderToItem."));
	if (type == CTYPE_Media)
	{
		TCHAR path[MAX_PATH];
		if (FindAndFixMountedPath(fileName, path, MAX_PATH))
			return OpenFolderToItem(path);
	}
	return OpenFolderToItem(fileName);
}
BOOL ActionManager::OpenFolderToItem(LPCTSTR fileName)
{		
	TCHAR bf[MAX_PATH + 12];
	_sntprintf(bf, MAX_PATH + 12, _T("/select,\"%s\""), fileName);
	ShellExecute(NULL, NULL, _T("explorer.exe"), bf, NULL, SW_SHOWNORMAL);
	return TRUE;
}

BOOL ActionManager::OpenFolder(LPCTSTR dirName)
{		
	ShellExecute(NULL, _T("explore"), dirName,0,0,SW_SHOWNORMAL);
	return TRUE;
}

INT ActionManager::TagTracks(FullTrackRecordCollection& col, TagInfo& ti)
{
	TRACEST(_T("ActionManager::TagTracks."));
	BOOL ret = FALSE;
	FullTrackRecordCollection::iterator it = col.begin();
	INT successes = 0;
	PrgAPI* pAPI = PRGAPI();
	UINT total = col.size();
	UINT pos = 0;

	CUserProgressDlg dlg;
	dlg.SetTitle(pAPI->GetString(IDS_TAGGING));
	for (;it!=col.end(); it++)
	{
		FullTrackRecordSP& rec = *it;
		dlg.SetMessage(rec->track.location.c_str());
		dlg.SetProgress(pos++ / (FLOAT)total);
		dlg.Show();
		successes += TagTrack(rec->track.location.c_str(), ti) ? 1 : 0;
	}
	return successes;
}

//===Returns FALSE if It fails to actually write the tags. 
//===If it succesfully write them or there is no need it returns TRUE
BOOL ActionManager::TagTrack(LPCTSTR path, TagInfo& ti)
{
	TRACEST(_T("ActionManager::TagTrack."));
	TCHAR fixedPath[MAX_PATH];
	if (!PathFileExists(path))
	{
		if (FindAndFixMountedPath(path, fixedPath, MAX_PATH))
			path = fixedPath;
		else
		{
			TRACE(_T("ActionManager::TagTrack. File Not found\r\n"));
			return FALSE;
		}
	}
	if (_taccess(path, 6) != 0)
	{
		TRACE(_T("ActionManager::TagTrack. No write permissions \r\n"));
		return FALSE;
	}

	PrgAPI* pAPI = PRGAPI();
	BOOL ret = TRUE;
	UINT validFields = ti.validFields;
	if (!pAPI->GetOption(OPT_TAG_WriteComments))
		ti.validFields &= (~TagInfo_Comment);
	if (!pAPI->GetOption(OPT_TAG_WriteLyrics))
		ti.validFields &= (~TagInfo_Lyrics);
	if (!pAPI->GetOption(OPT_TAG_WriteRating))
		ti.validFields &= (~TagInfo_Rating);
	if (ti.validFields != TagInfo_None)
	{
		TCHAR msg[400];
		SEVERITY sev = SEV_InfoL1;
		if (pAPI->GetFileTagger()->Write(path, ti))
		{
			_sntprintf(msg, 400, _T("%s '%s'."), 
				pAPI->GetString(IDS_TAGGING), 
				path);
		}
		else
		{
			_sntprintf(msg, 400, _T("%s '%s'. %s."), 
				pAPI->GetString(IDS_TAGGING), 
				path, 
				pAPI->GetString(IDS_OPERATIONFAILED));
			sev = SEV_Warning;
			TRACE(_T("@1 ActionManager::TagTrack Tagging Failed\r\n"));
			ret = FALSE;
		}
		pAPI->NotifyUser(msg, 0, sev);
	}
	else
		TRACE(_T("@3 ActionManager::TagTrack No Tag Applied due to options\r\n"));


	return ret;

}

BOOL ActionManager::ShowArtistPropertiesDlg(FullArtistRecordSP& rec, CWnd* pParent, BOOL bFull)
{
	TRACE(_T("@3 ActionManager::ShowArtistProperties. (%s)\r\n"), rec->artist.name.c_str());
	ASSERT(rec->artist.ID>0);
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();
	if (!pSM->GetFullArtistRecordByID(rec, rec->artist.ID))
		return FALSE;
	CPropertiesSheet ps(_T(""), pParent);
	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	//--------Prepare Main Artist Page
	CMainArtistPage MainPage(IDS_PROPERTIES);
	if (bFull)
		ps.AddPage(&MainPage);
	MainPage.Artist = rec->artist.name.c_str();
	MainPage.Genre = rec->genre.name.c_str();
	MainPage.Rating = rec->artist.rating;

	InfoRecord ir;
	//---------Prepare Bio Page (InfoPage)
	CInfoPage Bio(IDS_BIOGRAPHY);
	ps.AddPage(&Bio);
	if (pSM->GetArtistInfoRecord(rec->artist.ID, IIT_ArtistBio, ir))
		Bio.info = ir.name.c_str();
	//---------Prepare Pic Page (PicturePage)
	CMultiPicturePage Pic(IDS_ARTISTPICTURE);
	ps.AddPage(&Pic);
	std::tstringvector col;
	if (pLPM->GetArtistPictures(rec->artist, col))
	{
		CMultiPicturePage::PicInfo pInfo;
		PicRecord pr(IIT_ArtistPicture, rec->artist.ID);
		pSM->GetPicRecord(pr);
		for (size_t i =0; i<col.size();i++)
		{
			pInfo.bDefault = FALSE;
			if (pr.pictureType == PT_Main && _tcsicmp(col[i].c_str(), pr.path.c_str()) == 0)
				pInfo.bDefault = TRUE;
			pInfo.path = col[i].c_str();
			pInfo.status = CMultiPicturePage::PS_Normal;
			Pic.AddPicture(pInfo);
		}
	}

	//---------Prepare Similar Page
	CInfoPage Similars(IDS_SIMILAR);
	ps.AddPage(&Similars);
	Similars.EnableReadOnlyMode(TRUE);
	std::vector<SQLManager::ValuedID> valIDs;
	col.reserve(50);
	pSM->GetSimilarArtists(valIDs, rec->artist.ID);
	ArtistRecord art;
	for (UINT i = 0; i < valIDs.size(); i++)
	{
		pAPI->GetSQLManager()->GetArtistRecord(valIDs[i].ID, art);
		Similars.info += art.name.c_str();
		Similars.info += _T("\r\n");
	}


	ps.SetTitle(pAPI->GetString(IDS_PROPERTIES));
	FlexDialogHeader& header = ps.GetHeaderControl();
	header.SetTitleText(pAPI->GetString(IDS_ARTIST));
	header.SetDescText(rec->artist.name.c_str());
	if (col.size() > 0)
		header.SetIconImage(col[0].c_str());
	BOOL bSomethingChanged = FALSE;


	if (ps.DoModal() == IDOK)
	{
		TRACE(_T("@3 ActionManager::ShowArtistPropertiesDlg. Pressed OK\r\n"));
		SQLManager* pSM = pAPI->GetSQLManager();
		if (MainPage.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			BOOL bModifyTracks = FALSE;
			if (_tcscmp(rec->artist.name.c_str(), MainPage.Artist) != 0)
			{
				//=== Otherwise the artist picture will remain the same
				pAPI->GetInfoDownloadManager()->ResetArtistCache(rec->artist);
				pAPI->GetLocalPictureManager()->ResetArtistCache(rec->artist);
				bModifyTracks = TRUE;
				rec->artist.name = MainPage.Artist;
				//All the TracksTags with artID= must be modified
			}
			rec->genre.name = MainPage.Genre;
			rec->artist.rating = MainPage.Rating;
			if (bModifyTracks)
			{
				FullTrackRecordCollection col;
				TracksFilter tf;
				tf.ArtistID.match = NUMM_Exact;
				tf.ArtistID.val = rec->artist.ID;
				pSM->GetFullTrackRecordCollectionByTracksFilter(col, tf);
				ASSERT(col.size() > 0);
				TagInfo ti;
				ti.Artist = rec->artist.name;
				ti.validFields = TagInfo_Artist;
				TagTracks(col, ti);
			}
			pSM->UpdateFullArtistRecord(*rec, TRUE);
		}
		if (Bio.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			Bio.info.Trim();
			pSM->AdjustArtistInfo(rec->artist.ID, IIT_ArtistBio, Bio.info);
			pAPI->SendMessage(SM_InfoChanged);
		}
		if (Pic.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			for (UINT i=0; i<Pic.GetPictureCount();i++)
			{
				CMultiPicturePage::PicInfo pi;
				Pic.GetPicture(i, pi);
				INT fs = 0;
				switch (pi.status)
				{
				case CMultiPicturePage::PS_Removed:
					pLPM->RemoveArtistPicture(rec->artist, pi.path);
					//DeleteFile(pi.path);
					break;
				//case CMultiPicturePage::PS_Normal:
				case CMultiPicturePage::PS_AddedByFile:
					pLPM->AddArtistPicture(rec->artist, pi.path);
					fs = GetFileSize(pi.path);
					break;
				case CMultiPicturePage::PS_AddedByClipboard:
					pLPM->AddArtistPicture(rec->artist, pi.path);
					fs = GetFileSize(pi.path);
					DeleteFile(pi.path);
					break;
				case CMultiPicturePage::PS_Normal:
					break;
				default:
					ASSERT(0);
				}
				if (pi.bDefault == TRUE)
				{
					if (fs == 0)
						pLPM->SetMainArtistPicture(rec->artist, pi.path);
					else
					{
						std::tstringvector col;
						pLPM->GetArtistPictures(rec->artist, col);
						std::tstringvector::const_iterator it = col.begin();
						for (;it!=col.end();it++)
						{
							if (GetFileSize(it->c_str()) == fs)
							{
								pLPM->SetMainArtistPicture(rec->artist, it->c_str());
								break;
							}
						}
					}
				}
			}
			pAPI->SendMessage(SM_PictureChanged);
		}
	}
	return bSomethingChanged;
}

BOOL ActionManager::ShowAlbumPropertiesDlg(FullAlbumRecordSP& rec, CWnd* pParent, BOOL bFull)
{
	TRACE(_T("@3 ActionManager::ShowAlbumPropertiesDlg. (%s)\r\n"), rec->album.name.c_str());
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();//TO BE REMOVED
	CPropertiesSheet ps(_T(""), pParent);
	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	//Refreshes the data - Especially if the info have been changed
	if (!pSM->GetFullAlbumRecordByID(rec, rec->album.ID))
		return FALSE;
	//--- Main Album Properties
	CMainAlbumsPage MainPage(IDS_PROPERTIES);
	if (bFull)
		ps.AddPage(&MainPage);
	MainPage.Album = rec->album.name.c_str();
	MainPage.Artist = rec->artist.name.c_str();
	MainPage.Year = rec->album.year;
	MainPage.Rating = rec->album.rating;
	//m_cmbRating.SetCurSel(5 - theApp.Rating2Stars(avs.GetAlbumRating()));
	InfoRecord ir;
	//--- Review Properties
	CInfoPage Review(IDS_REVIEW);
	ps.AddPage(&Review);
	if (pSM->GetAlbumInfoRecord(rec->album.ID, IIT_AlbumReview, ir))
		Review.info = ir.name.c_str();
	//--- Album Picture
	CMultiPicturePage Pic(IDS_ALBUMPICTURE);
	ps.AddPage(&Pic);
	std::tstringvector col;
	if (pLPM->GetAlbumPictures(*rec, col))
	{
		CMultiPicturePage::PicInfo pInfo;
		PicRecord pr(IIT_AlbumPicture, rec->album.ID);
		pSM->GetPicRecord(pr);
		for (size_t i =0; i<col.size();i++)
		{
			pInfo.bDefault = FALSE;
			if (pr.pictureType == PT_Main && _tcsicmp(col[i].c_str(), pr.path.c_str()) == 0)
				pInfo.bDefault = TRUE;
			pInfo.path = col[i].c_str();
			pInfo.status = CMultiPicturePage::PS_Normal;
			Pic.AddPicture(pInfo);
		}
	}


	ps.SetTitle(pAPI->GetString(IDS_PROPERTIES));
	FlexDialogHeader& header = ps.GetHeaderControl();
	header.SetTitleText(pAPI->GetString(IDS_ALBUM));
	header.SetDescText(MainPage.Album);
	if (col.size() > 0)
		header.SetIconImage(col[0].c_str());

	BOOL bSomethingChanged = FALSE;

	if (ps.DoModal() == IDOK)
	{
		TRACEST(_T("ActionManager::ShowAlbumPropertiesDlg. Pressed OK"));
		if (MainPage.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			BOOL bModifyTracks = FALSE;
			if (_tcscmp(rec->album.name.c_str(), MainPage.Album) != 0)
			{
				//=== Otherwise the album picture will remain the same
				pAPI->GetInfoDownloadManager()->ResetAlbumCache(rec->album, rec->artist.name.c_str());
				pAPI->GetLocalPictureManager()->ResetAlbumCache(*rec);
				bModifyTracks = TRUE;
				rec->album.name = MainPage.Album;
			}
			//rec->artist.name = MainPage.Artist;
			rec->album.rating = MainPage.Rating;
			rec->album.year = MainPage.Year;
			if (bModifyTracks)
			{
				FullTrackRecordCollection col;
				TracksFilter tf;
				tf.AlbumID.match = NUMM_Exact;
				tf.AlbumID.val = rec->album.ID;
				pSM->GetFullTrackRecordCollectionByTracksFilter(col, tf);
				ASSERT(col.size() > 0);
				TagInfo ti;
				ti.Album = rec->album.name;
				ti.validFields = TagInfo_Album;
				TagTracks(col, ti);
			}
			pSM->UpdateFullAlbumRecord(*rec, TRUE);
		}
		if (Review.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			Review.info.Trim();
			pSM->AdjustAlbumInfo(rec->album.ID, IIT_AlbumReview, Review.info);
			pAPI->SendMessage(SM_InfoChanged);

		}
		if (Pic.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			TRACEST(_T("ActionManager::ShowAlbumPropertiesDlg. Pic.IsSomethingChanged"));
			for (UINT i=0; i<Pic.GetPictureCount();i++)
			{
				CMultiPicturePage::PicInfo pi;
				Pic.GetPicture(i, pi);
				INT fs = 0;
				switch (pi.status)
				{
				case CMultiPicturePage::PS_Removed:
					pLPM->RemoveAlbumPicture(*rec, pi.path);
					//DeleteFile(pi.path);
					break;
				//case CMultiPicturePage::PS_Normal:
				case CMultiPicturePage::PS_AddedByFile:
					pLPM->AddAlbumPicture(*rec, pi.path);
					fs = GetFileSize(pi.path);
					break;
				case CMultiPicturePage::PS_AddedByClipboard:
					pLPM->AddAlbumPicture(*rec, pi.path);
					fs = GetFileSize(pi.path);
					DeleteFile(pi.path);
					break;
				case CMultiPicturePage::PS_Normal:
					break;
				default:
					ASSERT(0);
				}
				if (pi.bDefault == TRUE)
				{
					if (fs == 0)
						pLPM->SetMainAlbumPicture(*rec, pi.path);
					else
					{
						//=== This handles the case that the user add a new picture and set it default
						std::tstringvector col;
						pLPM->GetAlbumPictures(*rec, col);
						std::tstringvector::const_iterator it = col.begin();
						for (;it!=col.end();it++)
						{
							if (GetFileSize(it->c_str()) == fs)
							{
								pLPM->SetMainAlbumPicture(*rec, it->c_str());
								break;
							}
						}
					}

				}

			}
			pAPI->SendMessage(SM_PictureChanged);
		}
	}
	return bSomethingChanged;
}


BOOL ActionManager::ShowTrackPropertiesDlg(FullTrackRecordSP& rec, CWnd* pParent )
{
	TRACE(_T("@3 ActionManager::ShowTrackPropertiesDlg. (%s)\r\n"), rec->track.location.c_str());
	ASSERT(rec->IsValid());
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	CPropertiesSheet ps(_T(""), pParent);
	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	//--- Main Track Properties
	CMainTrackPage MainPage(IDS_PROPERTIES);
	ps.AddPage(&MainPage);
	MainPage.Title = rec->track.name.c_str();
	MainPage.TrackNo = rec->track.trackNo;
	MainPage.Album = rec->album.name.c_str();
	MainPage.Artist = rec->artist.name.c_str();
	MainPage.Genre = rec->genre.name.c_str();
	MainPage.Year = rec->track.year;
	MainPage.CollectionName = rec->collection.name.c_str();
	MainPage.Location = rec->track.location.c_str();
	InfoRecord ir;
	//--- Comments
	CInfoPage CommentPage(IDS_COMMENTS);
	ps.AddPage(&CommentPage);
	if (pSM->GetTrackInfoRecord(rec->track.ID, IIT_TrackComment, ir))
		CommentPage.info = ir.name.c_str();

	//--- Lyrics
	CInfoPage LyricsPage(IDS_LYRICS);
	ps.AddPage(&LyricsPage);
	if (pSM->GetTrackInfoRecord(rec->track.ID, IIT_TrackLyrics, ir))
		LyricsPage.info = ir.name.c_str();

	//--- Personal
	CInfoPage PersonalPage(IDS_PERSONAL);
	ps.AddPage(&PersonalPage);
	if (pSM->GetTrackInfoRecord(rec->track.ID, IIT_TrackPersonal, ir))
		PersonalPage.info = ir.name.c_str();

	//--- Tabs
	CInfoPage TabsPage(IDS_TABLATURES);
	TabsPage.EnableFixedFont(TRUE);
	ps.AddPage(&TabsPage);
	if (pSM->GetTrackInfoRecord(rec->track.ID, IIT_TrackTablatures, ir))
		TabsPage.info = ir.name.c_str();



	ps.SetTitle(pAPI->GetString(IDS_PROPERTIES));
	FlexDialogHeader& header = ps.GetHeaderControl();
	header.SetTitleText(pAPI->GetString(IDS_TITLE));
	header.SetDescText(rec->track.name.c_str());
	BOOL bSomethingChanged = FALSE;

	if (ps.DoModal() == IDOK)
	{
		TRACE(_T("@3 ActionManager::ShowTrackProperties. Pressed OK\r\n"));
		AfxGetMainWnd()->UpdateWindow();
		TagInfo ti;
		if (MainPage.IsSomethingChanged())
		{
			//=== Otherwise the track info caches
			pAPI->GetInfoDownloadManager()->ResetTrackCache(rec->track, rec->artist.name.c_str());
			bSomethingChanged = TRUE;
			ti.validFields |= (TagInfo_Album | TagInfo_Artist | TagInfo_Genre | 
				TagInfo_Title | TagInfo_TrackNo | TagInfo_Year);
			ti.Album = MainPage.Album;
			ti.Artist = MainPage.Artist;
			ti.Genre = MainPage.Genre;
			ti.Title = MainPage.Title;
			ti.TrackNo = MainPage.TrackNo;
			ti.Year = MainPage.Year;
			rec->album.name = MainPage.Album;
			rec->artist.name = MainPage.Artist;
			rec->genre.name = MainPage.Genre;
			rec->track.name = MainPage.Title;
			rec->track.trackNo = MainPage.TrackNo;
			rec->track.year = MainPage.Year;
			pSM->UpdateFullTrackRecord(*rec);
			pAPI->SendMessage(SM_DatabaseUpdated);
		}
		if (CommentPage.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			CommentPage.info.Trim();
			pSM->AdjustTrackInfo(rec->track.ID, IIT_TrackComment, CommentPage.info);
			ti.Comment = CommentPage.info;
			ti.validFields |= TagInfo_Comment;
			pAPI->SendMessage(SM_InfoChanged);
		}
		if (LyricsPage.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			LyricsPage.info.Trim();
			pSM->AdjustTrackInfo(rec->track.ID, IIT_TrackLyrics, LyricsPage.info);
			ti.Lyrics = LyricsPage.info;
			ti.validFields |= TagInfo_Lyrics;
			if (pAPI->GetOption(OPT_GEN_UseNativeOnlineServices))
			{
				TSOnlineServices::TrackInfo ti;
				ti.artist = rec->artist.name.c_str();
				ti.title = rec->track.name.c_str();
				ti.rating = rec->track.rating;
				ti.lyrics = LyricsPage.info;
				ti.album = rec->album.name.c_str();
				ti.tag = rec->genre.name.c_str();
				pAPI->GetTSOnlineServices()->SendTrackInfo(ti);
			}
			pAPI->SendMessage(SM_InfoChanged);
		}
		if (PersonalPage.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			PersonalPage.info.Trim();
			pSM->AdjustTrackInfo(rec->track.ID, IIT_TrackPersonal, PersonalPage.info);
			pAPI->SendMessage(SM_InfoChanged);
		}
		if (TabsPage.IsSomethingChanged())
		{
			bSomethingChanged = TRUE;
			TabsPage.info.Trim();
			pSM->AdjustTrackInfo(rec->track.ID, IIT_TrackTablatures, TabsPage.info);
			pAPI->SendMessage(SM_InfoChanged);
		}
		if (ti.validFields != 0)
			TagTrack(rec->track.location.c_str(), ti);
	}
	return bSomethingChanged;
}

BOOL ActionManager::ShowMultipleTrackPropertiesDlg(FullTrackRecordCollection& col, CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowMultipleTrackPropertiesDlg."));
	ASSERT(col.size() > 0);
	CMultipleTracksPage MainPage(IDS_PROPERTIES);
	PrgAPI* pAPI = PRGAPI();
	//TCHAR txt[300];
	//_sntprintf(txt, 200, _T("%s"), pLang->GetString(IDS_MULTIPLETRACKPROPERTIES));

	CPropertiesSheet ps(pAPI->GetString(IDS_PROPERTIES), pParent, 0);
	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	ps.AddPage(&MainPage);

	FlexDialogHeader& header = ps.GetHeaderControl();
	header.SetTitleText(pAPI->GetString(IDS_MULTIPLETRACKPROPERTIES));

	BOOL bSomethingChanged = FALSE;

	if (ps.DoModal()==IDOK)
	{
		SQLManager* pSM = pAPI->GetSQLManager();
		FullTrackRecordCollection::iterator it = col.begin();
		TagInfo ti;
		UINT total = col.size();
		UINT pos = 0;
		CUserProgressDlg dlg;
		dlg.SetTitle(pAPI->GetString(IDS_TAGGING));
		for (; it != col.end(); it++)
		{
			ti.validFields = TagInfo_None;
			FullTrackRecordSP& rec = (*it);
			dlg.SetMessage(rec->track.location.c_str());
			dlg.SetProgress(pos++ / (FLOAT)total);
			dlg.Show();

			if (MainPage.bArtist)
			{
				ti.Artist = MainPage.Artist;
				ti.validFields |= TagInfo_Artist;
			}
			if (MainPage.bAlbum)
			{
				ti.Album = MainPage.Album;
				ti.validFields |= TagInfo_Album;
			}
			if (MainPage.bGenre)
			{
				ti.Genre = MainPage.Genre;
				ti.validFields |= TagInfo_Genre;
			}
			if (MainPage.bYear)
			{
				ti.Year = _ttoi(MainPage.Year);
				ti.validFields |= TagInfo_Year;
			}
			if (MainPage.bComment)
			{
				ti.Comment = MainPage.Comment;
				ti.validFields |= TagInfo_Comment;
			}
			if (ti.validFields != 0)
			{
				bSomethingChanged = TRUE;
				if (pSM->UpdateTrackFromTagInfo(rec->track.ID, rec->track.colID, rec->track.location.c_str(), ti))// pDB->UpdateTrack(tvs, ti))
					TagTrack(rec->track.location.c_str(), ti);
			}
			pAPI->SendMessage(SM_DatabaseUpdated);
		}
	}
	return bSomethingChanged;
}


//BOOL ActionManager::RenamerTranslatePattern(LPTSTR output, LPCTSTR pattern)
//{
//	ASSERT(output != NULL);
//	ASSERT(pattern != NULL);
//	if (output == NULL || pattern == NULL)
//	{
//		TRACE(_T("@1ActionManager::TranslatePattern. Parameter error: %d, %d\n"), output, pattern);
//		return FALSE;
//	}
//	CString newName = pattern;
//	PrgAPI* pLang = PRGAPI();
//	newName.Replace(PLACEHOLDER_ARTIST, pLang->GetString(IDS_ARTIST));
//	newName.Replace(PLACEHOLDER_ALBUM, pLang->GetString(IDS_ALBUM));
//	newName.Replace(PLACEHOLDER_TRACKNO, pLang->GetString(IDS_TRACKNO));
//	newName.Replace(PLACEHOLDER_TITLE, pLang->GetString(IDS_TITLE));
//	_tcsncpy(output, newName, MAX_PATH);
//	return TRUE;
//}
//
//BOOL ActionManager::RenamerGenerateName(LPTSTR newPath, LPCTSTR pattern, FullTrackRecordSP& track, BOOL relativePath/* = FALSE*/)
//{
//	ASSERT(newPath != NULL);
//	ASSERT(pattern != NULL);
//
//	CString newName = pattern;
//	newName.Replace(PLACEHOLDER_ARTIST, track->artist.name.c_str());
//	newName.Replace(PLACEHOLDER_ALBUM, track->album.name.c_str());
//	TCHAR bf[MAX_PATH];
//	_sntprintf(bf, MAX_PATH, _T("%0.2d"), track->track.trackNo);
//	newName.Replace(PLACEHOLDER_TRACKNO, bf);
//	newName.Replace(PLACEHOLDER_TITLE, track->track.name.c_str());
//	_tcschrrep(INVALID_CHARS_FOR_FILENAME, '_', newName.GetBuffer());
//	newName.ReleaseBuffer();
//	if (relativePath)
//		_tcsncpy(newPath, newName, MAX_PATH);
//	else
//		return MakeMewPath(newPath, newName, track->track.location.c_str());
//	return TRUE;
//}
//
//BOOL ActionManager::MakeMewPath(LPTSTR outPath, LPCTSTR newName, LPCTSTR originalPath)
//{
//	ASSERT(outPath != NULL);
//	ASSERT(newName != NULL);
//	ASSERT(originalPath != NULL);
//	const TCHAR* pos = _tcsrchr(originalPath, '\\');
//	if (pos != NULL)
//	{
//		pos++;
//		_tcsncpy(outPath, originalPath, pos - originalPath);
//		outPath[pos - originalPath] = 0;
//
//		_tcsncat(outPath, newName, MAX_PATH - _tcslen(outPath));
//		pos = _tcsrchr(originalPath, '.');
//		if (pos != NULL)
//		{
//			_tcsncat(outPath, pos, MAX_PATH - _tcslen(outPath));
//			return TRUE;
//		}
//	}
//	return FALSE;
//}


//------------------------------------------------------------------------
//EXPORT MANAGER ------------------------------HTML
//------------------------------------------------------------------------
//BOOL Export(FullTrackRecordCollection& col);
//BOOL Export(FullHistLogRecordCollection& col);
//BOOL Export(FullHistTrackRecordCollection& col);
//BOOL Export(FullHistArtistRecordCollection& col);


//BOOL ActionManager::Export(LPCTSTR title, CListCtrl* pList, BOOL bSelectionOnly)
//{
//	ASSERT(0);
//	//TODO: Using XMLLite + CSS. Meta bgale apo to MainListControl to DispInfo
//	//Check=== http://www.w3schools.com/xml/xml_display.asp
//	ASSERT(pList != NULL);
//	TCHAR bf[MAX_PATH];
//	_sntprintf(bf, MAX_PATH, _T("%s.html"), title);
//	CFileDialog fd(FALSE, _T(".html"), bf, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
//		_T("HTML Files (*.html)|*.html|All Files (*.*)|*.*||"), NULL);
//	if (fd.DoModal() != IDOK)
//		return FALSE;
//
//	CHeaderCtrl* pHeader = pList->GetHeaderCtrl();
//	HDITEM hdItem;
//	memset(&hdItem, 0, sizeof(HDITEM));
//	TCHAR hditemStr[MAX_PATH];
//	hdItem.cchTextMax = MAX_PATH;
//	hdItem.pszText = hditemStr;
//	CString output;
//	output = _T("<HTML><HEAD>\r\n<TITLE>");
//	output += title;
//	output += _T("</TITLE>\r\n<STYLE>\r\n");
//
//	output += GetHTMLStyle(title);
//
//	output += _T("</STYLE>\r\n");
//	output += _T("</HEAD>\r\n<BODY>\r\n<TABLE class=\"maintable\">\r\n");
//
//	output += _T("<tr class=\"columnheader\">\r\n");
//
//	int* iOrder = new int[pHeader->GetItemCount()];
//	hdItem.mask = HDI_ORDER;
//	for (int j = 0; j < pHeader->GetItemCount(); j++)
//	{
//		pHeader->GetItem(j, &hdItem);
//		iOrder[j] = hdItem.iOrder;
//	}
//	hdItem.mask = HDI_TEXT;
//	for (int j = 0; j < pHeader->GetItemCount(); j++)
//	{
//		output += _T("<td>");
//		pHeader->GetItem(iOrder[j], &hdItem);
//		hdItem.pszText[hdItem.cchTextMax - 1] = 0;
//		output += hdItem.pszText;
//		output += _T("</td>\r\n");
//	}
//	output += _T("</tr>\r\n");
//	int line = 0;
//	for (int i = 0; i < pList->GetItemCount(); i++)
//	{
//		if (!(bSelectionOnly && (pList->GetItemState(i, LVIS_SELECTED) == 0)))
//		{
//			output += (line%2 == 0) ? 
//				_T("<tr class=\"evenlines\">\r\n") : 
//			_T("<tr class=\"oddlines\">\r\n");
//			line++;
//			for (int j = 0; j < pHeader->GetItemCount(); j++)
//			{
//				pList->GetItemText(i, iOrder[j], bf, MAX_PATH);
//				bf[MAX_PATH - 1] = 0;
//				output += _T("<td>");
//				output += bf;
//				output += _T("</td>\r\n");
//			}
//			output += _T("</tr>\r\n");
//		}
//	}
//	delete iOrder;
//	output+= _T("</TABLE>\r\n</BODY>\r\n</HTML>");
//
//	CFile file;
//	if (file.Open(fd.GetPathName(), CFile::modeCreate | CFile::modeWrite))
//	{
//#ifdef _UNICODE
//		unsigned char ch = 0xff;
//		file.Write(&ch, 1);
//		ch = 0xfe;
//		file.Write(&ch, 1);
//#endif
//		file.Write((LPCTSTR) output, output.GetLength() * sizeof(TCHAR));
//	}
//	else
//	{
//		TRACE(_T("ActionManager::Export. Can't write file"));
//		return FALSE;
//	}
//	file.Close();
//
//	RequestViewExport(fd.GetPathName());
//
//	return TRUE;
//}
void ActionManager::RequestViewExport(LPCTSTR fileName)
{
	if (PRGAPI()->MessageBox(NULL, IDS_VIEWEXPORT, NULL, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		HINSTANCE result = ShellExecute(NULL, _T("open"), fileName, NULL, NULL, SW_SHOWNORMAL);
	}
}


CString ActionManager::GetHTMLStyle(LPCTSTR title)
{
	ASSERT(title != NULL);
	CFile css;

	TCHAR fn[MAX_PATH];
	VERIFY(::GetModuleFileName(AfxGetInstanceHandle(), fn, MAX_PATH));
	TCHAR *ch = _tcsrchr(fn,'\\');
	ch++;
	*ch = 0;
	_tcscat(fn, title);
	_tcscat(fn, _T(".css"));
	char* str = NULL;
	if (css.Open(fn, CFile::modeRead))
	{
		ULONGLONG fSize = css.GetLength();
		str = new char[UINT(fSize + 1)];
		css.Read(str, UINT(fSize));
		str[fSize] = 0;
	}
	else
	{
		HRSRC hres = FindResource(NULL, MAKEINTRESOURCE(IDR_DEFAULTEXPORTSTYLE), _T("TEXTRESOURCES"));
		if (hres != 0)
		{ 
			HGLOBAL hbytes = LoadResource(NULL, hres);
			LPVOID pdata = LockResource(hbytes);
			DWORD resSize = SizeofResource(NULL, hres);
			str = new char[resSize + 1];
			memcpy(str, pdata, resSize);
			str[resSize] = 0;
			FreeResource(hres);
			if (css.Open(fn, CFile::modeCreate | CFile::modeWrite))
				css.Write(str, resSize);
		}
	}
	CString style(str);
	delete str;
	return style;
}

BOOL ActionManager::FindAndFixMountedPath(LPCTSTR originalFileName, LPTSTR realFileName, UINT bfLen)
{
	//THESE ERROR HAVE BEEN ADDRESSED HERE
	//53=The network path was not found
	//1815=The specified resource language ID cannot be found in the image file.
	//TS have crashed inside here
	//---
	//21=The device is not ready
	//15105=The resource loader cache doesn't have loaded MUI entry.
	//TS crashes after the end of PathFileExistsEx

	TRACEST(_T("ActionManager::FindAndFixMountedPath."), originalFileName);
	ASSERT(originalFileName != 0 && realFileName != 0 && bfLen > 200);
	BOOL bPathFound = FALSE;
	//ASSERT(originalFileName[0] == '#');//Keep it as a warning
	if (originalFileName[1] != ':')
	{
		TRACE(_T("@1 ActionManager::FindAndFixMountedPath. Illegal FileName: '%s'\r\n"), originalFileName);
		return FALSE;
	}
	_tcsncpy(realFileName, originalFileName, bfLen);
	if (PathFileExists(realFileName))
		return TRUE;
	realFileName[bfLen - 1] = 0;
	TCHAR drv[4];
	drv[1] = ':';
	drv[2] = '\\';
	drv[3] = 0;
	UINT drvType = 0;

	//=== Maybe i could enum drives
	for (TCHAR i = 'c'; i <= 'z'; i++)
	{
		drv[0]= i;
		drvType = GetDriveType(drv);
		if (drvType == DRIVE_CDROM || drvType == DRIVE_REMOVABLE)
		{
			realFileName[0] = drv[0];
			if (PathFileExists(realFileName))
				return TRUE;
		}
	}
	TRACE(_T("@3 ActionManager::FindAndFixMountedPath.\r\n"));
	if (TRUE)//bNotifyUser)
	{
		TCHAR msg[1000];
		_sntprintf(msg, 1000, _T("%s ('%s')"), 
			PRGAPI()->GetString(IDS_CANTLOCATEFILE),
			originalFileName);
		PRGAPI()->NotifyUser(msg, NULL, SEV_Warning);
	}
	return FALSE;
}


//BOOL ActionManager::PathFileExistsEx(LPTSTR pathName, 
//									 CollectionTypesEnum cType/* = CTYPE_Unknown*/, 
//									 BOOL bNotifyUser /*= FALSE*/)
//{
//	//THESE ERROR HAVE BEEN ADDRESSED HERE
//	//53=The network path was not found
//	//1815=The specified resource language ID cannot be found in the image file.
//	//TS have crashed inside here
//	//---
//	//21=The device is not ready
//	//15105=The resource loader cache doesn't have loaded MUI entry.
//	//TS crashes after the end of PathFileExistsEx
//
//	TRACEST(_T("ActionManager::PathFileExistsEx."));
//	ASSERT(pathName != 0);
//	BOOL bPathFound = FALSE;
//	if (cType == CTYPE_Unknown)
//		cType = (pathName[0] != '#') ? CTYPE_LocalFolder : CTYPE_Removable;
//	switch (cType)
//	{
//	case CTYPE_LocalFolder:
//	case CTYPE_NetFolder:
//		ASSERT(pathName[0] != '#');
//		bPathFound = PathFileExists(pathName);
//		break;
//	case CTYPE_Removable:
//	case CTYPE_Media:
//		ASSERT(pathName[0] == '#');
//		{
//			TRACE(_T("@3 ActionManager::PathFileExistsEx. CTYPE_Removable/CTYPE_Media\r\n"));
//			TCHAR originalDrive = pathName[0];
//			TCHAR drv[4];
//			drv[1] = ':';
//			drv[2] = '\\';
//			drv[3] = 0;
//			UINT drvType = 0;
//			for (TCHAR i = 'c'; i < 'z'; i++)
//			{
//				drv[0]= i;
//				drvType = GetDriveType(drv);
//				if (drvType == DRIVE_CDROM || drvType == DRIVE_REMOVABLE)
//				{
//					pathName[0] = drv[0];
//					bPathFound = PathFileExists(pathName);
//					if (bPathFound)
//					{
//						//TRACE(_T("@3 ActionManager::PathFileExistsEx. Path found\r\n"));
//						break;
//					}
//				}
//			}
//			if (!bPathFound)//Restore the Initial Name
//				pathName[0] = originalDrive;
//		}
//		break;
//	case CTYPE_AudioCD:
//		break;
//	case CTYPE_Virtual:
//		bPathFound = TRUE;
//		break;
//	default:
//		TRACE(_T("ActionManager::PathFileExistsEx. Unknown Collection Type\r\n"));
//		ASSERT(0);
//	}
//	if (!bPathFound)
//	{
//		if (bNotifyUser)
//		{
//			TCHAR msg[1000];
//			_sntprintf(msg, 1000, _T("%s ('%s')"), 
//				PRGAPI()->GetString(IDS_CANTLOCATEFILE),
//				pathName);
//			PRGAPI()->NotifyUser(msg, NULL, SEV_Warning);
//		}
//		else
//			TRACE(_T("@3 ActionManager::PathFileExistsEx. Cannot locate: '%s'\r\n"), pathName);
//	}
//	return bPathFound;
//
//}

TCHAR ActionManager::IsCollectionMounted(INT driveSerial)
{
	TRACEST(_T("ActionManager::IsCollectionMounted."));
	TCHAR drv[4];
	drv[1] = ':';
	drv[2] = '\\';
	drv[3] = 0;
	UINT drvType = 0;
	TCHAR volumeName[MAX_PATH];
	DWORD serNum = 0;
	for (TCHAR i = 'c'; i <= 'z'; i++)
	{
		drv[0]= i;
		GetVolumeInformation(drv, volumeName, MAX_PATH, &serNum, NULL, NULL, NULL, NULL);
		if (serNum == driveSerial)
			return drv[0];
	}
	return TCHAR(0);

	//TCHAR buf[MAX_PATH];
	//HANDLE hVol = FindFirstVolume(buf, MAX_PATH);

	//TCHAR volumeName[MAX_PATH];
	//DWORD serNum = 0;
	//
	//while (hVol != INVALID_HANDLE_VALUE)
	//{
	//	GetVolumeInformation(buf, volumeName, MAX_PATH, &serNum, NULL, NULL, NULL, NULL);
	//	if (serNum == driveSerial)
	//		break;
	//	if (!FindNextVolume(hVol, buf, MAX_PATH))
	//		break;
	//}
	//FindVolumeClose(hVol);
	//if (serNum == driveSerial)
	//{
	//	GetVolumePathName(volumeName. )
	//}
	//return TCHAR(0);
}

//BOOL ActionManager::OpenFolderToItem(TracksQuery& tvs, UINT pos)
//{
//	ASSERT(tvs.IsOpen());
//	if (!tvs.IsOpen())
//		return FALSE;
//	if (pos > 0)
//		tvs.SetPosition(pos);
//	TCHAR sFile[MAX_PATH];
//	tvs.GetLocation(sFile, MAX_PATH);
//	if (PathFileExistsEx(sFile, tvs.GetCollectionType(), TRUE))
//	{
//		return OpenFolderToItem(sFile);
//	}
//	return FALSE;
//
//}


INT ActionManager::GetFileSize(LPCTSTR fileName)
{
	ASSERT(fileName != NULL);
	HANDLE f= CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f != INVALID_HANDLE_VALUE)
	{
		DWORD fs = ::GetFileSize(f, NULL);
		CloseHandle(f);
		return INT(fs);
	}
	return -1;
}


BOOL ActionManager::ReadTags(FullTrackRecordCollection& col)
{
	TRACEST(_T("ActionManager::ReadTags."));
	FileTagger* pFT = PRGAPI()->GetFileTagger();
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	FullTrackRecordCollection::iterator it = col.begin();
	for (;it!=col.end();it++)
	{
		TrackRecord* pTR = &(*it)->track;
		TagInfo info;
		info.validFields = pFT->Read(pTR->location.c_str(), info);
		if (info.validFields > 0)
			pSM->UpdateTrackFromTagInfo(pTR->ID, pTR->colID, pTR->location.c_str(), info);
	}
	PRGAPI()->SendMessage(SM_DatabaseUpdated);
	return TRUE;
}

BOOL ActionManager::WriteTags(FullTrackRecordCollection& col)
{
	TRACEST(_T("ActionManager::WriteTags."));
	FullTrackRecordCollection::iterator it = col.begin();
	for (;it!=col.end();it++)
	{
		WriteTags(*(*it));
	}
	return TRUE;
}
BOOL ActionManager::WriteTags(FullTrackRecord& rec)
{
	TRACEST(_T("ActionManager::WriteTags (FullTrackRecord)."));
	TagInfo ti;
	FullTrackRecord2TagInfo(rec, ti);
	return TagTrack(rec.track.location.c_str(), ti);
}


void ActionManager::FullTrackRecord2TagInfo(FullTrackRecord& rec, TagInfo& ti)
{
	TRACEST(_T("ActionManager::FullTrackRecord2TagInfo."));
	//Check for unknowm
	ti.Bitrate = rec.track.bitrate;
	ti.Length = rec.track.duration;
	ti.Rating = rec.track.rating;
	ti.TrackNo = rec.track.trackNo;
	ti.Year = rec.track.year;
	ti.validFields = TagInfo_Bitrate | TagInfo_Length | TagInfo_Rating | TagInfo_TrackNo | TagInfo_Year;


	if (rec.album.name != TS_UnknownString)
	{
		ti.Album = rec.album.name;
		ti.validFields |= TagInfo_Album;
	}
	if (rec.artist.name != TS_UnknownString)
	{
		ti.Artist = rec.artist.name;
		ti.validFields |= TagInfo_Artist;
	}
	if (rec.genre.name != TS_UnknownString)
	{
		ti.Genre = rec.genre.name;
		ti.validFields |= TagInfo_Genre;
	}
	if (rec.track.name != TS_UnknownString)
	{
		ti.Title = rec.track.name;
		ti.validFields |= TagInfo_Title;
	}

	SQLManager* pSM = PRGAPI()->GetSQLManager();
	InfoRecord ir;
	if (pSM->GetTrackInfoRecord(rec.track.ID, IIT_TrackLyrics, ir))
	{
		ti.validFields |= TagInfo_Lyrics;
		ti.Lyrics = ir.name;
	}
	if (pSM->GetTrackInfoRecord(rec.track.ID, IIT_TrackComment, ir))
	{
		ti.validFields |= TagInfo_Comment;
		ti.Comment = ir.name;
	}
}


BOOL ActionManager::CopyPathNameToClipboard(LPCTSTR path, CollectionTypesEnum cType)
{
	TRACEST(_T("ActionManager::CopyPathNameToClipboard."));
	ASSERT(path != NULL);
	return CopyStringToClipboard(path);
}


BOOL ActionManager::CopyStringToClipboard(LPCTSTR str)
{
	TRACEST(_T("ActionManager::CopyStringToClipboard."));
	size_t len = _tcslen(str);
	if (OpenClipboard(AfxGetMainWnd()->m_hWnd))
	{
		EmptyClipboard();
		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (len+1) * sizeof(TCHAR));
		TCHAR* pchData = (TCHAR*)GlobalLock(hClipboardData);
		_tcscpy(pchData, str);
		GlobalUnlock(hClipboardData);
#ifdef _UNICODE
		SetClipboardData(CF_UNICODETEXT, hClipboardData);
#else
		SetClipboardData(CF_TEXT, hClipboardData);
#endif
		CloseClipboard();
		return TRUE;
	}
	return FALSE;
}

void ActionManager::LocalizeXFolderDialog(CXFolderDialog& dlg)
{
	PrgAPI* pAPI = PRGAPI();
	dlg.SetTitle(pAPI->GetString(IDS_SELECTFOLDER));
	dlg.SetOkTitle(pAPI->GetString(IDS_OK));
	dlg.SetCancelTitle(pAPI->GetString(IDS_CANCEL_S));
	dlg.SetFolderNameTitle(pAPI->GetString(IDS_FOLDER));
	dlg.SetLookInTitle(pAPI->GetString(IDS_LOOKIN));
}


BOOL ActionManager::CopyFilesToFolder(FullTrackRecordCollection& col, LPCTSTR targetFolder/* = 0*/)
{
	TRACEST(_T("ActionManager::CopyFilesToFolder."));
	PrgAPI* pAPI = PRGAPI();
	CXFolderDialog dlg(GetLastTargetFolder(), 0,  AfxGetMainWnd());
	LocalizeXFolderDialog(dlg);
	if (targetFolder == 0)
	{
		if ((dlg.DoModal() == IDCANCEL))
			return FALSE;
		targetFolder = dlg.GetPath();
	}
	CShellFileOp op;
	FullTrackRecordCollection::const_iterator it = col.begin();
	TCHAR sFile[MAX_PATH];
	for (;it!=col.end();it++)
	{
		FullTrackRecordSP rec = *it;
		LPCTSTR path = rec->track.location.c_str();
		if (!PathFileExists(path))
		{
			if (FindAndFixMountedPath(path, sFile, MAX_PATH))
				path = sFile;
			else
				continue;
		}
		op.AddSourceFile(path);
	}
	if (op.GetSourceFileList().GetCount() > 0)
	{
		m_LastTargetFolder = targetFolder;
		op.AddDestFile(targetFolder);
		op.SetOperationFlags(FO_COPY, AfxGetMainWnd(), FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
		BOOL         bAPICalled;
		int          nAPIReturnVal;
		if (!op.Go(&bAPICalled, &nAPIReturnVal))
		{
			pAPI->NotifyUser(IDS_OPERATIONFAILED, NULL, SEV_Warning);
			TRACE(_T("@1 op.Go bAPICalled: %d nAPIReturnVal: %d\n"), bAPICalled, nAPIReturnVal);
		}
		else
		{
			pAPI->NotifyUser(IDS_SUCCESS, NULL, SEV_Warning);
			return TRUE;
		}
	}
	else
	{
		pAPI->NotifyUser(IDS_OPERATIONFAILED, NULL, SEV_Warning);
	}

	return FALSE;
}

BOOL ActionManager::CopyFilesToClipboard(FullTrackRecordCollection& col)
{
	TRACEST(_T("ActionManager::CopyFilesToClipboard."));
	ASSERT(col.size() > 0);
	std::vector<TCHAR> files;
	FullTrackRecordCollection::const_iterator it = col.begin();
	for (;it!=col.end();it++)
	{
		FullTrackRecordSP rec = *it;
		LPCTSTR path = rec->track.location.c_str();
		TCHAR sFile[MAX_PATH];
		if (!PathFileExists(path))
		{
			if (FindAndFixMountedPath(path, sFile, MAX_PATH))
				path = sFile;
			else
				continue;
		}
		UINT len = _tcslen(sFile) + 1;
		for (UINT j = 0; j < len; j++)
			files.push_back(sFile[j]);
	}
	if (files.size() > 0)
	{
		files.push_back(TCHAR(0));
		UINT uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * files.size();
		// Allocate memory from the heap for the DROPFILES struct.
		HGLOBAL hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );
		if ( NULL == hgDrop )
			return FALSE;
		DROPFILES* pDrop = (DROPFILES*) GlobalLock ( hgDrop );
		if ( NULL == pDrop )
		{
			GlobalFree ( hgDrop );
			return FALSE;
		}
		// Fill in the DROPFILES struct.
		pDrop->pFiles = sizeof(DROPFILES);

		// If we're compiling for Unicode, set the Unicode flag in the struct to
		// indicate it contains Unicode strings.
#ifdef _UNICODE
		pDrop->fWide = TRUE;
#else
		pDrop->fWide = FALSE;
#endif
		memcpy((void*) (LPBYTE(pDrop) + sizeof(DROPFILES)), &(files[0]), sizeof(TCHAR) * files.size());
		GlobalUnlock ( hgDrop );
		if (OpenClipboard(AfxGetMainWnd()->GetSafeHwnd()))
		{
			EmptyClipboard();
			SetClipboardData(CF_HDROP, hgDrop);
			CloseClipboard();
		}
	}
	else
	{
		PRGAPI()->NotifyUser(IDS_OPERATIONFAILED, NULL, SEV_Warning);
	}
	return files.size() > 0;
}

BOOL ActionManager::DeleteFiles(FullTrackRecordCollection& col)
{
	TRACEST(_T("ActionManager::DeleteFiles."));
	INT operationSucceeds = 0;
	//CWaitCursor w;
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	FullTrackRecordCollection::iterator it = col.begin();
	for (; it != col.end(); it++)
	{
		FullTrackRecordSP& rec = *it;
		TCHAR sFile[MAX_PATH];
		BOOL bDeleteOK = FALSE;
		BOOL bRemoveFromDatabase = TRUE;
		if (FindAndFixMountedPath(rec->track.location.c_str(), sFile, MAX_PATH))
		{
			switch (rec->collection.type)
			{
			case CTYPE_AudioCD:
				bDeleteOK = FALSE;
				break;
			case CTYPE_Virtual:
				bDeleteOK = TRUE;
				break;
			case CTYPE_LocalFolder:
			case CTYPE_NetFolder:
			case CTYPE_Removable:
			case CTYPE_Media:
			default:
				bDeleteOK = DeleteFile(sFile);
			}
		}
		if (!bDeleteOK)
		{
			TCHAR msg[2000];
			_sntprintf(msg, 2000, _T("\"%s\".\r\n%s. %s"), sFile, pAPI->GetString(IDS_FILECOULDNOTBEDELETED), pAPI->GetString(IDS_REMOVEITFROMDATABASE));
			if (PRGAPI()->MessageBox(NULL, msg, NULL, MB_YESNO) == IDNO)
				bRemoveFromDatabase = FALSE;
		}
		if (bRemoveFromDatabase)
		{
			if (pSM->DeleteTrackRecord((*it)->track.ID))
			{
				operationSucceeds++;
				(*it)->track.ID = 0;//Mark as invalid
			}
		}
	}
	if (operationSucceeds == 0)
		pAPI->NotifyUser(IDS_OPERATIONFAILED, NULL, SEV_Warning);
	return operationSucceeds > 0;
}

BOOL ActionManager::DeleteFile(LPCTSTR sFile)
{
	TRACEST(_T("ActionManager::DeleteFile."));
	if (::DeleteFile(sFile))
		return TRUE;
	TRACE(_T("@1 Delete File '%s' Failed.\r\n"), sFile);
	PRGAPI()->NotifyUser(IDS_OPERATIONFAILED, NULL, SEV_Warning);
	return FALSE;
}

BOOL ActionManager::SetTrackRating(FullTrackRecordCollection& col, INT Rating)
{
	TRACEST(_T("ActionManager::SetTrackRating."));
	FullTrackRecordCollection::iterator it = col.begin();
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	for (; it!=col.end(); it++)
	{
		FullTrackRecordSP& rec = *it;
		if (rec->track.rating != Rating)
		{
			rec->track.rating = Rating;
			pSM->UpdateTrackRecord(rec->track);
			TagInfo ti;
			ti.Rating = Rating;
			ti.validFields = TagInfo_Rating;
			TagTrack((*it)->track.location.c_str(), ti);
			if (pAPI->GetOption(OPT_GEN_UseNativeOnlineServices))
			{
				TSOnlineServices* pTS = pAPI->GetTSOnlineServices();
				TSOnlineServices::TrackInfo ti;
				ti.artist = rec->artist.name.c_str();
				ti.title = rec->track.name.c_str();
				ti.rating = rec->track.rating;
				ti.album = rec->album.name.c_str();
				ti.tag = rec->genre.name.c_str();
				pTS->SendTrackInfo(ti);
				pAPI->PostMessage(SM_TrackUpdated);
			}
		}
	}
	return TRUE;
}
//BOOL ActionManager::SetTrackRating(LPCTSTR path, INT Rating)
//{
//	DatabaseManager* pDB = PRGAPI()->GetDatabaseManager();
//	BOOL bModified = FALSE;
//	TracksQuery tq;
//	if (tq.OpenByLocation(path))
//	{
//		if (tq.GetRating() != Rating)
//		{
//			TagInfo ti;
//			ti.Rating = Rating;
//			if (pDB->UpdateTrack(tq, ti, TagInfo_Rating)) 
//				bModified = TagTracks(tq, ti, TagInfo_Rating, FALSE);
//			else
//				ASSERT(0);
//		}
//	}
//	return bModified;
//}

//BOOL ActionManager::RenameFiles(FullTrackRecordCollection& col, LPCTSTR sPattern)
//{
//	ASSERT(col.size() > 0);
//	ASSERT(sPattern != NULL);
//	TCHAR oldPath[MAX_PATH];
//	TCHAR newPath[MAX_PATH];
//	//TracksTableSchema tts;
//	INT operationSucceeds = 0;
//	PrgAPI* pAPI = PRGAPI();
//
//	FullTrackRecordCollection::iterator it = col.begin();
//	for (; it != col.end(); it++)
//	{
//		_tcsncpy(oldPath, (*it)->track.location.c_str(), MAX_PATH);
//		if (PathFileExistsEx(oldPath, (*it)->collection.type, TRUE))
//		{
//			if (RenamerGenerateName(newPath, sPattern, *it))
//			{
//				BOOL bVirtualPath = (newPath[0] == '#');
//				if (newPath[0] == '#')
//					newPath[0] = oldPath[0];
//				if (::MoveFile(oldPath, newPath))
//				{
//					if (bVirtualPath)
//						newPath[0] = '#';
//					(*it)->track.location = newPath;
//					if (pAPI->GetSQLManager()->UpdateTrackRecord((*it)->track))
//						operationSucceeds++;
//				}
//				else
//				{
//					TRACE(_T("@ECMainListCtrl::RenameTrack. MoveFile"));
//					ASSERT(0);
//				}
//			}
//
//		}
//
//	}
//	if (operationSucceeds == 0)
//		pAPI->NotifyUser(IDS_OPERATIONFAILED, NULL, SEV_Warning);
//	return operationSucceeds > 0;
//}


BOOL ActionManager::ChangeCaseFiles(FullTrackRecordCollection& col, CaseStyleEnum cstyle)
{
	TRACEST(_T("ActionManager::ChangeCaseFiles."));
	ASSERT(cstyle < CASE_Last);
	INT operationSucceeds = 0;

	SQLManager* pSM = PRGAPI()->GetSQLManager();
	FullTrackRecordCollection::iterator it = col.begin();
	TCHAR title[MAX_PATH];
	for (; it != col.end(); it++)
	{
		_tcsncpy(title, (*it)->track.name.c_str(), MAX_PATH);
		switch (cstyle)
		{
		case CASE_AllCaps:
			_tcsupr(title);
			break;
		case CASE_NoCaps:
			_tcslwr(title);
			break;
		case CASE_Proper:
			_tcsproper(title);
			break;
		case CASE_FirstCap:
			_tcslwr(title);
			title[0] = _totupper(title[0]);
			break;
		}
		(*it)->track.name = title;
		operationSucceeds += pSM->UpdateTrackRecord((*it)->track) ? 1 : 0;
	}
	return operationSucceeds > 0;
}




//BOOL ActionManager::AddVirtualTrackDialog(LPCTSTR path, TagInfo& ti, CWnd* pParent /*= NULL*/)
//{
//	TRACEST(_T("ActionManager::AddVirtualTrackDialog."));
//	if (pParent == NULL)
//		pParent = AfxGetMainWnd();
//	PrgAPI* pLang = PRGAPI();
//	CPropertiesSheet ps(_T(""), pParent);
//	ps.m_psh.dwFlags |= PSH_NOAPPLYNOW;
//
//	//--- Main Track Page
//	CMainTrackPage MainPage(IDS_PROPERTIES);
//	ps.AddPage(&MainPage);
//	MainPage.Artist = ti.Artist.c_str();
//	MainPage.Album = ti.Album.c_str();
//	MainPage.TrackNo = ti.TrackNo;
//	MainPage.Title = ti.Title.c_str();
//	MainPage.Genre = ti.Genre.c_str();
//	MainPage.Year = ti.Year;
//	MainPage.m_bLocationEnabled = TRUE;
//	MainPage.CollectionName = PRGAPI()->GetString(IDS_VIRTUALTRACKS);
//	MainPage.Location = path;
//
//	//--- Comments
//	CInfoPage CommentPage(IDS_COMMENTS);
//	ps.AddPage(&CommentPage);
//	CommentPage.info = ti.Comment.c_str();
//
//	//--- Lyrics
//	CInfoPage LyricsPage(IDS_LYRICS);
//	ps.AddPage(&LyricsPage);
//	LyricsPage.info = ti.Lyrics.c_str();
//
//	//---------Finally Start the dialog
//	ps.SetTitle(PRGAPI()->GetString(IDS_ADDVIRTUALTRACK));
//	while (true)//Loop while the user adding the same track
//	{
//		if (ps.DoModal() != IDOK )
//			return FALSE;
//		ti.Album = MainPage.Album;
//		ti.Artist = MainPage.Artist;
//		ti.Genre = MainPage.Genre;
//		ti.Title = MainPage.Title;
//		ti.TrackNo = MainPage.TrackNo;
//		ti.Year = MainPage.Year;
//		ti.Comment = CommentPage.info;
//		ti.Lyrics = LyricsPage.info;
//
//		SQLManager* pSM = PRGAPI()->GetSQLManager();
//		CollectionRecord cr;
//		if (pSM->GetVirtualTrackCollectionRecord(cr))
//		{
//			TrackRecord tr;
//			if (pSM->GetTrackRecordUnique(cr.ID, MainPage.Location, tr))
//				PRGAPI()->MessageBox(pParent == NULL ? NULL : pParent->m_hWnd, IDS_ITEMALREADYEXISTS);
//			else
//				return pSM->AddNewTrackFromTagInfo(cr.ID, MainPage.Location, ti);
//		}
//	}
//	return TRUE;
//}
//


#ifdef _DEBUG
LPCTSTR const sCheckPriceURL = _T("http://www.artificialspirit.com/services/ecommerce.php?debug=1&");
#else
LPCTSTR const sCheckPriceURL = _T("http://www.artificialspirit.com/services/ecommerce.php?");
#endif
//=== Build links here
//===	http://affiliate-program.amazon.com/gp/associates/network/build-links/text/main.html/ref=amb_link_179935_2/104-2536978-2138309?pf_rd_m=ATVPDKIKX0DER&pf_rd_s=assoc-center-1&pf_rd_r=&pf_rd_t=501&pf_rd_p=&pf_rd_i=assoc_help_t5_a9


BOOL ActionManager::GoToArtistBuyPage(LPCTSTR artist)
{
	TRACEST(_T("ActionManager::GoToArtistBuyPage."));
	ASSERT(artist != NULL);
	if (!(artist != NULL))
		return FALSE;
	if (artist[0] == '[')
		return FALSE;
	TCHAR bf[1000];
	_sntprintf(bf, 1000, _T("%stype=artist&artist=%s&langid=%d"), 
		sCheckPriceURL, 
		artist,
		GetSystemDefaultLangID());
	ShellExecute(NULL, NULL, bf, 0,0,0);
	return TRUE;
}

BOOL ActionManager::GoToAlbumBuyPage(LPCTSTR artist, LPCTSTR album)
{
	TRACEST(_T("ActionManager::GoToAlbumBuyPage."));
	ASSERT(artist != NULL && album != NULL);
	if (!(artist != NULL && album != NULL))
		return FALSE;
	if (artist[0] == '[' || album[0] == '[')
		return FALSE;
	TCHAR bf[1000];
	_sntprintf(bf, 1000, _T("%stype=album&artist=%s&album=%s&langid=%d"), 
		sCheckPriceURL, 
		artist,
		album,
		GetSystemDefaultLangID());
	ShellExecute(NULL, NULL, bf, 0,0,0);
	return TRUE;
}
BOOL ActionManager::GoToTrackBuyPage(LPCTSTR artist, LPCTSTR title)
{
	TRACEST(_T("ActionManager::GoToTrackBuyPage."));
	ASSERT(artist != NULL && title != NULL);
	if (!(artist != NULL && title != NULL))
		return FALSE;
	TCHAR bf[1000];
	_sntprintf(bf, 1000, _T("%stype=mp3&artist=%s&title=%s&langid=%d"), 
		sCheckPriceURL, 
		artist,
		title,
		GetSystemDefaultLangID());
	ShellExecute(NULL, NULL, bf, 0,0,0);
	return TRUE;
}



void ActionManager::LogUserMessage(LPCTSTR msg, LPCTSTR title/* = NULL*/, SEVERITY sev/* = SEV_InfoL1*/)
{
	//TRACEST(_T("ActionManager::LogUserMessage."));
	ASSERT(msg != NULL && msg[0] != NULL);
	if (msg != NULL && msg[0] != NULL)
	{
		CSingleLock lock(&m_LogCS, FALSE);
		if (lock.IsLocked())
			_T("@1ActionManager::LogUserMessage.. was locked\r\n");
		lock.Lock();
		if (m_logListPos >= MAX_LOG_LINES_SIZE)
			m_logListPos = 0;
		m_LogList[m_logListPos] = msg;
		m_logListPos++;
	}
}

void ActionManager::ShowUsersLog()
{
	TRACEST(_T("ActionManager::ShowUsersLog."));
	CSingleLock lock(&m_LogCS, FALSE);
	if (lock.IsLocked())
		_T("@1ActionManager::ShowUsersLog.. was locked\r\n");
	lock.Lock();
	TCHAR bf[MAX_PATH];
	if (GetTempFile(bf, MAX_PATH, _T("applog.txt")))
	{
		CFile f;
		if (f.Open(bf, CFile::modeCreate | CFile::modeWrite))
		{
			for (UINT i = m_logListPos; i < MAX_LOG_LINES_SIZE; i++)
			{
				if (m_LogList[i].empty())
					break;
				f.Write((LPCSTR)CT2CA(m_LogList[i].c_str()),m_LogList[i].size());
				f.Write("\r\n", 2);
			}
			for (UINT i = 0; i < m_logListPos; i++)
			{
				f.Write((LPCSTR)CT2CA(m_LogList[i].c_str()),m_LogList[i].size());
				f.Write("\r\n", 2);
			}
			f.Close();
			ShellExecute(NULL, NULL, bf, 0,0,SW_SHOWDEFAULT);
		}
	}
}
BOOL ActionManager::GetTempFile(LPTSTR pathName, UINT len /* = MAX_PATH */, LPCTSTR fileName/* = NULL*/)
{
	TRACEST(_T("ActionManager::GetTempFile."));
	UINT dwRetVal = GetTempPath(len, pathName); // buffer for path 
	ASSERT(dwRetVal < len);
	if (!(dwRetVal < len))
		return FALSE;
	// Create a temporary file.
	if (fileName == NULL)
	{
		dwRetVal = GetTempFileName(pathName, // directory for tmp files
			_T("TS"),        // temp file name prefix 
			1,            // create unique name 
			pathName);  // buffer for name 
		ASSERT(dwRetVal > 0);
	}
	else
		_tcsncat(pathName, fileName, len - _tcslen(pathName));
	return dwRetVal > 0;
}




void ActionManager::TrackHistory()
{
	PrgAPI* pAPI = PRGAPI();
	MediaPlayer* pMp = pAPI->GetMediaPlayer();
	if (pMp==NULL)		return;
	INT curIndex = pMp->GetPlayListPos();
	if (curIndex == -1)	return;
	DOUBLE mediaLength = pMp->GetMediaLength();
	if (mediaLength == 0.0)//This function will not record Unknown length tracks
		return;



	//This logic does this: Records the Track to the history if and only if (hah)
	//The user has passed first from the beginning and then from the other 2 positions
	//in the track. I don t want to record a track to the history if it hasn't fully played
	DOUBLE curSec = pMp->GetMediaPos();
	if (mediaLength != m_LastTrackLength)
	{
		m_LastTrackLength = mediaLength;
		m_LogTrackStatus = -1;//Reset it. We have a new song
	}
	else if (curSec < 0.1 * mediaLength && m_LogTrackStatus == -1)//Bug Fix. Sometimes Media Change Event does not appears
	{
		m_LogTrackStatus = 0;
	}	
	else if (curSec > 0.2 * mediaLength && curSec < 0.3 * mediaLength && m_LogTrackStatus == 0)
	{
		m_LogTrackStatus = 1;
	}
	else if (curSec > 0.8 * mediaLength && curSec < 0.9 * mediaLength && m_LogTrackStatus == 1)
	{
		TRACEST(_T("ActionManager::TrackHistory. Scrobbling Track"));
		m_LogTrackStatus = 2;
		TrackerTrack track;
		track.length = (UINT)pMp->GetMediaLength();
		MediaPlayListItem mpli;
		if (pMp->GetPlayListItem(mpli, curIndex))
		{
			FullTrackRecordSP rec;
			if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
			{
				track.album = rec->album.name.c_str();
				track.artist = rec->artist.name.c_str();
				track.rating = rec->track.rating;
				track.tag = rec->genre.name.c_str();
				track.title = rec->track.name.c_str();
				InfoRecord ir;
				if (pAPI->GetSQLManager()->GetTrackInfoRecord(rec->track.ID, IIT_TrackLyrics, ir))
					track.lyrics = ir.name.c_str();
				pAPI->GetTracker()->SubmitTrack(ITracker::ACT_Played, track, GetCurrentUnixTimeStamp());
			}
			else
			{
				TRACE(_T("@2 ActionManager::TrackHistory. File Not Found in DB\r\n"));
				if (mpli.title != NULL)
				{
					track.artist = (mpli.artist != NULL ? mpli.artist : NULL);
					track.title = mpli.title;
					pAPI->GetTracker()->SubmitTrack(ITracker::ACT_Played, track, GetCurrentUnixTimeStamp());
				}
			}
		}
		////MediaInfo mf = m_pPlayer->GetMediaInfo();
		//LPCTSTR artist = pMp->GetPlayList()->GetStringProperty(MSP_Artist, curIndex);
		//LPCTSTR title = pMp->GetPlayList()->GetStringProperty(MSP_Title, curIndex);
		//if (artist[0] != 0 && title[0] != 0)
		//{
		//	PRGAPI()->GetSQLManager()->LogTrackInHistory(artist, title, HLA_Played);
		//	TRACE(_T("@4CMediaPlayerCtrl::OnTimer. History Track Logged.\n"));
		//}
	}
}

//void ActionManager::AddRandomTracks(RandomTrackModeEnum mode, UINT ID, UINT AvoidID /*= 0*/, UINT numTracks/* = 5*/, INT insertPos /*= -1*/)
//{
//	DatabaseManager* pDB = PRGAPI()->GetDatabaseManager();
//	MediaPlayer* pPl = PRGAPI()->GetMediaPlayer();
//	BOOL ret = FALSE;
//	if (!(ID == 0 && mode != RTM_All))
//	{
//		std::vector<ULONG> IDs;
//		DatabaseManager::RandomTrackFilters rtf;
//		rtf.mode = mode;
//		rtf.rtmHelperID = ID;
//		rtf.avoidID = AvoidID;
//		ret = pDB->GetRandomTracks(IDs, numTracks, rtf);
//		if (ret)
//		{
//			std::vector<ULONG>::const_iterator it = IDs.begin();
//			int nItem = pPl->GetCount();
//			if (insertPos > -1 && insertPos < nItem)
//				nItem = insertPos;
//			for (it; it != IDs.end(); it++)
//			{
//				if (nItem > -1)
//				InsertMedia(*it, nItem);
//			}
//		}
//	}
//}




BOOL ActionManager::GetRandomTrackCollection(FullTrackRecordCollection& col, TracksFilter& tf, UINT desiredTrackCount)
{
	TRACEST(_T("ActionManager::GetRandomTrackCollection."));
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	std::vector<UINT> trackIDs;
	if (!pSM->GetTrackIDList(tf, trackIDs))
		return FALSE;
	if (trackIDs.size() == 0)
		return FALSE;
	std::vector<UINT> selectedIDs;
	UINT loopCount = 0;
	while (TRUE)
	{
		if (loopCount > desiredTrackCount + 20)
		{
			TRACE(_T("@1ActionManager::GetRandomTrackCollection. Loop Limit Reached\r\n"));
			return FALSE;
		}
		loopCount++;
		INT rndNum = m_rndGenerator() % trackIDs.size();
		INT ID = trackIDs[rndNum];
		std::vector<UINT>::iterator it = selectedIDs.begin();
		BOOL bExists = FALSE;
		for (; it != selectedIDs.end(); it++)
		{
			if (*it == ID)
			{
				bExists = TRUE;
				break;
			}
		}
		if (!bExists)
		{
			selectedIDs.push_back(ID);
			FullTrackRecordSP rec;
			if (pSM->GetFullTrackRecordByID(rec, ID))
				col.push_back(rec);
			if (col.size() >= desiredTrackCount)
				break;
		}
		if (selectedIDs.size() >= trackIDs.size())
			break;
	}
	return TRUE;
}

void ActionManager::GetRelativeRandomTrackFilter(TracksFilter& tf, FullTrackRecordSP& rec, RandomTrackModeEnum rtm)
{
	TRACEST(_T("ActionManager::GetRelativeRandomTrackFilter."));
	if (rec.get() != NULL)
	{
		tf.TrackID.match = NUMM_Diff;//Do not include current
		tf.TrackID.val = rec->track.ID;
		switch (rtm)
		{
		case RTM_Artist:
			tf.ArtistID.val = rec->track.artID;
			tf.ArtistID.match = NUMM_Exact;
			break;
		case RTM_Album:
			tf.AlbumID.val = rec->track.albID;
			tf.AlbumID.match = NUMM_Exact;
			break;
		case RTM_Genre:
			tf.GenreID.val = rec->track.genID;
			tf.GenreID.match = NUMM_Exact;
			break;
		case RTM_Year:
			tf.Year.val = rec->track.year;
			tf.Year.match = NUMM_Exact;
			break;
		case RTM_Collection:
			tf.CollectionID.val = rec->track.colID;
			tf.CollectionID.match = NUMM_Exact;
			break;
		}
	}
	if (rtm == RTM_Newest)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		DOUBLE dtime;
		SystemTimeToVariantTime(&st, &dtime);
		dtime -= PRGAPI()->GetOption(OPT_GEN_DaysMeansNew);
		VariantTimeToSystemTime(dtime, &st);
		tf.DateAdded.val = st;
		tf.DateAdded.match = DATM_After;
	}
}

BOOL ActionManager::DefaultPlayerAction(MediaPlayer* pPlayer, FullTrackRecordCollection& col)
{
	switch ((MediaPlayerActionsEnum)PRGAPI()->GetOption(OPT_PL_DefaultAction))
	{
	case MPA_Enqueue:
		return Enqueue(pPlayer, col);
		break;
	case MPA_EnqueueAfter:
		return EnqueueAfter(pPlayer, col);
		break;
	case MPA_Play:
	default:
		break;
	}
	return Play(pPlayer, col);
}



BOOL ActionManager::Play(MediaPlayer* pPlayer, FullTrackRecordCollection& col)
{
	TRACEST(_T("ActionManager::Play."));
	ASSERT(pPlayer != NULL);
	if (col.size() == 1)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		PRGAPI()->GetSQLManager()->LogTrackInHistory(
			col[0]->artist.name.c_str(), col[0]->track.name.c_str(), HLA_Clicked, st);
	}
	pPlayer->ClearPlayList();
	BOOL bOK = Enqueue(pPlayer, col);
	if (bOK)
	{
		pPlayer->SetPlayListPos(0);
		pPlayer->Start();
		return TRUE;
	}
	return FALSE;
}

BOOL ActionManager::Enqueue(MediaPlayer* pPlayer, FullTrackRecordCollection& col)
{
	ASSERT(pPlayer != NULL);
	return InsertMedia(pPlayer, col, pPlayer->GetPlayListCount());
}

BOOL ActionManager::EnqueueAfter(MediaPlayer* pPlayer, FullTrackRecordCollection& col)
{
	ASSERT(pPlayer != NULL);
	return InsertMedia(pPlayer, col, pPlayer->GetPlayListPos() + 1);
}

BOOL ActionManager::InsertMedia(MediaPlayer* pPlayer, FullTrackRecordCollection& col, INT Pos)
{
	TRACEST(_T("ActionManager::InsertMedia."));
	ASSERT(pPlayer != NULL && !col.empty());
	if (col.empty())			return FALSE;
	ASSERT(Pos >= 0 && Pos <= (INT)pPlayer->GetPlayListCount());
	INT tracksInserted = 0;
	FullTrackRecordCollection::const_iterator it = col.begin();
	for (;it!=col.end();it++)
	{
		const FullTrackRecordSP& rec = *it;
		MediaPlayListItem mpli;
		mpli.url = rec->track.location.c_str();
		mpli.artist = rec->artist.name.c_str();
		mpli.rating = rec->track.rating;
		mpli.length = rec->track.duration;
		mpli.title = rec->track.name.c_str();
		pPlayer->InsertMedia(mpli, Pos);
		Pos++;
		tracksInserted++;
	}
	if (tracksInserted > 0)
		PRGAPI()->SendMessage(SM_PlayListChanged);
	return tracksInserted > 0;
}

BOOL ActionManager::GetFullTrackRecordByLocation(FullTrackRecordSP& rec, LPCTSTR location)
{
	TRACEST(_T("ActionManager::GetFullTrackRecordByLocation."));
	//Search in the Database
	if (PRGAPI()->GetSQLManager()->GetFullTrackRecordByLocation(rec, location))
	{
		return TRUE;
	}
	return FALSE;
	//Use FileTagger

	//Add It to the database if it is in Local Computer
}


BOOL ActionManager::ExternalOpen(FullTrackRecordCollection& col, LPCTSTR operation)
{
	TRACEST(_T("ActionManager::ExternalOpen."));
	//ASSERT(col.size() == 1);//I do not know yet how to play/enqueue multiple files. 
	if (col.size() < 1)
		return FALSE;
	FullTrackRecordCollection::const_iterator it = col.begin();
	for (; it != col.end(); it++)
	{
		LPCTSTR path = (*it)->track.location.c_str();
		if ((*it)->collection.type == CTYPE_Media || (*it)->collection.type == CTYPE_Removable)
		{
			TCHAR bf[MAX_PATH];
			if (FindAndFixMountedPath(path, bf, MAX_PATH))
				ShellExecute(0, operation != NULL ? operation : _T(""), bf, 0,0,0);
		}
		else 
			ShellExecute(0, operation != NULL ? operation : _T(""), path, 0,0,0);
	}
	return TRUE;
}





BOOL ActionManager::InsertPlayList(MediaPlayer* pPlayer, INT AddToPosition /*=-1*/)
{
	TRACEST(_T("ActionManager::InsertPlayList."));
	CFileDialog fd(TRUE, _T("m3u"), NULL, NULL, _T("m3u Files (*.m3u)|*.m3u|"), AfxGetMainWnd());
	fd.m_ofn.Flags |= OFN_EXPLORER;
	if (fd.DoModal() == IDOK)
	{
		if (AddToPosition == -1)
		{
			AddToPosition = 0;
			pPlayer->ClearPlayList();
		}
		INT added = MediaPlayerUtilities::InsertM3U(*pPlayer, fd.GetPathName(), AddToPosition);
		if (added > 0)
		{
			PRGAPI()->SendMessage(SM_PlayListChanged);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL ActionManager::SavePlayList(MediaPlayer* pPlayer)
{
	TRACEST(_T("ActionManager::SavePlayList."));
	CFileDialog fd(FALSE, _T("m3u"), NULL, NULL, _T("m3u Files (*.m3u)|*.m3u|"), AfxGetMainWnd());
	fd.m_ofn.Flags |= OFN_EXPLORER;
	if (fd.DoModal() == IDOK)
		return MediaPlayerUtilities::SaveM3U(*pPlayer, fd.GetPathName());
	return FALSE;
}


BOOL ActionManager::GetToolTipText(LPCTSTR path, CString& tt)
{
	TRACEST(_T("ActionManager::GetToolTipText."));
	ASSERT(path != NULL);
	PrgAPI* pAPI= PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	FullTrackRecordSP rec;
	if (!pSM->GetFullTrackRecordByLocation(rec, path))
		return FALSE;
	LPCTSTR newLine = _T("<br>");
	LPCTSTR horLine = _T("<hr><br>");

	TCHAR line[400];
	//Track
	_sntprintf(line, 400, _T("<b>%s</b>: %s"), pAPI->GetString(IDS_TITLE), rec->track.name.c_str());
	tt += line;
	if (rec->track.rating > 0)
	{
		INT stars = (INT)(Rating2Stars(rec->track.rating));
		_sntprintf(line, 400, _T(" (%s)"), pAPI->GetStringForStars(stars));
		tt += line;	
	}
	tt += newLine;

	tt += horLine;
	//Album
	_sntprintf(line, 400, _T("<b>%s</b>: %s"), pAPI->GetString(IDS_ALBUM), rec->album.name.c_str());
	tt += line;	
	if (rec->album.rating > 0)
	{
		INT stars = (INT)Rating2Stars(rec->album.rating);
		_sntprintf(line, 400, _T(" (%s)"), pAPI->GetStringForStars(stars));
		tt += line;	
	}
	tt += newLine;
	//Artist
	_sntprintf(line, 400, _T("<b>%s</b>: %s"), pAPI->GetString(IDS_ARTIST), rec->artist.name.c_str());
	tt += line;	
	if (rec->artist.rating > 0)
	{
		INT stars = (INT)Rating2Stars(rec->artist.rating);
		_sntprintf(line, 400, _T(" (%s)"), pAPI->GetStringForStars(stars));
		tt += line;	
	}
	tt += newLine;
	//Track Info
	_sntprintf(line, 400, _T("<b>%s</b>: %s"), pAPI->GetString(IDS_GENRE), rec->genre.name.c_str());
	tt += line;	tt += newLine;

	_sntprintf(line, 400, _T("<b>%s</b>: %s"), pAPI->GetString(IDS_LOCATION), rec->track.location.c_str());
	tt += line;	tt += newLine;

	_sntprintf(line, 400, _T("<b>%s</b>: %s"), pAPI->GetString(IDS_COLLECTION), rec->collection.name.c_str());
	tt += line;	tt += newLine;


	tt += horLine;

	//Statistics
	SQLManager::HistoryTrackStats totalStats;
	SQLManager::HistoryTrackStats monthStats;

	TCHAR sTotalRank[20], sMonthRank[20], sAddedDate[100], sFTDate[100], sLDate[100];
	PrgAPI::FormatDate(sAddedDate, 100, rec->track.dateAdded);
	//SYSTEMTIME ft, lt;
	pSM->GetHistoryTrackStats(rec->artist.name.c_str(), rec->track.name.c_str(), NULL, NULL, totalStats);
	if (totalStats.hits > 0)
	{
		_itot(totalStats.rank, sTotalRank, 10);
		PrgAPI::FormatDate(sFTDate, 100, totalStats.firstTime);
		PrgAPI::FormatDate(sLDate, 100, totalStats.lastTime);
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);
		DOUBLE dCurTime;
		SystemTimeToVariantTime(&curTime, &dCurTime);
		dCurTime -= 30;
		VariantTimeToSystemTime(dCurTime, &curTime);
		pSM->GetHistoryTrackStats(rec->artist.name.c_str(), rec->track.name.c_str(), 
			&curTime, NULL, monthStats);
		if (monthStats.hits > 0)
			_itot(monthStats.rank, sMonthRank, 10);
		else
			_tcsncpy(sMonthRank, _T("-"), 20);
	}
	else
	{
		_tcscpy(sTotalRank, _T("-"));
		_tcscpy(sFTDate, _T("-"));
		_tcscpy(sLDate, _T("-"));
		_tcscpy(sMonthRank, _T("-"));
	}

	_sntprintf(line, 400, _T("<b>%s (%s)</b>: %s (%s: %d)"), 
		pAPI->GetString(IDS_RANK), 
		pAPI->GetString(IDS_TOTAL),
		sTotalRank,
		pAPI->GetString(IDS_HITS),
		totalStats.hits
		);
	tt += line;	tt += newLine;

	//CChangeCase p(pAPI->GetString(IDS_MONTH), CCT_Lower);

	_sntprintf(line, 400, _T("<b>%s (%s)</b>: %s (%s: %d)"), 
		pAPI->GetString(IDS_RANK), 
		(LPCTSTR)CChangeCase(pAPI->GetString(IDS_MONTH), CCT_Lower),
		sMonthRank,
		pAPI->GetString(IDS_HITS),
		monthStats.hits
		);
	tt += line;	tt += newLine;

	tt += horLine;

	_sntprintf(line, 400, _T("<b>%s</b>: %s"), 
		pAPI->GetString(IDS_DATEADDED), 
		sAddedDate);
	tt += line;	tt += newLine;

	_sntprintf(line, 400, _T("<b>%s</b>: %s"), 
		pAPI->GetString(IDS_FIRSTAPPEARANCE), 
		sFTDate);
	tt += line;	tt += newLine;

	_sntprintf(line, 400, _T("<b>%s</b>: %s"), 
		pAPI->GetString(IDS_LASTAPPEARANCE), 
		sLDate);
	tt += line;	tt += newLine;

	return TRUE;
}

void ActionManager::ShowMainWindow()
{
	if (AfxGetMainWnd()->IsWindowVisible())
		AfxGetMainWnd()->SetForegroundWindow();
	else
		AfxGetMainWnd()->PostMessage(WM_SYSCOMMAND, SC_RESTORE);
}


//void ActionManager::ShowMiniPlayer(BOOL bShow)
//{
//	if (bShow)
//	{
//		PrgAPI* pAPI = PRGAPI();
//		if (m_pMiniPlayerDlg == NULL)
//		{
//			m_pMiniPlayerDlg = new CMiniPlayerDlg;
//			m_pMiniPlayerDlg->Create(CWnd::GetDesktopWindow());
//			m_pMiniPlayerDlg->SetTransparency((BYTE) pAPI->GetOption(OPT_MINIPLR_Transparency));
//		}
//		if (!m_pMiniPlayerDlg->IsWindowVisible())
//			m_pMiniPlayerDlg->ShowWindow(SW_SHOW);
//		SetMiniPlayerOptions();
//	}
//	else
//	{
//		if (m_pMiniPlayerDlg == NULL)
//			return;
//		m_pMiniPlayerDlg->ShowWindow(SW_HIDE);
//	}
//}

//void ActionManager::DestroyMiniPlayer()
//{
//	if (m_pMiniPlayerDlg != NULL)
//	{
//		GetMiniPlayerOptions();
//		m_pMiniPlayerDlg->DestroyWindow();
//		delete m_pMiniPlayerDlg;
//		m_pMiniPlayerDlg = NULL;
//	}
//}

//void ActionManager::DestroyAdvancedSearchDlg()
//{
//	if (m_pSearchDlg != NULL)
//	{
//		m_pSearchDlg->DestroyWindow();
//		delete m_pSearchDlg;
//		m_pSearchDlg = NULL;
//	}
//}

//void ActionManager::GetMiniPlayerOptions()
//{
//	if (m_pMiniPlayerDlg != NULL)
//	{
//		PrgAPI* pAPI = PRGAPI();
//		pAPI->SetOption(OPT_MINIPLR_Transparency, m_pMiniPlayerDlg->GetTransparency());
//		pAPI->SetOption(OPT_MINIPLR_ShowTooltips, m_pMiniPlayerDlg->IsToolTipsEnabled());
//		pAPI->SetOption(OPT_MINIPLR_ShowExtraInfo, m_pMiniPlayerDlg->IsExtraInfoEnabled());
//		pAPI->SetOption(OPT_MINIPLR_MaxSize, m_pMiniPlayerDlg->GetMaxSize());
//		pAPI->SetOption(OPT_MINIPLR_ForceOnTop, m_pMiniPlayerDlg->GetForceOnTop());
//		pAPI->SetOption(OPT_MINIPLR_ForceOnTopDelay, m_pMiniPlayerDlg->GetForceOnTopDelay());
//		pAPI->SetOption(OPT_MINIPLR_DisableTransparencyOnMouseOver, m_pMiniPlayerDlg->GetDisableTransparencyOnMouseOver());
//	}
//}
//
//void ActionManager::SetMiniPlayerOptions()
//{
//	if (m_pMiniPlayerDlg != NULL)
//	{
//		PrgAPI* pAPI = PRGAPI();
//		m_pMiniPlayerDlg->SetTransparency(pAPI->GetOption(OPT_MINIPLR_Transparency));
//		m_pMiniPlayerDlg->EnableTooltips(pAPI->GetOption(OPT_MINIPLR_ShowTooltips));
//		m_pMiniPlayerDlg->EnableExtraInfo(pAPI->GetOption(OPT_MINIPLR_ShowExtraInfo));
//		m_pMiniPlayerDlg->SetMaxSize(pAPI->GetOption(OPT_MINIPLR_MaxSize));
//		m_pMiniPlayerDlg->SetForceOnTop(pAPI->GetOption(OPT_MINIPLR_ForceOnTop));
//		m_pMiniPlayerDlg->SetForceOnTopDelay(pAPI->GetOption(OPT_MINIPLR_ForceOnTopDelay));
//		m_pMiniPlayerDlg->SetDisableTransparencyOnMouseOver(pAPI->GetOption(OPT_MINIPLR_DisableTransparencyOnMouseOver));
//	}
//}


//BOOL ActionManager::IsMiniPlayerVisible()
//{
//	if (m_pMiniPlayerDlg == NULL)
//		return FALSE;
//	return m_pMiniPlayerDlg->IsWindowVisible();
//}

//void ActionManager::UpdateMiniPlayerOptions(BOOL bShowIfManual)
//{
//	TRACEST(_T("ActionManager::UpdateMiniPlayerOptions."));
//	BOOL bShowMiniPlayer = FALSE;
//	PrgAPI* pAPI = PRGAPI();
//
//	switch (PRGAPI()->GetOption(OPT_MINIPLR_Behaviour))
//	{
//	case MPB_Automatic://Show On Minimize
//		{
//			BOOL bMainWindowIsVisible = AfxGetMainWnd()->IsWindowVisible();
//			if (bMainWindowIsVisible)
//			{
//				//Check if it is minimized
//				WINDOWPLACEMENT wpl;
//				wpl.length = sizeof(WINDOWPLACEMENT);
//				AfxGetMainWnd()->GetWindowPlacement(&wpl);
//				bMainWindowIsVisible = !(wpl.showCmd == SW_SHOWMINIMIZED);
//			}
//			bShowMiniPlayer = !bMainWindowIsVisible;
//		}
//		break;
//	case MPB_Always://Always Show
//		bShowMiniPlayer = TRUE;
//		break;
//	case MPB_Never://NeverShow
//		bShowMiniPlayer = FALSE;
//		break;
//	case MPB_Manually:
//		bShowMiniPlayer = bShowIfManual;
//		break;
//	default:
//		ASSERT(0);
//		break;
//	}
//	if (m_pMiniPlayerDlg == NULL && bShowMiniPlayer ==FALSE)
//		return;
//	ShowMiniPlayer(bShowMiniPlayer);
//
//}

//===ShowImportVirtualTracksDlg
//-pRec: The collection record that will be merged. Can only be links collection
//			If it is null it will create a new collection based on the filename

//BOOL ActionManager::ShowImportCollectionDlg(FullTrackRecordCollection& rec, CWnd* pParent)
//{
//
//	CFileDialog fd(FALSE, _T(".xml"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
//		_T("Track Collection Files (*.xtc)|*.xtc|Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
//	if (fd.DoModal() != IDOK)
//		return TRUE;//- No Error. User interupted the process
//	FullTrackRecordCollection col;
//	if (!DataRecordsXmlSerializer::Import(fd.GetPathName(), col))
//		return FALSE;//There was an error
//	CTracksSerializerDlg serDlg;
//	serDlg.SetMode(TRUE);
//	serDlg.SetFullTrackRecordCollection(col);
//	if (serDlg.DoModal() == IDOK)
//	{
//		SQLManager* pSM = PRGAPI()->GetSQLManager();
//		CollectionRecord cr;
//		if (pRec == NULL)
//		{
//			cr.location = fd.GetPathName();
//			cr.name = fd.GetFileTitle();
//			cr.serial = 0;
//			cr.type = CTYPE_Virtual;
//			if (!pSM->AddNewCollectionRecord(cr))
//			{
//				ASSERT(0);
//				TRACE(_T("ActionManager::ShowImportVirtualTracksDlg. Cannot add collection\r\n"))
//				return FALSE;
//			}
//			pRec = &cr;
//		}
//		TagInfo ti;
//		INT tracksAdded = 0;
//		for (int i = 0; i < col.size(); i++)
//		{
//			if (serDlg.IsItemChecked(i) == TRUE)
//			{
//				FullTrackRecord& rec = col[i];
//				ti.Album = rec.album.name;
//				ti.Artist = rec.artist.name;
//				ti.Bitrate = rec.track.bitrate;
//				ti.Genre = rec.genre.name;
//				ti.Length = rec.track.duration;
//				ti.Rating = rec.track.rating;
//				ti.Size = rec.track.size;
//				ti.Title = rec.track.name;
//				ti.TrackNo = rec.track.trackNo;
//				ti.Year = rec.track.year;
//				if (pSM->AddNewTrackFromTagInfo(pRec->ID, rec.track.location.c_str(), ti))
//					tracksAdded++;
//				else
//				{
//					TRACE(_T("ActionManager::ShowImportVirtualTracksDlg. Cannot add\r\n"))
//						ASSERT(0);
//
//				}
//			}
//		}
//		if (tracksAdded > 0)
//			PRGAPI()->SendMessage(SM_CollectionManagerEvent);
//	}
//	return TRUE;//- No Error. User interupted the process
//
//
//
//
//
//	DatabaseManager* pDB = PRGAPI()->GetDatabaseManager();
//
//
//	TagInfo* ti = NULL;
//	LPCTSTR url = NULL;
//	BOOL bActive = FALSE;
//	CXmlDocumentWrapper xml;
//	if (xml.Load(fDlg.GetPathName()))
//	{
//		CXmlNodeWrapper root(xml.AsNode());
//		CString a = root.GetText();
//		a = root.Name();
//		if (root.IsValid() && root.Name() == xmlVTExport)
//		{
//			CXmlNodeWrapper trackNode(root.GetNode(xmlTrack));
//			TagInfo ti;
//			while (trackNode.IsValid())
//			{
//				CXmlNodeWrapper val(trackNode.GetNode(xmlArtist));
//				ti.Artist = val.GetText();
//				val = trackNode.GetNode(xmlAlbum);
//				ti.Album = val.GetText();
//				val = trackNode.GetNode(xmlTitle);
//				ti.Title = val.GetText();
//				val = trackNode.GetNode(xmlDuration);
//				ti.Length = _ttoi(val.GetText());
//				val = trackNode.GetNode(xmlGenre);
//				ti.Genre = val.GetText();
//				val = trackNode.GetNode(xmlUrl);
//				serDlg.AddTrack(ti, val.GetText());
//				trackNode = trackNode.GetNextSibling();
//			}
//		}
//	}
//	if (serDlg.DoModal() == IDOK)
//	{
//		TagInfo* ti = NULL;
//		LPCTSTR url = NULL;
//		BOOL bActive = FALSE;
//		DatabaseManager* pDB = PRGAPI()->GetDatabaseManager();
//		UINT colID = pDB->GetVirtualCollectionID();
//		for (UINT i = 0; i < serDlg.GetCount(); i++)
//		{
//			serDlg.GetTrack(i, ti, url, bActive);
//			if (bActive)
//			{
//				if (pDB->GetTrackID(url,colID, CTYPE_Virtual))
//					PRGAPI()->NotifyUser(IDS_ITEMALREADYEXISTS);
//				else
//					pDB->AddNewTrack(*ti, url, colID, CTYPE_Virtual, TRUE);
//			}
//		}
//		PRGAPI()->SendMessage(ActorMessage(SM_CollectionUpdateFinished));
//	}
//
//	return FALSE;
//
//}

//BOOL ActionManager::ShowExportVirtualTracksDlg(CWnd* pParent)
//{
//	ASSERT(0);//TODO
//	return FALSE;
	//CTracksSerializerDlg serDlg;
	//serDlg.SetMode(FALSE);
	//DatabaseManager* pDB = PRGAPI()->GetDatabaseManager();
	//TracksQuery tq;
	//if (tq.OpenByCollection(pDB->GetVirtualCollectionID(TRUE)))
	//{
	//	TagInfo ti;
	//	while (!tq.IsEOF())
	//	{
	//		pDB->GetTagInfoFromTrackQuery(ti, tq);
	//		TCHAR bf[MAX_PATH];
	//		tq.GetLocation(bf);
	//		serDlg.AddTrack(ti, bf);
	//		tq.MoveNext();
	//	}
	//}
	//if (serDlg.DoModal() == IDOK)
	//{
	//	TagInfo* ti = NULL;
	//	LPCTSTR url = NULL;
	//	BOOL bActive = FALSE;
	//	CXmlDocumentWrapper xml;
	//	TCHAR bf[200];
	//	_sntprintf(bf, 200, _T("<%s></%s>"), xmlVTExport, xmlVTExport);
	//	xml.LoadXML(bf);
	//	CXmlNodeWrapper root = (xml.AsNode());
	//	UINT additions = 0;
	//	for (UINT i = 0; i < serDlg.GetCount(); i++)
	//	{
	//		serDlg.GetTrack(i, ti, url, bActive);
	//		if (bActive)
	//		{
	//			CXmlNodeWrapper trackNode(root.InsertNode(additions, xmlTrack));
	//			CXmlNodeWrapper node(trackNode.InsertNode(0, xmlUrl));
	//			node.SetText(url);
	//			node = trackNode.InsertNode(0, xmlArtist);
	//			node.SetText(ti->Artist.c_str());
	//			node = trackNode.InsertNode(1, xmlAlbum);
	//			node.SetText(ti->Album.c_str());
	//			node = trackNode.InsertNode(2, xmlTitle);
	//			node.SetText(ti->Title.c_str());
	//			node = trackNode.InsertNode(3, xmlGenre);
	//			node.SetText(ti->Genre.c_str());
	//			node = trackNode.InsertNode(3, xmlDuration);
	//			TCHAR bf[20];
	//			_itot(ti->Length, bf, 10);
	//			node.SetText(bf);

	//			//CXmlNodeWrapper artist(trackNode.InsertNode(1, _T("artist")));
	//			//artist.SetText( ti->Artist.c_str());
	//			//CXmlNodeWrapper album(trackNode.InsertNode(2, _T("album")));
	//			//album.SetText(ti->Album.c_str());
	//			//CXmlNodeWrapper title(trackNode.InsertNode(3, _T("title")));
	//			//title.SetText(ti->Title.c_str());
	//			additions++;
	//		}
	//	}
	//	if (additions > 0)
	//	{
	//		CFileDialog fDlg(bImport, _T("xlc"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("xml link collection (*.xlc)|*.xlc|All Files (*.*)|*.*|"), pParent);
	//		if (fDlg.DoModal() != IDOK)
	//			return FALSE;
	//		return xml.Save(fDlg.GetPathName());
	//	}
	//}
	//return FALSE;
//
//}


BOOL ActionManager::ShowAddNewCollectionDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowAddNewCollectionDlg."));
	PrgAPI* pAPI = PRGAPI();
#ifdef _DEBUG
	//pAPI->GetString(IDS_SELECTFOLDER);
	CXFolderDialog dlg(_T("d:\\Musi0\\90s"), 0, pParent);
#else
	CXFolderDialog dlg(0, 0, pParent);
#endif
	LocalizeXFolderDialog(dlg);
	if (dlg.DoModal() == IDOK)
	{
		if (pAPI->GetCollectionManager()->AddCollection(dlg.GetPath(), dlg.GetPath()))
			return TRUE;
		pAPI->MessageBox(pParent->m_hWnd, IDS_COLLECTION_EXIST);
	}
	return FALSE;
}

BOOL ActionManager::ShowRenameCollectionDlg(CollectionRecord& cr, CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowRenameCollectionDlg."));
	PrgAPI* pAPI = PRGAPI();
	CInputBox inpBox(pParent);
	inpBox.SetTexts(pAPI->GetString(IDS_RENAME), pAPI->GetString(IDS_PLEASEENTERANEWNAME), cr.name.c_str());
	inpBox.SetButtonTexts(_T(""), pAPI->GetString(IDS_CANCEL_S));
	if (inpBox.DoModal() == IDOK)
	{
		cr.name = inpBox.GetInputString();
		pAPI->GetSQLManager()->UpdateCollectionRecord(cr);
		return TRUE;
	}
	return FALSE;
}




BOOL ActionManager::ShowHistoryDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowHistoryDlg."));
	CHistoryDlg dlg(pParent);
	dlg.DoModal();
	return TRUE;

}
//BOOL ActionManager::ShowAdvancedSearchDlg(CWnd* pParent)
//{
//	TRACEST(_T("ActionManager::ShowAdvancedSearchDlg."));
//	if (m_pSearchDlg == NULL)
//	{
//		m_pSearchDlg = new CSearchDlg;
//		m_pSearchDlg->Create(IDD_SEARCH, pParent);
//		m_pSearchDlg->SetWindowText(PRGAPI()->GetString(IDS_SEARCH));
//		m_pSearchDlg->CenterWindow(pParent);
//	}				
//	m_pSearchDlg->ShowWindow(SW_SHOW);
//	return TRUE;
//}

BOOL ActionManager::ShowLanguagesDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowLanguagesDlg."));
	CPropertySheet sht(PRGAPI()->GetString(IDS_LANGUAGE), pParent);
	CLanguagesPP languages(IDS_LANGUAGE);
	sht.AddPage(&languages);
	sht.DoModal();
	return TRUE;
}
BOOL ActionManager::ShowSkinsDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowSkinsDlg."));
	CPropertySheet sht(PRGAPI()->GetString(IDS_SKIN), pParent);
	CSkinsPP languages(IDS_SKIN);
	sht.AddPage(&languages);
	sht.DoModal();
	return TRUE;

}


BOOL ActionManager::ShowOptionsDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowOptionsDlg."));
	PrgAPI* pAPI = PRGAPI();
	COptionsSheet sht(IDS_OPTIONS, pParent, 0);

	CMainOptionsPage main(IDS_MAINOPTIONS);
	sht.AddTSPropertyPage(&main);

	CMiniPlayerPP minipl(IDS_MINIPLAYER);
	sht.AddTSPropertyPage(&minipl);

	//CTrackListPP trackList(IDS_TRACKLIST);
	//sht.AddTSPropertyPage(&trackList);

	//CPlayListPP playList(IDS_PLAYLIST);
	//sht.AddTSPropertyPage(&playList);
	//
	//CInfoControlPP infoctrl(IDS_SPECIALINFO);
	//sht.AddTSPropertyPage(&infoctrl);


	CInfoRetrievalPP info(IDS_INFORMATIONRETRIEVAL);
	sht.AddTSPropertyPage(&info);

	INT ipIdx = 0;
	InfoProviderFactory* pIPF = pAPI->GetInfoProviderFactory();
	IInfoProvider* pIP = pIPF->GetItemByIndex(ipIdx);
	std::vector<CGenericInfoProviderPP*> ipPPs;
	while (pIP != NULL)
	{
		IConfigurable::SettingInfo setting;
		if (pIP->GetSettingInfo(0, setting))
		{
			CGenericInfoProviderPP* pIPPP = new CGenericInfoProviderPP(*pIP);
			pIPPP->SetPPParent(&info);
			ipPPs.push_back(pIPPP);
			sht.AddTSPropertyPage(pIPPP);
		}
		ipIdx++;
		pIP = pIPF->GetItemByIndex(ipIdx);
	}


	CMediaPlayerPP mplayer(IDS_MEDIAPLAYER);
	mplayer.SetObjectsToConfigure(PRGAPI()->GetMediaPlayer());
	sht.AddTSPropertyPage(&mplayer);

	CGlobalHotkeysPP globalHotkeys(IDS_GLOBALHOTKEYS);
	sht.AddTSPropertyPage(&globalHotkeys);

	CTaggingPP tagging(IDS_TAGGING);
	sht.AddTSPropertyPage(&tagging);

	//CColorsPP colors;

	CLanguagesPP languages(IDS_LANGUAGE);
	sht.AddTSPropertyPage(&languages);

	CSkinsPP skins(IDS_SKIN);
	sht.AddTSPropertyPage(&skins);

	if (sht.DoModal() == IDOK)
	{
		pAPI->SaveSettings();
	}
	for (size_t i = 0; i < ipPPs.size(); i++)
		delete ipPPs[i];

	return TRUE;
}
BOOL ActionManager::ShowCollectionsDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowCollectionsDlg."));
	CCollectionsDlg dlg(pParent);
	dlg.DoModal();
	return TRUE;
}
BOOL ActionManager::ShowGamesDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowGamesDlg."));
	BOOL songPaused = FALSE;
	MediaPlayer* tsp = PRGAPI()->GetMediaPlayer();
	if (tsp->GetPlayState() == PS_Playing)
	{
		tsp->Pause();
		songPaused = TRUE;
	}
	CQuizRunningDlg quiz(pParent);
	//quizRunning.SetParameters();
	quiz.DoModal();
	if (songPaused)
		tsp->Start();
	return TRUE;

}
BOOL ActionManager::ShowAboutDlg(CWnd* pParent)
{
	TRACEST(_T("ActionManager::ShowAboutDlg."));
	CAboutDlg dlg(pParent);
	dlg.DoModal();
	return TRUE;
}

void ActionManager::CheckForUpdates(CWnd* pParent)
{
	PrgAPI* pAPI = PRGAPI();
	CWaitCursor w;
	std::string sysID;
	NewVersionChecker::CheckResult res;
	if (pAPI->GetURLSystemID(sysID))
		res = NewVersionChecker::CheckForNewVersion(pAPI->GetInternetHandle(), sUpdateURL, (LPCTSTR)CA2CT(sysID.c_str()));
	else
		res.error = NewVersionChecker::ERR_Failed;
	w.Restore();
	HWND parent = 0;
	if (pParent != NULL)
		parent = pParent->m_hWnd;
	if (res.error == NewVersionChecker::ERR_OK)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		DOUBLE thisDay;
		SystemTimeToVariantTime(&st, &thisDay);
		pAPI->SetOption(OPT_GEN_CheckLastCheck, INT(thisDay));
		FileVersion curVer;
		GetFileVersion(curVer);
		if (res.stableVersion > curVer)
		{
			if (pAPI->MessageBox(parent,IDS_UPDATEAVAILABLE, NULL, MB_YESNO| MB_ICONQUESTION) == IDYES)
				ShellExecute(0, _T("open"), res.infoURL.c_str(), 0,0,0);
		}
		else
			pAPI->MessageBox(parent, IDS_UPDATENOTAVAILABLE, NULL, MB_OK | MB_ICONWARNING);
	}
	else
	{
		pAPI->MessageBox(parent, IDS_UPDATECHECKFAILED, NULL, MB_OK | MB_ICONWARNING);
	}
}

void ActionManager::RandomizePlayListItems(MediaPlayer& mediaPlayer, std::vector<INT>& selItems)
{
	if (selItems.size() < 2)
	{
		return;
	}
	else if (selItems.size() == 2)
	{
		mediaPlayer.SwapPlayListItems(selItems[0], selItems[1]);
	}
	else
	{
		UINT numItems = selItems.size();
		UINT swapItem = 0;
		for (UINT i = 0; i < numItems; i++)
		{
			swapItem = m_rndGenerator() % numItems;
			if (swapItem != i)
			{
				mediaPlayer.SwapPlayListItems(selItems[swapItem], selItems[i]);
			}
		}	
	}
}





