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
#ifndef _BassEngine_h_
#define _BassEngine_h_

#include "MediaPlayerEngine.h"
#include "BassInstance.h"
#include <bass.h>


#define UM_BASS_MSG WM_USER + 555
//Check http://www.xs4all.nl/~mon/download.htm for sample music (12k)

#ifdef _UNITTESTING
BOOL TestBassEngine();
#endif

class BassEngine:public MediaPlayerEngine
{
public:
	BassEngine();
	virtual ~BassEngine();

	virtual void SetConfig(MediaEngineConfigOption meco, INT value);
	virtual INT GetConfig(MediaEngineConfigOption meco);
	virtual BOOL Supports(MediaEngineConfigOption meco);

	virtual void SetEventTarget(EventTarget* evTarg);
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

	virtual const TCHAR* GetEngineDescription()		{return _T("Bass");}

	virtual void SetVideoContainerHWND(HWND hwnd)						{}
	virtual HWND GetVideoContainerHWND()									{return 0;}
	virtual void SetVideoPosition(int x, int y, int cx, int cy)	{};

	//Extra IF;
	//void NotifyOnPosition(DOUBLE pos, BOOL bFromEnd);
	void FadedStart(DOUBLE fadeLength);
	void FadedStop(DOUBLE fadeLength);

private:
	void SetRealVolume(INT volume);
	void SetError(LPCTSTR msg);
	void PlayStateChanged();
	static LRESULT CALLBACK WindowProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WinProc(UINT message, WPARAM wParam, LPARAM lParam);
	BassInstance* m_pBassInst;

	HSTREAM m_stream;
	HSYNC m_EndSync;
	//HSYNC m_PosSync;
	//BOOL m_bInitialized;
	//MediaInfo m_mf;
	std::tstring m_location;
	BOOL m_bMute;
	INT m_Volume;
	MediaEngineState m_EngineState;
	MediaEngineErrorEnum m_LastError;
	BOOL m_bFadingOut;



	void MemberSyncProc(HSYNC handle, DWORD channel, DWORD data);
	EventTarget* m_pEvTarget;
	//void HandleBassError();

	static void CALLBACK SyncProc(HSYNC handle, DWORD channel, DWORD data, void* user);
	HWND m_hwndMessages;
	BOOL m_bStream;
	DOUBLE m_pos;
	BOOL m_bFromEnd;

	//===Equalizer
	MediaEngineEqualizerMode m_meem;
	HFX m_fxEQ;
	INT m_bandValues[10];
	void SetEqualizerMode(MediaEngineEqualizerMode meem);
	void SetEqualizerBandValue(INT band, INT newVal);
	FLOAT MediaPlayerToBassBandValues(INT mpValue);




};

#endif
