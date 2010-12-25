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
#ifndef _WinampVisualization_h_
#define _WinampVisualization_h_

#include "MediaVisualization.h"

#include "vis.h"//nullsoft stuff
#include "wa_ipc.h"

#define SOUND_BUFFER_LENGTH 4 * 4096 / 2



class WinampVisualization:public MediaVisualization
{
public:
	WinampVisualization();
	virtual ~WinampVisualization();
	virtual void SetMediaPlayerEngine(MediaPlayerEngine* mpe);
	virtual void SetContainerHWND(HWND hwnd);
	virtual HWND GetContainerHWND();

	virtual void Start();
	virtual void Stop();

	virtual BOOL IsActive();
	virtual void SetPosition(int x, int y, int cx, int cy);

	//Extra Interface
	BOOL LoadPlugin(LPCTSTR plugInPath);
	//BOOL GetPluginName(LPTSTR name, UINT bufferLen);
	//BOOL GetModuleName(UINT moduleIdx, LPTSTR name, UINT bufferLen);


private:
	void Init();
	void UnInit();

	BOOL LoadPluginReal();

	static UINT __cdecl VisThreadStatic(LPVOID lpParam);

private:
	BOOL m_bStarted;
	BOOL m_bInitialized;
	CWinThread* m_pThread;
	BOOL m_bThreadExit;
	HWND m_hwndVisualization;	//AVS creates & owns this window
	HWND m_hwndContainer;		//Host Application owns this window
	HWND m_hwndHiddenContainer;	//Use: Hides the Vis Window when m_hwndContainer = 0 (before set)
	HWND m_hwndNormalContainer; //Wraps the AVS window
	MediaPlayerEngine* m_mpe;
	CRect m_wndRC;
	signed short m_samplesBuffer[SOUND_BUFFER_LENGTH];
	TCHAR m_PluginPath[MAX_PATH];


private:
	//void ReleasePlugin();
	long Cnv16to8 (signed short *source,signed char *dest,unsigned long samples);
	void UpdatePlugin(winampVisModule* mod);
	DWORD WINAPI VisThread();



private://statics
	LRESULT HandleWAIPC(WPARAM wParam, LPARAM lParam);
	static HWND s_ThisContainer;
	static HWND getWindowState(embedWindowState* ws);
	static WinampVisualization* GetWinampVisualization(HWND hwnd);
	
	static LRESULT CALLBACK WinampWndProcST(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
