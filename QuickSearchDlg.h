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
#ifndef _QuickSearchDlg_h_
#define _QuickSearchDlg_h_

#include "TSTranslatable.h"
#include "TSSkinnable.h"
//#include "LabelEx.h"
#include "GuiControls/IconEditEx.h"
#include "TracksFilter.h"
#include "TeenSpiritEnums.h"
#include "TrackListCtrl.h"
#include "FlexButton.h"

class CIconEditEx;

class QuickSearchDlg :	public CWnd,
							public ISkinnable, 
							public ITranslatable
{
public:
	QuickSearchDlg();
	virtual ~QuickSearchDlg();

public:
	enum RelativePosition
	{
		RP_Centered,
		RP_Outside
	};

	void ShowRelative(CWnd* wndRelative, RelativePosition rp);
	void SetSearchText(LPCTSTR searchText);
	//INT GetDesiredHeight();

	virtual void ApplyTranslation(ITranslation& skin);
	virtual void ApplySkin(ISkin& skin);

public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_Last
	};

	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

protected:
	BOOL Create(CWnd* pParent);
	void Resize(int cx, int cy);
	void SetTitleHeight(INT height);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChangeSearchedt();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
private:
	void HideWindow();
	CIconEditEx m_searchEdit;
	TrackListCtrl m_trackList;
	FullTrackRecordCollection m_tracks;
	INT m_titleHeight;
	//INT m_records;
	std::tstring m_initialSearchText;
	FlexButton m_closeButton;

	BOOL m_bCloseOnAction;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};

#endif
