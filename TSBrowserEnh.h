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
#ifndef _TSBrowserEnh_h_
#define _TSBrowserEnh_h_

#include "SimpleBrowser.h"
#include "FlexButton.h"
#include "GuiControls/IconEditEx.h"
#include "TSSkinnable.h"

class TSBrowserEnh : public CWnd, public TSSkinnable, public SimpleBrowserNotifier
{
public:
	TSBrowserEnh();
	virtual ~TSBrowserEnh();

	void SetURL(LPCTSTR url);
	//---------------------------------------------------
	//ICtrlColors ---------------------------------------
	//#include "ICtrlColors.h" // ,public ICtrlColors
	//---------------------------------------------------
public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);


	virtual void ApplySkin(ISkin& skin);


public:
	virtual void OnBeforeNavigate(LPCTSTR url)								{}
	virtual void OnDocumentComplete(LPCTSTR url)							{}
	virtual void OnDownloadBegin()											{}
	virtual void OnProgressChange(long progress,long progress_max);
	virtual void OnDownloadComplete()										{}
	virtual void OnNavigateComplete(LPCTSTR url);
	virtual void OnStatusTextChange(LPCTSTR text)							{}
	virtual void OnTitleChange(LPCTSTR text)								{}
	virtual void EnableBackButton(BOOL bEnable);
	virtual void EnableNextButton(BOOL bEnable);


	//virtual LPCTSTR GetColorName(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	void OnInit();
	void OnHome();
private:
	enum BUTTONS
	{
		//BTN_Add,
		BTN_Back,
		BTN_Forward,
		BTN_Refresh,
//		BTN_Stop, 
		BTN_Home,
		BTN_Last
	};
	SimpleBrowser m_Browser;
	FlexButton m_Buttons[BTN_Last];
	CIconEditEx m_Address;
	BOOL m_bInitialized;

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

#endif
