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
#include "MultiEngine.h"
#include "EventTarget.h"



MultiEngine::MultiEngine():
m_pEvTarget(NULL),
m_pActiveEngine(NULL),
m_volume(80),
m_bMute(FALSE),
m_hwndVideo(NULL)
{
}

void MultiEngine::SetVideoPosition(int x, int y, int cx, int cy)
{
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		(*i)->SetVideoPosition(x,y,cx,cy);
}


MultiEngine::~MultiEngine()
{
	Close();
	RemoveAllEngines();
}

//BOOL MultiEngine::Init(HWND viewer)
//{
//	m_hwndViewers = viewer;
//	return TRUE;
//}

void MultiEngine::SetEventTarget(EventTarget* pEvTarg)
{
	m_pEvTarget = pEvTarg;
}


BOOL MultiEngine::Open(LPCTSTR str)
{
	//TRACE(_T("@2MultiEngine::Open URL: %s\r\n"), str);
	Close();
	BOOL bRet = FALSE;
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
	{
		if ((*i)->CanPlay(str) == TRUE)
		{
			if ((*i)->GetEngineState() == MES_Ready)
			{
				(*i)->SetEventTarget(this);
				(*i)->SetVideoContainerHWND(m_hwndVideo);
				if ((*i)->Open(str))
				{
					//m_ActiveEngine = (INT)i;
					m_pActiveEngine = (*i);
					bRet = TRUE;
					break;
				}
			}
		}
	}
	if (!bRet)
	{
		//Try to open
		for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		{
			if ((*i)->GetEngineState() == MES_Ready)
			{
				(*i)->SetEventTarget(this);
				(*i)->SetVideoContainerHWND(m_hwndVideo);
				if ((*i)->Open(str))
				{
					//m_ActiveEngine = (INT)i;
					m_pActiveEngine = (*i);
					bRet = TRUE;
					break;
				}
			}
		}

	}
	if (!bRet)
	{
		TRACE(_T("@1 MultiEngine::Open None can handle it\r\n"));
		m_pActiveEngine = NULL;
	}
	else
	{
		TRACE(_T("@3 MultiEngine::Open [END] Selected Engine: %s\r\n"), GetEngineDescription());
		m_pActiveEngine->SetVolume(m_volume);
	}
	return bRet;
}

void MultiEngine::Close()
{
	//TRACE(_T("@2MultiEngine::Close Eng: %s\r\n"), GetEngineDescription());
	if (m_pActiveEngine != NULL)
	{
		m_volume = m_pActiveEngine->GetVolume();
		m_pActiveEngine->Close();
		m_pActiveEngine = NULL;
	}

	//for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
	//	(*i)->Close();
}

BOOL MultiEngine::Start()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->Start();
	return FALSE;
}

BOOL MultiEngine::Pause()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->Pause();
	return FALSE;
}

void MultiEngine::SetError(LPCTSTR msg)
{
	TRACE(msg);
	if (m_pEvTarget)
		m_pEvTarget->OnEvent(MPE_Error);
}

BOOL MultiEngine::Stop()
{
	TRACE(_T("MultiEngine::Stop Eng: %s\r\n"), GetEngineDescription());
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->Stop();
	return FALSE;
}

PlayState MultiEngine::GetPlayState()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->GetPlayState();
	return PS_Closed;
}

INT MultiEngine::GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)
{
	if (m_pActiveEngine == NULL)
		return 0;
	return m_pActiveEngine->GetMediaInfo(meoii);
}
LPCTSTR MultiEngine::GetLocation()
{
	if (m_pActiveEngine == NULL)
		return _T("");
	return m_pActiveEngine->GetLocation();
}
DOUBLE MultiEngine::GetMediaLength()
{
	if (m_pActiveEngine == NULL)
		return 0.0;
	return m_pActiveEngine->GetMediaLength();
}

DOUBLE MultiEngine::GetMediaPos()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->GetMediaPos();
	return 0.0;

}

void MultiEngine::SetMediaPos(DOUBLE secs)
{
	if (m_pActiveEngine != NULL)
		m_pActiveEngine->SetMediaPos(secs);
}

void MultiEngine::SetVolume(INT volume)
{
	//if (m_pActiveEngine != NULL)
	//	m_pActiveEngine->SetVolume(volume);
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		(*i)->SetVolume(volume);
	m_volume = volume;

}
INT MultiEngine::GetVolume()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->GetVolume();
	return m_volume;

}
void MultiEngine::SetMute(BOOL bMute)
{
	m_bMute = bMute;
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		(*i)->SetMute(bMute);

}
BOOL MultiEngine::GetMute()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->GetMute();
	return m_bMute;
}

void MultiEngine::PlayStateChanged()
{
	if (m_pEvTarget) 
		m_pEvTarget->OnEvent(MPE_PlayStateChanged, 0, this);
}

void MultiEngine::AddEngine(MediaPlayerEngine* mpe)
{
	m_pEngines.push_back(mpe);
}
void MultiEngine::RemoveAllEngines()
{
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		delete *i;
	m_pEngines.clear();
}

INT MultiEngine::OnEvent(INT ID, INT param, void* sender)
{
	ASSERT(m_pEvTarget != NULL);
	if (sender == m_pActiveEngine)
		return m_pEvTarget->OnEvent(ID, param, sender);
	return 0;
}

BOOL MultiEngine::CanPlay(LPCTSTR str)
{
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		if ((*i)->CanPlay(str))
			return TRUE;
	return FALSE;
}
MediaEngineState MultiEngine::GetEngineState()
{
	return MES_Ready;
}
MediaEngineErrorEnum MultiEngine::GetLastError()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->GetLastError();
	return MEE_NoError;
}

//LRESULT MultiEngine::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	if (m_pActiveEngine != NULL)
//		return m_pActiveEngine->WindowProc(message, wParam, lParam);
//	return MEE_NoError;
//}

BOOL MultiEngine::IsVideo()
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->IsVideo();
	return FALSE;
}

BOOL MultiEngine::GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)
{
	if (m_pActiveEngine != NULL)
		return m_pActiveEngine->GetSoundData(mesd, buffer, bufferLen);
	return FALSE;
}

const TCHAR* MultiEngine::GetEngineDescription()
{
	if (m_pActiveEngine != NULL)
		_sntprintf(m_engDesc, 100, _T("MultiEngine [%s]"), m_pActiveEngine->GetEngineDescription());
	else
		return _T("MultiEngine [NULL]");
	m_engDesc[99] = 0;
	return m_engDesc;
}

BOOL MultiEngine::Supports(MediaEngineConfigOption meco)
{
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
	{
		if ((*i)->Supports(meco))
			return TRUE;
	}
	return FALSE;
}

void MultiEngine::SetConfig(MediaEngineConfigOption meco, INT value)
{
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		(*i)->SetConfig(meco, value);		
}

INT MultiEngine::GetConfig(MediaEngineConfigOption meco)
{
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
	{
		if ((*i)->Supports(meco))
			return (*i)->GetConfig(meco);		
	}
	return 0;
}

void MultiEngine::SetVideoContainerHWND(HWND hwnd)
{
	m_hwndVideo = hwnd;
	for (std::vector<MediaPlayerEngine*>::iterator i = m_pEngines.begin(); i < m_pEngines.end(); i++)
		(*i)->SetVideoContainerHWND(hwnd);		
}





