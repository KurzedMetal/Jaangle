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
#ifndef _CVisualizationContainer_h_
#define _CVisualizationContainer_h_

#pragma once
//#include "IVisualizationContainer.h"
class MediaPlayer;

class CVisualizationContainer:public CWnd
{
public:
	CVisualizationContainer():
	m_pMediaPlayer(NULL),
		m_WaitToStopVis(0),
		m_windowStyle(0),
		m_windowExStyle(0),
		m_windowParent(0),
		m_bFullScreen(FALSE)
	{}
	virtual ~CVisualizationContainer();

	virtual void SetMediaPlayer(MediaPlayer* pPlayer);	

private:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	void UpdateVisualization();

private:
	BOOL SetFullScreen(BOOL bFullScreen);
	MediaPlayer* m_pMediaPlayer;
	INT m_WaitToStopVis;
	BOOL m_bFullScreen;
	DWORD m_windowStyle;
	DWORD m_windowExStyle;
	CWnd* m_windowParent;
	CRect m_rcWindow;

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
};

#endif
