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

#include "TSPropertyPage.h"
#include "afxcmn.h"
#include "afxwin.h"

class CTaggingPP : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CTaggingPP)

public:
	CTaggingPP(UINT nIDCaption);
	virtual ~CTaggingPP();

// Dialog Data
	enum { IDD = IDD_PPTAGGING };
	BOOL IsSomethingChanged()	{return m_bSomethingChanged;}
	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings
	virtual LPCTSTR GetHelpID()						{return _T("tagging");}
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChange();
	virtual void OnOK();
	void SaveChanges();

private:
	BOOL m_bSomethingChanged;
	BOOL m_bNeedsUpdating;

	void LoadTagManagerOptions();
	void SaveTagManagerOptions();
	//void SetControlState();
	void UpdateScrollers();
	void AddString(LPCTSTR text, INT option);

	CSliderCtrl m_rdManual;
	CSliderCtrl m_rdAuto;
	CCheckListBox m_lstRead;
	CCheckListBox m_lstWrite;
};
