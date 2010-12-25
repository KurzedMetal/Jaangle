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
#ifndef _CCollectionsDlg_h_
#define _CCollectionsDlg_h_
#include "CollectionsList.h"
//#include "DirectorPat/Director.h"
//#include "TransmitterPattern/Transmitter.h"
//#include "XHTMLStatic.h"
#include "LabelEx.h"
#include "GuiControls\FlexDialogHeader.h"
#include "StateManager.h"


class CollectionManager;

class CCollectionsDlg : public CDialog, public TSStateChangeListener
{
	DECLARE_DYNAMIC(CCollectionsDlg)

public:
	CCollectionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCollectionsDlg();
	enum { IDD = IDD_COLLECTIONS };

public:
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()				{return _T("CCollectionsDlg");}

	void UpdateInfo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedUpdate();
	afx_msg LRESULT ListNotification(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void Refresh();
	void ResizeControls(int cx, int cy);


private:
	HICON m_hIcon;
	CCollectionsList m_List;
	CLabelEx m_info;
	FlexDialogHeader m_header;

};
#endif
