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
#include "StateManager.h"
#include "PrgAPI.h"
#include "SQLManager.h"
#include "AppSettings/AppSettings.h"

//===========================================================================
//=== TSStateChangeListener
//===========================================================================

LPCTSTR SM2Str(UINT stateMessage)
{
	switch (stateMessage)
	{
	case SM_MediaChanged:				return _T("MediaChanged");
	case SM_PlayListChanged:			return _T("PlayListChanged");
	case SM_PlayerVolumeChanged:		return _T("PlayerVolumeChanged");
	case SM_PlayerSettingsChanged:		return _T("PlayerSettingsChanged");
	case SM_InfoChanged:				return _T("InfoChanged");
	case SM_PictureChanged:				return _T("PictureChanged");
	case SM_DatabaseUpdated:			return _T("DatabaseUpdated");
	case SM_CollectionManagerEvent:		return _T("CollectionManagerEvent");
	case SM_CurrentSectionChanged:		return _T("SectionChanged");
	case SM_CurrentTrackChanged:		return _T("TrackChanged");
	case SM_LocateTrackRequest:			return _T("SectionChangeRequest");
	}
	return NULL;
}


TSStateChangeListener::TSStateChangeListener()
{
	PRGAPI()->GetStateManager()->RegisterStateChangeListener(*this);
}
TSStateChangeListener::~TSStateChangeListener()
{
	PRGAPI()->GetStateManager()->UnRegisterStateChangeListener(*this);
}

//========================================================================
//===TSState
//========================================================================

FullTrackRecordCollection& TSState::GetTrackList()
{
	if (bIsTrackListDirty)
	{
		//=== This function has been crashed several times in a certain user (290558176 v866)
		//=== Other user (919176317 v866) said "in vista,when searching in window,the application will quit" multiple times
		TRACEST(_T("TSState::GetTrackList"));
		m_trackList.clear();
		if (activeTracksFilter.IsEmpty() == FALSE)
		{
			SQLManager* pSM = PRGAPI()->GetSQLManager();
			pSM->GetFullTrackRecordCollectionByTracksFilter(m_trackList, activeTracksFilter);
		}
		//pSM->GetFullTrackRecordCollectionBySimpleSearch(m_trackList, quickSearchString.c_str());
		bIsTrackListDirty = FALSE;
	}
	return m_trackList;

}

//========================================================================
//===StateManager
//========================================================================

StateManager::StateManager()//:m_messageInQueue(SM_None)
{
	m_MainThread = GetCurrentThreadId();
}

void StateManager::RegisterStateChangeListener(IStateChangeListener& listener)
{
	m_pListeners.insert(&listener);
}
void StateManager::UnRegisterStateChangeListener(IStateChangeListener& listener)
{
	std::set<IStateChangeListener*>::iterator it = m_pListeners.find(&listener);
	if (it != m_pListeners.end())
		m_pListeners.erase(it);
	else
		ASSERT(0);
}

void StateManager::SendMessage(UINT stateMessage)
{
	if (m_MainThread != GetCurrentThreadId())
	{
		TRACE(_T("@2BaseDirector::SendMessage. Message from another thread...Posted"));
		//If the SendMessage have came from another thread then we will "Post" the
		//message for the following "Heartbeat" which will run from the Main Thread
		//PostMessage Also is "Thread Safe" for the messages vector
		PostMessage(stateMessage);
	}
	else
	{
		switch (stateMessage)
		{
		case SM_CurrentTrackChanged:
		case SM_CurrentSectionChanged:
		case SM_DatabaseUpdated:
		case SM_CollectionManagerEvent:
			m_state.bIsTrackListDirty = TRUE;
			break;
		}

#ifdef _DEBUG
		TRACE(_T("StateManager::SendMessage Broadcasting '%s' * * * * * * *\r\n"), SM2Str(stateMessage));
		std::set<IStateChangeListener*>::iterator it = m_pListeners.begin();
		for (; it != m_pListeners.end(); it++)
		{
			if ((*it)->OnStateChanged(stateMessage))
				TRACE(_T("\t+ %s\r\n"), (*it)->GetStateListenerName());
			else
				TRACE(_T("\t- %s\r\n"), (*it)->GetStateListenerName());
		}
		TRACE(_T("StateManager::SendMessage Broadcasting '%s' - - - - - - -\r\n"), SM2Str(stateMessage));
#else
		for (std::set<IStateChangeListener*>::iterator i = m_pListeners.begin(); i != m_pListeners.end(); i++)
			(*i)->OnStateChanged(stateMessage);
#endif
	}


}

void StateManager::PostMessage(UINT stateMessages)
{
	CSingleLock lock(&m_cs, TRUE);
	m_messagesInQueue.insert(stateMessages);
	//m_messageInQueue = stateMessages;
}

void StateManager::HeartBeat()
{
	//I do this, this way because a Message May Generate other Messages
	//which i want to be sent in the next heartbeat
	CSingleLock lock(&m_cs, TRUE);
	std::set<UINT>::iterator it = m_messagesInQueue.begin();
	for (; it != m_messagesInQueue.end(); it++)
		SendMessage(*it);
	m_messagesInQueue.clear();
}

//BOOL StateManager::LoadState(AppSettings& params)
//{
//	std::tstring sortingStr;
//	params.Read(_T("StateManager"), _T("Sorting"), sortingStr, _T(""));
//	m_state.sortOptionCollection.LoadFromString(sortingStr.c_str());
//	return TRUE;
//}
//
//BOOL StateManager::SaveState(AppSettings& params)
//{
//	TCHAR bf[300];
//	m_state.sortOptionCollection.SaveToString(bf, 300);
//	params.Write(_T("StateManager"), _T("Sorting"), bf);
//	return TRUE;
//}






