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
#ifndef _MediaPlayerEngine_h_
#define _MediaPlayerEngine_h_

enum PlayState
{
	PS_First,
	PS_Closed,
	PS_Playing,
	PS_Stopped,
	PS_Paused,
	PS_Preparing,
	PS_Stalled,
	PS_Last
};

enum MediaEngineState
{
	MES_NotInitialized,
	MES_Ready,
	MES_Error,
	MES_Last
};

enum MediaEngineErrorEnum
{
	MEE_NoError
};

enum MediaEngineSoundData
{
	MSD_Float,
	MSD_FFT,
	MSD_8BitRaw,
	MSD_16BitRaw
};
enum MediaEngineEqualizerMode
{
	MEEM_Disabled,
	MEEM_3Channel,
	MEEM_10Channel,
	MEEM_Last
};
#define MEDIAENGINE_MAX_BANDS 10
enum MediaEngineConfigOption
{
	MECO_First,
	MECO_Crossfade,				//0: Disabled 1:Enabled
	MECO_CrossfadeAtStart,		//Crossfade mix time in ms
	MECO_CrossfadeAtEnd,		//Crossfade mix time in ms
	MECO_EqualizerMode,			//0=Disabled, 1 = 3-Channel, 2 = 8-Channel
	MECO_EqualizerBandStart,	//Equalizer Band 0 (Depends in the mode) (Valid value 0-100)
	MECO_EqualizerBandEnd = MECO_EqualizerBandStart + MEDIAENGINE_MAX_BANDS,
	MECO_VolumeLevel,			//Returns Left Level (0-100) in LOWORD (R Level in HIWORD) Read Only Property. Returns 0 on any error.
	MECO_Last
};


enum MediaEngineOptionalIntInfoEnum
{
	MEOII_Channels,
	MEOII_Frequency,
	MEOII_Bitrate,
	MEOII_Last
};

class EventTarget;



class MediaPlayerEngine
{
public:
	enum Events
	{
		MPE_Error,
		MPE_ReachedEnd,
		MPE_PlayStateChanged,
		MPE_Last
	};

	MediaPlayerEngine()						{}
	virtual ~MediaPlayerEngine()			{}

	virtual void SetConfig(MediaEngineConfigOption meco, INT value)	= 0;
	virtual INT GetConfig(MediaEngineConfigOption meco)				= 0;
	virtual BOOL Supports(MediaEngineConfigOption meco)				= 0;

	virtual void SetEventTarget(EventTarget* evTarg)	= 0;
	virtual EventTarget* GetEventTarget()				= 0;

	virtual BOOL Open(LPCTSTR str)			= 0;
	virtual void Close()					= 0;

	virtual BOOL Start()					= 0;
	virtual BOOL Pause()					= 0;
	virtual BOOL Stop()						= 0;

	virtual PlayState GetPlayState()		= 0;

	virtual INT GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)	= 0;
	virtual LPCTSTR GetLocation()			= 0;
	virtual DOUBLE GetMediaLength()			= 0;


	virtual DOUBLE GetMediaPos()			= 0;
	virtual void SetMediaPos(DOUBLE secs)	= 0;

	virtual void SetVolume(INT volume)		= 0;
	virtual INT GetVolume()					= 0;
	virtual void SetMute(BOOL bMute)		= 0;
	virtual BOOL GetMute()					= 0;

	virtual BOOL CanPlay(LPCTSTR str)		= 0;
	virtual MediaEngineState GetEngineState()		= 0;
	virtual MediaEngineErrorEnum GetLastError()		= 0;

	virtual BOOL IsVideo()							= 0;
	virtual const TCHAR* GetEngineDescription()		= 0;

	virtual void SetVideoContainerHWND(HWND hwnd)						= 0;
	virtual HWND GetVideoContainerHWND()									= 0;
	virtual void SetVideoPosition(int x, int y, int cx, int cy)	= 0;

	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen) = 0;

};

#ifdef _UNITTESTING
BOOL TestGenericEngine(MediaPlayerEngine* pEng);
#endif

#endif
