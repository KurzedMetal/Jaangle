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
#include "TeenSpiritPlayer.h"
#include "MediaPlayerEngine.h"
#include "MediaPlayList.h"
#include "MediaVisualization.h"
#include "ActionManager.h"
#include "PrgAPI.h"
#include "cStringUtils.h"
#include "SQLManager.h"
//#include "FileTagger.h"
#include "MediaPlayerUtilities.h"

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

TeenSpiritPlayer::TeenSpiritPlayer():
m_pEngine(NULL),
m_pPlayList(NULL),
m_pVis(NULL),
m_playListPos(-1),
m_mpnmParam(0)
{
}

TeenSpiritPlayer::~TeenSpiritPlayer()
{
	SetMediaVisualization(NULL);
	SetMediaPlayList(NULL);
	SetMediaPlayerEngine(NULL);
}

void TeenSpiritPlayer::SetMediaPlayerEngine(MediaPlayerEngine* pEngine)
{
	delete m_pEngine;
	m_pEngine = pEngine;
	if (m_pEngine != NULL)
		m_pEngine->SetEventTarget(this);
	if (m_pVis != NULL)
		m_pVis->SetMediaPlayerEngine(m_pEngine);

}
void TeenSpiritPlayer::SetMediaPlayList(MediaPlayList* pList)
{
	delete m_pPlayList;
	m_pPlayList = pList;
}
void TeenSpiritPlayer::SetMediaVisualization(MediaVisualization* pVis)
{
	delete m_pVis;
	m_pVis = pVis;
	if (m_pVis != NULL)
		m_pVis->SetMediaPlayerEngine(m_pEngine);
}

BOOL TeenSpiritPlayer::Play(const MediaPlayListItem& mpli)
{
	ASSERT(mpli.IsValid() != NULL);
	ClearPlayList();
	if (InsertMedia(mpli, -1))
	{
		SetPlayListPos(0);
		Start();
		return TRUE;
	}
	return FALSE;
}
BOOL TeenSpiritPlayer::Enqueue(const MediaPlayListItem& mpli)
{
	ASSERT(mpli.IsValid() != NULL);
	return InsertMedia(mpli, -1);
}
BOOL TeenSpiritPlayer::InsertMedia(const MediaPlayListItem& mpli, INT Pos)
{
	ASSERT(mpli.IsValid());
	if (mpli.IsValid())
	{
		if (Pos == -1)
			Pos = GetPlayListCount();

		MediaPlayListItem it = mpli;

		UINT sLen = _tcslen(mpli.url);
		if (_tcsicmp(&mpli.url[sLen - 3], _T("m3u")) == 0)
			MediaPlayerUtilities::InsertM3U(*this, mpli.url, Pos);
		else
		{
			//TagInfo ti;
			//if (it.title == NULL || it.title[0] == 0)
			//{
			//	FileTagger* pFT = PRGAPI()->GetFileTagger();
			//	if (pFT->Read(it.url, ti))
			//	{
			//		it.artist = ti.Artist.c_str();
			//		it.title = ti.Title.c_str();
			//		it.length = ti.Length;
			//		it.rating = ti.Rating;
			//	}
			//}
			if (m_pPlayList->InsertPlayListItem(it, Pos))
			{
				m_shuffle.InsertItem(Pos);
				if (Pos <= m_playListPos)
					m_playListPos++;
				m_changes.tickPlayList = GetTickCount();
				return TRUE;
			}
		}

	}
	TRACE(_T("@1 TeenSpiritPlayer::InsertMedia Failed\r\n"));
	return FALSE;
}


INT TeenSpiritPlayer::OnEvent(INT ID, INT param, void* sender)
{
	switch (ID)
	{
	case MediaPlayerEngine::MPE_ReachedEnd:
		//=== This code will make it "Stop" and return to the start of the track
		if (Next() == FALSE)
		{
			if ((GetMediaLength() - GetMediaPos())  < .5)
				Stop();
		}
		break;
	case MediaPlayerEngine::MPE_PlayStateChanged:
		m_changes.tickPlayState = GetTickCount();
		break;
	case MediaPlayerEngine::MPE_Error:
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

INT TeenSpiritPlayer::GetPlayListPos()								
{
#ifdef _DEBUG
	INT count = GetPlayListCount();
	ASSERT(m_playListPos < count);
	if (m_playListPos >= count)	//=== Must NOT happen
		m_playListPos = count - 1;
#endif
	return m_playListPos;
}


BOOL TeenSpiritPlayer::SetPlayListPos(INT index)
{
	TRACEST(_T("TeenSpiritPlayer::SetPlayListPos"), index);
	INT count = GetPlayListCount();
	ASSERT(index >=-1 && index < count);
	if (index >=0 && index < count)
	{
		if (index != m_playListPos)
		{
			m_playListPos = index;
			m_changes.tickMediaChanged = GetTickCount();
			m_changes.tickPlayState = m_changes.tickMediaChanged;
			m_pEngine->Close();
		}
		//m_pEngine->Close();
		return TRUE;
	}

	m_playListPos = index;
	m_pEngine->Close();
	return FALSE;
}

BOOL TeenSpiritPlayer::RemovePlayListItem(INT index)
{
	TRACEST(_T("TeenSpiritPlayer::RemovePlayListItem"), index);
	INT count = GetPlayListCount();
	ASSERT(index >=0 && index < count);
	if (index >=0 && index < count)
	{
		if (m_pPlayList->RemovePlayListItem(index))
		{
			m_shuffle.RemoveItem(index);
			count--;
			if (index < m_playListPos)
				m_playListPos--;
			else if (index == m_playListPos)
			{
				Stop();
				m_pEngine->Close();
				if (m_playListPos < count)
					SetPlayListPos(m_playListPos);
				else
				{
					if (count > 0)
						SetPlayListPos(count - 1);
					else
						m_playListPos = -1;
				}
			}
			m_changes.tickPlayList = GetTickCount();

			return TRUE;
			//PRGAPI()->SendMessage(SM_PlayListChanged);
		}
	}
	return FALSE;

}
UINT TeenSpiritPlayer::GetPlayListCount()
{
	return m_pPlayList->GetCount();
}

BOOL TeenSpiritPlayer::SwapPlayListItems(INT index1, INT index2)
{
	if (m_pPlayList->SwapPlayListItems(index1, index2))
	{
		m_shuffle.SwapItems(index1, index2);
		if (index1 == m_playListPos)
			m_playListPos = index2;
		else if (index2 == m_playListPos)
			m_playListPos = index1;
		m_changes.tickPlayList = GetTickCount();
		return TRUE;
	}
	return FALSE;
}


BOOL TeenSpiritPlayer::GetPlayListItem(MediaPlayListItem& mpli, INT index)
{
	INT count = GetPlayListCount();
	ASSERT(index >= 0 && index < count);
	if (index >= 0 && index < count)
		return m_pPlayList->GetPlayListItem(mpli, index);
	return FALSE;
}

BOOL TeenSpiritPlayer::UpdatePlayListItem(const MediaPlayListItem& mpli, INT index)
{
	INT count = GetPlayListCount();
	ASSERT(index >= 0 && index < count);
	if (index >= 0 && index < count)
		return m_pPlayList->UpdatePlayListItem(mpli, index);
	return FALSE;
}

void TeenSpiritPlayer::ClearPlayList()
{
	TRACEST(_T("TeenSpiritPlayer::ClearPlayList"));
	Stop();
	m_pEngine->Close();
	m_pPlayList->Clear();
	m_shuffle.Clear();
	m_playListPos = -1;
	m_changes.tickPlayList = GetTickCount();
}

BOOL TeenSpiritPlayer::CanAccess(LPCTSTR& path)
{
	if (_tcsstr(path, _T("://")) != NULL)
		return TRUE;
	if (!PathFileExists(path))
	{
		ActionManager* pAM = PRGAPI()->GetActionManager();
		if (pAM->FindAndFixMountedPath(path, m_tempPathBuffer, MAX_PATH))
			path = m_tempPathBuffer;
		else
			return FALSE;
	}
	if (_taccess(path, 04) == 0)
		return TRUE;
	return FALSE;
}


//BOOL TeenSpiritPlayer::Open()
//{
//	INT count = GetPlayListCount();
//	if (count > 0)
//	{
//		if (m_playListPos == -1)
//			m_playListPos = 0;
//		INT consecutiveFails = 0;
//		while (TRUE)
//		{
//			INT newPlayListPos = m_playListPos;
//			MediaPlayListItem mpli;
//			if (m_pPlayList->GetPlayListItem(mpli, newPlayListPos))
//			{
//				if (CanAccess(mpli.url) && m_pEngine->Open(mpli.url))
//				{
//					UINT length = (UINT)m_pEngine->GetMediaLength();
//					if (mpli.length != length && length != 0)
//					{
//						mpli.length = length;
//						m_pPlayList->UpdatePlayListItem(mpli, newPlayListPos);
//					}
//					SetPlayListPos(newPlayListPos);
//					TRACE(_T("@4 TeenSpiritPlayer::Open OK\r\n"));
//					return TRUE;
//				}
//				else
//					TRACE(_T("@1 TeenSpiritPlayer::Open Failed '%s'\r\n"), mpli.url);
//			}
//			else
//				TRACE(_T("@1 TeenSpiritPlayer::Open Illegal pos (%d/%d)\r\n"), m_playListPos, count - 1);
//			consecutiveFails++;
//			if (consecutiveFails >= 10)
//				break;
//			newPlayListPos = GetNext(newPlayListPos);
//			if (newPlayListPos == -1)
//				break;
//		}
//	}
//	else
//	{
//		TRACE(_T("@1 TeenSpiritPlayer::Open PlayList is empty\r\n"));
//		ASSERT(m_playListPos == -1);
//		m_playListPos = -1;
//	}
//	return FALSE;
//}

void TeenSpiritPlayer::SetMediaPos(DOUBLE secs)
{
	TRACEST(_T("TeenSpiritPlayer::SetMediaPos."), m_playListPos);
	PlayState ps = GetPlayState();
	if (ps == PS_Closed)
	{
		if (!OpenPlayListItem(m_playListPos))
			return;
	}
	if (secs < 0.0)
		secs = 0.0;
	m_pEngine->SetMediaPos(secs);
}



void TeenSpiritPlayer::Start()
{
	TRACEST(_T("TeenSpiritPlayer::Start."), m_playListPos);
	PlayState ps = GetPlayState();
	switch (ps)
	{
	case PS_Playing:
		//=== Restart
		Stop();
		m_pEngine->Start();
		break;
	case PS_Stopped:
	case PS_Paused:
		m_pEngine->Start();
		m_changes.tickPlayState = GetTickCount();
		break;
	case PS_Closed:
		{
			BOOL bOpened = OpenPlayListItem(m_playListPos);
			if (bOpened == TRUE)
			{
				m_pEngine->Start();
				m_changes.tickPlayState = GetTickCount();
			}
			else
				Next();
		}
		break;
	default:
		TRACE(_T("TeenSpiritPlayer::Start Unhandled playstate: %d\r\n"), GetPlayState());
		//Do Nothing.
		break;
	}
}
void TeenSpiritPlayer::Pause()
{
	TRACEST(_T("TeenSpiritPlayer::Pause."), m_playListPos);
	if (m_pEngine->GetPlayState() == PS_Paused)
		m_pEngine->Start();
	else
		m_pEngine->Pause();
	m_changes.tickPlayState = GetTickCount();
}

void TeenSpiritPlayer::Stop()
{
	TRACEST(_T("TeenSpiritPlayer::Stop."), m_playListPos);
	m_pEngine->Stop();
	m_changes.tickPlayState = GetTickCount();
}


INT TeenSpiritPlayer::GetNext(INT relative)
{
	INT count = GetPlayListCount();
	PrgAPI* pAPI = PRGAPI();
	INT newPlayListPos = -1;
	if (count > 0)
	{
		if (m_mpnm == MPNM_Shuffle)
			newPlayListPos = m_shuffle.GetNext(relative);
		else
		{
			newPlayListPos = relative + 1;
			if (newPlayListPos >= count)
				newPlayListPos = ((m_mpnm == MPNM_Loop) ? 0 : -1);
		}
	}
	if (newPlayListPos == -1 && m_mpnm == MPNM_AutoContinue)
	{
		ActionManager* pAM = pAPI->GetActionManager();
		TracksFilter tf;
		MediaPlayListItem mpli;
		if (relative != -1)
		{
			if (GetPlayListItem(mpli, relative))
			{
				FullTrackRecordSP rec;
				if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
					pAM->GetRelativeRandomTrackFilter(tf, rec, (RandomTrackModeEnum)m_mpnmParam);
				else
					TRACE(_T("@2 TeenSpiritPlayer::Next Cannot find item by location"));
			}
		}
		//=== This is Added to make it avoid video files on autocontinue.
		tf.TrackType.val = 100;
		tf.TrackType.match = NUMM_Under;

		FullTrackRecordCollection col;
		pAM->GetRandomTrackCollection(col, tf, 1);
		if (col.size() > 0)
		{
			mpli.artist = col[0]->artist.name.c_str();
			mpli.title = col[0]->track.name.c_str();
			mpli.rating = col[0]->track.rating;
			mpli.length = col[0]->track.duration;
			mpli.url = col[0]->track.location.c_str();
			mpli.flags = MPIF_None;
			InsertMedia(mpli, count);
			newPlayListPos = count;
			//pAPI->SendMessage(SM_PlayListChanged);
		}
	}

	return newPlayListPos;
}





INT TeenSpiritPlayer::GetPrevious(INT relative)
{
	int count = GetPlayListCount();
	INT newPlayListPos = -1;
	if (count != 0)
	{
		if (m_mpnm == MPNM_Shuffle)
			newPlayListPos = m_shuffle.GetPrev(relative);
		else
		{
			newPlayListPos = relative - 1;
			if (newPlayListPos < 0)
				newPlayListPos = ((m_mpnm == MPNM_Loop) ? count - 1 : -1);
		}
	}
	return newPlayListPos;
}

BOOL TeenSpiritPlayer::OpenPlayListItem(INT item)
{
	if (item == -1)
		return FALSE;
	MediaPlayListItem mpli;
	if (!m_pPlayList->GetPlayListItem(mpli, item))
	{
		TRACE(_T("@1 TeenSpiritPlayer::OpenPlayListItem Illegal pos %d\r\n"), item);
		return FALSE;
	}
	if ((mpli.flags & MPIF_CannotPlay) == MPIF_CannotPlay)
	{
		TRACE(_T("@2 TeenSpiritPlayer::OpenPlayListItem Marked as invalid\r\n"));
		return FALSE;
	}
	if (CanAccess(mpli.url))
	{
		TRACEST(_T("TeenSpiritPlayer::OpenPlayListItem"), mpli.url);
		if (m_pEngine->Open(mpli.url))
		{
			UINT length = (UINT)m_pEngine->GetMediaLength();
			if (mpli.length != length && length != 0)
			{
				mpli.length = length;
				m_pPlayList->UpdatePlayListItem(mpli, item);
			}
			return TRUE;
		}
		else
			TRACE(_T("@1 TeenSpiritPlayer::OpenPlayListItem Engine Failed on '%s'\r\n"), mpli.url);
	}
	else
		TRACE(_T("@1 TeenSpiritPlayer::OpenPlayListItem Cannot Access '%s'\r\n"), mpli.url);
	mpli.flags &= MPIF_CannotPlay;
	m_pPlayList->UpdatePlayListItem(mpli, item);
	return FALSE;
}


BOOL TeenSpiritPlayer::Next()
{
	TRACEST(_T("TeenSpiritPlayer::Next."), m_playListPos);
	INT newPos = m_playListPos;
	INT consecutiveFails = 0;
	BOOL bOpenSuccess = FALSE;
	while (consecutiveFails < 20)
	{
		newPos = GetNext(newPos);
		if (newPos == -1)
		{
			TRACE(_T("@1 TeenSpiritPlayer::Next. No valid Next (plcount:%d)\r\n"), GetPlayListCount());
			return FALSE;
		}
		bOpenSuccess = OpenPlayListItem(newPos);
		m_changes.tickMediaChanged = GetTickCount();
		if (bOpenSuccess)
			break;
		consecutiveFails++;
	}
	if (bOpenSuccess)
	{
		m_playListPos = newPos;
		m_pEngine->Start();
		m_changes.tickPlayState = m_changes.tickMediaChanged;
	}
	else
	{
		TRACE(_T("TeenSpiritPlayer::Next. consecutiveFails Limit found\r\n"));
		return FALSE;
	}
	return TRUE;
}



BOOL TeenSpiritPlayer::Previous()
{
	TRACEST(_T("TeenSpiritPlayer::Previous."), m_playListPos);
	INT newPos = m_playListPos;
	INT consecutiveFails = 0;
	BOOL bOpenSuccess = FALSE;//OpenPlayListItem(newPos);
	while (consecutiveFails < 10)
	{
		newPos = GetPrevious(newPos);
		if (newPos == -1)
		{
			TRACE(_T("TeenSpiritPlayer::Previous. Na valid previous\r\n"));
			return FALSE;
		}
		bOpenSuccess = OpenPlayListItem(newPos);
		if (bOpenSuccess)
			break;
		m_changes.tickMediaChanged = GetTickCount();
		consecutiveFails++;
	}
	if (bOpenSuccess)
	{
		m_playListPos = newPos;
		m_pEngine->Start();
		m_changes.tickMediaChanged = GetTickCount();
		m_changes.tickPlayState = m_changes.tickMediaChanged;
	}
	else
	{
		TRACE(_T("TeenSpiritPlayer::Previous. consecutiveFails Limit found\r\n"));
		return FALSE;
	}
	return TRUE;
}

const MediaPlayer::Changes& TeenSpiritPlayer::GetChanges()
{
	return m_changes;
}

void TeenSpiritPlayer::StartVis()
{
	TRACEST(_T("TeenSpiritPlayer::StartVis."));
	m_pVis->SetMediaPlayerEngine(m_pEngine);
	m_pVis->Start();
}




