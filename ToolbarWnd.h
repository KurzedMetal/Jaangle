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
#ifndef _CMainToolbarDlg_h_
#define _CMainToolbarDlg_h_

#include "afxwin.h"

#include "FlexButton.h"
#include "TSTranslatable.h"
#include "TSSkinnable.h"

#include "GuiControls/BCMenu.h"
#include "PrgAPI.h"

class CToolbarWnd : public CWnd, public TSSkinnable, public TSTranslatable
{
public:
	CToolbarWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CToolbarWnd();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);



	enum Buttons
	{
		CMD_Search = 1400,
		CMD_History,
		CMD_Options,
		CMD_Collections,
		CMD_Games,
		CMD_About,

		//CMD_DecreaseFontSize,
		//CMD_ResetFontSize,
		//CMD_IncreaseFontSize,
		CMD_SetFontSize,

		CMD_Min,
		CMD_Restore,
		CMD_Close,

		CMD_Last
	};

	//virtual void SetFont(CFont* m_ToolFont);

	//---------------------------------------------------
	//IControl ---------------------------------------
	//---------------------------------------------------
public:
	virtual void ApplyTranslation(ITranslation& translation);
	virtual void ApplySkin(ISkin& skin);

	//---------------------------------------------------
	//ICtrlColors ---------------------------------------
	//---------------------------------------------------
public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_ButtonBk,
		COL_ButtonText,
		COL_ButtonOverBk,
		COL_ButtonOverText,
		COL_ButtonDownBk,
		COL_ButtonDownText,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	//virtual LPCTSTR GetColorName(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------


protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnOK(){}
	virtual void OnCancel(){}

	DECLARE_MESSAGE_MAP()

private:
	void ClearMenu();

	FlexButton m_About;

	FlexButton m_Search;
	FlexButton m_History;
	FlexButton m_Options;
	FlexButton m_Collections;
	FlexButton m_Games;

	//FlexButton m_decreaseFontSize;
	//FlexButton m_resetFontSize;
	//FlexButton m_increaseFontSize;
	FlexButton m_setFontSize;

	FlexButton m_min;
	FlexButton m_restore;
	FlexButton m_close;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	void PositionControls(INT cx, INT cy);
	INT PositionButtonLeft(FlexButton& btn, INT curLeft, INT Height);
	void ConfigureButton(FlexButton& btn, APP_ICONS ico);
	void ConfigureRightButtons(FlexButton& btn, APP_ICONS ico);

	INT m_ToolbarHeight;
	BCMenu m_collectionsMenu;
	BCMenu m_aboutMenu;
	BCMenu m_optionsMenu;
	BCMenu m_fontSizeMenu;
	BOOL m_bPassedOnEraseBk;

};

#endif
