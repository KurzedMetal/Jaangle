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

#include <vector>
#include "IPane.h"
#include "FlexButton.h"
#include "ITSMenu.h"
#include "TSTranslatable.h"

class PaneController;

class TSPaneContainer :	public CWnd, public TSTranslatable
{
public:
	TSPaneContainer(PaneController* pPaneController);
	virtual ~TSPaneContainer();

	BOOL Create(CWnd* pParent, INT nID);
	void SetContainedPane(IPane* pPane);
	IPane* GetContainedPane()				{return m_pContainedPane;}

	void SetCaptionHeight(UINT height);
	UINT GetCaptionHeight()					{return m_captionHeight;}




	void EnableCloseButton(BOOL bEnable);
	BOOL IsCloseButtonEnabled();
	void SetMaximizedState(BOOL bEnable);
	BOOL IsMaximizedState();

	enum COLORS
	{
		COL_Bk,						//General background
		COL_Text,					//General Text (+Button Caption)
		COL_CaptionBk,				//Captions (Title) Bk
		COL_CaptionText,			//Captions Text
		COL_CaptionOutline,			//The line
		COL_Last
	};

	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);


	virtual void ApplyTranslation(ITranslation& translation);


private:
	COLORREF m_colors[COL_Last];

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);


private:
	typedef std::vector<FlexButton*> ButtonCollection;
	ButtonCollection m_userButtons;
	ButtonCollection m_systemButtons;

	UINT m_rotationDelay;
	UINT m_captionPos;
	PaneController* m_pPaneController;
	IPane* m_pContainedPane;
	UINT m_captionHeight;
	BOOL m_bPassedOnEraseBk;
	CRect m_captionRect;
	BOOL m_bAddVertical;
	ITSMenu* m_pPosistioningMenu;
	BOOL m_bIsMaximized;

private:
	BOOL AddUserButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon);
	BOOL SetUserButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon);
	BOOL AddSystemButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon);
	BOOL SetSystemButton(INT nID, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon);
	BOOL ShowSystemButton(INT nID);
	BOOL RemoveSystemButton(INT nID);

	void SetButton(FlexButton& btn, LPCTSTR text, HICON hIcon, UINT cxIcon, UINT cyIcon);;
	FlexButton* TSPaneContainer::FindButton(INT nID, ButtonCollection& btnCollection);

	void RemoveButtons(BOOL bLeft);
	void DrawPaneCaption(CDC& dc);
	void PositionControls(int cx, int cy);
	void ShowMenu(ITSMenu* pMenu, INT nButtonID);

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
