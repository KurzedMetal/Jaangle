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
#include "stdafx.h"
#include "BassEngine.h"
#include "EventTarget.h"

#include <bass_fx.h>

#ifdef _UNITTESTING
BOOL TestBassEngine()
{
	UNITTESTSTART;
	BassEngine* pEng = new BassEngine;
	UNITTEST(TestGenericEngine(pEng));
	return TRUE;
}
#endif

#define ID_FADEDSTOP 500

void CALLBACK BassEngine::SyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	((BassEngine*) user)->MemberSyncProc(handle, channel, data);
}

void BassEngine::MemberSyncProc(HSYNC handle, DWORD channel, DWORD data)
{
	if (m_hwndMessages != 0)
	{
		if (handle == m_EndSync)
			::PostMessage(m_hwndMessages, UM_BASS_MSG, MPE_ReachedEnd, 0);
	}
}

BassEngine::BassEngine():
m_stream(0),
m_bMute(FALSE),
m_pEvTarget(NULL),
m_EngineState(MES_Ready),
m_LastError(MEE_NoError),
m_hwndMessages(0),
m_EndSync(0),
m_pos(0.0),
m_bFromEnd(FALSE),
m_Volume(70),
m_meem(MEEM_Disabled),
m_fxEQ(NULL),
m_pBassInst(NULL)
{
	for (INT i=0;i<10;i++)
		m_bandValues[i] = 50;
}

BassEngine::~BassEngine()
{
	Close();
	DestroyWindow(m_hwndMessages);
	delete m_pBassInst;
}

LRESULT CALLBACK BassEngine::WindowProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR ud = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (ud != NULL)
		return ((BassEngine*)ud)->WinProc(uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT BassEngine::WinProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_BASS_MSG && m_pEvTarget != NULL)
		return m_pEvTarget->OnEvent((INT)wParam, (INT)lParam, this);
	
	if (message == WM_TIMER && wParam == ID_FADEDSTOP)
	{
		KillTimer(m_hwndMessages, ID_FADEDSTOP);
		if (m_bFadingOut)
			Stop();
	}
	return 0;
}

BOOL BassEngine::Open(LPCTSTR str)
{
	if (m_pBassInst == NULL)
	{
		m_pBassInst = new BassInstance;
		if (!BassInstance::IsValid())
		{
			SetError(_T("BassEngine::Open. Cannont Initialize Bass Instance"));
			delete m_pBassInst;
			m_pBassInst = NULL;
			return FALSE;
		}
		m_EngineState = MES_Ready;
		m_hwndMessages = CreateWindowEx(0,_T("Message"),0,0,0,0,0,0,HWND_MESSAGE,0,AfxGetInstanceHandle(),0);
		SetWindowLongPtr(m_hwndMessages, GWLP_USERDATA, (__int3264)(LONG_PTR)this);
		SetWindowLongPtr(m_hwndMessages, GWLP_WNDPROC, (__int3264)(LONG_PTR)WindowProcST);
	}
	m_bFadingOut = FALSE;
	Close();
	LPCTSTR filePath = &str[_tcslen(str) - 4];
	//if (_tcsstr(_T(".MO3.IT .XM .S3M.MTM.MOD.UMX"), filePath) != NULL)
	//	m_stream = BASS_MusicLoad(FALSE, CT2W(str), 0, 0, BASS_UNICODE, 0);
	//else
	m_bStream = TRUE;
	INT floatFlag = BassInstance::bUseSampleFloat ? BASS_SAMPLE_FLOAT : 0;
	m_stream = BASS_StreamCreateFile(FALSE, (LPCWSTR)CT2CW(str), 0, 0, BASS_UNICODE | floatFlag);
	if (m_stream == 0)
	{
		m_bStream = FALSE;
		SetError(_T("BassEngine::Open. BASS_StreamCreateFile (stream)"));
		m_stream = BASS_MusicLoad(FALSE, (LPCWSTR)CT2CW(str), 0, 0, BASS_UNICODE | BASS_MUSIC_PRESCAN | floatFlag, 0);
	}
	if (m_stream)
	{
		m_EndSync = BASS_ChannelSetSync(m_stream, BASS_SYNC_END, 0, &(BassEngine::SyncProc), this);
		m_location = str;
		SetEqualizerMode(m_meem);
		SetMute(GetMute());
		PlayStateChanged();
		return TRUE;
	}
	SetError(_T("BassEngine::Open. BASS_StreamCreateFile (music)"));
	return FALSE;

}

void BassEngine::Close()
{
	if (m_stream)
	{
		//TRACEST(_T("BassEngine::Close"));
		m_bFadingOut = FALSE;
		if (m_bStream)
			BASS_StreamFree(m_stream);
		else
			BASS_MusicFree(m_stream);
		SetEqualizerMode(MEEM_Disabled);
		m_stream = 0;
		m_location.clear();
		PlayStateChanged();
	}
}

BOOL BassEngine::Start()
{
	if (m_stream == NULL)							
		return FALSE;
	if (BASS_ChannelIsSliding(m_stream, BASS_ATTRIB_VOL))			
		return FALSE;
	if (!BASS_ChannelPlay(m_stream, FALSE))
	{
		SetError(_T("BassEngine::Start. BASS_ChannelPlay"));
		return FALSE;
	}
	else
		PlayStateChanged();
	return TRUE;
}

BOOL BassEngine::Pause()
{
	if (m_stream == NULL)										
		return FALSE;
	if (BASS_ChannelIsSliding(m_stream, BASS_ATTRIB_VOL))		
		return FALSE;
	if (GetPlayState() != PS_Playing)
		return TRUE;
	if (!BASS_ChannelPause(m_stream))
	{
		SetError(_T("BassEngine::Pause. BASS_ChannelPause"));
		return FALSE;
	}
	PlayStateChanged();
	return TRUE;
}

void BassEngine::SetError(LPCTSTR msg)
{
	ASSERT(msg != NULL);
	INT errCode = BASS_ErrorGetCode();
	TRACE(_T("@0 BassEngine::SetError %s : %d\r\n"), msg, errCode);
	if (m_pEvTarget)
		m_pEvTarget->OnEvent(MPE_Error,0,this);
		//0 BASS_OK  
	//1 BASS_ERROR_MEM  
	//2 BASS_ERROR_FILEOPEN  
	//3 BASS_ERROR_DRIVER  
	//4 BASS_ERROR_BUFLOST  
	//5 BASS_ERROR_HANDLE  
	//6 BASS_ERROR_FORMAT  
	//7 BASS_ERROR_POSITION  
	//8 BASS_ERROR_INIT  
	//9 BASS_ERROR_START  
	//14 BASS_ERROR_ALREADY  
	//18 BASS_ERROR_NOCHAN  
	//19 BASS_ERROR_ILLTYPE  
	//20 BASS_ERROR_ILLPARAM  
	//21 BASS_ERROR_NO3D  
	//22 BASS_ERROR_NOEAX  
	//23 BASS_ERROR_DEVICE  
	//24 BASS_ERROR_NOPLAY  
	//25 BASS_ERROR_FREQ  
	//27 BASS_ERROR_NOTFILE  
	//29 BASS_ERROR_NOHW  
	//31 BASS_ERROR_EMPTY  
	//32 BASS_ERROR_NONET  
	//33 BASS_ERROR_CREATE  
	//34 BASS_ERROR_NOFX  
	//37 BASS_ERROR_NOTAVAIL  
	//38 BASS_ERROR_DECODE  
	//39 BASS_ERROR_DX  
	//40 BASS_ERROR_TIMEOUT  
	//41 BASS_ERROR_FILEFORM  
	//42 BASS_ERROR_SPEAKER  
	//43 BASS_ERROR_VERSION  
	//44 BASS_ERROR_CODEC  
	//45 BASS_ERROR_ENDED  
	//-1 BASS_ERROR_UNKNOWN 

}

void BassEngine::SetEventTarget(EventTarget* evTarg)
{
	m_pEvTarget = evTarg;
}


BOOL BassEngine::Stop()
{
	if (m_stream == NULL)						
		return FALSE;
	m_bFadingOut = FALSE;
	if (!BASS_ChannelStop(m_stream))
	{
		SetError(_T("BassEngine::Stop. BASS_ChannelStop"));
		return FALSE;
	}
	SetMediaPos(0.0);
	PlayStateChanged();
	return TRUE;
}

PlayState BassEngine::GetPlayState()
{
	if (m_stream == NULL)
		return PS_Closed;
	switch(BASS_ChannelIsActive(m_stream))
	{
	case BASS_ACTIVE_STOPPED:	return PS_Stopped;//PS_Ready;
	case BASS_ACTIVE_PLAYING:	return PS_Playing;
	case BASS_ACTIVE_PAUSED:	return PS_Paused;
	case BASS_ACTIVE_STALLED:	return PS_Stalled;
	}
	return PS_Closed;
}

INT BassEngine::GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)
{
	if(m_stream == NULL)
		return 0;
	BASS_CHANNELINFO inf;
	BASS_ChannelGetInfo(m_stream, &inf);
	switch (meoii)
	{
	case MEOII_Channels: return inf.chans;
	case MEOII_Bitrate: return inf.origres;
	case MEOII_Frequency: return inf.freq;
	}
	return 0;
}
LPCTSTR BassEngine::GetLocation()
{
	if(m_stream == NULL)
		return _T("");
	return m_location.c_str();
}
DOUBLE BassEngine::GetMediaLength()
{
	if(m_stream == NULL)
		return 0.0;
	QWORD len = BASS_ChannelGetLength(m_stream, BASS_POS_BYTE);
	if (len == -1)
	{
		SetError(_T("BassEngine::GetMediaLength BASS_ChannelGetLength"));
		return 0.0;
	}
	DOUBLE secs = BASS_ChannelBytes2Seconds(m_stream, len);
	if (secs < 0.0)
	{
		SetError(_T("BassEngine::GetMediaLength BASS_ChannelBytes2Seconds"));
		return 0.0;
	}
	return secs;
}

DOUBLE BassEngine::GetMediaPos()
{
	DOUBLE f = 0.0;
	if(m_stream != NULL && !BASS_ChannelIsSliding(m_stream, BASS_ATTRIB_VOL))
	{
		QWORD pos = BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE);
		if (pos != -1)
		{
			f = BASS_ChannelBytes2Seconds(m_stream, pos);
			if (f < 0.0)
			{
				SetError(_T("BassEngine::GetMediaPos BASS_ChannelBytes2Seconds"));
				f = 0.0;
			}
		}
		else
			SetError(_T("BassEngine::GetMediaPos BASS_ChannelGetPosition"));
	}
	return f;
}

void BassEngine::SetMediaPos(DOUBLE secs)
{
	if(m_stream != NULL && !BASS_ChannelIsSliding(m_stream, BASS_ATTRIB_VOL))
	{
		QWORD bytes = BASS_ChannelSeconds2Bytes(m_stream, (float)secs);
		if (bytes != -1)
		{
			if (!BASS_ChannelSetPosition(m_stream, bytes, BASS_POS_BYTE))
				SetError(_T("BassEngine::SetMediaPos BASS_ChannelSetPosition"));
		}
		else
			SetError(_T("BassEngine::SetMediaPos BASS_ChannelSeconds2Bytes"));
	}
}

void BassEngine::SetVolume(INT volume)
{
	ASSERT(volume>=0 && volume <= 100);
	if (volume < 0)		volume = 0;
	if (volume > 100)	volume = 100;
	m_Volume = volume;
	if (m_bMute == FALSE)
		SetRealVolume(m_Volume);
}

void BassEngine::SetRealVolume(INT volume)
{
	ASSERT(volume>=0 && volume <= 100);
	if (volume < 0)		volume = 0;
	if (volume > 100)	volume = 100;
	if (m_stream == NULL)					return;
	if (BASS_ChannelIsSliding(m_stream, BASS_ATTRIB_VOL))	return;
	BOOL ret = BASS_ChannelSetAttribute(m_stream, BASS_ATTRIB_VOL, volume / 100.0f);
	if (!ret)
		SetError(_T("BassEngine::SetRealVolume"));
}

INT BassEngine::GetVolume()
{
	return m_Volume;
}
void BassEngine::SetMute(BOOL bMute)
{
	m_bMute = bMute;
	SetRealVolume(bMute ? 0 : m_Volume);

}
BOOL BassEngine::GetMute()
{
	return m_bMute;
}

void BassEngine::PlayStateChanged()
{
	//TRACE(_T("@3 BassEngine::PlayStateChanged Sending MPE_PlayStateChanged\n"));
	if (m_pEvTarget) 
		m_pEvTarget->OnEvent(MPE_PlayStateChanged, 0, this);
}

BOOL BassEngine::CanPlay(LPCTSTR str)
{
	if (m_pBassInst == NULL)
		m_pBassInst = new BassInstance;
	return BassInstance::IsFileSupported(str);
}
MediaEngineState BassEngine::GetEngineState()
{
	return m_EngineState;
}
MediaEngineErrorEnum BassEngine::GetLastError()
{
	return m_LastError;
}

BOOL BassEngine::GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)
{
	if (m_stream == 0)	return FALSE;
	switch (mesd)
	{
	case MSD_Float:
		BASS_ChannelGetData(m_stream, buffer, bufferLen | BASS_DATA_FLOAT);
		break;
	case MSD_FFT:
		{
			DWORD flag = NULL;
			if (bufferLen == 4096)
				flag = BASS_DATA_FFT2048;
			if (flag != NULL)
				BASS_ChannelGetData(m_stream, buffer, flag);
		}
		break;
	case MSD_16BitRaw:
		BASS_ChannelGetData(m_stream, buffer, bufferLen);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void BassEngine::FadedStart(DOUBLE fadeLength)
{
	if (m_stream != 0 && !BASS_ChannelIsSliding(m_stream, BASS_ATTRIB_VOL))
	{
		if (fadeLength == 0)
			Start();
		else
		{
			INT vol = GetVolume();
			SetVolume(0);	
			BASS_ChannelSlideAttribute(m_stream, BASS_ATTRIB_VOL, vol / 100.0f, DWORD(fadeLength * 1000));
		}
	}
}
void BassEngine::FadedStop(DOUBLE fadeLength)
{
	if (m_stream != 0)
	{
		if (fadeLength == 0)
			Stop();
		else
		{
			m_bFadingOut = TRUE;
			BASS_ChannelSlideAttribute(m_stream, BASS_ATTRIB_VOL, 0, DWORD(fadeLength * 1000));
			DOUBLE f = BASS_ChannelBytes2Seconds(m_stream, BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE));
			QWORD stopPos = BASS_ChannelGetPosition(m_stream, BASS_POS_BYTE) + 
				BASS_ChannelSeconds2Bytes(m_stream, float(fadeLength + 1.0));
			KillTimer(m_hwndMessages, ID_FADEDSTOP);
			SetTimer(m_hwndMessages, ID_FADEDSTOP, DWORD(fadeLength * 1000), NULL);
			//BASS_ChannelRemoveSync(m_stream, m_PosSync);
			//m_PosSync = BASS_ChannelSetSync(m_stream, 
			//	BASS_SYNC_POS|BASS_SYNC_MIXTIME, stopPos, &(BassEngine::SyncProc), (DWORD)this);
		}
	}
}

void BassEngine::SetEqualizerMode(MediaEngineEqualizerMode meem)
{
	if (m_stream == NULL)
		return;
	if (m_fxEQ != NULL)
	{
		BASS_ChannelRemoveFX(m_stream, m_fxEQ);
		m_fxEQ = NULL;
	}
	if (meem == MEEM_Disabled)
		return;
	m_fxEQ = BASS_ChannelSetFX(m_stream, BASS_FX_BFX_PEAKEQ, 0);
	if (m_fxEQ == NULL)
	{
		//TRACE(_T("@1 BassEngine::SetEqualizerMode BASS_ChannelSetFX"));
		SetError(_T("BassEngine::SetEqualizerMode BASS_ChannelSetFX"));
		return;
	}
	BASS_BFX_PEAKEQ eq;
	eq.lChannel=BASS_BFX_CHANALL;
	eq.fQ = 0;
	const float c3ChannelBandFreq[] = {125.0f, 1000.0f, 8000.0f};
	const float c10ChannelBandFreq[] = {32.0f, 64.0f, 125.0f, 250.0f, 500.0f, 
		1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f};
	switch (meem)
	{
	case MEEM_3Channel:
		{
			eq.fBandwidth = 2.5f;
			for (INT i = 0; i < 3; i++)
			{
				eq.fGain = MediaPlayerToBassBandValues(m_bandValues[i]);
				eq.lBand = i;
				eq.fCenter = c3ChannelBandFreq[i];
				BASS_FXSetParameters(m_fxEQ, &eq);
			}
		}
		break;
	case MEEM_10Channel:
		{
			eq.fBandwidth = 1.0f;
			for (INT i = 0; i < 10; i++)
			{
				eq.fGain = MediaPlayerToBassBandValues(m_bandValues[i]);
				eq.lBand = i;
				eq.fCenter = c10ChannelBandFreq[i];
				BASS_FXSetParameters(m_fxEQ, &eq);
			}

		}
		break;
	default:
		ASSERT(0);
	}
}
void BassEngine::SetEqualizerBandValue(INT band, INT newVal)
{
	ASSERT(band >= 0 && band < 10);
	if (!(band >= 0 && band < 10))
		return;
	if (newVal < 0)
		newVal = 0;
	if (newVal > 100)
		newVal = 100;
	m_bandValues[band] = newVal;
	if (m_stream == NULL || m_fxEQ == NULL)
		return;
	BASS_BFX_PEAKEQ eq;
	eq.lBand = band;
	BASS_FXGetParameters(m_fxEQ, &eq);
	eq.fGain = MediaPlayerToBassBandValues(newVal);
	BASS_FXSetParameters(m_fxEQ, &eq);
}

FLOAT BassEngine::MediaPlayerToBassBandValues(INT mpValue)
{
	FLOAT ret = (float)((mpValue * 31) / 100.0f -  (31 / 2.0f));
	if (ret > 15.0f)
		ret = 15.0f;
	if (ret < -15.0f)
		ret = -15.0f;
	return ret;
}


void BassEngine::SetConfig(MediaEngineConfigOption meco, INT value)
{
	if (meco == MECO_EqualizerMode)
	{
		MediaEngineEqualizerMode meem = (MediaEngineEqualizerMode) value;
		ASSERT(meem >= MEEM_Disabled && meem < MEEM_Last);
		if (meem < MEEM_Last && m_meem != meem)
		{
			m_meem = meem;
			SetEqualizerMode(m_meem);
		}
	}

	if (meco >= MECO_EqualizerBandStart && meco <= MECO_EqualizerBandEnd)
	{
		SetEqualizerBandValue(meco - MECO_EqualizerBandStart, value);
	}
}	
INT BassEngine::GetConfig(MediaEngineConfigOption meco)
{
	if (meco == MECO_EqualizerMode)
		return m_meem;
	if (meco >= MECO_EqualizerBandStart && meco <= MECO_EqualizerBandEnd)
		return m_bandValues[meco - MECO_EqualizerBandStart];
	if (meco == MECO_VolumeLevel && m_stream != NULL)
	{
		DWORD lev = BASS_ChannelGetLevel(m_stream);
		if (lev != (DWORD)-1)
		{
			INT lLev = INT((LOWORD(lev) * 100)/32768);
			INT rLev = INT((HIWORD(lev) * 100)/32768);
			return MAKELONG(lLev, rLev);
		}
	}
	return 0;
}
BOOL BassEngine::Supports(MediaEngineConfigOption meco)
{
	switch (meco)
	{
	case MECO_EqualizerMode:
	case MECO_VolumeLevel:
		return TRUE;
	default:
		if (meco >= MECO_EqualizerBandStart && meco <= MECO_EqualizerBandEnd)
			return TRUE;
	}
	return FALSE;
}



