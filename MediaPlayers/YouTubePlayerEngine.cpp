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
#include "YouTubePlayerEngine.h"
#include "SimpleBrowser.h"
#include "EventTarget.h"

YouTubePlayerEngine::YouTubePlayerEngine():
m_pBrowser(NULL),
m_hwndParent(NULL),
m_volume(80),
m_bMute(FALSE),
m_mediaLength(-1.0),
m_bNavigating(FALSE),
m_pEventTarget(NULL),
m_bReachedEndSend(FALSE)
{
	memset(&m_rcVideoPosition, 0, sizeof(RECT));
}

YouTubePlayerEngine::~YouTubePlayerEngine()
{
	if (m_pBrowser != NULL)
	{
		m_pBrowser->DestroyWindow();
		delete m_pBrowser;
		m_pBrowser = 0;
	}

}

BOOL YouTubePlayerEngine::GetVideoID(LPCTSTR url, LPTSTR id, UINT idLen)
{
	//http://www.youtube.com/watch?v=DkSdIeMeEBE&feature=player_embedded#
	LPCTSTR posStart = _tcsstr(url, _T("?v="));
	if (posStart == NULL)
		posStart = _tcsstr(url, _T("&v="));
	if (posStart == NULL)
		return FALSE;
	posStart += 3;
	LPCTSTR posEnd = _tcschr(posStart, _T('&'));
	if (posEnd == NULL)
	{
		INT len = (INT)_tcslen(posStart);
		if (len >= (INT)idLen)
			return FALSE;
		_tcscpy(id, posStart);
		return TRUE;
	}
	if (posEnd - posStart >= (INT)idLen)
		return FALSE;
	_tcsncpy(id, posStart, posEnd - posStart);
	id[posEnd - posStart] = 0;
	return TRUE;
}

BOOL YouTubePlayerEngine::Open(LPCTSTR url)
{
	Close();
	if (!CanPlay(url))
		return FALSE;

	TCHAR id[50];
	if (GetVideoID(url, id, 50) == FALSE)
		return FALSE;

	if (m_pBrowser == NULL)
	{
		m_pBrowser = new SimpleBrowser;
		if (!m_pBrowser->Create(WS_VISIBLE | WS_CHILD, m_rcVideoPosition, CWnd::FromHandle(m_hwndParent), 453))
			return FALSE;
		m_pBrowser->SetNotifier(this);
		if (m_apiURL.empty())
			return FALSE;
		if (!m_pBrowser->Navigate(m_apiURL.c_str()))
			return FALSE;
		m_bNavigating = TRUE;
	}
	m_videoURL = url;
	TCHAR bf[100];
	_sntprintf(bf, 100, _T("loadNewVideo(\"%s\",0);"), id);
	ExecuteCommand(bf);
	Pause();
	SetVolume(m_volume);
	SetMute(m_bMute);
	return TRUE;
}
void YouTubePlayerEngine::Close()
{
	Stop();
	m_videoURL.clear();
}

BOOL YouTubePlayerEngine::ExecuteCommand(LPCTSTR command)
{
	if (m_pBrowser == NULL)
		return FALSE;
	if (m_bNavigating)
	{
		m_commandsToExecute.push_back(command);
		return TRUE;
	}
	return m_pBrowser->ExecuteJavascript(command);
}

BOOL YouTubePlayerEngine::Start()
{
	return ExecuteCommand(L"play();");
}
BOOL YouTubePlayerEngine::Pause()
{
	return ExecuteCommand(L"pause();");
}
BOOL YouTubePlayerEngine::Stop()
{
	if (m_pBrowser)
	{
		if (Pause())
		{
			SetMediaPos(0.0);
			return TRUE;
		}
	}
	return FALSE;
}

PlayState YouTubePlayerEngine::GetPlayState()
{
	INT iPs = GetIntValue(_T("playerstate"));
	switch (iPs)
	{
	case -1:
		return PS_Closed;
	case 0:
		return PS_Closed;
	case 1:
		return PS_Playing;
	case 2:
		return PS_Paused;
	case 3:
		return PS_Preparing;
	case 5:
		return PS_Stopped;
	default:
		return PS_Closed;
		//return PS_Stalled;
	}
	TRACE(_T("YouTubePlayerEngine::GetPlayState Unknown PS: %d\r\n"), iPs);
	return PS_Closed;
}

INT YouTubePlayerEngine::GetIntValue(LPCTSTR divName)
{
	if (m_pBrowser)
	{
		TCHAR bf[100];
		if (m_pBrowser->GetElementText(divName, bf, 100))
			return _ttoi(bf);
	}
	return -1;
}

DOUBLE YouTubePlayerEngine::GetDoubleValue(LPCTSTR divName)
{
	if (m_pBrowser)
	{
		TCHAR bf[100];
		if (m_pBrowser->GetElementText(divName, bf, 100))
			return _tstof(bf);
	}
	return -1.0;
}


DOUBLE YouTubePlayerEngine::GetMediaLength()
{
	DOUBLE dRet = GetDoubleValue(_T("videoduration"));
	if (dRet != -1.0)
		m_mediaLength = dRet;
	return m_mediaLength;
}


DOUBLE YouTubePlayerEngine::GetMediaPos()
{
	DOUBLE pos = GetDoubleValue(_T("videotime"));
	if (pos == m_mediaLength && pos > 0)
	{
		if (m_bReachedEndSend == FALSE)
		{
			m_bReachedEndSend = TRUE;
			if (m_pEventTarget != NULL)
				m_pEventTarget->OnEvent(MPE_ReachedEnd, 0, this);
		}
	}
	else
		m_bReachedEndSend = FALSE;
	return  pos;
}
void YouTubePlayerEngine::SetMediaPos(DOUBLE secs)
{
	if (m_pBrowser)
	{
		TCHAR bf[100];
		_sntprintf(bf, 100, _T("seekTo(%d);"), (INT) secs);
		ExecuteCommand(bf);
	}
}

void YouTubePlayerEngine::SetVolume(INT volume)
{
	m_volume = volume;
	if (m_pBrowser)
	{
		TCHAR bf[100];
		_sntprintf(bf, 100, _T("setVolume(%d);"), (INT) volume);
		ExecuteCommand(bf);
	}

}
INT YouTubePlayerEngine::GetVolume()
{
	return m_volume;
}
void YouTubePlayerEngine::SetMute(BOOL bMute)
{
	m_bMute = bMute;
	if (m_pBrowser)
	{
		ExecuteCommand(bMute ? _T("ytplayer.mute();") : _T("ytplayer.unMute();"));
	}
}
BOOL YouTubePlayerEngine::GetMute()
{
	return m_bMute;
}

BOOL YouTubePlayerEngine::CanPlay(LPCTSTR url)
{
	return _tcsstr(url, _T("youtube.com/")) != NULL;
}
MediaEngineState YouTubePlayerEngine::GetEngineState()
{
	return MES_Ready;
}
MediaEngineErrorEnum YouTubePlayerEngine::GetLastError()
{
	return MEE_NoError;
}
void YouTubePlayerEngine::SetVideoContainerHWND(HWND hwnd)
{
	m_hwndParent = hwnd;
	if (m_pBrowser != NULL)
	{
		m_pBrowser->SetParent(CWnd::FromHandle(hwnd));
		m_pBrowser->ShowWindow(hwnd ? SW_SHOW : SW_HIDE);
	}
}
HWND YouTubePlayerEngine::GetVideoContainerHWND()
{
	return m_hwndParent;
}
void YouTubePlayerEngine::SetVideoPosition(int x, int y, int cx, int cy)
{
	m_rcVideoPosition.left = x;
	m_rcVideoPosition.top = y;
	m_rcVideoPosition.right = x + cx;
	m_rcVideoPosition.bottom = y + cy;
	if (m_pBrowser != NULL)
		m_pBrowser->SetWindowPos(0, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}
//void YouTubePlayerEngine::OnNavigateComplete(LPCTSTR url)
//void YouTubePlayerEngine::OnDocumentComplete(LPCTSTR url)
//{
//	m_bDocumentCompletePassed = TRUE;
//}
//void YouTubePlayerEngine::OnDownloadComplete()
//{
//	if (m_bDocumentCompletePassed)
//	{
//		if (m_commandsToExecute.empty() == FALSE)
//		{
//			m_bNavigating = FALSE;
//			std::vector<std::basic_string<TCHAR> >::iterator it = m_commandsToExecute.begin();
//			for (; it != m_commandsToExecute.end(); it++)
//			{
//				m_pBrowser->ExecuteJavascript((*it).c_str());
//			}
//			m_commandsToExecute.clear();
//		}
//	}
//}

void YouTubePlayerEngine::OnStatusTextChange(LPCTSTR text)
{
	TRACE(_T("OnStatusTextChange: %s\r\n"), text);
	if (_tcscmp(text, _T("LOADEDOK")) == 0)
	{
		if (m_commandsToExecute.empty() == FALSE)
		{
			m_bNavigating = FALSE;
			std::vector<std::basic_string<TCHAR> >::iterator it = m_commandsToExecute.begin();
			for (; it != m_commandsToExecute.end(); it++)
			{
				m_pBrowser->ExecuteJavascript((*it).c_str());
			}
			m_commandsToExecute.clear();
		}
	}
}



