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
#include "afxwin.h"

#include "GuiControls\FlexDialogHeader.h"

#define IDACCEPTALL 100
#define IDREJECTALL 101

class CInfoQueryDlg:public CDialog
{
	DECLARE_DYNAMIC(CInfoQueryDlg)

	enum ResultCode
	{
		RES_Accept,
		RES_Cancel,
		RES_Retry,
		RES_AcceptAll,
		RES_CancelAll,
		RES_Last
	};
public:
	CInfoQueryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoQueryDlg();

// Dialog Data
	enum { IDD = IDD_INFOQUERY };

	void SetExtraInfo(LPCTSTR title, LPCTSTR subTitle, LPCTSTR helpURL);
	void SetMainText(const std::tstring& mainText);
	const std::tstring& GetMainText()	{return m_mainText;}
	void EnableFixedFont(BOOL bFixedFont)	{m_bFixedFont = bFixedFont;}

protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	std::tstring m_mainText;
	LPCTSTR m_title;
	LPCTSTR m_subTitle;
	LPCTSTR m_helpURL;
	HBRUSH m_EditBrush;
	const COLORREF m_bkColor;
	FlexDialogHeader m_header;
	BOOL m_bFixedFont;

	virtual void OnCancel();
};
