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
#ifndef _MediaPlayer_h_
#define _MediaPlayer_h_

#define INVALID_INDEX -1
#include "MediaPlayerEngine.h"
#include "MediaPlayList.h"
#include "MediaVisualization.h"

enum MediaPlayerNextMode
{
	MPNM_Normal,
	MPNM_Shuffle,
	MPNM_Loop,
	MPNM_AutoContinue,
	MPNM_Last
};


class MediaPlayer
{
public:
	MediaPlayer()								{}
	virtual ~MediaPlayer()						{}

	virtual BOOL CanPlay(LPCTSTR url)									= 0;
	virtual BOOL Play(const MediaPlayListItem& mpli)					= 0;
	virtual BOOL Enqueue(const MediaPlayListItem& mpli)					= 0;
	virtual BOOL InsertMedia(const MediaPlayListItem& mpli, INT index)	= 0;

	//virtual BOOL Open()									= 0;
	virtual void Start()								= 0;
	virtual void Pause()								= 0;
	virtual void Stop()									= 0;

	virtual BOOL Next()									= 0;
	virtual BOOL Previous()								= 0;

	virtual INT GetPlayListPos()						= 0;
	virtual BOOL SetPlayListPos(INT index)				= 0;

	virtual void ClearPlayList()										= 0;
	virtual BOOL GetPlayListItem(MediaPlayListItem& mpli, INT index)	= 0;
	virtual BOOL UpdatePlayListItem(const MediaPlayListItem& mpli, INT index) = 0;
	virtual BOOL RemovePlayListItem(INT index)							= 0;
	virtual UINT GetPlayListCount()										= 0;
	virtual BOOL SwapPlayListItems(INT index1, INT index2)				= 0;

	virtual PlayState GetPlayState()		= 0;
	virtual DOUBLE GetMediaLength()			= 0;
	virtual DOUBLE GetMediaPos()			= 0;
	virtual void SetMediaPos(DOUBLE secs)	= 0;

	virtual void SetVolume(INT volume)		= 0;
	virtual INT GetVolume()					= 0;
	virtual void SetMute(BOOL bMute)		= 0;
	virtual BOOL GetMute()					= 0;

	virtual BOOL IsVideo() 					= 0;

	virtual void SetVideoContainerHWND(HWND hwnd)					= 0;
	virtual HWND GetVideoContainerHWND()							= 0;
	virtual void SetVideoPosition(int x, int y, int cx, int cy)		= 0;

	virtual void SetEngineConfig(MediaEngineConfigOption meco, INT value)	= 0;
	virtual INT GetEngineConfig(MediaEngineConfigOption meco)				= 0;
	virtual BOOL EngineSupports(MediaEngineConfigOption meco)				= 0;

	virtual void SetNextMode(MediaPlayerNextMode mpnm, INT param)			= 0;
	virtual MediaPlayerNextMode GetNextMode(INT* param)						= 0;

	virtual void SetVisContainerHWND(HWND hwnd)					= 0;
	virtual HWND GetVisContainerHWND()							= 0;
	virtual void SetVisPosition(int x, int y, int cx, int cy)	= 0;

	virtual void StartVis()										= 0;
	virtual void StopVis()										= 0;
	virtual BOOL IsVisActive()									= 0;

	virtual void SetMediaPlayerEngine(MediaPlayerEngine* pEngine)	= 0;
	virtual void SetMediaPlayList(MediaPlayList* pList)				= 0;
	virtual void SetMediaVisualization(MediaVisualization* pVis)	= 0;

	struct Changes
	{
		Changes(){memset(this, 0, sizeof(Changes));}
		UINT tickPlayState;
		UINT tickPlayList;
		UINT tickMediaChanged;
	};
	virtual const Changes& GetChanges()								= 0;

};

#endif
