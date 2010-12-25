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
#include "afxwin.h"
#include "afxcmn.h"

// CUserProgressDlg dialog

class CUserProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CUserProgressDlg)

public:
	CUserProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUserProgressDlg();

// Dialog Data
	enum { IDD = IDD_USERPROGRESS };

	void SetTitle(LPCTSTR title);
	void SetMessage(LPCTSTR message);
	void SetProgress(FLOAT value);
	void SetIcon(HICON icon);

	void Show();
	void Hide();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	FLOAT m_progressValue;
	CString m_title;
	CString m_message;
	HICON m_icon;


private:
	FlexDialogHeader m_header;
	CProgressCtrl m_progress;
	CWnd* m_pParent;

	HCURSOR m_hWaitCursor; 
	HCURSOR m_hOldCursor;
};
