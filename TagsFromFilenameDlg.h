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

#include "DataRecords.h"
#include "afxwin.h"
#include "GuiControls\FlexDialogHeader.h"
// CTagsFromFilenameDlg dialog
struct TagInfo;

class CTagsFromFilenameDlg : public CDialog
{
	DECLARE_DYNAMIC(CTagsFromFilenameDlg)

public:
	CTagsFromFilenameDlg(FullTrackRecordCollection& col, CWnd* pParent = NULL);   // standard constructor
	virtual ~CTagsFromFilenameDlg();

// Dialog Data
	enum { IDD = IDD_TAGSFROMFILENAME };

	void SetFullTrackRecordCollection(FullTrackRecordCollection& col);

	BOOL IsSomethingChanged()	{return m_bSomethingChanged;}

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedGetTags();
	afx_msg void OnBnClickedSkip();
	afx_msg void OnBnClickedAll();
	afx_msg void OnBnClickedCancel();
	afx_msg void UpdatePreview();
	afx_msg void UpdatePreviewDelayed();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	BOOL PrepareNextTrack();


private:
	BOOL m_bSomethingChanged;
	CListCtrl m_lstPreview;
	CComboBox m_cmbCasing;
	CComboBox m_cmbPattern;
	FullTrackRecordCollection& m_col;
	FlexDialogHeader m_header;
	BOOL GetTagInfo(TagInfo& ti, LPCTSTR location, LPCTSTR pattern);
	BOOL UpdateTrack(FullTrackRecordSP& rec, LPCTSTR pat, BOOL bWriteTags);

public:
	afx_msg void OnBnClickedBtnAuto();
};
