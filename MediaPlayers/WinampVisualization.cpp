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
#include "WinampVisualization.h"
#include "MediaPlayerEngine.h"

#define TIMER_VIS_REFRESH 100

//This class uses a number of window handlers. Those are:
//	1. m_hwndViewer: 
//		The Container Window of the visualization (Set by the application)
//	2. m_hwndContainer (child of m_hwndViewer):
//		Internal container of the winamp visualization. Plug in tries to destroy this window
//		on quit
//	3. m_hwndVisualization (child of m_hwndContainer):
//		This is the actual Visualization Window created by the plug in
//	4. m_hwndWinamp:
//		This window is used internally for Winamp Emulation

const TCHAR* cWinampClassName = _T("Winamp v1.x");

WinampVisualization::WinampVisualization():
m_hwndContainer(NULL),
m_mpe(NULL),
m_hwndVisualization(0),
m_hwndHiddenContainer(0),
m_hwndNormalContainer(0),
m_bStarted(FALSE),
m_bInitialized(FALSE),
m_pThread(NULL),
m_bThreadExit(FALSE)
{
}

WinampVisualization::~WinampVisualization()
{
	TRACEST(_T("WinampVisualization::~WinampVisualization"));
	UnInit();
}

void WinampVisualization::SetMediaPlayerEngine(MediaPlayerEngine* mpe)
{
	m_mpe = mpe;
}
void WinampVisualization::SetContainerHWND(HWND hwnd)
{
	m_hwndContainer = hwnd;
	if (m_hwndNormalContainer)
		SetParent(m_hwndNormalContainer, m_hwndContainer);
	else
		SetParent(m_hwndNormalContainer, m_hwndHiddenContainer);

}
HWND WinampVisualization::GetContainerHWND()
{
	return m_hwndContainer;
}

void WinampVisualization::Start()
{
	if (!m_bStarted)
	{
		m_bStarted = TRUE;
		if (!m_bInitialized)
			Init();
	}
}

void WinampVisualization::Stop()
{
	if (m_bStarted)
	{
		m_bStarted = FALSE;
		//m_pThread->SuspendThread();
	}
}

BOOL WinampVisualization::IsActive()
{
	return m_bStarted;
}


void WinampVisualization::Init()
{
	if (!m_bInitialized)
	{
		m_pThread = AfxBeginThread(VisThreadStatic, this, 0, 0, CREATE_SUSPENDED, 0);
		m_pThread->m_bAutoDelete = FALSE;
		m_bInitialized = TRUE;
		m_pThread->ResumeThread();
	}
}

void WinampVisualization::UnInit()
{
	if (m_bInitialized)
	{
		HANDLE hThread = m_pThread->m_hThread;
		ASSERT(hThread != NULL);
		m_bThreadExit = TRUE;
		DWORD exitCode = 0;
		GetExitCodeThread(m_pThread->m_hThread, &exitCode);
		DWORD timeLimit = GetTickCount();
		const int timeLimitMax = 5000;

		while (exitCode == STILL_ACTIVE)
		{
			if (GetTickCount() - timeLimit > timeLimitMax)
			{
				TRACE(_T("@0 WinampVisualization::UnInit Aborting Thread\r\n"));
				break;
			}
			Sleep(50);
			GetExitCodeThread(m_pThread->m_hThread, &exitCode);
		}
		TRACE(_T("@2 WinampVisualization::UnInit. Abort Loop took: %d (%d) ms\r\n"), 
			GetTickCount() - timeLimit, timeLimitMax);
		ASSERT(exitCode != STILL_ACTIVE);
		delete m_pThread;
		m_pThread = 0;
	}
}

void WinampVisualization::SetPosition(int x, int y, int cx, int cy)
{
	m_wndRC.SetRect(x, y, x+cx, y+cy);
	if (m_hwndNormalContainer != NULL)
	{
		SetWindowPos(m_hwndNormalContainer, NULL, x, y, cx, cy, SWP_NOZORDER | SWP_NOREDRAW | SWP_SHOWWINDOW);
		if (m_hwndVisualization != NULL)
			SetWindowPos(m_hwndVisualization, NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOREDRAW | SWP_SHOWWINDOW);
	}
}
//--------------------------------------------------------------------------------
//Extra Interface
//--------------------------------------------------------------------------------


BOOL WinampVisualization::LoadPlugin(LPCTSTR plugInPath)
{	
	//ReleasePlugin();
	_tcsncpy(m_PluginPath, plugInPath, MAX_PATH);
	m_PluginPath[MAX_PATH - 1] = 0;
	return TRUE;
}

//--------------------------------------------------------------------------------
//Private
//--------------------------------------------------------------------------------

UINT __cdecl WinampVisualization::VisThreadStatic(LPVOID lpParam)
{
	ASSERT(lpParam != NULL);
	return ((WinampVisualization*)lpParam)->VisThread();
}

DWORD WINAPI WinampVisualization::VisThread()
{	
	TRACEST(_T("WinampVisualization::VisThread"));
	//==================Initialization
	//===LoadModule

	HINSTANCE pluginInstance = LoadLibrary(m_PluginPath);
	if (pluginInstance == NULL)
	{
		ASSERT(0);
		return 0;
	}

	winampVisGetHeaderType visHeaderType = 
		(winampVisGetHeaderType) GetProcAddress(pluginInstance, "winampVisGetHeader");
	if (visHeaderType == NULL)
	{
		ASSERT(0);
		return 0;
	}
	
	winampVisHeader* visHeader = visHeaderType();
	//INT moduleCount = 0;
	//while (visHeader->getModule(moduleCount) > 0)
	//	moduleCount++;
	winampVisModule* visModule = visHeader->getModule(0);
	if (visModule == 0)
	{
		ASSERT(0);
		return 0;
	}


	//=== HiddenContainer
	HWND hwndHiddenContainer = ::CreateWindow(_T("#32770"), 0, 
		0, 0, 0, 0, 0, 
		0, 
		0, AfxGetInstanceHandle(), 0);


	
	//=== WinampWindow - RegisterClass
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof(wndclass);
	wndclass.style =  CS_PARENTDC | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = AfxGetInstanceHandle();
	wndclass.hIcon = 0;
	wndclass.hCursor = (HICON) NULL;
	wndclass.hbrBackground = (HBRUSH) NULL;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = cWinampClassName;
	wndclass.lpfnWndProc = DefWindowProc;
	wndclass.hIconSm = (HICON) NULL;
	ATOM clClass = RegisterClassEx(&wndclass);

	//=== WinampWindow - CreateWindow
	HWND hwndWinamp = CreateWindowEx(0, (LPCTSTR)MAKEWPARAM(clClass, 0), _T("Music"), 0, 0, 0, 0, 0, 
		NULL, (HMENU) NULL, AfxGetInstanceHandle(), NULL);
	SetWindowLongPtr(hwndWinamp, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLongPtr(hwndWinamp, GWLP_WNDPROC, (LONG_PTR)WinampWndProcST);


	//=== Winamp Module Init
	INT delayMS = 40;
	BOOL bActuallyStarted = FALSE;
	BOOL bWMQuitPosted = FALSE;
	while (TRUE)
	{
		if (m_bStarted && !bActuallyStarted)
		{
			TRACE(_T("@2 WinampVisualization::VisThread. Activating Module\r\n"));
			//=== Normal Container
			m_hwndNormalContainer = ::CreateWindow(_T("Static"), _T("WAVisContainer"), 
				WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, 
				m_hwndContainer ? m_hwndContainer : m_hwndHiddenContainer, 
				0, AfxGetInstanceHandle(), 0);

			visModule->hDllInstance = pluginInstance;
			visModule->sRate = 44100;			//TODO
			visModule->nCh = 2;					//TODO
			visModule->userData = this;
			visModule->delayMs = delayMS;
			visModule->hwndParent = hwndWinamp;
			int ret = -1;
			try
			{
				TRACEST(_T("WinampVisualization::VisThread visModule->Init"));
				ret = visModule->Init(visModule);	
			}
			catch (...)	
			{
				ASSERT(0);
				return 0;
			}
			if (ret != 0)//Success
			{
				ASSERT(0);
				return 0;
			}

			m_hwndVisualization = ::GetWindow(m_hwndNormalContainer, GW_CHILD);
			ASSERT(m_hwndVisualization);
			SetWindowPos(m_hwndNormalContainer, NULL, 
				m_wndRC.left, m_wndRC.top, m_wndRC.Width(), m_wndRC.Height(), SWP_NOZORDER);
			SetWindowPos(m_hwndVisualization, NULL, 
				0, 0, m_wndRC.Width(), m_wndRC.Height(), SWP_NOZORDER);
			bActuallyStarted = TRUE;
		}
		else if (!m_bStarted && bActuallyStarted)
		{
			TRACEST(_T("WinampVisualization::VisThread. DE-Activating Visualization"));
			visModule->Quit(visModule);
			//=== Remember. Quit Destroys The vis window
			m_hwndVisualization = NULL;
			//=== Remember. Quit Destroys also the parent (!!!) of the window 
			m_hwndNormalContainer = NULL;
			bActuallyStarted = FALSE;
		}

		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) != 0)
		{
			bWMQuitPosted = message.message == WM_QUIT;//(message.message == WM_QUIT && message.hwnd == 0);
			BOOL bHandled = FALSE;
			LPCTSTR wnd = _T("???");
			if (message.hwnd == hwndWinamp)
				wnd = _T("Amp");
			else if (message.hwnd == m_hwndVisualization)
				wnd = _T("Vis");
			else if (message.hwnd == m_hwndHiddenContainer)
				wnd = _T("Hid");
			else if (message.hwnd == m_hwndNormalContainer)
				wnd = _T("Nor");
			else if (message.hwnd == 0)
				wnd = _T("THREAD");
			else
			{
				DestroyWindow(message.hwnd);
			}
			//if (message.message == WM_QUIT)
			//	TRACE(_T("@D WinampVisualization::VisThread WM_QUIT.. Message ON %s\r\n")
			//	, wnd);
			//if (m_bThreadExit)
			//	TRACE(_T("@D WinampVisualization::VisThread [Final Messages]: %d ON %s\r\n")
			//	,message.message, wnd);

			if (message.hwnd == m_hwndVisualization && message.hwnd != NULL)// && bModuleInitialized)
			{
				ASSERT(bActuallyStarted);
				switch (message.message)
				{
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_LBUTTONDBLCLK:
				case WM_SETCURSOR:
				case WM_MOUSEACTIVATE:
				case WM_CONTEXTMENU:
					::PostMessage(m_hwndContainer, message.message, message.wParam, message.lParam);
					bHandled = TRUE;
					break;
				case WM_ERASEBKGND:
					if (bActuallyStarted)
					{
						visModule->Render(visModule);
						bHandled = TRUE;
					}
					break;
				}
			}
			if (!bHandled && bActuallyStarted)
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
		if (m_bThreadExit)
		{
			delayMS = 0;//Accelerate closing
			TRACE(_T("@D WinampVisualization::VisThread Trying To Exit...\r\n"));
			if (bActuallyStarted)
			{
				TRACE(_T("@D WinampVisualization::VisThread Quiting Module\r\n"));
				visModule->Quit(visModule);
				//=== Remember. Quit Destroys The vis window
				m_hwndVisualization = NULL;
				//=== Remember. Quit Destroys also the parent (!!!) of the window 
				m_hwndNormalContainer = NULL;
				bActuallyStarted = FALSE;
				m_bStarted = FALSE;
			}
			else if (bWMQuitPosted)
			{
				TRACE(_T("@D WinampVisualization::VisThread bWMQuitPosted... Exiting\r\n"));
				break;
			}
			else
			{
				TRACE(_T("@D WinampVisualization::VisThread PostQuitMessage\r\n"));
				PostQuitMessage(0);
				//bWMQuitPosted = TRUE;//Freshly Added
			}
		}
		if (bActuallyStarted)
		{
			ASSERT(visModule);
			if (visModule)
				UpdatePlugin(visModule);
		}
		Sleep(delayMS);//m_pVisModule->delayMs);
	}
	//Sleep(10);
	DestroyWindow(hwndWinamp);
	hwndWinamp = NULL;
	UnregisterClass(cWinampClassName, AfxGetInstanceHandle());
	if (m_hwndContainer)
		::InvalidateRect(m_hwndContainer, NULL, TRUE);
	if (pluginInstance > 0)
	{
		FreeLibrary(pluginInstance);
		pluginInstance = 0;
	}
	return 0;
}


void WinampVisualization::UpdatePlugin(winampVisModule* mod)
{
	//ASSERT(m_ActiveStatus != VTS_Active);//Happens the first time
	ASSERT(mod != NULL);
	ASSERT(m_mpe != NULL);
	if ( mod == NULL)					return;
	if ( m_mpe == NULL)					return;

	if (m_mpe->GetPlayState() == PS_Playing)
	{
		if (mod->waveformNch > 0)
		{
			const int rawDataLen = 1052;
			m_mpe->GetSoundData(MSD_16BitRaw, m_samplesBuffer, 2 * 2 * 512);
			//BASS_ChannelGetData(m_hChannel, m_samplesBuffer, 2 * PLUGIN_SAMPLES);
			Cnv16to8(m_samplesBuffer,(signed char*) &mod->waveformData[0][0], rawDataLen);
			Cnv16to8(m_samplesBuffer+1,(signed char*) &mod->waveformData[1][0], rawDataLen);
		}
		if (mod->spectrumNch > 0)
		{
			float		realfft[4097]={0};
			m_mpe->GetSoundData(MSD_FFT, realfft, 4 * 4096);
			//BASS_ChannelGetData(m_hChannel,realfft,BASS_DATA_FFT2048 | BASS_DATA_FFT_INDIVIDUAL);
			for (int a=0;a<575;a++)
			{
				m_samplesBuffer[a*2] = (signed short)(96000*realfft[a*2]);
			}
			Cnv16to8(m_samplesBuffer,(signed char*) &mod->spectrumData[0][0], 575);
			Cnv16to8(m_samplesBuffer+1,(signed char*) &mod->spectrumData[1][0], 575);
		}
	}
	mod->Render(mod);
}

//LRESULT CALLBACK WinampVisualization::ContainerWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}

static CHAR bf[MAX_PATH];

LRESULT WinampVisualization::HandleWAIPC(WPARAM wParam, LPARAM lParam)
{
	LPCSTR emptyString = "";
	LPCSTR iniFile = emptyString;//"H:\\Bin\\TeenSpirit\\output\\winamp.ini";
	//LPCSTR iniDir = "H:\\Bin\\TeenSpirit\\output";
	//ASSERT(m_mpe != NULL);
#ifdef _DEBUG
	LPCTSTR ipcName = 0;
	switch (lParam)
	{
	case IPC_GETINIDIRECTORY:	ipcName = _T("IPC_GETINIDIRECTORY");	break;
	case IPC_GETVERSION:		ipcName = _T("IPC_GETVERSION");			break;
	case IPC_GETINIFILE:		ipcName = _T("IPC_GETINIFILE");			break;
	case IPC_GET_EMBEDIF:		ipcName = _T("IPC_GET_EMBEDIF");		break;
	case IPC_SETVISWND:			ipcName = _T("IPC_SETVISWND");			break;
	case IPC_GETSKIN:			ipcName = _T("IPC_GETSKIN");			break;
	default:					ipcName = _T("***");					break;
	}
#endif
	switch(lParam)
	{
	case IPC_GET_EMBEDIF:
		s_ThisContainer = m_hwndNormalContainer;//Hack: Usually a plugin asks for 
		//the callback and immediately afterwards call it to get the state
		//It is a C Callback. I am trying to support more than one visualization
		//meaning to make WinampVisualization NOT a Singleton class
		return (LRESULT) &getWindowState;
	case IPC_GETPLAYLISTTITLE://I ve seen many times to crash here....
		//memcpy((LPTSTR)lParam, "Track", 5+1);
		//return TRUE;
		return FALSE;
	case IPC_GETOUTPUTTIME:
		/* int res = SendMessage(hwnd_winamp,WM_WA_IPC,mode,IPC_GETOUTPUTTIME);
		** returns the position in milliseconds of the current track (mode = 0), 
		** or the track length, in seconds (mode = 1). Returns -1 if not playing or error.
		*/
		if (m_mpe != NULL)
		{
			switch (wParam)
			{
			case 0:
				return LRESULT(1000 * m_mpe->GetMediaPos());
			case 1:
				return LRESULT(1000 * m_mpe->GetMediaLength());
			}
		}
		break;
	case IPC_GETINFO:
		/* (requires Winamp 2.05+)
		** int inf=SendMessage(hwnd_winamp,WM_WA_IPC,mode,IPC_GETINFO);
		** IPC_GETINFO returns info about the current playing song. The value
		** it returns depends on the value of 'mode'.
		** Mode      Meaning
		** ------------------
		** 0         Samplerate (i.e. 44100)
		** 1         Bitrate  (i.e. 128)
		** 2         Channels (i.e. 2)
		** 3 (5+)    Video LOWORD=w HIWORD=h
		** 4 (5+)    > 65536, string (video description)
		*/
		if (m_mpe != NULL)
		{
			if (wParam == 0) return m_mpe->GetMediaInfo(MEOII_Frequency);
			if (wParam == 1) return m_mpe->GetMediaInfo(MEOII_Bitrate);
			if (wParam == 2) return m_mpe->GetMediaInfo(MEOII_Channels);
			return 0;
		}
		break;
	case IPC_GETLISTLENGTH:		return 1;		//TODO
	case IPC_GETLISTPOS:		return 1;		//TODO
	case IPC_GETVERSION:		return 0x20529;	//Emulate Winamp 5.29
	case IPC_GETINIDIRECTORY:	
		{
			GetModuleFileNameA(NULL, bf, MAX_PATH);
			char* pos = strrchr(bf, '\\');
			pos++;
			*pos = 0;
			return (LRESULT)bf;
		}
	case IPC_ISPLAYING:
		if (m_mpe != NULL)
		{
			switch (m_mpe->GetPlayState())
			{
			case PS_Playing:	return 1;
			case PS_Paused:		return 3;
			}
		}
		return 1;//Needs it to keep it playing (Geiss)
	case IPC_GETSKIN:		
		//TRACE(_T("Asked Skin Dir\n"));
		return 0;//(LRESULT)emptyString;
	case IPC_GETINIFILE:	
		//TRACE(_T("Asked Ini File\n"));
		return 0;//(LRESULT)iniFile;//(LRESULT)emptyString;
	case 336:				
		//TRACE(_T("Asked Plugin Dir\n"));
		return 0;//(LRESULT)iniDir;//(LRESULT)emptyString;
	case IPC_SETVISWND:		
		//TRACE(_T("IPC_SETVISWND\n"));
		return 1;
	case IPC_STARTPLAY:							//TODO
	default:
		TRACE(_T("Unhandled Plugin IPC: %d\n"), lParam);
		return 0;
		break;
	}
	return 0;
}

WinampVisualization* WinampVisualization::GetWinampVisualization(HWND hwnd)
{
	LONG_PTR ud = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	return (WinampVisualization*) ud;
}


LRESULT CALLBACK WinampVisualization::WinampWndProcST(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	//return DefWindowProc(hwnd, msg, wParam, lParam);
	switch(msg)
	{
	case WM_GETTEXT:
		{
			_tcsncpy((TCHAR*)lParam, _T("TS:getText"), wParam);
			return _tcslen((const TCHAR*)lParam);
		}
		break;
	case WM_WA_IPC://Same as WM_USER
		{
			WinampVisualization* pVis = (WinampVisualization*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (pVis)
				return pVis->HandleWAIPC(wParam, lParam);
			ASSERT(0);
			return 0;
		}
		break;

	}
	//return 0;
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

long WinampVisualization::Cnv16to8(signed short *source,signed char *dest,unsigned long samples)
{
	unsigned long i;
	for (i=0;i<samples;i=i+2)
		dest[i>>1]=source[i]>>8;
	return i;
}

HWND WinampVisualization::s_ThisContainer = 0;
HWND WinampVisualization::getWindowState(embedWindowState* ws)
{
	if (ws!=NULL)
	{

		ws->me = s_ThisContainer;//m_inst->m_hwndContainer;
		ws->r.left = 10;
		ws->r.top = 10;
		ws->r.bottom = 200;
		ws->r.right = 200;
		//GetClientRect(s_ThisContainer, &(ws->r));

	}
	return s_ThisContainer;
}

