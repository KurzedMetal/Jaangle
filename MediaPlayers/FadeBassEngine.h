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
#ifndef _FadeBassEngine_h_
#define _FadeBassEngine_h_

#include "EventTarget.h"
#include "MediaPlayerEngine.h"
#include "BassEngine.h"

class FadeBassEngine:public MediaPlayerEngine, public EventTarget
{
public:
	FadeBassEngine();
	virtual ~FadeBassEngine();

	virtual void SetConfig(MediaEngineConfigOption meco, INT value);
	virtual INT GetConfig(MediaEngineConfigOption meco);
	virtual BOOL Supports(MediaEngineConfigOption meco);

	virtual void SetEventTarget(EventTarget* evTarg)	{m_pEvTarget = evTarg;}
	virtual EventTarget* GetEventTarget()				{return m_pEvTarget;}

	//This will be The MediaPlayer Engine
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

	virtual BOOL CanPlay(LPCTSTR str);
	virtual MediaEngineState GetEngineState();
	virtual MediaEngineErrorEnum GetLastError();

	virtual BOOL IsVideo()						{return FALSE;}
	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen);

	virtual const TCHAR* GetEngineDescription()		{return _T("Bass-Fade");}


	virtual INT OnEvent(INT ID, INT param = 0, void* sender = 0);

	virtual void SetVideoContainerHWND(HWND hwnd)						{}
	virtual HWND GetVideoContainerHWND()									{return 0;}
	virtual void SetVideoPosition(int x, int y, int cx, int cy)	{};


private:
	enum EngineStateEnum
	{
		ES_Active,
		ES_Activating,
		ES_Deactive,
		ES_Deactivating,
		ES_Last
	};
	EngineStateEnum m_engStatus[2];
	BOOL m_bSendMPE_ReachedEnd[2];
	BassEngine m_bass[2];
	INT m_EngineInUse;

	static LRESULT CALLBACK WindowProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WinProc(UINT message, WPARAM wParam, LPARAM lParam);

	void PlayStateChanged();
	BOOL InitEngine(BassEngine& eng);
	BOOL m_bBassInitialized;
	INT m_volume;
	BOOL m_bMute;
	MediaEngineState m_EngineState;
	MediaEngineErrorEnum m_LastError;
	EventTarget* m_pEvTarget;

	HWND m_hwndMessages;
	BOOL m_bStream;

	BOOL m_bCrossfadeEnabled;
	DOUBLE m_crossFadeAtStart;
	DOUBLE m_crossFadeAtEnd;


};

#endif
