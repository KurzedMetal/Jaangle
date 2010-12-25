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
#include "CollectionManager.h"
#include "MultiThreadJobQueue.h"
#include "SQLManager.h"
#include "ObjFactory.h"
#include "FileTagger.h"
#include "FreeDb/FreeDb.h"
#include "StateManager.h"


CollectionManager::CollectionManager():
m_pFileTagger(NULL)
{
	m_pMTQueue = new MultiThreadJobQueue;
	m_pMTQueue->SetMaxThreads(1);
	
}
CollectionManager::~CollectionManager()
{
	delete m_pMTQueue;
	delete m_pFileTagger;
}

FileTagger& CollectionManager::GetFileTagger()
{
	if (m_pFileTagger == NULL)
		m_pFileTagger = ObjFactory::Instance()->CreateFileTagger();
	m_pFileTagger->SetConfig(FileTagger::CONF_ReadDepth, PRGAPI()->GetOption(OPT_TAG_SafetyOption));
	return *m_pFileTagger;
}


BOOL CollectionManager::AddCollection(LPCTSTR path, LPCTSTR name)
{
	TRACEST(_T("CollectionManager::AddCollection."));
	ASSERT(path != NULL);
	if (path == NULL)
		return FALSE;
	if (_taccess(path, 0) != 0)
		return FALSE;
	//---Get Information--------------------------------------------
	CollectionTypesEnum ct = GetCollectionType(path);
	DWORD serialNum = GetVolumeSerial(path);

	//---Add The Record
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	CollectionRecordSP rec(new CollectionRecord);
	GetLocalTime(&rec->dateAdded);
	rec->location = path;
	rec->name = name;
	rec->serial = serialNum;
	rec->type = ct;
	if (pSM->AddNewCollectionRecord(*rec))
	{
		if (ct == CTYPE_AudioCD)
			return ImportAudioCDCollection(rec);
		else
		{
			CollectionUpdateJob::Instructions instr;
			instr.bAllowAddNew = TRUE;
			instr.bProcessFiles = TRUE;
			instr.bAllowUpdate = FALSE;
			instr.bAllowRemove = FALSE;
			m_workingCollections.push_back(rec->ID);
			JobSP job(CollectionUpdateJob::CreateJob(rec, instr, *pSM, GetFileTagger(), m_progress));
			pAPI->SendMessage(SM_CollectionManagerEvent);
			return m_pMTQueue->AddJob(job);
		}
	}
	return FALSE;
}

BOOL CollectionManager::UpdateCollection(CollectionRecordSP rec, BOOL bAllowAddNew, BOOL bAllowUpdate, BOOL bAllowRemove)
{
	TRACEST(_T("CollectionManager::UpdateCollection."));
	ASSERT(rec.get() != NULL);
	if (rec.get() == NULL)
	{
		TRACE(_T("@1 CollectionManager::UpdateCollection. rec is null!"));
		return FALSE;
	}
	if (!IsCollectionPending(*rec))
	{
		CollectionUpdateJob::Instructions instr;
		instr.bAllowAddNew = bAllowAddNew;
		instr.bProcessFiles = TRUE;
		instr.bAllowUpdate = bAllowUpdate;
		instr.bAllowRemove = bAllowRemove;

		SQLManager* pSM = PRGAPI()->GetSQLManager();
		m_workingCollections.push_back(rec->ID);
		JobSP job(CollectionUpdateJob::CreateJob(rec, instr, *pSM, GetFileTagger(), m_progress));
		PRGAPI()->SendMessage(SM_CollectionManagerEvent);
		TRACE(_T("@3 CollectionManager::UpdateCollection. Adding Job"));
		return m_pMTQueue->AddJob(job);
	}
	return FALSE;
}

BOOL CollectionManager::ImportFullTrackRecordCollection(CollectionRecord& cr, FullTrackRecordCollection& col)
{
	TRACEST(_T("CollectionManager::ImportFullTrackRecordCollection."));
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	TagInfo ti;
	INT tracksAdded = 0;
	for (size_t i = 0; i < col.size(); i++)
	{
		FullTrackRecord& rec = (*col[i]);
		ti.Album = rec.album.name;
		ti.Artist = rec.artist.name;
		ti.Bitrate = rec.track.bitrate;
		ti.Genre = rec.genre.name;
		ti.Length = rec.track.duration;
		ti.Rating = rec.track.rating;
		ti.Size = rec.track.size;
		ti.Title = rec.track.name;
		ti.TrackNo = rec.track.trackNo;
		ti.Year = rec.track.year;
		ti.validFields = TagInfo_All;
		if (pSM->AddNewTrackFromTagInfo(cr.ID, rec.track.location.c_str(), ti))
			tracksAdded++;
		else
		{
			TRACE(_T("CollectionManager::ImportFulltrackRecordCollection. Cannot add track\r\n"));
			ASSERT(0);
		}
	}
	if (tracksAdded > 0)
		PRGAPI()->SendMessage(SM_CollectionManagerEvent);
	return tracksAdded > 0;
}



BOOL CollectionManager::RefreshLocalCollections(BOOL bAllowAddNew, BOOL bAllowUpdate, BOOL bAllowRemove)
{
	TRACEST(_T("CollectionManager::RefreshLocalCollections."));
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	UINT collectionsUpdated = 0;
	CollectionRecordCollection col;
	if (pSM->GetCollectionRecordCollectionByTypeID(col, CTYPE_LocalFolder))
	{
		CollectionRecordCollection::const_iterator it = col.begin();
		for (; it != col.end(); it++)
		{
			if (UpdateCollection(*it, bAllowAddNew, bAllowUpdate, bAllowRemove))
				collectionsUpdated++;
		}
	}
	col.clear();
	if (pSM->GetCollectionRecordCollectionByTypeID(col, CTYPE_NetFolder))
	{
		CollectionRecordCollection::const_iterator it = col.begin();
		for (; it != col.end(); it++)
		{
			if (PathFileExists((*it)->location.c_str()))
			{
				if (UpdateCollection(*it, bAllowAddNew, bAllowUpdate, bAllowRemove))
					collectionsUpdated++;
			}
		}
	}
	return collectionsUpdated > 0;
}
BOOL CollectionManager::CollectionExists(LPCTSTR path)
{
	//---Get Information--------------------------------------------
	TCHAR volName[MAX_PATH];
	GetVolumeLabel(path, volName);
	CollectionTypesEnum ct = GetCollectionType(path);
	DWORD serialNum = GetVolumeSerial(path);

	//---Determine if collection already exists--------------------------------------------
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	CollectionRecord rec;
	return pSM->GetCollectionRecordUnique(serialNum, path, rec);
}


BOOL CollectionManager::RemoveCollection(UINT ID)
{
	PrgAPI* pAPI = PRGAPI();
	if (pAPI->GetSQLManager()->DeleteCollectionRecord(ID, TRUE))
	{
		pAPI->SendMessage(SM_CollectionManagerEvent);
		return TRUE;
	}
	return FALSE;
}
BOOL CollectionManager::RenameCollection(UINT ID, LPCTSTR newName)
{
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	CollectionRecord rec;
	if (pSM->GetCollectionRecord(ID, rec))
	{
		rec.name = newName;
		if (pAPI->GetSQLManager()->UpdateCollectionRecord(rec))
		{
			pAPI->SendMessage(SM_CollectionManagerEvent);
			return TRUE;
		}

	}
	return FALSE;
}


CollectionManager::State CollectionManager::GetState()
{
	if (m_workingCollections.size() != 0)
		return S_Working;
	return S_Idle;
}

INT CollectionManager::GetPendingJobs()
{
	MultiThreadJobQueue::JobStatisics stats = m_pMTQueue->GetStatistics();
	return stats.waiting + stats.working;
}


BOOL CollectionManager::IsCollectionPending(const CollectionRecord& rec) const
{
	for (size_t i = 0; i < m_workingCollections.size(); i++)
	{
		if (m_workingCollections[i] == rec.ID)
			return TRUE;
	}
	return FALSE;
}
BOOL CollectionManager::ReleaseCollectionWorkingStatus(const CollectionRecord& rec)
{
	for (size_t i = 0; i < m_workingCollections.size(); i++)
	{
		if (m_workingCollections[i] == rec.ID)
		{
			m_workingCollections.erase(m_workingCollections.begin() + i);
			return TRUE;
		}
	}
	return FALSE;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

DWORD CollectionManager::GetVolumeSerial(LPCTSTR path)
{
	ASSERT(path != NULL);
	TCHAR rootPath[4];
	_tcsncpy(rootPath, path, 3);
	rootPath[3] = 0;
	DWORD serNum = 0;
	SetErrorMode(SEM_FAILCRITICALERRORS);
	BOOL ret = GetVolumeInformation(rootPath, NULL, 0, &serNum, NULL, NULL, NULL, NULL);
	SetErrorMode(0);
	return serNum;
}
BOOL CollectionManager::GetVolumeLabel(LPCTSTR path, LPTSTR bfMaxPath)
{
	ASSERT(path != NULL && bfMaxPath != NULL);
	TCHAR rootPath[4];
	_tcsncpy(rootPath, path, 3);
	rootPath[3] = 0;
	SetErrorMode(SEM_FAILCRITICALERRORS);
	BOOL ret = GetVolumeInformation(rootPath, bfMaxPath, MAX_PATH, NULL, NULL, NULL, NULL, NULL);
	SetErrorMode(0);
	return ret;
}

CollectionTypesEnum CollectionManager::GetCollectionType(LPCTSTR pathName)
{
	ASSERT(pathName != NULL);
	CollectionTypesEnum ct = CTYPE_Unknown;
	if (pathName[0] == '\\' && pathName[1] == '\\')
	{
		ct = CTYPE_NetFolder;
	}
	else if (pathName[1] == ':' && pathName [2] == '\\')
	{
		TCHAR RootPathName[4];
		_tcsncpy(RootPathName, pathName, 3);
		RootPathName[3] = 0;
		switch (GetDriveType(RootPathName))
		{
		case DRIVE_FIXED:		//The drive is a type that cannot be removed, for example, a fixed hard drive.
			ct = CTYPE_LocalFolder;
			break;
		case DRIVE_CDROM:		//The drive is a CD-ROM drive.
			{
				ct = CTYPE_Media;
				AudioCDInfo cdInfo;
				if (FreeDB::ReadDisc(RootPathName, cdInfo) && cdInfo.discID != 0)
					ct = CTYPE_AudioCD;
			}
			break;
		case DRIVE_REMOTE:		//The drive is a remote (network) drive.
			ct = CTYPE_NetFolder;
			break;
		case DRIVE_REMOVABLE:	//The drive is a type that has removable media, for example, a floppy drive or removable hard disk or USB Memory stick.
			ct = CTYPE_Removable;
			break;
		case DRIVE_NO_ROOT_DIR:	//The root path is invalid, for example, no volume is mounted at the path.
		case DRIVE_UNKNOWN:		//The drive type cannot be determined.
		case DRIVE_RAMDISK:		//The drive is a RAM disk.
		default:
			TRACE(_T("@1CollectionManager::GetPathNameCollectionType. Unsupported Drive Type: '%s'\n"), pathName);
			break;
		}
	}
	return ct;
}

BOOL CollectionManager::ImportAudioCDCollection(CollectionRecordSP col)
{
	TRACEST(_T("CollectionManager::ImportAudioCDCollection."));
	AudioCDInfo cdInfo;
	if (!FreeDB::ReadDisc(col->location.c_str(), cdInfo))			
		return FALSE;//*** Unable To read disc

	FreeDB fdb;
	fdb.Init(_T("TeenSpirit"), _T("0.9"), PRGAPI()->GetInternetHandle());
	std::vector<FreeDBResult> qres;
	if (fdb.Query(qres, cdInfo))
		fdb.Read(qres[0], cdInfo);
	TagInfo info;
	if (FreeDB::GetFirstPart(info.Artist, cdInfo.title.c_str()))
		FreeDB::GetSecondPart(info.Album, cdInfo.title.c_str());
	else
		info.Album = cdInfo.title;
	info.Bitrate = 1378;
	info.Genre = cdInfo.genre;
	info.Year = cdInfo.year;

	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();

	for (UINT i = 0; i < cdInfo.tracks.size(); i++)
	{
		if (FreeDB::GetFirstPart(info.Artist, cdInfo.tracks[i].title.c_str()))
			FreeDB::GetSecondPart(info.Title, cdInfo.tracks[i].title.c_str());
		else
			info.Title = cdInfo.tracks[i].title;
		TCHAR bf[20];
		_sntprintf(bf, 10, _T("cda://%d"), i + 1);
		info.Length = cdInfo.tracks[i].length.toSec();
		info.Size = info.Length * info.Bitrate / 8;
		info.TrackNo = i + 1;
		pSM->AddNewTrackFromTagInfo(col->ID, bf, info);
	}
	if (!cdInfo.title.empty())
	{
		col->name = cdInfo.title;
		pSM->UpdateCollectionRecord(*col);
	}
	return TRUE;
}

void CollectionManager::ConsumeResult()
{
	if (m_pMTQueue == NULL)
		return;
	JobSP job;
	while (m_pMTQueue->RetrieveFinishedJob(job))
	{
		Job::State jst = job->GetState();
		ASSERT(jst == Job::ST_Finished || jst == Job::ST_Aborted);
		if (jst == Job::ST_Finished || jst == Job::ST_Aborted)
		{
			CollectionUpdateJob* pColJob = dynamic_cast<CollectionUpdateJob*>(job.get());
			ASSERT(pColJob != NULL);
			if (pColJob != NULL)
			{
				TRACEST(_T("CollectionManager::ConsumeResult. Consuming Result"));
				CollectionRecordSP col;
				if (pColJob->GetCollectionRecord(col))
				{
					ASSERT(col.get() != NULL);
					ReleaseCollectionWorkingStatus(*col);
					PrgAPI* pAPI = PRGAPI();
					pAPI->SendMessage(SM_CollectionManagerEvent);
					const CollectionUpdateJob::Statistics& stats = pColJob->GetStatistics();
					if (stats.FilesAdded || stats.FilesUpdated || stats.FilesDeleted)
					{
						pAPI->SendMessage(SM_DatabaseUpdated);
					}
				}
			}
		}
	}
}







