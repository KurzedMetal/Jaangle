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
#ifndef _STATEMANAGER_H
#define _STATEMANAGER_H

#include "IStateChangeListener.h"
#include <set>
#include "TeenSpiritEnums.h"
#include "TracksFilter.h"
#include "DataRecords.h"
#include "RecordCollectionSorter.h"
class AppSettings;


enum StateMessageEnum
{
	SM_None,

	SM_MediaChanged,
	SM_PlayListChanged,
	SM_PlayerVolumeChanged,
	SM_PlayerSettingsChanged,

	SM_InfoChanged,
	SM_PictureChanged,

	SM_DatabaseUpdated,
	SM_TrackUpdated,

	SM_CollectionManagerEvent,

	SM_CurrentSectionChanged,
	SM_CurrentTrackChanged,

	SM_LocateTrackRequest,

	SM_SetQuickSearch
};

//=== The Interface implementation that must inherit all the controls that listen to state changes
class TSStateChangeListener: public IStateChangeListener
{
public:
	TSStateChangeListener();
	virtual ~TSStateChangeListener();
};

class TSState
{
public:
	//enum SectionSelectionStyleEnum
	//{
	//	SSS_Normal,
	//	SSS_QuickSearch,
	//	SSS_Last
	//};
public:
	TSState():
		trackListTrackID(0),
//		sectionSelectionStyle(SSS_Normal),
		bIsTrackListDirty(TRUE),
		locateTrackID(0)
		//m_pTrackList(NULL)
	{}
	~TSState()				{}


	//=== TS State is the common object that all GUI controls must see to modify their view.
	//=== The GUI concepts are:
	//		Controls that modify the Tracks Filter (Producers).
	//			- Music Browser (modifies activeTracksFilter, activeItemType)
	//			- Quick Search Box (modifies activeTracksFilter)
	//		Controls that display information based on that filter (Consumers)
	//			- Track List (listens activeTracksFilter) (modifies activeItemType)
	//			- Info Control (listens activeTracksFilter, activeItemType)
	//		Controls that modify the current play state
	//			- Player(s)	(Producer)
	//		Controls that display the current play state
	//			- PlayerBar
	//			- PlayList
	//			- InfoControl

	//SectionSelectionStyleEnum sectionSelectionStyle;
	TracksFilter activeTracksFilter;
	//std::tstring quickSearchString;
	ItemTypeEnum activeItemType;
	SortOptionCollection sortOptionCollection;
	UINT trackListTrackID;
	BOOL bIsTrackListDirty;

	FullTrackRecordCollection& GetTrackList();

	std::basic_string<TCHAR> quickSearchString;
	UINT locateTrackID;
private:
	FullTrackRecordCollection m_trackList;

};

class StateManager
{
public:
	StateManager();
	virtual ~StateManager()														{}

public:
	virtual void RegisterStateChangeListener(IStateChangeListener& listener);
	virtual void UnRegisterStateChangeListener(IStateChangeListener& listener);

	virtual void SendMessage(UINT stateMessage);
	virtual void PostMessage(UINT stateMessage);

	virtual void HeartBeat();

	TSState& GetState()				{return m_state;}

	//BOOL LoadState(AppSettings& params);
	//BOOL SaveState(AppSettings& params);
	//void ClearState();

private:
	std::set<IStateChangeListener*> m_pListeners;
	TSState m_state;

	//UINT m_messageInQueue;
	std::set<UINT> m_messagesInQueue;
	DWORD m_MainThread;
	CCriticalSection m_cs;


};
#endif
