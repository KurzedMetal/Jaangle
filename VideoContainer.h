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
#ifndef _CVideoContainer_h_
#define _CVideoContainer_h_

#pragma once

#include "MediaPlayer.h"

class CVideoContainer:public CWnd
{
public:
	CVideoContainer():
	m_pMediaPlayer(NULL),
		m_windowStyle(0),
		m_windowExStyle(0),
		m_windowParent(0),
		m_bFullScreen(FALSE)
	{}
	virtual ~CVideoContainer();

	BOOL Init(MediaPlayer* pPlayer);
	BOOL IsFullScreen()								{return m_bFullScreen;}
	void SetFullScreen(BOOL bFullScreen);


protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	MediaPlayer* m_pMediaPlayer;
	BOOL m_bFullScreen;
	DWORD m_windowStyle;
	DWORD m_windowExStyle;
	CWnd* m_windowParent;
	CRect m_rcWindow;
};

#endif
