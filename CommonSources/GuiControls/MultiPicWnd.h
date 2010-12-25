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
#pragma once
#include "RectAlphaDrawer.h"
#include "FlickerFree.h"
#include "GdiPlusPicDrawer.h"
#include <string>
#include <vector>

// CAnimationWnd

class CMultiPicWnd : public CWnd
{
	DECLARE_DYNAMIC(CMultiPicWnd)
public:
	CMultiPicWnd();
	virtual ~CMultiPicWnd();

	void AddImage(LPCTSTR imgFile, LPCTSTR imgName = _T(""));
	void ClearImages();
	UINT GetCount();

	void SetBkColor(COLORREF rgb, BYTE opacity = 255);
	GdiPlusPicDrawer::DrawParams& GetDrawParams()		{return m_picDrawer.GetDrawParams();}
	void SetTitleFont(CFont* pTitleFont)	{m_pTitleFont = pTitleFont;}

	void SetTransitionTime(UINT ms);

	enum Mode
	{
		MOD_Normal,
		MOD_ShowParts,
		MOD_Last
	};
	void SetMode(Mode mod)						{m_mode = mod;}
	Mode GetMode()								{return m_mode;}

	void Start();
	void Stop();
	//The Time starts from the start of the transition time
	void SetPictureDelay(UINT ms);
	LPCTSTR GetCurrentImagePath();
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
private:
	void Redraw();

	CFlickerFree m_ff;
	RectAlphaDrawer m_AlphaDrawer;
	INT m_curFadeStep;
	INT m_totalFadeSteps;
	BOOL m_bTransitioning;
	BOOL m_bMemDCInvalidated;
	CFont* m_pTitleFont;
	GdiPlusPicDrawer m_picDrawer;
	struct Frame
	{
		std::basic_string<TCHAR> path;
		std::basic_string<TCHAR> title;
		Frame(LPCTSTR Path, LPCTSTR Title):
		path(Path), title(Title)
		{}
	};
	std::vector<Frame> m_frames;
	INT m_curPic;
	UINT m_ChangeDelayMS;
	BOOL m_bStarted;
	BOOL m_bTemporaryDisableTransitioning;
	Mode m_mode;
};


