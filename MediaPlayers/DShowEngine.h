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
#ifndef _DShowEngine_h_
#define _DShowEngine_h_

#include "MediaPlayerEngine.h"

struct IGraphBuilder;
struct IMediaSeeking;
struct IMediaControl;
struct IMediaEventEx;
struct IMediaPosition;
struct IVideoWindow;
struct IBasicAudio;
struct IBaseFilter;
struct IVMRWindowlessControl;
struct IFilterGraph;

#ifdef _UNITTESTING
BOOL TestDShowEngine();
#endif



class DShowEngine:public MediaPlayerEngine
{
	friend class CDShowWnd;
public:
	DShowEngine();
	virtual ~DShowEngine();

	virtual void SetConfig(MediaEngineConfigOption meco, INT value)		{}
	virtual INT GetConfig(MediaEngineConfigOption meco)					{return -1;}
	virtual BOOL Supports(MediaEngineConfigOption meco)					{return FALSE;}

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

	virtual BOOL IsVideo()				{return m_bIsVideo;}
	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)	{return FALSE;}
	virtual const TCHAR* GetEngineDescription()		{return _T("Direct Show");}

	virtual void SetVideoContainerHWND(HWND hwnd);
	virtual HWND GetVideoContainerHWND();
	virtual void SetVideoPosition(int x, int y, int cx, int cy);

private:
	HWND GetVideoHWND();
	HRESULT InitializeWindowlessVMR(
		HWND hwndApp,         // Application window.
		IFilterGraph* pFG,    // Pointer to the Filter Graph Manager.
		IVMRWindowlessControl** ppWc,  // Receives the interface.
		DWORD dwNumStreams,  // Number of streams to use.
		BOOL fBlendAppImage  // Are we alpha-blending a bitmap?
		);

	HWND m_hwndVideoContainer;
	HWND m_hwndVideo;
	HWND m_hwndVideoHiddenContainer;

	void InitWindow();
	void SetRealVolume(INT volume);
	//HWND m_hwndVideo;
	MediaEngineState m_EngineState;
	MediaEngineErrorEnum m_LastError;

	void SetError(LPCTSTR msg);
	void PlayStateChanged();
	//void OnSize(UINT nType, int cx, int cy);

	EventTarget* m_pEvTarget;
	//MediaInfo m_mf;
	std::tstring m_location;
	void OnGraphEvent();
	void OnDisplayModeChanged();
	BOOL OnPaint();


	IGraphBuilder*	m_pGraphBuilder;
	IMediaSeeking* m_pMediaSeeking;
	IMediaControl*	m_pMediaControl;
	IMediaEventEx*	m_pMediaEvent;
	IMediaPosition*	m_pMediaPosition;
	IVideoWindow* m_pVideoWindow;
	IBasicAudio* m_pBasicAudio;
	IBaseFilter* m_pVMR;
	IVMRWindowlessControl* m_pWindowlessControl;
	BOOL m_bIsVideo;
	BOOL m_bFullScreen;
	//INT m_mutedVolume;
	BOOL m_bMuted;
	INT m_volume;

	DWORD m_dwCaps;
	//DWORD m_oldStyle;

	static LRESULT CALLBACK WindowProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WinProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//CPlayerWindow m_PlayerWindow;
	//CRect m_oldRect;

};

#endif
