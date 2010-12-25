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
#include "DShowEngine.h"
#include "EventTarget.h"
#include <DShow.h>
#include "cStringUtils.h"

#define WM_GRAPHEVENT	WM_APP + 244
#pragma comment(lib,"strmiids.lib")

#define HELPER_RELEASE(x)   if (x != NULL) \
{ \
	x->Release(); \
	x = NULL; }


const wchar_t* const VideoFilterName = L"Video Mixing Renderer";

#ifdef _UNITTESTING
BOOL TestDShowEngine()
{
	UNITTESTSTART;
	DShowEngine* pEng = new DShowEngine;
	UNITTEST(TestGenericEngine(pEng));
	return TRUE;
}
#endif



DShowEngine::DShowEngine():
m_pGraphBuilder(NULL),
m_pMediaControl(NULL),
m_pMediaEvent(NULL),
m_pMediaPosition(NULL),
m_pMediaSeeking(NULL),
m_pBasicAudio(NULL),
m_pVideoWindow(NULL),
m_pWindowlessControl(NULL),
m_dwCaps(NULL),
m_pEvTarget(NULL),
m_EngineState(MES_Ready),
m_bFullScreen(FALSE),
m_hwndVideo(0),
m_hwndVideoContainer(0),
m_hwndVideoHiddenContainer(0),
m_volume(0),
m_bMuted(FALSE),
m_LastError(MEE_NoError)
{
	
}

DShowEngine::~DShowEngine()
{
	Close();
	DestroyWindow(m_hwndVideo);
	DestroyWindow(m_hwndVideoHiddenContainer);
}

HWND DShowEngine::GetVideoHWND()
{
	if (IsWindow(m_hwndVideo) == FALSE)
	{
		if (IsWindow(m_hwndVideoHiddenContainer) == FALSE)
		{
			//=== This is a hidden temporary container when we want the video to be invisible
			m_hwndVideoHiddenContainer = ::CreateWindow(_T("#32770"), 0, 
				0, 0, 0, 0, 0, 
				0, 
				0, AfxGetInstanceHandle(), 0);
		}
		m_hwndVideo = ::CreateWindow(_T("Static"), _T("PlayerWnd"), 
			WS_CHILD, 0, 0, 0, 0, 
			/*m_hwndVideoContainer ? m_hwndVideoContainer : */m_hwndVideoHiddenContainer, 
			0, AfxGetInstanceHandle(), 0);
		SetWindowLongPtr(m_hwndVideo, GWLP_USERDATA, (LONG_PTR)this);
		SetWindowLongPtr(m_hwndVideo, GWLP_WNDPROC, (LONG_PTR)WindowProcST);
	}
	return m_hwndVideo;
}


void DShowEngine::SetVideoContainerHWND(HWND hwnd)
{
	m_hwndVideoContainer = hwnd;
	HWND hwndVideo = GetVideoHWND();
	::SetParent(hwndVideo, m_hwndVideoContainer ? m_hwndVideoContainer : m_hwndVideoHiddenContainer);
	//::ShowWindow(hwndVideo, SW_SHOW);
}

HWND DShowEngine::GetVideoContainerHWND()
{
	return m_hwndVideoContainer;
}

BOOL DShowEngine::Open(LPCTSTR str)
{
	TRACEST(_T("DShowEngine::Open"));
	ASSERT(str != NULL);
	Close();
	HWND hwndVideo = GetVideoHWND();
	if (hwndVideo == NULL)
		return FALSE;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder,(void**)&m_pGraphBuilder);
	if (FAILED(hr))		return FALSE;

#ifdef _DEBUG
	HANDLE hLogFile = CreateFile(_T("c:\\dshow.log"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	m_pGraphBuilder->SetLogFile((DWORD_PTR)hLogFile);
#endif
	//VMR7 (Default)
	hr = InitializeWindowlessVMR(hwndVideo, m_pGraphBuilder, &m_pWindowlessControl, 1, FALSE);
	
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, (void**)&m_pMediaControl);
	if (FAILED(hr))		return FALSE;
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&m_pMediaEvent);
	if (FAILED(hr))		return FALSE;
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaPosition, (void**)&m_pMediaPosition);
	if (FAILED(hr))		return FALSE;
	hr = m_pGraphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&m_pMediaSeeking);
	if (FAILED(hr))		return FALSE;
	hr = m_pGraphBuilder->QueryInterface(IID_IBasicAudio, (void**)&m_pBasicAudio);
	if (FAILED(hr))		return FALSE;

	//Media Events
	m_pMediaEvent->SetNotifyWindow((OAHWND)hwndVideo, WM_GRAPHEVENT, 0);
	m_pMediaEvent->SetNotifyFlags(0);	// turn on notifications
	try
	{
		TRACEST(_T("DShowEngine::Open RenderFile"));
		hr = m_pGraphBuilder->RenderFile(CT2CW(str), NULL);
	}
	catch (...)
	{
		TRACE(_T("@0 DShowEngine::Open RenderFile Error catched\r\n"));
		Close();
		return FALSE;
	}
	if (hr != S_OK)
	{
#ifdef DEBUG
		switch (hr)
		{
		case VFW_S_AUDIO_NOT_RENDERED:	//Partial success; the audio was not rendered. 
			break;
		case VFW_S_DUPLICATE_NAME:	//Success; the Filter Graph Manager modified the filter name to avoid duplication. 
			break;
		case VFW_S_PARTIAL_RENDER:	//Some of the streams in this movie are in an unsupported format. 
			break;
		case VFW_S_VIDEO_NOT_RENDERED:	//Partial success; some of the streams in this movie are in an unsupported format. 
			break;
		case E_ABORT:	//Operation aborted. 
			break;
		case E_FAIL:	//Failure. 
			break;
		case E_INVALIDARG:	//Argument is invalid. 
			break;
		case E_OUTOFMEMORY:	//Insufficient memory. 
			break;
		case E_POINTER:	// NULLpointer argument. 
			break;
		case VFW_E_CANNOT_CONNECT:	//No combination of intermediate filters could be found to make the connection. 
			break;
		case VFW_E_CANNOT_LOAD_SOURCE_FILTER://The source filter for this file could not be loaded. 
			break;
		case VFW_E_CANNOT_RENDER:	//No combination of filters could be found to render the stream. 
			break;
		case VFW_E_INVALID_FILE_FORMAT:	//The file format is invalid. 
			break;
		case VFW_E_NOT_FOUND: // An object or name was not found. 
			break;
		case VFW_E_UNKNOWN_FILE_TYPE://The media type of this file is not recognized. 
			break;
		case VFW_E_UNSUPPORTED_STREAM:	//Cannot play back the file: the format is not supported. 
			break;
		}
#endif
		TRACE(_T("@0 DShowEngine::Open RenderFile Failed (%u)\r\n"), hr);
		Close();
		return FALSE;
	}
#ifdef _DEBUG
	IEnumFilters* pEnum = NULL;
	if (SUCCEEDED(m_pGraphBuilder->EnumFilters(&pEnum)))
	{
		ULONG cFetched;
		IBaseFilter* pFilter = NULL;
		while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
		{
			FILTER_INFO FilterInfo;
			hr = pFilter->QueryFilterInfo(&FilterInfo);
			if (FAILED(hr))
			{
				TRACE(_T("@1 DShowEngine::Open. Could not get the filter info\r\n"));
				continue;  // Maybe the next one will work.
			}
			else
				TRACE(_T("@2 DShowEngine::Open. Filter info: %s\r\n"), FilterInfo.achName);
			if (FilterInfo.pGraph != NULL)
				FilterInfo.pGraph->Release();
			pFilter->Release();
		}
		pEnum->Release();
	}
#endif
#ifdef _DEBUG
	m_pGraphBuilder->SetLogFile(NULL);
	CloseHandle(hLogFile);
#endif
	m_location = str;
	hr = m_pMediaSeeking->GetCapabilities(&m_dwCaps);
	if (FAILED(hr))		return FALSE;
	m_bIsVideo = FALSE;
	IBaseFilter* pRenderer = NULL;

	hr = m_pGraphBuilder->FindFilterByName(VideoFilterName, &pRenderer);
	if (pRenderer != NULL)
	{
		IEnumPins*	EnumPins;
		if (SUCCEEDED(pRenderer->EnumPins(&EnumPins)))
		{
			ULONG fetched = 0;
			IPin* InPin = NULL;
			IPin* FromPin = NULL;
			
			EnumPins->Reset();
			while (EnumPins->Next(1, &InPin, &fetched) == S_OK)
			{
				if (SUCCEEDED(InPin->ConnectedTo(&FromPin)))
				{
					FromPin->Release();
					m_bIsVideo = TRUE;
				}
				InPin->Release();
				if (m_bIsVideo)
					break;
			}
			EnumPins->Release();
		}
		pRenderer->Release();
	}

	if (m_bIsVideo)
	{
		ShowWindow(hwndVideo, SW_SHOW);
		if (m_pWindowlessControl != NULL)
		{
			CRect rc;
			::GetClientRect(hwndVideo, &rc);
			hr = m_pWindowlessControl->SetVideoPosition(0, &rc);
			hr = m_pWindowlessControl->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
		}
		else
		{
			hr = m_pGraphBuilder->QueryInterface(IID_IVideoWindow, (void**)&m_pVideoWindow);
			if (FAILED(hr))		return FALSE;

			m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			m_pVideoWindow->put_Owner((OAHWND)hwndVideo);
			::ShowWindow(hwndVideo, SW_SHOW);
			m_pVideoWindow->put_MessageDrain((OAHWND)hwndVideo);
			m_pVideoWindow->put_AutoShow(-1);
			CRect rc;
			::GetWindowRect(hwndVideo, &rc);
			m_pVideoWindow->SetWindowPosition(0, 0, rc.Width(), rc.Height());
			m_pVideoWindow->SetWindowForeground(OATRUE);
			m_pVideoWindow->put_Visible(TRUE);
		}
	}
	return TRUE;
}

HRESULT DShowEngine::InitializeWindowlessVMR(
								HWND hwndApp,         // Application window.
								IFilterGraph* pFG,    // Pointer to the Filter Graph Manager.
								IVMRWindowlessControl** ppWc,  // Receives the interface.
								DWORD dwNumStreams,  // Number of streams to use.
								BOOL fBlendAppImage  // Are we alpha-blending a bitmap?
								)
{
	ASSERT(pFG != NULL && ppWc != NULL);
	IBaseFilter* pVmr = NULL;
	*ppWc = NULL;
	// Create the VMR and add it to the filter graph.
	HRESULT hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = pFG->AddFilter(pVmr, VideoFilterName);
	if (FAILED(hr))
	{
		pVmr->Release();
		return hr;
	}

	// Set the rendering mode and number of streams.  
	IVMRFilterConfig* pConfig;
	hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	if (SUCCEEDED(hr)) 
	{
		pConfig->SetRenderingMode(VMRMode_Windowless);

		// Set the VMR-7 to mixing mode if you want more than one video
		// stream, or you want to mix a static bitmap over the video.
		// (The VMR-9 defaults to mixing mode with four inputs.)
		if (dwNumStreams > 1 || fBlendAppImage) 
		{
			pConfig->SetNumberOfStreams(dwNumStreams);
		}
		pConfig->Release();

		hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)ppWc);
		if (SUCCEEDED(hr)) 
		{
			(*ppWc)->SetVideoClippingWindow(hwndApp);
		}
	}
	pVmr->Release();

	// Now the VMR can be connected to other filters.
	return hr;
}



void DShowEngine::Close()
{
	if (m_pGraphBuilder != NULL)
	{
		m_location.clear();
		if (m_hwndVideo != NULL)
			::ShowWindow(m_hwndVideo, SW_HIDE);

		if (m_pMediaControl != NULL)
			m_pMediaControl->StopWhenReady();
		VERIFY(SUCCEEDED(m_pGraphBuilder->Abort()));
		if (m_pVideoWindow != NULL)
		{
			m_pVideoWindow->put_Visible(OAFALSE);
			m_pVideoWindow->put_Owner(NULL);
		}
		HELPER_RELEASE(m_pVideoWindow);
		HELPER_RELEASE(m_pBasicAudio);
		HELPER_RELEASE(m_pMediaSeeking);
		HELPER_RELEASE(m_pMediaPosition);
		HELPER_RELEASE(m_pMediaEvent);
		HELPER_RELEASE(m_pMediaControl);
		HELPER_RELEASE(m_pGraphBuilder);
	}
	return;
}

BOOL DShowEngine::Start()
{
	if (m_pMediaControl == NULL)	
		return FALSE;
	if (FAILED(m_pMediaControl->Run()))
	{
		SetError(_T("@0DShowEngine::Start Failed"));
		return FALSE;
	}
	return TRUE;
	//else
	//	if (m_pEvTarget) 
	//		m_pEvTarget->OnEvent(MPE_PlayStateChanged);
}

BOOL DShowEngine::Pause()
{
	if (m_pMediaControl == NULL)	
		return FALSE;
	if (FAILED(m_pMediaControl->Pause()))
	{
		SetError(_T("@0DShowEngine::Pause Failed"));
		return FALSE;
	}
	return TRUE;
}

void DShowEngine::SetError(LPCTSTR msg)
{
	TRACE(msg);
	if (m_pEvTarget)
		m_pEvTarget->OnEvent(MPE_Error, 0, this);
}

BOOL DShowEngine::Stop()
{
	Pause();
	SetMediaPos(0);
	return TRUE;
}

PlayState DShowEngine::GetPlayState()
{
	OAFilterState fs;
	if (m_pMediaControl != NULL && SUCCEEDED(m_pMediaControl->GetState(10, &fs)))
	{
		switch (fs)
		{
		case 1: return GetMediaPos() == 0 ? PS_Stopped : PS_Paused;
		case 2: return PS_Playing;
		case 0:	return PS_Stopped;//No File has been opened
			//File has been opened but not started to be played
			//m_pMediaControl->Stop() has been called
			break;
		default:
			TRACE(_T("New!!!!\n"));
			ASSERT(0);//New status value
		}
	}
	return PS_Closed;
}

INT DShowEngine::GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)
{
	return 0;
}
LPCTSTR DShowEngine::GetLocation()
{
	return m_location.c_str();
}
DOUBLE DShowEngine::GetMediaLength()
{
	DOUBLE len = 0.0;
	if (m_pMediaPosition != NULL)
		m_pMediaPosition->get_Duration(&len);
	return len;
}


DOUBLE DShowEngine::GetMediaPos()
{
	if (m_pMediaPosition != NULL)
	{
		REFTIME rTime;
		if (SUCCEEDED(m_pMediaPosition->get_CurrentPosition(&rTime)))
			return rTime;
	}
	SetError(_T("DShowEngine::GetMediaPos"));
	return 0.0;
	//LONGLONG value = -1;
	//if (m_pMediaSeeking != NULL)
	//{
	//	HRESULT hr = m_pMediaSeeking->GetCurrentPosition(&value);
	//	TRACE(_T("MediaPos: %d, %d\n"), value, value/10000);
	//}
	//if (value < 0)	value = 0;
	//return (DOUBLE (value))/10000000.0;
}

void DShowEngine::SetMediaPos(DOUBLE secs)
{
	if (m_pMediaPosition != NULL)
	{
		if (SUCCEEDED(m_pMediaPosition->put_CurrentPosition(secs)))
			return;
	}
	SetError(_T("DShowEngine::GetMediaPos"));
	return;

	//LONGLONG newPos = (LONGLONG)(secs * 10000000.0);
	//if (m_pMediaSeeking && (m_dwCaps & AM_SEEKING_CanSeekBackwards))
	//{
	//	if (FAILED(m_pMediaSeeking->SetPositions(
	//		&newPos, 
	//		AM_SEEKING_AbsolutePositioning,
	//		0,
	//		NULL)))
	//		SetError(_T("@0DShowEngine::SetMediaPos"));
	//}
	return; 
}

void DShowEngine::SetRealVolume(INT volume)
{
	if (m_pBasicAudio != NULL)
	{
		INT newvolume = INT(-10000.0 + pow((double)volume/100.0, 0.1) * 10000);
		TRACE(_T("@2DShow volume is: %d\r\n"), newvolume);
		m_pBasicAudio->put_Volume(newvolume);
	}
}

void DShowEngine::SetVolume(INT volume)
{
	m_volume = volume;
	if (!m_bMuted)
		SetRealVolume(volume);
}
INT DShowEngine::GetVolume()
{
	return m_volume;
}
void DShowEngine::SetMute(BOOL bMute)
{
	SetRealVolume(bMute ? 0 : m_volume);
	m_bMuted = bMute;
}
BOOL DShowEngine::GetMute()
{
	return m_bMuted;
}

void DShowEngine::PlayStateChanged()
{
	if (m_pEvTarget) 
	{
		TRACE(_T("@3 DShowEngine::OnGraphEvent Sending MPE_PlayStateChanged\n"));
		m_pEvTarget->OnEvent(MPE_PlayStateChanged, 0, this);
	}
}

void DShowEngine::OnGraphEvent()
{
	if (m_pEvTarget == NULL)
		return;
	if (m_pMediaEvent == NULL)//Can happen if we are closing it
		return;
	LONG EventCode;
	LONG Param1;
	LONG Param2;
	if (m_pMediaEvent->GetEvent(&EventCode, &Param1, &Param2, 0) != E_ABORT)
	{
		TRACE(_T("@3 DShowEngine::OnGraphEvent EventCode %d\n"), EventCode);
		m_pMediaEvent->FreeEventParams(EventCode, Param1, Param2);
		switch (EventCode)
		{
		case EC_COMPLETE:
			TRACE(_T("@3 DShowEngine::OnGraphEvent Sending MPE_ReachedEnd\n"));
			m_pEvTarget->OnEvent(MPE_ReachedEnd, 0, this);
			break;
		case EC_ERRORABORT:
		case EC_STREAM_ERROR_STOPPED:
		case EC_STREAM_ERROR_STILLPLAYING:
		case EC_ERROR_STILLPLAYING:
			TRACE(_T("@3 DShowEngine::OnGraphEvent Sending MPE_Error\n"));
			m_pEvTarget->OnEvent(MPE_Error, 0, this);
			break;
		case EC_PAUSED:
		case EC_STATE_CHANGE:
			PlayStateChanged();
			break;
		case EC_OPENING_FILE:
		case EC_BUFFERING_DATA:
		default:
			TRACE(_T("@3 DShowEngine::OnGraphEvent. Unhandled\n"));
			break;
		}
	}
}

void DShowEngine::SetVideoPosition(int x, int y, int cx, int cy)
{
	TRACE(_T("DShowEngine::OnSize\n"));
	HWND hwndVideo = GetVideoHWND();
	::MoveWindow(hwndVideo, x, y, cx, cy, FALSE);
	if (m_pWindowlessControl != NULL)
	{
		m_pWindowlessControl->SetVideoPosition(0, &CRect(0,0,cx,cy));
	}
	else if (m_pVideoWindow != NULL)
	{
		m_pVideoWindow->SetWindowPosition(0,0,cx,cy);
	}

}


BOOL DShowEngine::OnPaint()
{
	if (m_bIsVideo && m_pWindowlessControl != NULL)
	{
		ASSERT(m_hwndVideo != NULL);
		CRect rc;
		if (GetUpdateRect(m_hwndVideo, &rc, FALSE))
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hwndVideo, &ps);
			m_pWindowlessControl->RepaintVideo(m_hwndVideo, hdc);
			EndPaint(m_hwndVideo, &ps);
		}
		return TRUE;

	}
	return FALSE;
}

void DShowEngine::OnDisplayModeChanged()
{
	if (m_pWindowlessControl != NULL)
	{
		m_pWindowlessControl->DisplayModeChanged();
	}
}


BOOL DShowEngine::CanPlay(LPCTSTR str)
{
	LPCTSTR filePath = &str[_tcslen(str) - 4];
	TCHAR ext[5];
	_tcsncpy(ext, filePath, 4);
	ext[4] = 0;
	_tcslwr(ext);
	return _tcsistr(_T(".mp3.mp2.mpgmpeg.avi.asf.wma.wmv"), ext) != NULL;
}
MediaEngineState DShowEngine::GetEngineState()
{
	return m_EngineState;
}
MediaEngineErrorEnum DShowEngine::GetLastError()
{
	return m_LastError;
}
LRESULT CALLBACK DShowEngine::WindowProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG_PTR ud = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (ud != NULL)
	{
		BOOL bHandled = TRUE;
		LRESULT res = ((DShowEngine*)ud)->WinProc(uMsg, wParam, lParam, bHandled);
		if (bHandled)
			return res;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT DShowEngine::WinProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	LRESULT res = 0;
	switch (message)
	{
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_SETCURSOR:
	case WM_MOUSEACTIVATE:
	case WM_CONTEXTMENU:
		::PostMessage(m_hwndVideoContainer, message, wParam, lParam);
		bHandled = TRUE;
		break;
	case WM_GRAPHEVENT:
		OnGraphEvent();
		bHandled = TRUE;
		break;
	case WM_DISPLAYCHANGE:
		OnDisplayModeChanged();
		//=== Continue process to the DefProc
		break;
	case WM_PAINT:
		OnPaint();
		bHandled = TRUE;
		break;
	case WM_ERASEBKGND:
		bHandled = TRUE;
		res = 1;
		break;
	}
	return res;

}

