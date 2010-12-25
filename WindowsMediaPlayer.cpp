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
#include "resource.h"
#include "WindowsMediaPlayer.h"
#include <wmpids.h>

#define IDC_REALMEDIAPLAYER 10
// CTSMediaPlayer

IMPLEMENT_DYNAMIC(WindowsMediaPlayer, CWnd)
const INT cMPControlsHeight = 64;

WindowsMediaPlayer::WindowsMediaPlayer():
m_bInfoRead(TRUE),
m_stopIt(FALSE)
{

}

WindowsMediaPlayer::~WindowsMediaPlayer()
{
	if (m_hWnd) DestroyWindow();

}


BEGIN_MESSAGE_MAP(WindowsMediaPlayer, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CTSMediaPlayer message handlers

BOOL WindowsMediaPlayer::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, RGB(0,0,0));
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void WindowsMediaPlayer::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

// Standard create

BOOL WindowsMediaPlayer::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	ASSERT(m_WMP.m_hWnd == NULL);
	// create this window
	if (!CWnd::CreateEx(NULL, NULL /*AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW)*/,
		NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN , rect, pParentWnd, nID))
		return FALSE;
	//if (!CDialog::Create(130, pParentWnd))
	//	return FALSE;

	// create browser control window as child of this window; 
	// this window sinks events from control
	CRect rc(0,0, rect.right - rect.left, rect.bottom - rect.top + cMPControlsHeight);


	//Control Activation in...
	//		(COleControlSite::CreateControlCommon) hr = DoVerb(OLEIVERB_INPLACEACTIVATE);
	//WindowName SHOULD BE NULL!!! otherwise we get ...
	//		Warning: constructing COleException, scode = DISP_E_MEMBERNOTFOUND ($80020003).
	//		First-chance exception at 0x7c81eb33 in TestMPNoWrap.exe: Microsoft C++ exception: COleException at memory location 0x0013eb9c..
	//but not a crash
	TRACE(_T("@1WindowsMediaPlayer::Create. Entering unsafe code: m_WMP.CreateControl\r\n"));
	if (!m_WMP.CreateControl(__uuidof(WindowsMediaPlayer), NULL, WS_VISIBLE, rc, this, IDC_REALMEDIAPLAYER))
	{
		TRACE(_T("@0WindowsMediaPlayer::Create. Failed\r\n"));
		//DestroyWindow();//This crashes in w2k machines
		return FALSE;
	}

	// get control interfaces
	HRESULT hr = S_FALSE;
	LPUNKNOWN unknown = m_WMP.GetControlUnknown();
	if (unknown)
		hr = unknown->QueryInterface(__uuidof(IWMPPlayer4),(void **)&m_spWMPPlayer);
	//if (SUCCEEDED(hr))
	//	hr = unknown->QueryInterface(IID_IDispatch,(void **)&m_spDispatch);
	if (!SUCCEEDED(hr)) 
	{
		TRACE(_T("@0WindowsMediaPlayer::Create. QueryInterface IWMPPlayer4 Failed\r\n"));
		m_WMP.DestroyWindow();
		return FALSE;        
	}

	//"none" dont work with visualizations
	//m_spWMPPlayer->put_uiMode(CComBSTR(_T("none"))); //check also "custom", "full", "none", "mini"
	m_spWMPPlayer->put_uiMode(CComBSTR(_T("custom")));
	m_spWMPPlayer->put_enableContextMenu(FALSE);
	m_spWMPPlayer->put_stretchToFit(TRUE);
	m_spWMPPlayer->put_enabled(FALSE);
	CComPtr<IWMPSettings> settings;
	hr = m_spWMPPlayer->get_settings(&settings);
	VARIANT_BOOL b = -1;
	if (SUCCEEDED(hr))
		settings->put_autoStart(0);

	//m_spWMPPlayer->put_windowlessVideo(TRUE);
	TRACE(_T("@4WindowsMediaPlayer::Create. Exiting TRUE \r\n"));

	return TRUE;
}

// Create in place of dialog control

BOOL WindowsMediaPlayer::CreateFromControl(CWnd *pParentWnd, UINT nID)
{
	BOOL result = FALSE;
	ASSERT(pParentWnd != NULL);
	CWnd *control = pParentWnd->GetDlgItem(nID);
	if (control != NULL) 
	{
		// get control location
		CRect		rect;
		control->GetWindowRect(&rect);
		pParentWnd->ScreenToClient(&rect);
		// destroy control, since the browser will take its place
		control->DestroyWindow();
		// create browser
		result = Create((WS_CHILD | WS_VISIBLE), rect, pParentWnd, nID);
	}
	return result;
}

void WindowsMediaPlayer::OnDestroy()
{
	Close();
	CWnd::OnDestroy();
}

void WindowsMediaPlayer::OnSize(UINT nType, int cx, int cy)
{
	if (m_WMP.GetControlSite() != NULL)
	{
		CRect rc;
		//hack for "full" ui mode because "none" don't work with visualizations
		m_WMP.MoveWindow(0, 0, cx, cy + cMPControlsHeight);
	}
}

BOOL WindowsMediaPlayer::Play(LPCTSTR str)
{
	ASSERT(str != NULL);
	BOOL ret = FALSE;
	if (m_spWMPPlayer != NULL)
	{
		ClearAllMedia();
		//ret = Enqueue(str);

		//CComPtr<IWMPControls> ctrls = NULL;
		//HRESULT hr = m_spWMPPlayer->get_controls(&ctrls);
		//if (SUCCEEDED(hr))
		//{
		//	hr = ctrls->play();
		//	ret = SUCCEEDED(hr);
		//}
		HRESULT hr = m_spWMPPlayer->put_URL(CComBSTR(str));
		if (SUCCEEDED(hr))
			PlayPause();//We have set autostart to false
		ret = SUCCEEDED(hr);
		m_bInfoRead = FALSE;
	}
	return ret;
}
BOOL WindowsMediaPlayer::Enqueue(LPCTSTR path)
{
	ASSERT(path != NULL);
	BOOL ret = FALSE;
	CComPtr<IWMPPlaylist> plList = NULL;
	if (SUCCEEDED(m_spWMPPlayer->get_currentPlaylist(&plList)))
	{
		CComPtr<IWMPMedia> pMedia = NULL;
		if (SUCCEEDED(m_spWMPPlayer->newMedia(CComBSTR(path), &pMedia)))
		{

			if (SUCCEEDED(plList->appendItem(pMedia)))
				ret = TRUE;
			long c = 0;
			plList->get_count(&c);
			if (c==1)
			{
				m_spWMPPlayer->put_currentMedia(pMedia);
				m_stopIt = TRUE;
			}
		}
	}
	if (!ret)
		ASSERT(0);
	return ret;
}

//Playback Commands
void WindowsMediaPlayer::PlayPause()
{
	WMPPlayState st;
	if (FAILED(m_spWMPPlayer->get_playState(&st)))
		return;
	CComPtr<IWMPControls> ctrls = NULL;
	if (FAILED(m_spWMPPlayer->get_controls(&ctrls)))
		return;

	switch (st)
	{
	case wmppsPlaying:
		ctrls->pause();
		break;
	default:
		ctrls->play();
	}

}
void WindowsMediaPlayer::Stop()
{
	CComPtr<IWMPControls> ctrls = NULL;
	HRESULT hr = m_spWMPPlayer->get_controls(&ctrls);
	if (FAILED(hr))
		return;
	ctrls->stop();
}
void WindowsMediaPlayer::Next()
{
	CComPtr<IWMPControls> ctrls = NULL;
	HRESULT hr = m_spWMPPlayer->get_controls(&ctrls);
	if (FAILED(hr))
		return;
	ctrls->next();
}
void WindowsMediaPlayer::Previous()
{
	CComPtr<IWMPControls> ctrls = NULL;
	HRESULT hr = m_spWMPPlayer->get_controls(&ctrls);
	if (FAILED(hr))
		return;
	ctrls->previous();
}


MediaPlayer::PlayState WindowsMediaPlayer::GetPlayState()
{
	WMPPlayState pState;
	HRESULT hr = m_spWMPPlayer->get_playState(&pState);
	switch (pState)
	{
	case wmppsPaused:
		return PS_Paused;
	case wmppsPlaying:
		return PS_Playing;
	case wmppsStopped:
		return PS_Stopped;
	}
	return PS_Inactive;
}



void WindowsMediaPlayer::Close()
{
	m_spWMPPlayer->close();
}

//Total Media Length in seconds
DOUBLE WindowsMediaPlayer::GetCurrentMediaLength()
{
	//if (!m_bInfoRead)
	//	ReadInfo();

	CComPtr<IWMPMedia> media;
	HRESULT hr = m_spWMPPlayer->get_currentMedia(&media);
	if (SUCCEEDED(hr) && media != NULL)
	{
		return GetMediaLengthPriv(media);
	}
	return 0.0;
}

DOUBLE WindowsMediaPlayer::GetMediaLengthPriv(CComPtr<IWMPMedia>& media)
{
	ASSERT(media != NULL);
	DOUBLE m_MediaLength = 0.0;
	HRESULT hr = media->get_duration(&m_MediaLength);
	return m_MediaLength;
}

//Media Position is seconds
DOUBLE WindowsMediaPlayer::GetMediaPos()
{
	//if (!m_bInfoRead)
	//	ReadInfo();
	CComPtr<IWMPControls> ctrls = NULL;
	HRESULT hr = m_spWMPPlayer->get_controls(&ctrls);
	DOUBLE curPos = 0.0;
	if (SUCCEEDED(hr))
	{
		ctrls->get_currentPosition(&curPos);
	}
	return curPos;

}

void WindowsMediaPlayer::SetMediaPos(DOUBLE secs)
{
	//CComPtr<IWMPControls> ctrls = NULL;
	IWMPControls* ctrls = NULL;
	HRESULT hr = m_spWMPPlayer->get_controls(&ctrls);
	if (SUCCEEDED(hr))
	{
		ctrls->put_currentPosition(secs);
	}
}

void WindowsMediaPlayer::GetCurrentMediaInfo(TCHAR* artist, int artistBufferLen, 
						  TCHAR* track, int trackBufferLen)
{
	ASSERT(artist != NULL);
	ASSERT(track != NULL);
	artist[0] = 0;
	track[0] = 0;
	CComPtr<IWMPMedia> media;
	HRESULT hr = m_spWMPPlayer->get_currentMedia(&media);
	if (SUCCEEDED(hr) && media != NULL)
		GetMediaInfoPriv(media, artist, artistBufferLen, track, trackBufferLen);
}
void WindowsMediaPlayer::GetMediaInfoPriv(CComPtr<IWMPMedia>& media, TCHAR* artist, int artistBufferLen, 
											 TCHAR* track, int trackBufferLen)
{
	ASSERT(media != NULL);
	CComBSTR name;
	HRESULT hr = media->getItemInfo(CComBSTR(_T("Author")), &name);
	if (SUCCEEDED(hr))
		_tcsncpy(artist, name.m_str, artistBufferLen);
	hr = media->getItemInfo(CComBSTR(_T("Title")), &name);
	if (SUCCEEDED(hr))
	{
		if (artist[0] == 0)
		{
			TCHAR* tokenArtist = _tcstok(name.m_str, _T("-"));
			TCHAR* tokenTrack = _tcstok(NULL, _T("-"));
			if (tokenTrack != NULL)
			{
				_tcsncpy(artist, tokenArtist, artistBufferLen);
				_tcsncpy(track, tokenTrack, trackBufferLen);
			}
			else
			{
				_tcsncpy(track, name.m_str, trackBufferLen);
			}
		}
		else
			_tcsncpy(track, name.m_str, trackBufferLen);
		_tcsltrim(_tcsrtrim(artist));
		_tcsltrim(_tcsrtrim(track));
	}
	else
	{
		artist[0] = 0;
		track[0] = 0;
	}
	artist[artistBufferLen-1] = 0;
	track[trackBufferLen-1] = 0;
}

void WindowsMediaPlayer::GetCurrentMediaPath(TCHAR* path, int pathBufferLen)
{
	ASSERT(path != NULL);
	path[0] = 0;
	CComPtr<IWMPMedia> media;
	HRESULT hr = m_spWMPPlayer->get_currentMedia(&media);
	if (SUCCEEDED(hr) && media != NULL)
		GetMediaPathPriv(media, path, pathBufferLen);

}
void WindowsMediaPlayer::GetMediaPath(UINT index, TCHAR* path, int pathBufferLen)
{
	ASSERT(path != NULL);
	path[0] = 0;
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWMPMedia> media;
		CComBSTR bstr;
		if (SUCCEEDED(plList->get_item(index, &media)))
		{
			GetMediaPathPriv(media, path, pathBufferLen);
		}
	}

}
void WindowsMediaPlayer::GetMediaPathPriv(CComPtr<IWMPMedia>& media, TCHAR* path, int pathBufferLen)
{
	ASSERT(media != NULL);
	CComBSTR name;
	HRESULT hr = media->get_sourceURL(&name);
	if (SUCCEEDED(hr))
		_tcsncpy(path, name.m_str, pathBufferLen);
}


void WindowsMediaPlayer::ReadInfo()
{
	CComPtr<IWMPMedia> media;
	HRESULT hr = m_spWMPPlayer->get_currentMedia(&media);
	TRACE(_T("@3Tries To Read Info\n"));
	if (SUCCEEDED(hr))
	{
		hr = media->get_duration(&m_MediaLength);
		if (SUCCEEDED(hr) && m_MediaLength > 0.0)
		{
			m_bInfoRead = TRUE;
			TRACE (_T("@2Duration: %3.2f \n"), m_MediaLength);
			CComBSTR name;
			media->get_name(&name);
			TRACE (_T("@2Name: %s \n"), name.m_str);
			media->get_sourceURL(&name);
			TRACE (_T("@2SourceURL: %s \n"), name);
			media->getItemInfo(CComBSTR(_T("Author")), &name);
			TRACE (_T("@2Author: %s \n"), name);
			media->getItemInfo(CComBSTR(_T("Title")), &name);
			TRACE (_T("@2Title: %s \n"), name);

			//CComBSTR attribValue;
			//CComBSTR attribName;
			//long attribCount = 0;
			//media->get_attributeCount(&attribCount);
			//for (int i = 0; i<attribCount; i++)
			//{
			//	media->getAttributeName(i, &attribName);
			//	media->getItemInfo(attribName, &attribValue);
			//	TRACE(_T("\"%s\" - \"%s\"\n"), attribName.m_str, attribValue.m_str);
			//}
		}
	}
}


UINT WindowsMediaPlayer::GetMediaCount()
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	long count = 0;
	if (SUCCEEDED(hr))
	{
		hr = plList->get_count(&count);
		if (FAILED(hr))
			TRACE(_T("GetMediaCount::get_count FAILED\n"));
	}
	else
		TRACE(_T("GetMediaCount::get_currentPlaylist FAILED\n"));

	return count;
}


DOUBLE WindowsMediaPlayer::GetMediaLength(UINT index)
{
	DOUBLE ret = 0.0;
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWMPMedia> media;
		CComBSTR bstr;
		if (SUCCEEDED(plList->get_item(index, &media)))
		{
			ret = GetMediaLengthPriv(media);
			if (ret == 0)
			{
				CComPtr<IWMPMedia> curMedia;
				if (SUCCEEDED(m_spWMPPlayer->get_currentMedia(&curMedia)))
				{
					ret = GetMediaLengthPriv(media);
				}
			}
		}
	}
	return ret;
}
void WindowsMediaPlayer::GetMediaInfo(UINT index, TCHAR* artist, int artistBufferLen, 
						  TCHAR* track, int trackBufferLen)
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWMPMedia> media;
		CComBSTR bstr;
		if (SUCCEEDED(plList->get_item(index, &media)))
		{
			GetMediaInfoPriv(media, artist, artistBufferLen, track, trackBufferLen);
			//media->get_name(&bstr);
			//_tcsncpy(name, bstr.m_str, nameLen);
		}
	}
}


void WindowsMediaPlayer::ClearAllMedia()
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
		plList->clear();
}


void WindowsMediaPlayer::RemoveMedia(UINT index)
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWMPMedia> media;
		plList->get_item(index, &media);
		if (SUCCEEDED(hr))
			plList->removeItem(media);
	}
}
void WindowsMediaPlayer::MoveMedia(UINT index, UINT newPos)
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
		plList->moveItem(index, newPos);

}
BOOL WindowsMediaPlayer::InsertMedia(LPCTSTR path, UINT Pos)
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWMPMedia> pMedia = NULL;
		if (SUCCEEDED(m_spWMPPlayer->newMedia(CComBSTR(path), &pMedia)))
		{
			plList->insertItem(Pos, pMedia);
			return TRUE;
		}
	}
	return FALSE;
}


INT WindowsMediaPlayer::GetCurrentMediaIndex()
{
	UINT num = GetMediaCount();
	INT ret = -1;
	if (num > 0)
	{
		CComPtr<IWMPMedia> curMedia;
		HRESULT hr = m_spWMPPlayer->get_currentMedia(&curMedia);
		if (SUCCEEDED(hr))
		{
			CComPtr<IWMPPlaylist> plList;
			hr = m_spWMPPlayer->get_currentPlaylist(&plList);
			if (SUCCEEDED(hr))
			{
				IWMPMedia* media;
				VARIANT_BOOL identical;
				for (UINT i = 0; i < num; i++)
				{
					hr = plList->get_item(i, &media);
					if (FAILED(hr))
						break;
					hr = media->get_isIdentical(curMedia, &identical);
					if (FAILED(hr))
						break;
					if (identical != FALSE)
					{
						ret = i;
						break;
					}
				}
			}
		}
	}
	return ret;
}
void WindowsMediaPlayer::SetCurrentMedia(UINT index, BOOL bAutoPlay/* = TRUE*/)
//void WindowsMediaPlayer::PlayListMedia(UINT index)
{
	CComPtr<IWMPPlaylist> plList;
	HRESULT hr = m_spWMPPlayer->get_currentPlaylist(&plList);
	if (SUCCEEDED(hr))
	{
		CComPtr<IWMPMedia> media;
		hr = plList->get_item(index, &media);
		if (SUCCEEDED(hr))
		{
			CComPtr<IWMPControls> controls;
			if (SUCCEEDED(m_spWMPPlayer->get_controls(&controls)))
			{
				hr = controls->put_currentItem(media);
				if (SUCCEEDED(hr))
				{
					if (bAutoPlay)
						PlayPause();
				}
			}
		}
	}
}

void WindowsMediaPlayer::SetVolume(UINT volume)
{
	if (volume > 100)
		volume = 100;
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	if (SUCCEEDED(hr))
	{
		settings->put_volume((long) volume);
	}
}

UINT WindowsMediaPlayer::GetVolume()
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	long volume = 0;
	if (SUCCEEDED(hr))
		settings->get_volume(&volume);
	return volume;

}
void WindowsMediaPlayer::SetMute(BOOL bMute)
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	if (SUCCEEDED(hr))
		settings->put_mute(bMute ? -1 : 0);
}
BOOL WindowsMediaPlayer::GetMute()
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	VARIANT_BOOL b = -1;
	if (SUCCEEDED(hr))
		settings->get_mute(&b);
	return b == -1;

}
void WindowsMediaPlayer::SetShuffle(BOOL bShuffle)
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	if (SUCCEEDED(hr))
		settings->setMode(CComBSTR(_T("shuffle")), bShuffle ? -1 : 0);
}
BOOL WindowsMediaPlayer::GetShuffle()
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	VARIANT_BOOL b = -1;
	if (SUCCEEDED(hr))
		settings->getMode(CComBSTR(_T("shuffle")), &b);
	return b == -1;
}
void WindowsMediaPlayer::SetLoop(BOOL bLoop)
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	if (SUCCEEDED(hr))
		settings->setMode(CComBSTR(_T("loop")), bLoop ? -1 : 0);
}
BOOL WindowsMediaPlayer::GetLoop()
{
	CComPtr<IWMPSettings> settings;
	HRESULT hr = m_spWMPPlayer->get_settings(&settings);
	VARIANT_BOOL b = -1;
	if (SUCCEEDED(hr))
		settings->getMode(CComBSTR(_T("loop")), &b);
	return b == -1;

}






BEGIN_EVENTSINK_MAP(WindowsMediaPlayer, CDialog)
	ON_EVENT(WindowsMediaPlayer, IDC_REALMEDIAPLAYER, DISPID_WMPCOREEVENT_PLAYSTATECHANGE, 
	WindowsMediaPlayer::PlayStateChangeMediaplayer, VTS_I4)
	ON_EVENT(WindowsMediaPlayer, IDC_REALMEDIAPLAYER, DISPID_WMPCOREEVENT_CURRENTITEMCHANGE, 
	WindowsMediaPlayer::CurrentItemChangeMediaplayer, VTS_DISPATCH)
	ON_EVENT(WindowsMediaPlayer, IDC_REALMEDIAPLAYER, DISPID_WMPCOREEVENT_CURRENTPLAYLISTCHANGE, 
	WindowsMediaPlayer::CurrentPlaylistChangeMediaplayer, VTS_I4)
	ON_EVENT(WindowsMediaPlayer, IDC_REALMEDIAPLAYER, DISPID_WMPCOREEVENT_OPENSTATECHANGE, 
	WindowsMediaPlayer::OpenStateChangeMediaplayer, VTS_I4)
	ON_EVENT(WindowsMediaPlayer, IDC_REALMEDIAPLAYER, DISPID_WMPCOREEVENT_MEDIAERROR, 
	WindowsMediaPlayer::MediaErrorMediaplayer, VTS_DISPATCH)


	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 5002, CTSMediaPlayer::StatusChangeMediaplayer, VTS_NONE)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 6505, CTSMediaPlayer::ClickMediaplayer, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 5803, CTSMediaPlayer::CurrentMediaItemAvailableMediaplayer, VTS_BSTR)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 5805, CTSMediaPlayer::CurrentPlaylistItemAvailableMediaplayer, VTS_BSTR)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 6506, CTSMediaPlayer::DoubleClickMediaplayer, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 5802, CTSMediaPlayer::MediaChangeMediaplayer, VTS_DISPATCH)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 5810, CTSMediaPlayer::PlaylistCollectionChangeMediaplayer, VTS_NONE)
	//ON_EVENT(CTSMediaPlayer, IDC_REALMEDIAPLAYER, 5202, CTSMediaPlayer::PositionChangeMediaplayer, VTS_R8 VTS_R8)
END_EVENTSINK_MAP()

void WindowsMediaPlayer::PlayStateChangeMediaplayer(long NewState)
{
	m_PlayState = NewState;
}

void WindowsMediaPlayer::CurrentItemChangeMediaplayer(LPDISPATCH pdispMedia)
{
	m_bInfoRead = FALSE;
}

void WindowsMediaPlayer::CurrentPlaylistChangeMediaplayer(long change)
{
}

void WindowsMediaPlayer::OpenStateChangeMediaplayer(long change)
{
	//WARNING !!!
	//This is a workaround because the idiot MP could not read the first
	//track in any list. e.g. when the Application starts the first track in the list
	//did not have Media Length info or correct name
	if (m_stopIt && change == wmposMediaOpen)
	{
		CComPtr<IWMPControls> ctrls = NULL;
		if (SUCCEEDED(m_spWMPPlayer->get_controls(&ctrls)))
			ctrls->stop();
		m_stopIt = FALSE;
	}
}

void WindowsMediaPlayer::MediaErrorMediaplayer(LPDISPATCH pdispMedia)
{
	
}

//void CTSMediaPlayer::StatusChangeMediaplayer()
//{
//	TRACE(_T("CTSMediaPlayer::StatusChangeMediaplayer\n"));
//}

//void CTSMediaPlayer::ClickMediaplayer(short nButton, short nShiftState, long fX, long fY)
//{
//	TRACE(_T("CTSMediaPlayer::ClickMediaplayer (short nButton, short nShiftState, long fX, long fY)\n"));
//}

//void CTSMediaPlayer::CurrentMediaItemAvailableMediaplayer(LPCTSTR bstrItemName)
//{
//	TRACE(_T("CTSMediaPlayer::CurrentMediaItemAvailableMediaplayer (bstrItemName:%s)\n"), bstrItemName);
//}

//void CTSMediaPlayer::CurrentPlaylistItemAvailableMediaplayer(LPCTSTR bstrItemName)
//{
//	TRACE(_T("CTSMediaPlayer::CurrentPlaylistItemAvailableMediaplayer(LPCTSTR bstrItemName)\n"));
//}
//
//void CTSMediaPlayer::DoubleClickMediaplayer(short nButton, short nShiftState, long fX, long fY)
//{
//	TRACE(_T("CTSMediaPlayer::DoubleClickMediaplayer((short nButton, short nShiftState, long fX, long fY))\n"));
//}
//
//void CTSMediaPlayer::MediaChangeMediaplayer(LPDISPATCH Item)
//{
//	TRACE(_T("CTSMediaPlayer::MediaChangeMediaplayer(LPDISPATCH Item)\n"));
//}
//
//void CTSMediaPlayer::PlaylistCollectionChangeMediaplayer()
//{
//	TRACE(_T("CTSMediaPlayer::PlaylistCollectionChangeMediaplayer\n"));
//}
//
//void CTSMediaPlayer::PositionChangeMediaplayer(double oldPosition, double newPosition)
//{
//	TRACE(_T("CTSMediaPlayer::PositionChangeMediaplayer oldPosition :%3.2f newPostion:%3.2f\n"), oldPosition, newPosition);
//}



