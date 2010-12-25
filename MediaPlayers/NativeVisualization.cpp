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
#include "NativeVisualization.h"
#include "MediaPlayerEngine.h"
#include <stdio.h>
#include <math.h>
#include <malloc.h>

#define TIMER_VIS_UPDATE 100

std::map<HWND, NativeVisualization*> NativeVisualization::m_ObjectMapping;

NativeVisualization::NativeVisualization():
m_mpe(NULL),
m_viewer(0),
m_OldWindowProc(NULL),
m_width(368),
m_height(127),
m_specmode(0),
m_specdc(0),
m_specbmp(0),
m_specbuf(0),
m_specpos(0),
m_hwndVis(NULL)
{

}

NativeVisualization::~NativeVisualization()
{
	SetMediaPlayerEngine(NULL);
	SetContainerHWND(NULL);
}

void NativeVisualization::SetMediaPlayerEngine(MediaPlayerEngine* mpe)
{
	if (m_specdc)	
		DeleteDC(m_specdc);
	if (m_specbmp)	
		DeleteObject(m_specbmp);
	m_mpe = mpe;
	if (mpe)
		SetBitmap();
}
void NativeVisualization::SetContainerHWND(HWND hwnd)
{
	m_viewer = hwnd;
	m_ObjectMapping[m_viewer] = this;
}
HWND NativeVisualization::GetContainerHWND()
{
	return m_viewer;
}

void NativeVisualization::Start()
{
	if (m_hwndVis != NULL)	return;
	ASSERT(m_viewer != NULL);
	ASSERT(m_OldWindowProc == NULL);

	CRect rc;
	::GetClientRect(m_viewer, &rc);

	m_hwndVis = CreateWindowEx(0, _T("Static"), NULL, WS_VISIBLE | WS_CHILD, 0, 0, rc.Width(), rc.Height(), 
		m_viewer, (HMENU) NULL, AfxGetInstanceHandle(), NULL);
	m_OldWindowProc = (WNDPROC) SetWindowLongPtr(m_hwndVis, GWLP_WNDPROC, (LONG_PTR) WinProcST);
	SetWindowLongPtr(m_hwndVis, GWLP_USERDATA, (LONG_PTR) this);

	SetTimer(m_hwndVis, TIMER_VIS_UPDATE, 25, NULL);
}

void NativeVisualization::Stop()
{
	if (m_hwndVis == NULL)	return;
	ASSERT(m_OldWindowProc != NULL);
	KillTimer(m_hwndVis, TIMER_VIS_UPDATE);
	m_OldWindowProc = NULL;
	DestroyWindow(m_hwndVis);
	m_hwndVis = NULL;
}
BOOL NativeVisualization::IsActive()
{
	return m_hwndVis != NULL;
}
void NativeVisualization::SetPosition(int x, int y, int cx, int cy)
{
	::MoveWindow(m_hwndVis, x, y, cx, cy, TRUE);
}
//-----------------------------------------------------
//Private
//-----------------------------------------------------
LRESULT CALLBACK NativeVisualization::WinProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	NativeVisualization* pNVis = (NativeVisualization*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (pNVis != NULL)
		return pNVis->WinProc(hwnd, uMsg, wParam, lParam);
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK NativeVisualization::WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		if (GetUpdateRect(hwnd,0,0)) {
			PAINTSTRUCT p;
			HDC dc;
			if (!(dc=BeginPaint(hwnd,&p))) return 0;
			//BitBlt(dc,0,0,m_width,m_height,m_specdc,0,0,SRCCOPY);
			RECT rc;
			GetClientRect(hwnd, &rc);
			StretchBlt(dc, 0,0,rc.right - rc.left, rc.bottom - rc.top, m_specdc, 0, 0, m_width, m_height, SRCCOPY);
			EndPaint(hwnd,&p);
		}
		return 0;

	case WM_LBUTTONUP:
		m_specmode=(m_specmode+1)%5; // swap spectrum mode
		memset(m_specbuf,0,m_width*m_height);	// clear display
		return 0;
	//case WM_LBUTTONDBLCLK:
	//	return OnFullScreen();
	case WM_TIMER:
		if (wParam == TIMER_VIS_UPDATE)
		{
			UpdateSpectrum();
			return 0;
		}
		break;
	}
	return CallWindowProc(m_OldWindowProc, hwnd, uMsg, wParam, lParam);
}
/*
BOOL NativeVisualization::IsFullScreen()
{
	return m_bFullScreen;
}

BOOL NativeVisualization::SetFullScreen(BOOL bFullScreen)
{
	m_bFullScreen = bFullScreen;
	if (m_bFullScreen)
	{
		CRect rc;
		GetWindowRect(::GetParent(m_PlayerWindow.m_hWnd), &rc);
		HMONITOR mon = ::MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof( mi );
		::GetMonitorInfo( mon, &mi );
		rc = mi.rcMonitor;

		m_oldStyle = GetWindowLong(m_PlayerWindow.m_hWnd, GWL_EXSTYLE);
		DWORD newStyle = m_oldStyle & (~WS_EX_CLIENTEDGE);
		//GetWindowRect()
		m_PlayerWindow.SetParent(NULL);
		//SetParent(m_viewer, NULL);
		SetWindowLong(m_PlayerWindow.m_hWnd, GWL_EXSTYLE, newStyle);
		SetWindowPos(m_PlayerWindow.m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW);
		//ShowWindow(m_viewer, SW_SHOWMAXIMIZED);
		//UpdateWindow(m_viewer);
	}
	else
	{
		//style &= ~WS_POPUP;
		//DWORD style = m_oldStyle;
		SetWindowLong(m_PlayerWindow.m_hWnd, GWL_EXSTYLE, m_oldStyle);
		//SetParent(m_viewer, m_originalParent);
		SetWindowPos(m_PlayerWindow.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOREPOSITION | SWP_SHOWWINDOW);
		m_PlayerWindow.SetParent(m_PlayerWindow.m_hWndOldParent);
		//CRect rc;
		//GetClientRect(m_originalParent, &rc);
		//SendMessage(m_originalParent, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
		//ShowWindow(m_viewer, SW_RESTORE);
		//UpdateWindow(m_viewer);

	}

}
*/
BOOL NativeVisualization::SetBitmap()
{
	BYTE data[2000]={0};
	BITMAPINFOHEADER *bh=(BITMAPINFOHEADER*)data;
	RGBQUAD *pal=(RGBQUAD*)(data+sizeof(*bh));
	int a;
	bh->biSize=sizeof(*bh);
	bh->biWidth=m_width;
	bh->biHeight=m_height; // upside down (line 0=bottom)
	bh->biPlanes=1;
	bh->biBitCount=8;
	bh->biClrUsed=bh->biClrImportant=256;
	// setup palette
	for (a=1;a<128;a++) {
		pal[a].rgbGreen=256-2*a;
		pal[a].rgbRed=2*a;
	}
	for (a=0;a<32;a++) {
		pal[128+a].rgbBlue=8*a;
		pal[128+32+a].rgbBlue=255;
		pal[128+32+a].rgbRed=8*a;
		pal[128+64+a].rgbRed=255;
		pal[128+64+a].rgbBlue=8*(31-a);
		pal[128+64+a].rgbGreen=8*a;
		pal[128+96+a].rgbRed=255;
		pal[128+96+a].rgbGreen=255;
		pal[128+96+a].rgbBlue=8*a;
	}
	// create the bitmap
	m_specbmp=CreateDIBSection(0,(BITMAPINFO*)bh,DIB_RGB_COLORS,(void**)&m_specbuf,NULL,0);
	m_specdc=CreateCompatibleDC(0);
	SelectObject(m_specdc,m_specbmp);
	return TRUE;

}

//#define SPEC_MAX_WAVEFORM_RES 200
void NativeVisualization::UpdateSpectrum()
{
	int y,y1;
	UINT channels = m_mpe->GetMediaInfo(MEOII_Channels);
	if (channels == 0)	return;//No Info Loaded

	if (m_specmode==3) 
	{ // waveform
		//BASS_CHANNELINFO ci;
		memset(m_specbuf,0,m_width*m_height);
		//BASS_ChannelGetInfo(chan,&ci); // get number of channels
		UINT bufLen = channels * m_width * sizeof(float);
		float *buf = (float*)alloca(bufLen); // allocate buffer for data
		m_mpe->GetSoundData(MSD_Float, buf, bufLen);
		//BASS_ChannelGetData(chan,buf,(ci.chans*SPECWIDTH*sizeof(float))|BASS_DATA_FLOAT); // get the sample data (floating-point to avoid 8 & 16 bit processing)
		for (UINT c=0; c < channels; c++) {
			for (int x=0;x<m_width;x++) {
				int v=(int)((1 - buf[x * channels + c]) * m_height / 2); // invert and scale to fit display
				if (v<0) v=0;
				else if (v>=(int)m_height) v=m_height-1;
				if (!x) y=v;
				do { // draw line from previous sample...
					if (y<v) y++;
					else if (y>v) y--;
					m_specbuf[y*m_width+x]=c&1?127:1; // left=green, right=red (could add more colours to palette for more chans)
				} while (y!=v);
			}
		}
	} 
	else 
	{
		float fft[1024];
		m_mpe->GetSoundData(MSD_FFT, fft, 1024 * sizeof(float));
		//BASS_ChannelGetData(chan,fft,BASS_DATA_FFT2048); // get the FFT data
		switch (m_specmode)
		{
		case 0:// "normal" FFT
			memset(m_specbuf, 0, m_width * m_height);
			for (int x=0;x<m_width/2;x++) {
#if 1
				y=(int)(sqrt(fft[x+1])*3*m_height-4); // scale it (sqrt to make low values more visible)
#else
				y=fft[x+1]*10*m_height; // scale it (linearly)
#endif
				if (y < 0)		y = 0;
				if (y>(int)m_height) y=m_height; // cap it
				if (x && (y1=(y+y1)/2)) // interpolate from previous to make the display smoother
					while (--y1>=0) m_specbuf[y1*m_width+x*2-1]=y1+1;
				y1=y;
				while (--y>=0) m_specbuf[y*m_width+x*2]=y+1; // draw level
			}
			break;
		case 1:
			{ // logarithmic, acumulate & average bins
				int b0=0;
				memset(m_specbuf,0,m_width*m_height);
	#define BANDS 10
				for (int x=0;x<BANDS;x++) 
				{
					float sum=0;
					int sc,b1=(int)pow(2.0f,float(x*10.0/(BANDS-1)));
					if (b1>1023) b1=1023;
					if (b1<=b0) b1=b0+1; // make sure it uses at least 1 FFT bin
					sc=10+b1-b0;
					for (;b0<b1;b0++) sum+=fft[1+b0];
					y=(int)((sqrt(sum/log10(float(sc)))*1.7*m_height)-4); // scale it
					if (y < 0)		y = 0;
					if (y>(int)m_height) y=m_height; // cap it
					while (--y>=0)
						memset(m_specbuf+y*m_width+x*(m_width/BANDS),y+1,m_width/BANDS-2); // draw bar
				}
			} 
			break;
		case 2:// "3D"
			for (int x = 0;x<m_height;x++) 
			{
				y=(int)(sqrt(fft[x+1])*3*127); // scale it (sqrt to make low values more visible)
				if (y < 0)		y = 0;
				if (y>127) y=127; // cap it
				m_specbuf[x*m_width+m_specpos]=128+y; // plot it
			}
			// move marker onto next position
			m_specpos=(m_specpos+1)%m_width;
			for (int x=0;x<m_height;x++) 
				m_specbuf[x*m_width+m_specpos]=255;
			break;
		case 5:// "Alex 1"
			{

				float sum=0;
				for (int b0 = 0; b0 < 1023; b0++) 
					sum+=fft[1+b0];
				y=(int)((sqrt(sum/log10(1023.0))*1.7*m_height)-4); // scale it
				if (y < 0)		y = 0;
				if (y>m_height) y=m_height; // cap it
				while (--y>=0)
					memset(m_specbuf+y*m_width+m_width,y+1,-m_width); // draw bar
			}
			break;
		}
	}

	// update the display
	RECT rc;
	GetClientRect(m_hwndVis, &rc);
	HDC dc=GetDC(m_hwndVis);
	//BitBlt(dc,0,0,m_width,m_height,m_specdc,0,0,SRCCOPY);
	StretchBlt(dc, 0,0,rc.right - rc.left, rc.bottom - rc.top, m_specdc, 0, 0, m_width, m_height, SRCCOPY);
	ReleaseDC(m_hwndVis,dc);
}
