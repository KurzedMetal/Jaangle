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

#include "TagInfo.h"
#include "LabelEx.h"
#include "FlexButton.h"

class CTracksSerializerDlg : public CDialog
{
	DECLARE_DYNAMIC(CTracksSerializerDlg)

public:
	CTracksSerializerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTracksSerializerDlg();

	void SetMode(BOOL bImport)				{m_bImport = bImport;}
	void AddTrack(TagInfo& ti, LPCTSTR URL);
	void GetTrack(UINT itemID, TagInfo*& ti, LPCTSTR& URL, BOOL& bActive);
	UINT GetCount();

// Dialog Data
	enum { IDD = IDD_SERIALIZETRACKS };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnLvnGetdispinfoTracklist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMCustomdrawTracklist(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
private:
	std::vector<TagInfo> m_ItemsTagInfo;
	std::vector<std::tstring> m_ItemsURL;
	std::vector<BOOL> m_ItemsChecked;
	CListCtrl m_TrackList;
	HICON m_hIcon;
	BOOL m_bImport;
	CLabelEx m_description;
	FlexButton m_btnOK;
	FlexButton m_btnCancel;



private:
	void PositionControls(int cx, int cy);
	afx_msg void OnLvnItemchangedTracklist(NMHDR *pNMHDR, LRESULT *pResult);
};
