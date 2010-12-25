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
#ifndef _MultiEngine_h_
#define _MultiEngine_h_

#include "MediaPlayerEngine.h"
#include "EventTarget.h"

class MultiEngine:public MediaPlayerEngine, public EventTarget
{
public:
	MultiEngine();
	virtual ~MultiEngine();

	virtual void SetConfig(MediaEngineConfigOption meco, INT value);
	virtual INT GetConfig(MediaEngineConfigOption meco);
	virtual BOOL Supports(MediaEngineConfigOption meco);

	virtual void SetEventTarget(EventTarget* pEvTarg);
	virtual EventTarget* GetEventTarget()				{return m_pEvTarget;}

	virtual BOOL Open(LPCTSTR str);
	virtual void Close();
	//Playback Commands
	virtual BOOL Start();
	virtual BOOL Pause();
	virtual BOOL Stop();
	virtual PlayState GetPlayState();

	//Total Media Length in seconds
	virtual INT GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii);
	virtual LPCTSTR GetLocation();
	virtual DOUBLE GetMediaLength();


	//Media Position is seconds
	virtual DOUBLE GetMediaPos();
	virtual void SetMediaPos(DOUBLE secs);

	//Settings
	//Should be between 0-100
	virtual void SetVolume(INT volume);
	virtual INT GetVolume();
	virtual void SetMute(BOOL bMute);
	virtual BOOL GetMute();

	//Extra Interface
	//MultiEngine gets the responsibility to clear it
	void AddEngine(MediaPlayerEngine* mpe);
	void RemoveAllEngines();

	virtual INT OnEvent(INT ID, INT param = 0, void* sender = 0);

	virtual BOOL CanPlay(LPCTSTR str);
	virtual MediaEngineState GetEngineState();
	virtual MediaEngineErrorEnum GetLastError();

	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen);

	//Visualization
	virtual BOOL IsVideo();
	virtual const TCHAR* GetEngineDescription();

	virtual void SetVideoContainerHWND(HWND hwnd);
	virtual HWND GetVideoContainerHWND()						{return m_hwndVideo;}
	virtual void SetVideoPosition(int x, int y, int cx, int cy);

private:
	HWND m_hwndVideo;
	void SetError(LPCTSTR msg);
	void PlayStateChanged();

	EventTarget* m_pEvTarget;
	std::vector<MediaPlayerEngine*> m_pEngines;
	MediaPlayerEngine* m_pActiveEngine;
	//HWND m_hwndViewers;
	TCHAR m_engDesc[100];
	UINT m_volume;
	BOOL m_bMute;
	//INT m_ActiveEngine;

};

#endif
