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
#include "GuiControls/FlexDialogHeader.h"
#include "FlexButton.h"
#include "afxwin.h"

// CEmptyDlg dialog

class CMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMessageDlg();

// Dialog Data
	enum { IDD = IDD_MESSAGE };

	enum TextType
	{
		TT_Title,
		TT_SubTitle,
		TT_ButtonRight,			//Right button
		TT_ButtonMid,
		TT_ButtonLeft,
		TT_CheckBox,
		TT_Last
	};
	enum Result
	{
		RES_Close = 100,
		RES_Right,
		RES_Mid,
		RES_Left,
		RES_Last
	};
	void SetText(TextType tt, LPCTSTR text)				{m_text[tt] = text;}
	void SetIcon(HICON hIcon)							{m_hIcon = hIcon;}
	void SetHelpLink(LPCTSTR text, LPCTSTR url)			{m_helpText = text; m_helpURL = url;}
	BOOL IsChecked()									{return m_bChecked;}
	void SetCheck(BOOL bCheck)							{m_bChecked = bCheck;}
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClicked1();
	afx_msg void OnBnClicked2();
	afx_msg void OnBnClicked3();
	afx_msg void OnStnClickedLink();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	FlexDialogHeader m_header;
	FlexButton m_helpLink;
	CString m_text[TT_Last];
	CString m_helpText;
	CString m_helpURL;
	HICON m_hIcon;
	BOOL m_bChecked;
public:
	afx_msg void OnDestroy();
};
