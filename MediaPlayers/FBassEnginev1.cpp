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
#include "BassInstance.h"
//#include "BassLib/BassVisualization.h"

#define UM_BASS_MSG WM_USER + 555
#define CROSSFADING_TIME 4000

void CALLBACK BassEngine::SyncProc(HSYNC handle, DWORD channel, DWORD data, DWORD user)
{
	((BassEngine*) user)->MemberSyncProc(handle, channel, data);
}

void BassEngine::MemberSyncProc(HSYNC handle, DWORD channel, DWORD data)
{
#ifdef _DEBUG
	TRACE(_T("@1tmp BassEngine::MemberSyncProc"));
	if (handle == m_SyncEnd)
	{
		TRACE(_T("@1tmp 1 stream End\r\n"));
		ASSERT(channel == m_stream);
	}
	else if (handle == m_SyncEnd2)
	{
		TRACE(_T("@1tmp 2 stream End\r\n"));
		ASSERT(channel == m_stream2);
	}
	else if (handle == m_SyncPos)
	{
		TRACE(_T("@1tmp 1 stream Pos\r\n"));
		ASSERT(channel == m_stream);
	}
	else if (handle == m_SyncPos2)
	{
		TRACE(_T("@1tmp 2 stream Pos2\r\n"));
		ASSERT(channel == m_stream2);
	}
	else
		TRACE(_T("@1BassEngine::MemberSyncProc Unknown Handle\r\n"));

#endif
	if (m_pEvTarget == NULL || m_viewer == 0)
		return;
	if  (handle == m_SyncEnd || handle == m_SyncEnd2)
	{
		//::PostMessage(m_viewer, UM_BASS_MSG, MPE_ReachedEnd, 0);
		if (channel == m_stream)
		{
			CloseStream(channel, m_bIsStream);
			m_stream = 0;
		}
		else if (channel == m_stream2)
		{
			CloseStream(channel, m_bIsStream2);
			m_stream2 = 0;
		}
		else
			ASSERT(0);
			
	}
	else if  (handle == m_SyncPos || handle == m_SyncPos2)
		::PostMessage(m_viewer, UM_BASS_MSG, MPE_ReachedEnd, 0);
	else
		TRACE(_T("@1BassEngine::MemberSyncProc Unknown Handle\r\n"));
}

BassEngine::BassEngine():
m_bBassInitialized(FALSE),
m_stream(0),
m_stream2(0),
m_bIsStream(FALSE),
m_bIsStream2(FALSE),
m_MutedVolume(-1),
m_pEvTarget(NULL),
m_EngineState(MES_NotInitialized),
m_LastError(MEE_NoError),
m_viewer(0),
m_SyncEnd(0),
m_SyncPos(0),
m_SyncEnd2(0),
m_SyncPos2(0)
{
}

BassEngine::~BassEngine()
{
	CloseStream(m_stream, m_bIsStream);
	CloseStream(m_stream2, m_bIsStream2);
	if (m_bBassInitialized)	BassInstance::Instance()->UnInitBass();
}



BOOL BassEngine::Init(HWND viewer)
{
	m_bBassInitialized  = BassInstance::Instance()->InitBass();
	if (!m_bBassInitialized)
	{
		m_EngineState = MES_Error;
		return FALSE;
	}
	m_viewer = viewer;
	m_EngineState = MES_Ready;
	//Check http://www.xs4all.nl/~mon/download.htm for sample music (12k)
	return TRUE;
}

LRESULT BassEngine::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_BASS_MSG && m_pEvTarget != NULL)
		return m_pEvTarget->OnEvent(wParam);
	return 0;
}

BOOL BassEngine::Open(LPCTSTR str)
{
	ASSERT(m_bBassInitialized);
	Close();
	if (m_bFirstChannelInUse)//Active stream is the 1
	{
		TRACE(_T("@1Switching to 2\r\n"));
		if (m_stream2 != 0)
			CloseStream(m_stream2, m_bIsStream2);
		if (m_stream != 0)
			BASS_ChannelSlideAttributes(m_stream, -1, 0, -101, CROSSFADING_TIME);
	}
	else
	{
		TRACE(_T("@1Switching to 1\r\n"));
		if (m_stream != 0)
			CloseStream(m_stream, m_bIsStream);
		if (m_stream2 != 0)
			BASS_ChannelSlideAttributes(m_stream2, -1, 0, -101, CROSSFADING_TIME);
	}
	m_bFirstChannelInUse = !m_bFirstChannelInUse;



	//Use BASS_STREAM_PRESCAN for ( length is always exact for MP3/MP2/MP1 files when the BASS_STREAM_PRESCAN flag is used )
	
	LPCTSTR filePath = &str[_tcslen(str) - 4];
	//if (_tcsstr(_T(".MO3.IT .XM .S3M.MTM.MOD.UMX"), filePath) != NULL)
	//	m_stream = BASS_MusicLoad(FALSE, CT2W(str), 0, 0, BASS_UNICODE, 0);
	//else
	BOOL bIsStrem = TRUE;
	HSTREAM stream = BASS_StreamCreateFile(FALSE, CT2W(str), 0, 0, BASS_UNICODE);
	if (stream == 0)
	{
		bIsStrem = FALSE;
		stream = BASS_MusicLoad(FALSE, CT2W(str), 0, 0, BASS_UNICODE | BASS_MUSIC_PRESCAN, 0);
	}
	HSYNC syncEnd = 0;
	HSYNC syncPos = 0;


	if (stream)
	{
		//		BASS_SYNC_POS Sync when a channel reaches a position.
		//param : position in bytes (automatically rounded down to nearest sample). data : not used.  
		//		BASS_SYNC_END Sync when a channel reaches the end, including when looping. Note that some MOD musics never reach the end, they may jump to another position first. If the BASS_MUSIC_STOPBACK flag is used with a MOD music (through BASS_MusicLoad or BASS_ChannelSetFlags), then this sync will also be called when a backward jump effect is played.
		//param : not used. data : 1 = the sync is triggered by a backward jump in a MOD music, otherwise not used.  
		//		BASS_SYNC_META Sync when metadata is received in a Shoutcast stream. This sync is also triggered when a new logical-bitstream begins in a chained OGG stream (multiple streams are strung one-after-another), in which case the metadata is the updated OGG tags. (see example below)
		//param : not used. data : pointer to the metadata.  
		//		BASS_SYNC_SLIDE Sync when an attribute slide has completed.
		//param : not used. data : the type of slide completed (one of the BASS_SLIDE_xxx values).  
		//		BASS_SYNC_STALL Sync when playback of the channel is stalled/resumed.
		//param : not used. data : 0 = stalled, 1 = resumed.  
		//		BASS_SYNC_DOWNLOAD Sync when downloading of an internet (or "buffered" user file) stream is done.
		//param : not used. data : not used.  
		//		BASS_SYNC_FREE Sync when a channel is freed. This can be useful when you need to release some resources associated with the channel. Note that you will not be able to use any BASS functions with the channel in the callback (the channel will no longer exist).
		//param : not used. data : not used.  
		//		BASS_SYNC_MUSICPOS Sync when a MOD music reaches an order:row position.
		//param : LOWORD = order (0=first, -1=all), HIWORD = row (0=first, -1=all). data : LOWORD = order, HIWORD = row.
		//
		//		BASS_SYNC_MUSICINST Sync when an instrument (sample for the MOD/S3M/MTM formats) is played in a MOD music (not including retrigs).
		//param : LOWORD = instrument (1=first), HIWORD = note (0=c0...119=b9, -1=all). data : LOWORD = note, HIWORD = volume (0-64).  
		//		BASS_SYNC_MUSICFX Sync when the sync effect is used in a MOD music. The sync effect is E8x or Wxx for the XM/MTM/MOD formats, and S2x for the IT/S3M formats (where x = any value).
		//param : 0 = the position is passed to the callback (data : LOWORD = order, HIWORD = row), 1 = the value of x is passed to the callback (data : x value).  
		syncEnd = BASS_ChannelSetSync(stream, BASS_SYNC_END, 0, &(BassEngine::SyncProc), (DWORD)this);
		QWORD pos = BASS_ChannelGetLength(stream) - BASS_ChannelSeconds2Bytes(stream, CROSSFADING_TIME / 1000.0);
		syncPos = BASS_ChannelSetSync(stream, BASS_SYNC_POS|BASS_SYNC_MIXTIME, pos, &(BassEngine::SyncProc), (DWORD)this);
		
		//if (m_pVisualization)
		//	m_pVisualization->SetBassChannel(m_stream);
		PlayStateChanged();
		//if (m_bVisStarted)
		//{
		//	Sleep(1000);
		//	BASS_WA_SetChannel(m_stream);
		//	BASS_WA_IsPlaying(1);
		//}
	}
	if (m_bFirstChannelInUse)
	{
		m_stream = stream;
		m_bIsStream = bIsStrem;
		m_SyncEnd = syncEnd;
		m_SyncPos = syncPos;
	}
	else
	{
		m_stream2 = stream;
		m_bIsStream2 = bIsStrem;
		m_SyncEnd2 = syncEnd;
		m_SyncPos2 = syncPos;
	}
	return stream != NULL;

}

void BassEngine::Close()
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream != 0)
	{
		BASS_ChannelSlideAttributes(stream, -1, 0, -101, CROSSFADING_TIME);
		//CloseStream(*m_pActiveStream, *m_pbIsStream);
		//*m_pActiveStream = 0;
		m_mf = MediaInfo();
		PlayStateChanged();
	}
}

void BassEngine::CloseStream(HSTREAM stream, BOOL bIsStream)
{
	if (bIsStream)
		BASS_StreamFree(stream);
	else
		BASS_MusicFree(stream);
}

void BassEngine::Start()
{
	ASSERT(m_bBassInitialized);
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	ASSERT(stream != NULL);
	if (!(m_bBassInitialized && stream != NULL))	return;
	if (!BASS_ChannelPlay(stream, FALSE))
		SetError(_T("@1BassEngine::PlayPause. BASS_ChannelPlay"));
	PlayStateChanged();
	//BassVisualization::Instance()->Start(0);

	//StartVis();
}

void BassEngine::Pause()
{
	ASSERT(m_bBassInitialized);
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	ASSERT(stream!= NULL);
	if (!(m_bBassInitialized && stream != NULL))	return;
	if (GetPlayState() != PS_Playing)
		return;
	if (!BASS_ChannelPause(stream))
		SetError(_T("@1BassEngine::Pause. BASS_ChannelPause"));
	PlayStateChanged();
}

void BassEngine::SetError(LPCTSTR msg)
{
	INT errCode = BASS_ErrorGetCode();
	TRACE(msg);
	if (m_pEvTarget)
		m_pEvTarget->OnEvent(MPE_Error);
}

void BassEngine::Stop()
{
	ASSERT(m_bBassInitialized);
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (!(m_bBassInitialized && stream != NULL))	return;
	if (!BASS_ChannelStop(stream))
		SetError(_T("@1BassEngine::PlayPause. BASS_ChannelPlay"));
	SetMediaPos(0.0);
	PlayStateChanged();
}

PlayState BassEngine::GetPlayState()
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream == NULL)
		return PS_Inactive;
	switch(BASS_ChannelIsActive(stream))
	{
	case BASS_ACTIVE_STOPPED:	return PS_Stopped;
	case BASS_ACTIVE_PLAYING:	return PS_Playing;
	case BASS_ACTIVE_PAUSED:	return PS_Paused;
	case BASS_ACTIVE_STALLED:	return PS_Stalled;
	}
	return PS_Inactive;
}

MediaInfo& BassEngine::GetMediaInfo()
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream == NULL)
	{
		m_mf = MediaInfo();
	}
	else
	{
		m_mf.mediaLength = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetLength(stream));
		BASS_CHANNELINFO inf;
		BASS_ChannelGetInfo(stream, &inf);

		m_mf.channels = inf.chans;
		m_mf.freq = inf.freq;
		m_mf.bitrate = inf.origres;
	}
	return m_mf;
}

DOUBLE BassEngine::GetMediaPos()
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream == NULL)
		return 0.0;
	float f = BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream));
	if (f < 0.0)
		f = 0.0;
	return (DOUBLE) f;
}

void BassEngine::SetMediaPos(DOUBLE secs)
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream != NULL)
		BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, (float)secs)); 
}

void BassEngine::SetVolume(INT volume)
{
	ASSERT(volume>=0 && volume <= 100);
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (volume < 0)		volume = 0;
	if (volume > 100)	volume = 100;
	if (stream == NULL)
		return;
	BOOL ret = BASS_ChannelSetAttributes(stream, -1, volume, -101);
	//BOOL ret = BASS_SetVolume(volume);
	if (!ret)
		m_LastBassErrorCode = BASS_ErrorGetCode();
}
INT BassEngine::GetVolume()
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream == NULL)	return 0;

	DWORD volume = 0;
	BOOL ret = BASS_ChannelGetAttributes(stream, NULL, &volume, NULL);
	if (!ret)
		m_LastBassErrorCode = BASS_ErrorGetCode();
	return (INT)volume;
}
void BassEngine::SetMute(BOOL bMute)
{
	if (bMute && m_MutedVolume == -1)
	{
		m_MutedVolume = GetVolume();
		SetVolume(0);
	}
	if (!bMute && m_MutedVolume != -1)
	{
		SetVolume(m_MutedVolume);
		m_MutedVolume = -1;
	}
}
BOOL BassEngine::GetMute()
{
	return m_MutedVolume != -1;
}

void BassEngine::PlayStateChanged()
{
	if (m_pEvTarget) 
		m_pEvTarget->OnEvent(MPE_PlayStateChanged, 0, this);
}

BOOL BassEngine::CanPlay(LPCTSTR str)
{
	return BassInstance::Instance()->IsSupported(str);
}
MediaEngineState BassEngine::GetEngineState()
{
	return m_EngineState;
}
MediaEngineError BassEngine::GetLastError()
{
	return m_LastError;
}

BOOL BassEngine::GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)
{
	HSTREAM stream = m_bFirstChannelInUse ? m_stream : m_stream2;
	if (stream == NULL)	return FALSE;
	switch (mesd)
	{
	case MSD_Float:
		BASS_ChannelGetData(stream, buffer, bufferLen | BASS_DATA_FLOAT);
		break;
	case MSD_FFT:
		{
			DWORD flag = NULL;
			if (bufferLen == 4096)
				flag = BASS_DATA_FFT2048;
			if (flag != NULL)
				BASS_ChannelGetData(stream, buffer, flag);
		}
		break;
	case MSD_16BitRaw:
		BASS_ChannelGetData(stream, buffer, bufferLen);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


