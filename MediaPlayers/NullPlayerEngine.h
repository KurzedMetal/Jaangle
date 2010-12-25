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
#ifndef _NullPlayerEngine_h_
#define _NullPlayerEngine_h_

#include "MediaPlayerEngine.h"

class NullPlayerEngine: public MediaPlayerEngine
{
public:
	virtual ~NullPlayerEngine()											{}
	virtual void SetEventTarget(EventTarget* evTarg)					{ASSERT(0);}
	virtual EventTarget* GetEventTarget()								{ASSERT(0);return NULL;}


	virtual void SetConfig(MediaEngineConfigOption meco, INT value)		{}
	virtual INT GetConfig(MediaEngineConfigOption meco)					{return -1;}
	virtual BOOL Supports(MediaEngineConfigOption meco)					{return FALSE;}


	virtual BOOL Open(LPCTSTR str)			{ASSERT(0);return TRUE;}
	virtual void Close()					{}

	virtual BOOL Start()					{return FALSE;}
	virtual BOOL Pause()					{return FALSE;}
	virtual BOOL Stop()						{return FALSE;}

	virtual PlayState GetPlayState()		{return PS_Closed;}

	virtual INT GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii);
	virtual LPCTSTR GetLocation();
	virtual DOUBLE GetMediaLength();


	virtual DOUBLE GetMediaPos()			{return 0.0;}
	virtual void SetMediaPos(DOUBLE secs)	{}

	virtual void SetVolume(INT volume)		{}
	virtual INT GetVolume()					{return 0;}
	virtual void SetMute(BOOL bMute)		{}
	virtual BOOL GetMute()					{return FALSE;}

	virtual BOOL CanPlay(LPCTSTR str)		{return FALSE;}
	virtual MediaEngineState GetEngineState()	{return MES_Ready;}
	virtual MediaEngineErrorEnum GetLastError()		{return MEE_NoError;}

	//Visualization
	virtual BOOL IsVideo()						{return FALSE;}
	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)	{return FALSE;}

	virtual void SetVideoContainerHWND(HWND hwnd)						{}
	virtual HWND GetVideoContainerHWND()									{return 0;}
	virtual void SetVideoPosition(int x, int y, int cx, int cy)	{};


private:
	//MediaInfo mf;

};

#endif
