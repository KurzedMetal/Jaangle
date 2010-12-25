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

#include <bass.h>

class BassVisualization;

class BassEngine:public MediaPlayerEngine
{
public:
	BassEngine();
	virtual ~BassEngine();

	//virtual BOOL Init(HWND msgTarget, HWND viewer);

	virtual BOOL Init(HWND hwndViewer = 0);
	virtual void SetEventTarget(EventTarget* evTarg)	{m_pEvTarget = evTarg;}

	//This will be The MediaPlayer Engine
	virtual BOOL Open(LPCTSTR str);
	virtual void Close();
	//Playback Commands
	virtual void Start();
	virtual void Pause();
	virtual void Stop();
	virtual PlayState GetPlayState();

	//Total Media Length in seconds
	virtual MediaInfo& GetMediaInfo();

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
	virtual MediaEngineError GetLastError();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual BOOL IsVideo()						{return FALSE;}
	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen);

	virtual const TCHAR* GetEngineDescription()		{return _T("Bass");}

	//Visualization
	//virtual BOOL HasVisualization();
	//virtual void ShowVisualization(BOOL bShow);
	//virtual BOOL IsVisualizationActive();

private:
	void SetError(LPCTSTR msg);
	void PlayStateChanged();
	void CloseStream(HSTREAM stream, BOOL bIsStream);

	HSTREAM m_stream;
	HSTREAM m_stream2;

	HSYNC m_SyncEnd;
	HSYNC m_SyncPos;
	HSYNC m_SyncEnd2;
	HSYNC m_SyncPos2;

	BOOL m_bBassInitialized;
	MediaInfo m_mf;
	INT m_MutedVolume;
	MediaEngineState m_EngineState;
	MediaEngineError m_LastError;


	static void CALLBACK SyncProc(HSYNC handle, DWORD channel, DWORD data, DWORD user);
	void MemberSyncProc(HSYNC handle, DWORD channel, DWORD data);
	EventTarget* m_pEvTarget;

	HWND m_viewer;
	BOOL m_bIsStream;
	BOOL m_bIsStream2;
	INT m_LastBassErrorCode;
	BOOL m_bFirstChannelInUse;
};

#endif
