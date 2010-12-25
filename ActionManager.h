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
#ifndef _ActionManager_h_
#define _ActionManager_h_

class CMainTreeCtrl;
class CMainListCtrl;
class MediaPlayer;
struct MediaInfo;
//#include "CollectionsTableSchema.h" //For Collection Types
#include "TeenSpiritEnums.h"
#include "TagInfo.h"
#include <list>
#include "MessageViewer.h"
#include "PrgAPI.h"
#include "DataRecords.h"
#include "TracksFilter.h"
#include <boost/random/linear_congruential.hpp>

//class FullTrackRecordCollection;
class CMiniPlayerDlg;
class QuickSearchDlg;
class FullTracksPlayList;
class CXFolderDialog;

class ActionManager
{
public:
	ActionManager();
	~ActionManager();

public:
	//Simplifies Tag Tracking
	//JOBS: 
	//	1. Checks the tagging Options
	//	2. Tags the file(s) (uses the tagger created by PrgAPI)
	//	3. Notifies Application
	//USED BY:
	//	1. MainTreeCtrl
	//	2. ActionManager (UpdateArtist - UpdateAlbum - After Changing Track Properties - SetTrackRating)
	BOOL TagTrack(LPCTSTR path, TagInfo& ti);
	//BOOL TagTracks(TracksQuery& tvs, TagInfo& ti, UINT TagInfoFlags, BOOL bAllTracks = TRUE);
	//Returns number of succeed Track modifications
	INT TagTracks(FullTrackRecordCollection& col, TagInfo& ti);

public:
	//Shows Common Dialogs
	//JOBS:
	//	1. Prepares and shows the dialogs
	//	2. Perform the actions that needed to execute the changes (Tagging - DB Update)
	//	3. Notifies for the changes
	//USED BY:
	//	1. Tree
	//	2. InfoControl
	//	3. List
	//BOOL AddVirtualTrackDialog(LPCTSTR path, TagInfo& ti, CWnd* pParent = NULL);

	//BOOL ShowItemProperties(ItemType it, UINT ID);

	BOOL ShowTrackPropertiesDlg(FullTrackRecordSP& rec, CWnd* pParent);
	BOOL ShowMultipleTrackPropertiesDlg(FullTrackRecordCollection& col, CWnd* pParent);
	BOOL ShowAlbumPropertiesDlg(FullAlbumRecordSP& rec, CWnd* pParent, BOOL bFull);
	BOOL ShowArtistPropertiesDlg(FullArtistRecordSP& rec, CWnd* pParent, BOOL bFull);

	BOOL ShowAddNewCollectionDlg(CWnd* pParent);
	BOOL ShowRenameCollectionDlg(CollectionRecord& cr, CWnd* pParent);

	//BOOL ShowImportCollectionDlg(FullTrackRecordCollection& rec, CWnd* pParent);
	//BOOL ShowExportCollectionDlg(FullTrackRecordCollection& rec, CWnd* pParent);

	BOOL ShowHistoryDlg(CWnd* pParent);
	//BOOL ShowAdvancedSearchDlg(CWnd* pParent);
	//BOOL DestroyAdvancedSearchDlg();
	BOOL ShowOptionsDlg(CWnd* pParent);
	BOOL ShowLanguagesDlg(CWnd* pParent);
	BOOL ShowSkinsDlg(CWnd* pParent);
	BOOL ShowCollectionsDlg(CWnd* pParent);
	BOOL ShowGamesDlg(CWnd* pParent);
	BOOL ShowAboutDlg(CWnd* pParent);

	void CheckForUpdates(CWnd* pParent);

private:
	//CSearchDlg* m_pSearchDlg;





public:
	//Rename Engine: Tags->FileName Filename->Tags
	//JOBS:
	//	1. Creates a new filename based on a pattern. (Does NOT rename the file)
	//USED BY:
	//	1. List
	//	2. ActionManager (RenameFiles)
	BOOL RenamerTranslatePattern(LPTSTR output, LPCTSTR pattern);
	BOOL RenamerGenerateName(LPTSTR newPath, LPCTSTR pattern, FullTrackRecordSP& track, BOOL relativePath = FALSE);
private:
	BOOL MakeMewPath(LPTSTR outPath, LPCTSTR newName, LPCTSTR originalPath);

public:
	//DynLink Files on CD/DVDs
	//returns TRUE if File id Found
	//pathname is modified if necessary to point to the right location (on CD/DVDs)
	//bNotifyUser: if its TRUE then it ll notify the user
	//USED BY:
	//	1. CMainListCtrl::OnContextMenu
	//	2. ActionManager (TagTracks - OpenFolderToItem - CopyPathNameToClipboard -
	//		CopyFilesToFolder - CopyFilesToClipboard - DeleteFiles - RenameFiles
	//	3. MediaPlayer::InsertMedia
	//BOOL PathFileExistsEx(LPTSTR pathName, CollectionTypesEnum cType = CTYPE_Unknown, BOOL bNotifyUser = FALSE);
	BOOL FindAndFixMountedPath(LPCTSTR originalFileName, LPTSTR realFileName, UINT bfLen);

	//returns 0 if it is not mounted. Drive Letter if it is.
	//USED BY:
	//	1. CCollectionsList::OnCommand
	//	2. CollectionManager::ProcessCollection
	TCHAR IsCollectionMounted(INT driveSerial);

public:
	//Open Explorer locating the item in tvs. If pos > 0 then the function will call setposition
	//otherwise it ll assume that it is already correctly positioned
	//USED BY: CMainListCtrl::OnCommand
	BOOL OpenFolderToItem(LPCTSTR fileName, CollectionTypesEnum type);
	//BOOL OpenFolderToItem(TracksQuery& tvs, UINT pos = 0);
	static BOOL OpenFolderToItem(LPCTSTR fileName);
	static BOOL OpenFolder(LPCTSTR dirName);

	//Returns size in bytes. -1 on error (limit 2Gb)
	static INT GetFileSize(LPCTSTR fileName);


	//USED BY: CMainListCtrl::OnCommand

	BOOL CopyPathNameToClipboard(LPCTSTR path, CollectionTypesEnum cType);
	static BOOL CopyStringToClipboard(LPCTSTR str);

	//USED BY: CMainListCtrl::OnCommand
	BOOL CopyFilesToFolder(FullTrackRecordCollection& col, LPCTSTR targetFolder = 0);

	//USED BY: CMainListCtrl::OnContextMenu (Reminds the last directory for continual copying)
	LPCTSTR GetLastTargetFolder(){return m_LastTargetFolder.c_str();}

	//USED BY: CMainListCtrl::OnCommand
	BOOL CopyFilesToClipboard(FullTrackRecordCollection& col);

	//USED BY: CMainListCtrl::OnCommand
	BOOL DeleteFiles(FullTrackRecordCollection& col);
	
	//USED BY: CMainListCtrl::OnCommand - OnNMClick
	BOOL SetTrackRating(FullTrackRecordCollection& col, INT Rating);
	
	//BOOL SetTrackRating(LPCTSTR path, INT Rating);

	//USED BY: CMainListCtrl::OnCommand
	BOOL RenameFiles(FullTrackRecordCollection& col, LPCTSTR sPattern);
	//BOOL RenameFiles(TracksQuery& tvs, const std::vector<UINT>& schemaPositions, LPCTSTR sPattern);


	//USED BY: CMainListCtrl::OnCommand
	BOOL ChangeCaseFiles(FullTrackRecordCollection& col, CaseStyleEnum cstyle);

	//USED BY: CMainListCtrl::OnCommand
	BOOL ReadTags(FullTrackRecordCollection& col);
	BOOL WriteTags(FullTrackRecordCollection& col);
	BOOL WriteTags(FullTrackRecord& rec);

private:
	void FullTrackRecord2TagInfo(FullTrackRecord& rec, TagInfo& ti);
public:


	//USED BY: CMainListCtrl::OnCommand
	//BOOL ShowTrackProperties(FullTrackRecordCollection& col, INT InitialPage = 0, CWnd* pParent = NULL);
	
	//USED BY: CMainListCtrl::ExecuteTracks
	//void ExecuteTracks(FullTrackRecordCollection& tracks, BOOL enqueue);

	//USED BY: 
	//	1. CMainTreeCtrl::OnCommand
	//	2. CPlayListListCtrl::OnCommand
	//void AddRandomTracks(RandomTrackModeEnum mode, UINT ID, UINT AvoidID = 0, UINT numTracks = 5, INT insertPos = -1);
	//Returns then number of tracks added
	//INT AddRandomTracks(MediaPlayer* pPlayer, TracksFilter& tf, UINT numTracks, INT insertPos = -1);
	////INT AddRelativeRandomTracks(MediaPlayer* pPlayer, RandomTrackModeEnum rtm, UINT trackCount, INT insertPos = -1);
	//INT AddRelativeRandomTracks(MediaPlayer* pPlayer, FullTrackRecordSP& rec, RandomTrackModeEnum rtm, UINT trackCount, INT insertPos = -1);

	BOOL GetRandomTrackCollection(FullTrackRecordCollection& col, TracksFilter& tf, UINT trackCount);
	//void GetRelativeRandomTrackCollection(FullTrackRecordCollection& col, FullTrackRecordSP& rec, RandomTrackModeEnum rtm, UINT desiredTrackCount);
	void GetRelativeRandomTrackFilter(TracksFilter& tf, FullTrackRecordSP& rec, RandomTrackModeEnum rtm);


	//JOBS:
	//	1. Checks the Player and gets the status
	//	2. Uses DatabaseManager to log history
	void TrackHistory();

	static BOOL DeleteFile(LPCTSTR sFile);
public:
	BOOL GoToArtistBuyPage(LPCTSTR artist);
	BOOL GoToAlbumBuyPage(LPCTSTR artist, LPCTSTR album);
	BOOL GoToTrackBuyPage(LPCTSTR artist, LPCTSTR title);

public:
	//User Logging
	void LogUserMessage(LPCTSTR msg, LPCTSTR title = NULL, SEVERITY sev = SEV_InfoL1);
	void ShowUsersLog();
private:
	//Set the file name or get a random (unique) filename
	BOOL GetTempFile(OUT LPTSTR pathName, IN UINT len = MAX_PATH, IN LPCTSTR fileName = NULL);
	CString GetHTMLStyle(LPCTSTR title);//Used By Export
	void RequestViewExport(LPCTSTR fileName);//Used By Export
	std::tstring m_LastTargetFolder;
	CCriticalSection m_LogCS;
	//std::list<std::tstring> m_LogList;
	#define MAX_LOG_LINES_SIZE 100
	std::tstring m_LogList[MAX_LOG_LINES_SIZE];
	UINT m_logListPos;
private:
	INT m_LogTrackStatus;
	DOUBLE m_LastTrackLength;

public://Global Manu Handling for Items
	//CMenu& ActionManager::GetMenu(ItemType it);
	//void OnMenuCommand(ItemType it, WPARAM wparam);

public:

	FullTracksPlayList* GetFullTracksList(MediaPlayer* pPlayer);
	//BOOL GetFullTrackRecordFromPlayer(FullTrackRecordSP& rec, MediaPlayer* pPlayer, INT Pos);
	BOOL GetFullTrackRecordByLocation(FullTrackRecordSP& rec, LPCTSTR location);


	BOOL DefaultPlayerAction(MediaPlayer* pPlayer, FullTrackRecordCollection& col);
	BOOL Play(MediaPlayer* pPlayer, FullTrackRecordCollection& col);
	BOOL Enqueue(MediaPlayer* pPlayer, FullTrackRecordCollection& col);
	BOOL EnqueueAfter(MediaPlayer* pPlayer, FullTrackRecordCollection& col);
	BOOL InsertMedia(MediaPlayer* pPlayer, FullTrackRecordCollection& col, INT Pos);

	//BOOL Play(MediaPlayer* pPlayer, LPCTSTR path);
	//BOOL Enqueue(MediaPlayer* pPlayer, LPCTSTR path);
	//BOOL InsertMedia(MediaPlayer* pPlayer, LPCTSTR path, INT pos);

	//BOOL Play(MediaPlayer* pPlayer, UINT TrackID);
	//BOOL Enqueue(MediaPlayer* pPlayer, UINT TrackID);
	//BOOL InsertMedia(MediaPlayer* pPlayer, UINT TrackID, INT Pos);

	//BOOL Play(MediaPlayer* pPlayer, TracksQuery& tvs, BOOL bOnlyCurrentRecord);
	//BOOL Enqueue(TracksQuery& tvs, BOOL bOnlyCurrentRecord);
	//BOOL InsertMedia(TracksQuery& tvs, INT Pos, BOOL bOnlyCurrentRecord);

	BOOL InsertPlayList(MediaPlayer* pPlayer, INT AddToPosition =-1);
	BOOL SavePlayList(MediaPlayer* pPlayer);
	//BOOL LoadPlayListFromFile(MediaPlayer* pPlayer, LPCTSTR FileName  = _T("LastTracks.m3u"), INT AddToPosition = -1);
	//BOOL SavePlayListToFile(MediaPlayer* pPlayer, LPCTSTR FileName = _T("LastTracks.m3u"));

	BOOL ExternalOpen(FullTrackRecordCollection& col, LPCTSTR operation);

	BOOL GetToolTipText(LPCTSTR path, CString& tt);
	//BOOL Play(LPCTSTR Path, LPCTSTR Title, LPCTSTR Artist = NULL, UINT Duration = NULL);
	//BOOL Enqueue(LPCTSTR Path, LPCTSTR Title, LPCTSTR Artist = NULL, UINT Duration = NULL);
	//BOOL InsertMedia(LPCTSTR Path, LPCTSTR Title, LPCTSTR Artist, UINT Duration, INT Pos);

	//void UpdateMiniPlayerOptions(BOOL bShowIfManual = FALSE);

	void ShowMainWindow();
	//void ShowMiniPlayer(BOOL bShow);

	//void DestroyMiniPlayer();
	//void DestroyAdvancedSearchDlg();

	//BOOL IsMiniPlayerVisible();
	//void GetMiniPlayerOptions();
	//void SetMiniPlayerOptions();

	void LocalizeXFolderDialog(CXFolderDialog& dlg);

	//BOOL ShowQuickSearch(LPCTSTR initialSearch, INT x, INT y, BOOL bDownwards);
	//void HideQuickSearch();

	void RandomizePlayListItems(MediaPlayer& mediaPlayer, std::vector<INT>& selItems);


private:
	//CMiniPlayerDlg* m_pMiniPlayerDlg;
	//QuickSearchDlg* m_pQuickSearchDlg;
	boost::rand48 m_rndGenerator;

};

#endif
