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
#ifndef _HttpLinkEngine_h_
#define _HttpLinkEngine_h_

#include "MediaPlayerEngine.h"

class BassVisualization;

class HttpLinkEngine:public MediaPlayerEngine
{
public:
	HttpLinkEngine();
	virtual ~HttpLinkEngine();

	virtual void SetConfig(MediaEngineConfigOption meco, INT value)		{}
	virtual INT GetConfig(MediaEngineConfigOption meco)					{return -1;}
	virtual BOOL Supports(MediaEngineConfigOption meco)					{return FALSE;}

	virtual void SetEventTarget(EventTarget* evTarg)	{}
	virtual EventTarget* GetEventTarget()								{return NULL;}

	//This will be The MediaPlayer Engine
	virtual BOOL Open(LPCTSTR str);
	virtual void Close();
	//Playback Commands
	virtual BOOL Start();
	virtual BOOL Pause()		{return FALSE;}
	virtual BOOL Stop()			{return FALSE;}
	virtual PlayState GetPlayState()			{return m_ps;}

	//Total Media Length in seconds
	virtual INT GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)	{return 0;}
	virtual LPCTSTR GetLocation()									{return m_URL.c_str();}
	virtual DOUBLE GetMediaLength()									{return 0.0;}


	//Media Position is seconds
	virtual DOUBLE GetMediaPos()				{return 0.0;}
	virtual void SetMediaPos(DOUBLE secs)		{}

	//Settings
	//Should be between 0-100
	virtual void SetVolume(INT volume);
	virtual INT GetVolume()						{return m_volume;}
	virtual void SetMute(BOOL bMute)			{m_mute = bMute;}
	virtual BOOL GetMute()						{return m_mute;}

	virtual BOOL CanPlay(LPCTSTR str);
	virtual MediaEngineState GetEngineState()	{return MES_Ready;}
	virtual MediaEngineErrorEnum GetLastError()		{return MEE_NoError;}

	virtual BOOL IsVideo()						{return TRUE;}
	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)
												{return FALSE;}
	virtual const TCHAR* GetEngineDescription()	{return _T("HttpLink");}

	virtual void SetVideoContainerHWND(HWND hwnd)						{}
	virtual HWND GetVideoContainerHWND()									{return 0;}
	virtual void SetVideoPosition(int x, int y, int cx, int cy)	{};


private:
	std::tstring m_URL;
	PlayState m_ps;
	INT m_volume;
	BOOL m_mute;
};

#endif
