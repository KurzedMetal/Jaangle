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
#ifndef _CollectionManager_h_
#define _CollectionManager_h_

#include "PrgAPI.h"
#include "DataRecords.h"
#include "EventTarget.h"
#include "CollectionUpdateJob.h"

class MultiThreadJobQueue;

class CollectionManager
{
public:
	CollectionManager();
	~CollectionManager();

public:
	//The caller should call "CollectionExists" first so it can inform the user.
	//The caller may call the "GetCollectionType". 
	//		If it is a CD must Inform the user that it will Add the full CD
	//The caller may call the "GetVolumeLabel" to suggest a name for the collection
	BOOL AddCollection(LPCTSTR path, LPCTSTR name);
	BOOL UpdateCollection(CollectionRecordSP rec, BOOL bAllowAddNew, BOOL bAllowUpdate, BOOL bAllowRemove);
	BOOL CollectionExists(LPCTSTR pathName);

	BOOL RemoveCollection(UINT ID);
	BOOL RenameCollection(UINT ID, LPCTSTR newName);


	BOOL ImportFullTrackRecordCollection(CollectionRecord& cr, FullTrackRecordCollection& col);

	BOOL RefreshLocalCollections(BOOL bAllowAddNew, BOOL bAllowUpdate, BOOL bAllowRemove);
	BOOL IsCollectionPending(const CollectionRecord& rec) const;
	
	const CollectionUpdateJob::Progress& GetProgress() const					{return m_progress;}

	INT GetPendingJobs();

	//===Async Result Producer
	void ConsumeResult();
	enum State
	{
		S_Idle,			//There are no more jobs to execute
		S_Working,		//Working on some jobs
		S_Paused,		//Paused.
		S_Last
	};
	State GetState();
	//===Async Result Producer END!



	static DWORD GetVolumeSerial(LPCTSTR path);
	static BOOL GetVolumeLabel(LPCTSTR path, LPTSTR bfMaxPath);
	static CollectionTypesEnum GetCollectionType(LPCTSTR pathName);
private:
	BOOL ImportAudioCDCollection(CollectionRecordSP rec);
	BOOL ReleaseCollectionWorkingStatus(const CollectionRecord& rec);
	FileTagger& GetFileTagger();
private:
	MultiThreadJobQueue* m_pMTQueue;
	FileTagger* m_pFileTagger;
	State m_state;
	std::vector<UINT> m_workingCollections;
	CollectionUpdateJob::Progress m_progress;
};

#endif
