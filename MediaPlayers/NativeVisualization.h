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
#ifndef _NativeVisualization_h_
#define _NativeVisualization_h_

#include "MediaVisualization.h"
#include <map>

#define SOUND_BUFFER_LENGTH 4 * 4096 / 2



class NativeVisualization:public MediaVisualization
{
public:
	NativeVisualization();
	virtual ~NativeVisualization();

	virtual void SetMediaPlayerEngine(MediaPlayerEngine* mpe);
	virtual void SetContainerHWND(HWND hwnd);
	virtual HWND GetContainerHWND();

	virtual void Start();
	virtual void Stop();

	virtual BOOL IsActive();
	virtual void SetPosition(int x, int y, int cx, int cy);

private:
	MediaPlayerEngine* m_mpe;
	HWND m_viewer;
//	BOOL m_bActive;
	WNDPROC m_OldWindowProc;

	int m_width;
	int m_height;
	UINT m_specmode;
	HDC m_specdc;
	HBITMAP m_specbmp;
	BYTE* m_specbuf;
	INT m_specpos;

private:
	static std::map<HWND, NativeVisualization*> m_ObjectMapping;
	static LRESULT CALLBACK WinProcST(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void UpdateSpectrum();
	BOOL SetBitmap();

	HWND m_hwndVis;
};

#endif
