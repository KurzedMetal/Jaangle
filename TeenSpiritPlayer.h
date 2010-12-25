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
#ifndef _TeenSpiritPlayer_h_
#define _TeenSpiritPlayer_h_

#include "MediaPlayer.h"
#include "EventTarget.h"
#include "ShufflePlaylistHelper.h"


class TeenSpiritPlayer: public MediaPlayer, public EventTarget
{
public:
	TeenSpiritPlayer();
	virtual ~TeenSpiritPlayer();

	virtual BOOL CanPlay(LPCTSTR url)							{return m_pEngine->CanPlay(url);}
	virtual BOOL Play(const MediaPlayListItem& mpli);
	virtual BOOL Enqueue(const MediaPlayListItem& mpli);
	virtual BOOL InsertMedia(const MediaPlayListItem& mpli, INT index);

	virtual void Start();
	virtual void Pause();
	virtual void Stop();

	virtual BOOL Next();
	virtual BOOL Previous();

	virtual INT GetPlayListPos();
	virtual BOOL SetPlayListPos(INT index);

	virtual void ClearPlayList();
	virtual BOOL GetPlayListItem(MediaPlayListItem& mpli, INT index);
	virtual BOOL UpdatePlayListItem(const MediaPlayListItem& mpli, INT index);
	virtual BOOL RemovePlayListItem(INT index);
	virtual UINT GetPlayListCount();
	virtual BOOL SwapPlayListItems(INT index1, INT index2);

	virtual void SetNextMode(MediaPlayerNextMode mpnm, INT param)	
	{
		m_mpnm = mpnm; 
		if (param != -1)
			m_mpnmParam = param;
	}
	virtual MediaPlayerNextMode GetNextMode(INT* param)
	{
		if (param != NULL)
			*param = m_mpnmParam;
		return m_mpnm;
	}

	virtual void SetMediaPlayerEngine(MediaPlayerEngine* pEngine);
	virtual void SetMediaPlayList(MediaPlayList* pList);
	virtual void SetMediaVisualization(MediaVisualization* pVis);

	virtual PlayState GetPlayState()							{return m_pEngine->GetPlayState();}
	virtual DOUBLE GetMediaLength()								{return m_pEngine->GetMediaLength();}
	virtual DOUBLE GetMediaPos()								{return m_pEngine->GetMediaPos();}
	virtual void SetMediaPos(DOUBLE secs);

	virtual void SetVolume(INT volume)							{m_pEngine->SetVolume(volume);}
	virtual INT GetVolume()										{return m_pEngine->GetVolume();}
	virtual void SetMute(BOOL bMute)							{m_pEngine->SetMute(bMute);}
	virtual BOOL GetMute()										{return m_pEngine->GetMute();}

	virtual BOOL IsVideo()										{return m_pEngine->IsVideo();}

	virtual void SetVideoContainerHWND(HWND hwnd)				{m_pEngine->SetVideoContainerHWND(hwnd);}
	virtual HWND GetVideoContainerHWND()						{return m_pEngine->GetVideoContainerHWND();}
	virtual void SetVideoPosition(int x, int y, int cx, int cy)	{m_pEngine->SetVideoPosition(x,y,cx,cy);}

	virtual void SetEngineConfig(MediaEngineConfigOption meco, INT value)	
	{
		m_pEngine->SetConfig(meco, value);
	}
	virtual INT GetEngineConfig(MediaEngineConfigOption meco)				
	{
		return m_pEngine->GetConfig(meco);
	}
	virtual BOOL EngineSupports(MediaEngineConfigOption meco)				{return m_pEngine->Supports(meco);}


	virtual void SetVisContainerHWND(HWND hwnd)					{m_pVis->SetContainerHWND(hwnd);}
	virtual HWND GetVisContainerHWND()							{return m_pVis->GetContainerHWND();}
	virtual void SetVisPosition(int x, int y, int cx, int cy)	{m_pVis->SetPosition(x,y,cx,cy);}

	virtual void StartVis();
	virtual void StopVis()										{m_pVis->Stop();}
	virtual BOOL IsVisActive()									{return m_pVis->IsActive();}

	virtual const Changes& GetChanges();

	virtual INT OnEvent(INT ID, INT param = 0, void* sender = 0);

private:
	BOOL CanAccess(LPCTSTR& path);
	INT GetNext(INT relative);
	INT GetPrevious(INT relative);
	BOOL OpenPlayListItem(INT item);
	//BOOL Open();
private:
	MediaPlayerEngine* m_pEngine;
	MediaPlayList* m_pPlayList;
	MediaVisualization* m_pVis;
	INT m_playListPos;
	MediaPlayerNextMode m_mpnm;
	INT m_mpnmParam;
	ShufflePlayListHelper m_shuffle;
	MediaPlayer::Changes m_changes;
	TCHAR m_tempPathBuffer[MAX_PATH];

};

#endif
