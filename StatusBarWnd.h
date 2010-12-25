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

#include "afxcmn.h"
#include "MessageViewer.h"
#include "FlexButton.h"
#include "LabelEx.h"
#include "TSTranslatable.h"
#include "TSSkinnable.h"
#include "TransmitterPattern/Transmitter.h"
#include "GuiControls/AnimationWnd.h"
#include "TickerWnd.h"
#include "CollectionUpdateJob.h"
#include "LevelMeter.h"
#include "MediaSlider.h"
#include "CtrlColorsMediaSliderAdapter.h"
#include "GuiControls/BCMenu.h"


class MediaPlayer;
// CStatusBarChDlg dialog
class IVisualizationContainer;

class CStatusBarWnd : public CWnd, public MessageViewer, public TSSkinnable, public TSTranslatable//, public Receiver
{
public:
	CStatusBarWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatusBarWnd();
	
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	virtual void Show(LPCTSTR msg, LPCTSTR title, SEVERITY sev);
	void Update();
// Dialog Data
	virtual void ApplyTranslation(ITranslation& skin);	//ITranslatable
	virtual void ApplySkin(ISkin& _skin);				//TSSkinnable


#ifdef _DEBUG
	virtual LPCTSTR GetReceiverName()					{return _T("CStatusBarChDlg");}
#endif


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
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

private:
	//void Resize(int cx = 0, int cy = 0);
	void ConfigureButton(FlexButton& btn, HICON hIcon);
	void ResizePlayerControls();
	void ResizeInfoControls();
	void SetTimeInfo(DOUBLE pos, DOUBLE total);
	void ResizeWindow(int cx, int cy);


private:
	CTickerWnd m_ticker;
	//CLabelEx m_InfoText;
	CAnimationWnd m_MediaAnim;
	CLabelEx m_MediaText;
	CAnimationWnd m_NetAnim;
	CLabelEx m_NetText;

	CFont m_MainFont;
	CRect m_rcInfo;
	INT m_netPendingJobs;
	CollectionUpdateJob::Progress m_oldColProgress;
};
