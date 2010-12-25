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

// CSimpleEditorDlg dialog

class CSimpleEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CSimpleEditorDlg)

public:
	CSimpleEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSimpleEditorDlg();

	void SetTitle(LPCTSTR title)			{m_title = title;}
	void SetSubtitle(LPCTSTR subTitle)		{m_subTitle = subTitle;}

	void SetText(LPCTSTR text)				{m_text = text;}
	LPCTSTR GetText()						{return m_text;}

// Dialog Data
	enum { IDD = IDD_SIMPLEEDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	FlexDialogHeader m_header;

public:
	virtual BOOL OnInitDialog();
	CString m_title;
	CString m_subTitle;
	CString m_text;
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedSave();
};
