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
#include "CollectionUpdateJob.h"

#include "SQLManager.h"
#include "FileTagger.h"
#include "stlStringUtils.h"
#include "resource.h"
#include "PrgAPI.h"
#include "StateManager.h"

//=== ENABLE THIS FOR BENCHMARKS
//#define BENCMARK_COLLECTIONUPDATE

CollectionUpdateJob::~CollectionUpdateJob()
{
}

#ifdef	BENCMARK_COLLECTIONUPDATE
#include "ALog/cputicker.h"
CCPUTicker timer;
INT sTotalTrackCount = 0;
INT sAddNewTrackCount = 0;
INT sUpdateTrackCount = 0;
__int64 sTotalTime = 0;
__int64 sPureTime = 0;
__int64 sTrackUnique = 0;
__int64 sNotifications = 0;
__int64 sAddNewTime = 0;
__int64 sUpdateTime = 0;
__int64 sReadTime = 0;

#endif

void CollectionUpdateJob::Execute()
{
	m_bAbort = FALSE;
	m_state = ST_Running;
	m_progress = Progress();
	if (m_instructions.bAllowRemove)
	{
		m_progress.curAction = AC_RemovingInexistentFiles;
		m_statistics.FilesDeleted = ClearDeletedTracks(m_collection->ID, m_collection->location.c_str());
	}
	if (!m_bAbort && (m_instructions.bAllowAddNew || m_instructions.bAllowUpdate))
	{
#ifdef	BENCMARK_COLLECTIONUPDATE
		__int64 curTime = timer.Measure();
#endif
		m_progress.curAction = AC_CountingDirectoryFiles;
		m_progress.curPos = 0;
		Instructions pinfo = m_instructions;
		pinfo.bProcessFiles = FALSE;
		Statistics stats;
		RecurseDirectories(m_collection->ID, m_collection->location.c_str(), pinfo, stats);
		m_progress.maxPos = stats.FilesSupported;
		pinfo.bProcessFiles = TRUE;
		m_progress.curAction = CollectionUpdateJob::AC_AddingUpdatingFiles;
		RecurseDirectories(m_collection->ID, m_collection->location.c_str(), pinfo, m_statistics);
#ifdef	BENCMARK_COLLECTIONUPDATE
		sTotalTime = timer.Measure() - curTime;
		DOUBLE msFactor = 1000.0 / CCPUTicker::GetCachedCPUFrequency();
		TRACE(_T("BENCH: Total Time: %d (ms)\r\n"), INT(sTotalTime * msFactor));
		TRACE(_T("BENCH: Pure Time: %d (ms) %d (ms/track)\r\n"), INT(sPureTime * msFactor), (INT)((sPureTime * msFactor) / sTotalTrackCount));
		TRACE(_T("BENCH: TrackUniqueness: %d (ms) %d (ms/track)\r\n"), INT(sTrackUnique * msFactor), (INT)((sTrackUnique * msFactor) / sTotalTrackCount));
		TRACE(_T("BENCH: Notification: %d (ms) %d (ms/track)\r\n"), INT(sNotifications * msFactor), (INT)((sTrackUnique * msFactor) / sTotalTrackCount));
		TRACE(_T("BENCH: AddNew: %d (ms) %d (ms/track)\r\n"), INT(sAddNewTime * msFactor), (INT)((sAddNewTime * msFactor) / sAddNewTrackCount));
		if (sUpdateTrackCount > 0)
			TRACE(_T("BENCH: Update: %d (ms) %d (ms/track)\r\n"), INT(sUpdateTime * msFactor), (INT)((sUpdateTime * msFactor) / sUpdateTrackCount));
		TRACE(_T("BENCH: ReadTime: %d (ms) %d (ms/track)\r\n"), INT(sReadTime * msFactor), (INT)((sReadTime * msFactor) / sTotalTrackCount));
#endif
	}
	GetLocalTime(&m_collection->dateUpdated);
	m_sqlManager.UpdateCollectionRecord(*m_collection.get());

	m_progress = Progress();
	m_state = m_bAbort ? ST_Aborted : ST_Finished;
	m_bAbort = FALSE;

}


INT CollectionUpdateJob::ClearDeletedTracks(UINT collectionID, LPCTSTR startPath)
{
	PrgAPI* pAPI = PRGAPI();
	INT fileDeleted = 0;
	TrackRecordCollection col;
	TracksFilter tf;
	tf.CollectionID.match = NUMM_Exact;
	tf.CollectionID.val = collectionID;
	if (!m_sqlManager.GetTrackRecordCollectionByTracksFilter(col, tf))
		return -1;
	m_progress.maxPos = col.size();
	std::tstring msg = pAPI->GetString(IDS_REMOVE);
	for (int i=0; i < m_progress.maxPos; i++)
	{
		if (m_bAbort)	break;
		m_progress.curPos = i;
		if (!PathFileExists(col[i]->location.c_str()))
		{
			m_sqlManager.DeleteTrackRecord(col[i]->ID);
			TCHAR bf[500];
			_sntprintf(bf, 500, _T("%s: %s"), msg.c_str(), col[i]->location.c_str());
			pAPI->NotifyUser(bf, NULL, SEV_InfoL3);
			fileDeleted++;
			m_progress.updateActionsPerformed++;
		}	
	}
	return fileDeleted;
}

void CollectionUpdateJob::RecurseDirectories(UINT collectionID, 
									   LPCTSTR startPath, const Instructions& pinfo, Statistics& stats)
{
	ASSERT(collectionID != 0);
	ASSERT(startPath != NULL);
	ASSERT(_tcslen(startPath) < MAX_PATH - 10);
	PrgAPI* pAPI = PRGAPI();
	TCHAR bf[500];
	UINT NumTracks = 0;
	WIN32_FIND_DATA wfd;
	
	std::tstring wildCard;
	//TCHAR wildCard[MAX_PATH];
	if (CombinePath(wildCard, startPath, _T("*.*")) == FALSE)
		return;

	//TRACE(_T("@3 CollectionUpdateJob::RecurseDirectories. Entering '%s'\r\n"), wildCard.c);
	HANDLE hFF = ::FindFirstFile(wildCard.c_str(), &wfd);
	BOOL bContinueSearch = TRUE;
	while (bContinueSearch && (hFF != INVALID_HANDLE_VALUE))
	{
		if (m_bAbort)	break;
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			BOOL IsDots = FALSE;
			if (wfd.cFileName[0] == '.')
			{
				if (wfd.cFileName[1] == '\0' ||	(wfd.cFileName[1] == '.' &&	wfd.cFileName[2] == '\0'))
					IsDots = TRUE;
			}
			if (!IsDots)
			{
				stats.DirsChecked++;
				if (CombinePath(wildCard, startPath, wfd.cFileName))
					RecurseDirectories(collectionID, wildCard.c_str(), pinfo, stats);
			}
		}
		else
		{
			stats.FilesTotal++;
			TrackTypesEnum trackType = GetTrackType(wfd.cFileName);
			if (trackType != TTYPE_Unsupported)
			{
				stats.FilesSupported++;
				if (pinfo.bProcessFiles)
				{
#ifdef	BENCMARK_COLLECTIONUPDATE
					__int64 PureTime0 = timer.Measure();
#endif
					m_progress.curPos++;
					CombinePath(wildCard, startPath, wfd.cFileName);
					TRACEST(_T("CollectionUpdateJob::RecurseDirectories (process)"), wildCard.c_str());
#ifdef	BENCMARK_COLLECTIONUPDATE
					__int64 GetTrackUnique0 = timer.Measure();
#endif
					TrackRecord rec;
					BOOL bExists = m_sqlManager.GetTrackRecordUnique(collectionID, wildCard.c_str(), rec);
#ifdef	BENCMARK_COLLECTIONUPDATE
					sTrackUnique += timer.Measure() - GetTrackUnique0;
#endif
					if ((!bExists && pinfo.bAllowAddNew) || (bExists && pinfo.bAllowUpdate))
					{
						//Process File
#ifdef	BENCMARK_COLLECTIONUPDATE
						__int64 Read0 = timer.Measure();
#endif
						TagInfo info;
						info.validFields = m_fileTagger.Read(wildCard.c_str(), info);
#ifdef	BENCMARK_COLLECTIONUPDATE
						sReadTime += timer.Measure() - Read0;
#endif
						if (info.validFields > 0)
						{
							trim(info.Title);
							trim(info.Album);
							trim(info.Artist);
							trim(info.Genre);
							trim(info.Lyrics);
							trim(info.Comment);
							if (bExists)//Update
							{
#ifdef	BENCMARK_COLLECTIONUPDATE
								__int64 Notification0 = timer.Measure();
#endif
								_sntprintf(bf, 500, _T("%s: %s"), pAPI->GetString(IDS_UPDATING), wildCard.c_str());
								pAPI->NotifyUser(bf, 0, SEV_InfoL3);
#ifdef	BENCMARK_COLLECTIONUPDATE
								sNotifications += timer.Measure() - Notification0;
#endif

#ifdef	BENCMARK_COLLECTIONUPDATE
								__int64 UpdateTrackFromTagInfo0 = timer.Measure();
#endif
								if (m_sqlManager.UpdateTrackFromTagInfo(rec.ID, collectionID, wildCard.c_str(), info))
								{
									stats.FilesUpdated++;
									m_progress.updateActionsPerformed++;
								}
								else
									stats.FilesFailed++;
#ifdef	BENCMARK_COLLECTIONUPDATE
								sUpdateTime += timer.Measure() - UpdateTrackFromTagInfo0;
								sUpdateTrackCount++;
#endif
							}
							else
							{
#ifdef	BENCMARK_COLLECTIONUPDATE
								__int64 Notification0 = timer.Measure();
#endif
								_sntprintf(bf, 500, _T("%s: %s"), pAPI->GetString(IDS_ADDING), wildCard.c_str());
								bf[499] = 0;
								pAPI->NotifyUser(bf, 0, SEV_InfoL3);
#ifdef	BENCMARK_COLLECTIONUPDATE
								sNotifications += timer.Measure() - Notification0;
#endif
#ifdef	BENCMARK_COLLECTIONUPDATE
								__int64 AddNewTrackFromTagInfo0 = timer.Measure();
#endif
								if (m_sqlManager.AddNewTrackFromTagInfo(collectionID, wildCard.c_str(), info))
								{
									m_progress.updateActionsPerformed++;
									stats.FilesAdded++;
								}
								else
									stats.FilesFailed++;
#ifdef	BENCMARK_COLLECTIONUPDATE
								sAddNewTime += timer.Measure() - AddNewTrackFromTagInfo0;
								sAddNewTrackCount++;
#endif
							}
						}
						else
						{
							//TRACE(_T("@1 CollectionUpdateJob::RecurseDirectories ReadTags Failure on: %s\r\n"), wildCard.c);
							stats.FilesFailed++;
						}
					}
#ifdef	BENCMARK_COLLECTIONUPDATE
					sPureTime += timer.Measure() - PureTime0;
					sTotalTrackCount++;
#endif#endif
				}
			}
		}
		bContinueSearch = ::FindNextFile(hFF, &wfd);
	}
	if (hFF != INVALID_HANDLE_VALUE)
		::FindClose(hFF);
}

BOOL CollectionUpdateJob::CombinePath(std::basic_string<TCHAR>& destPath, LPCTSTR path, LPCTSTR fileName)
{
	ASSERT(path != NULL && fileName != NULL);
	if (path == NULL || fileName == NULL)
	{
		TRACE(_T("@1 CollectionUpdateJob::CombinePath path|| fName NULL\r\n"));
		return FALSE;
	}
	if (path[0] == 0 || fileName[0] == 0)
	{
		TRACE(_T("@1 CollectionUpdateJob::CombinePath path|| fName 0\r\n"));
		return FALSE;
	}
	destPath = path;
	TCHAR ch = destPath[destPath.size() - 1];
	if (ch != '\\')
		destPath += _T("\\");

	destPath += fileName;
	return TRUE;

	//UINT curLen = _tcslen(destPath);
	//if (destPath[curLen - 1] != '\\')
	//{
	//	destPath[curLen] = '\\';
	//	destPath[curLen + 1];
	//}
	//_tcsncat(destPath, fileName, MAX_PATH);
}


