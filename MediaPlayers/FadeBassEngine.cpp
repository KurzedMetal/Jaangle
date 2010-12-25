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
#include "FadeBassEngine.h"
#include "EventTarget.h"
#include "BassInstance.h"
#include "BassEngine.h"
//#include "BassLib/BassVisualization.h"

//#define CROSSFADE_TIME 4.0


FadeBassEngine::FadeBassEngine():
m_bBassInitialized(FALSE),
m_volume(70),
m_bMute(FALSE),
m_pEvTarget(NULL),
m_EngineState(MES_Ready),
m_LastError(MEE_NoError),
m_hwndMessages(0),
m_EngineInUse(0),
m_bCrossfadeEnabled(FALSE),
m_crossFadeAtStart(0.0),
m_crossFadeAtEnd(0.0)
{
	m_engStatus[0] = ES_Deactive;
	m_engStatus[1] = ES_Deactive;
	m_bSendMPE_ReachedEnd[0] = FALSE;
	m_bSendMPE_ReachedEnd[1] = FALSE;

}

FadeBassEngine::~FadeBassEngine()
{
	m_bass[0].Close();
	m_bass[1].Close();
	DestroyWindow(m_hwndMessages);
}

BOOL FadeBassEngine::InitEngine(BassEngine& eng)
{
	if (m_hwndMessages == 0)
	{
		m_hwndMessages = CreateWindowEx(0,_T("Message"),0,0,0,0,0,0,HWND_MESSAGE,0,AfxGetInstanceHandle(),0);
		SetWindowLongPtr(m_hwndMessages, GWLP_USERDATA, (LONG_PTR)this);
		SetWindowLongPtr(m_hwndMessages, GWLP_WNDPROC, (LONG_PTR)WindowProcST);
		SetTimer(m_hwndMessages, 100, 200, NULL);
	}
	eng.SetEventTarget(this);
	eng.SetVolume(m_volume);
	eng.SetMute(m_bMute);

	//if (m_bCrossfadeEnabled && m_crossFadeAtEnd > 0)
	//	eng.NotifyOnPosition(m_crossFadeAtEnd, TRUE);
	return TRUE;
}

//LRESULT FadeBassEngine::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (message == UM_BASS_MSG && m_pEvTarget != NULL)
//	{
//		if (lParam == m_EngineInUse)
//		{
//			TRACE(_T("@3FadeBassEngine::WindowProc Forward Message: %d\r\n"), wParam);
//			if (wParam == MPE_ReachedPos)
//				wParam = MPE_ReachedEnd;
//			m_pEvTarget->OnEvent(wParam);
//		}
//		else if (wParam == MPE_ReachedEnd)
//		{
//			TRACE(_T("@3FadeBassEngine::WindowProc Closing unused engine: %d\r\n"), wParam);
//			ASSERT(m_pBass[lParam] !=0);
//			if (m_pBass[lParam] !=0)
//				m_pBass[lParam]->Close();
//		}
//
//	}
//	return 0;
//}

BOOL FadeBassEngine::Open(LPCTSTR str)
{
	if (m_bCrossfadeEnabled)
		m_EngineInUse = m_EngineInUse == 0 ? 1 : 0;
//	TRACE(_T("@2 FadeBassEngine::Open. Will Use Engine: %d File: %s\r\n"), m_EngineInUse, str);
	if (m_bass[m_EngineInUse].GetEventTarget() != this)
		InitEngine(m_bass[m_EngineInUse]);
	return m_bass[m_EngineInUse].Open(str);
}

void FadeBassEngine::Close()
{
	//TRACE(_T("@D FadeBassEngine::Close Eng: %d\r\n"), m_EngineInUse);
	if (m_bCrossfadeEnabled)
	{
		m_bass[m_EngineInUse].FadedStop(m_crossFadeAtEnd);
		m_engStatus[m_EngineInUse] = ES_Deactivating;
	}
	else
	{
		m_bass[m_EngineInUse].Close();
		m_engStatus[m_EngineInUse] = ES_Deactive;
	}
}

BOOL FadeBassEngine::Start()
{
	m_engStatus[m_EngineInUse] = ES_Active;
	return m_bass[m_EngineInUse].Start();
}

BOOL FadeBassEngine::Pause()
{
	return m_bass[m_EngineInUse].Pause();
}

BOOL FadeBassEngine::Stop()
{
	m_engStatus[m_EngineInUse] = ES_Deactive;
	return m_bass[m_EngineInUse].Stop();
}

PlayState FadeBassEngine::GetPlayState()
{
	return m_bass[m_EngineInUse].GetPlayState();
}

INT FadeBassEngine::GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)
{
	return m_bass[m_EngineInUse].GetMediaInfo(meoii);
}
LPCTSTR FadeBassEngine::GetLocation()
{
	return m_bass[m_EngineInUse].GetLocation();
}
DOUBLE FadeBassEngine::GetMediaLength()
{
	return m_bass[m_EngineInUse].GetMediaLength();
}

DOUBLE FadeBassEngine::GetMediaPos()
{
	return m_bass[m_EngineInUse].GetMediaPos();
}

void FadeBassEngine::SetMediaPos(DOUBLE secs)
{
	m_bass[m_EngineInUse].SetMediaPos(secs);
}

void FadeBassEngine::SetVolume(INT volume)
{
	m_volume = volume;
	m_bass[0].SetVolume(volume);
	m_bass[1].SetVolume(volume);
}
INT FadeBassEngine::GetVolume()
{
	return m_volume;
}
void FadeBassEngine::SetMute(BOOL bMute)
{
	m_bass[0].SetMute(bMute);
	m_bass[1].SetMute(bMute);
	m_bMute = bMute;
}
BOOL FadeBassEngine::GetMute()
{
	return m_bMute;
}

BOOL FadeBassEngine::CanPlay(LPCTSTR str)
{
	return BassInstance::IsFileSupported(str);
}
MediaEngineState FadeBassEngine::GetEngineState()
{
	return m_EngineState;
}
MediaEngineErrorEnum FadeBassEngine::GetLastError()
{
	return m_LastError;
}

BOOL FadeBassEngine::GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)
{
	return m_bass[m_EngineInUse].GetSoundData(mesd, buffer, bufferLen);
}


BOOL FadeBassEngine::Supports(MediaEngineConfigOption meco)
{
	switch (meco)
	{
	case MECO_Crossfade:
	case MECO_CrossfadeAtEnd:
	case MECO_CrossfadeAtStart:
		return TRUE;
	}
	return m_bass[m_EngineInUse].Supports(meco);
}



INT FadeBassEngine::GetConfig(MediaEngineConfigOption meco)
{
	switch (meco)
	{
	case MECO_Crossfade:
		return m_bCrossfadeEnabled ? 1 : 0;
	case MECO_CrossfadeAtEnd:
		return INT(m_crossFadeAtEnd * 1000);
	case MECO_CrossfadeAtStart:
		return INT(m_crossFadeAtStart * 1000);
	}
	return m_bass[m_EngineInUse].GetConfig(meco);
}


void FadeBassEngine::SetConfig(MediaEngineConfigOption meco, INT value)
{
	switch (meco)
	{
	case MECO_Crossfade:
		m_bCrossfadeEnabled = value != 0;
		break;
	case MECO_CrossfadeAtEnd:
		if (value < 60000)
			m_crossFadeAtEnd = value / 1000.0;
		break;
	case MECO_CrossfadeAtStart:
		if (value < 60000)
			m_crossFadeAtStart = value / 1000.0;
		break;
	default:
		m_bass[0].SetConfig(meco, value);
		m_bass[1].SetConfig(meco, value);
		return;
	}
	DOUBLE dVal = 0.0;
	if (m_bCrossfadeEnabled && m_crossFadeAtEnd > 0)
		dVal = m_crossFadeAtEnd;
	else
	{
		INT otherEngine = m_EngineInUse == 0 ? 1 : 0;
		m_bass[otherEngine].Close();
	}
}

LRESULT CALLBACK FadeBassEngine::WindowProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR ud = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (ud != NULL)
		return ((FadeBassEngine*)ud)->WinProc(uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT FadeBassEngine::WinProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_TIMER && wParam == 100)
	{
		if (m_bCrossfadeEnabled)
		{
			if (m_crossFadeAtEnd > 0)
			{
				//Check Active Engine
				switch (m_engStatus[m_EngineInUse])
				{
				case ES_Active:
					{
						DOUBLE curPos = m_bass[m_EngineInUse].GetMediaPos();
						DOUBLE mediaLength = m_bass[m_EngineInUse].GetMediaLength();
						if (mediaLength - curPos < m_crossFadeAtEnd)
						{
							//Close();
							if (m_bSendMPE_ReachedEnd[m_EngineInUse] == FALSE)
							{
								OnEvent(MPE_ReachedEnd, 0, &m_bass[m_EngineInUse]);
								m_bSendMPE_ReachedEnd[m_EngineInUse] = TRUE;
							}
							//m_pBass[m_EngineInUse]->FadedStop(mediaLength - curPos);
							//m_engStatus[m_EngineInUse] = ES_Deactivating;
						}
						else
							m_bSendMPE_ReachedEnd[m_EngineInUse] = FALSE;
					}
					break;
				case ES_Activating:
					//Not yet supported
					break;
				case ES_Deactivating:
				case ES_Deactive:
					//Happens when no Engine is actually active. (It has been closed - video is playing)
					break;
				default:
					ASSERT(0);//????
					break;
				}
				//Check InActive Engine
				INT m_EngineNotInUse = ((m_EngineInUse == 1) ? 0 : 1);
				switch (m_engStatus[m_EngineNotInUse])
				{
				case ES_Deactive:
					break;
				case ES_Deactivating:
					if (m_bass[m_EngineNotInUse].GetVolume() == 0)
					{
						TRACE(_T("@0FadeBassEngine::WM_TIMER. Closing Eng: %d\r\n"), m_EngineNotInUse);
						m_bass[m_EngineNotInUse].Close();
						m_engStatus[m_EngineNotInUse] = ES_Deactive;
					}

					//if (m_pBass[m_EngineNotInUse]->GetPlayState() != PS_Playing)
					//{
					//	TRACE(_T("@0FadeBassEngine::WM_TIMER. Closing Eng: %d\r\n"), m_EngineNotInUse);
					//	m_pBass[m_EngineNotInUse]->Close();
					//	m_engStatus[m_EngineNotInUse] = ES_Deactive;
					//}
					break;
				default:
					ASSERT(0);//????
					break;
				}
			}
		}
	}
	//if (message == UM_BASS_MSG && m_pEvTarget != NULL)
	//{
	//	//TRACE(_T("@3FadeBassEngine::WindowProc. wParam: %d lParam: %d\r\n"), wParam, lParam); 

	//	if (lParam == m_EngineInUse)
	//	{
	//		TRACE(_T("@0FadeBassEngine::WindowProc Forward Message: %d Engine: %d\r\n"), wParam, lParam);
	//		if (wParam == MPE_ReachedPos)
	//			wParam = MPE_ReachedEnd;
	//		m_pEvTarget->OnEvent(wParam);
	//	}
	//	else if (wParam == MPE_ReachedEnd)
	//	{
	//		TRACE(_T("@0FadeBassEngine::WindowProc Closing unused engine: %d\r\n"), wParam);
	//		ASSERT(m_pBass[lParam] !=0);
	//		if (m_pBass[lParam] !=0)
	//			m_pBass[lParam]->Close();
	//	}

	//}
	return 0;
}

INT FadeBassEngine::OnEvent(INT ID, INT param/* = 0*/, void* sender/* = 0*/)
{
	ASSERT(sender != NULL);
	MediaPlayerEngine::Events ev = (MediaPlayerEngine::Events) ID;
	INT curEngine = 1;
	if (sender == &m_bass[0])
		curEngine = 0;
	//TRACE(_T("@DFadeBassEngine::OnEvent. Engine: %d (IsCurrent: %s) (ID: %d)\r\n"), 
	//	m_EngineInUse, curEngine == m_EngineInUse ? _T("YES") : _T("NO"), ID);
	if (m_EngineInUse != curEngine)
	{
		//TRACE(_T("@DFadeBassEngine::OnEvent. Closing Not Active Engine: %d\r\n"), curEngine);
		m_bass[curEngine].Close();
		m_engStatus[curEngine] = ES_Deactive;
		return 0;//Do NOT Forward these messages
	}
	if (m_pEvTarget != NULL)
		m_pEvTarget->OnEvent(ID, param, this);
	return 0;
}


