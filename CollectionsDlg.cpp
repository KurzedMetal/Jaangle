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

#include "stdafx.h"
#include "TeenSpirit.h"
#include "CollectionsDlg.h"
#include "ActionManager.h"
#include "SQLManager.h"
#include "CollectionManager.h"
#include "DriveDlg.h"
#include "PrgAPI.h"
//#include "CollectionsTableSchema.h"

#define ID_COL_ADDFOLDER	500
#define ID_COL_UPDATEL1		501
#define ID_COL_UPDATEL2		502
#define ID_COL_RENAME		503
#define ID_COL_REMOVE		504


IMPLEMENT_DYNAMIC(CCollectionsDlg, CDialog)
CCollectionsDlg::CCollectionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCollectionsDlg::IDD, pParent)
{
	m_hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_COLLECTIONS32), IMAGE_ICON, 16, 16, LR_VGACOLOR);//LR_LOADTRANSPARENT LR_VGACOLOR
}

CCollectionsDlg::~CCollectionsDlg()
{
	DestroyIcon(m_hIcon);
	//if (m_hWnd) DestroyWindow();
}

void CCollectionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_INFO, m_info);
}


BEGIN_MESSAGE_MAP(CCollectionsDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(ID_ADD,			&CCollectionsDlg::OnBnClickedAdd)
	ON_BN_CLICKED(ID_UPDATE,		&CCollectionsDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(ID_EXIT,			&CCollectionsDlg::OnBnClickedExit)
	ON_MESSAGE(CCollectionsList::MsgIDLeftClick, &CCollectionsDlg::ListNotification)
END_MESSAGE_MAP()


afx_msg LRESULT CCollectionsDlg::ListNotification(WPARAM wParam, LPARAM lParam)
{
	UpdateInfo();
	return TRUE;
}


// CCollectionsDlg message handlers

BOOL CCollectionsDlg::OnInitDialog()
{
	PrgAPI* pAPI = PRGAPI();
	CDialog::OnInitDialog();
	m_List.Create(WS_CHILD | WS_BORDER | WS_VISIBLE, CRect(0,0,0,0), this, 456);
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)pAPI->GetFont(FONT_Dialogs)->m_hObject);
	SetIcon(m_hIcon, FALSE);
	SetWindowText(pAPI->GetString(IDS_COLLECTIONS));
	
	SetDlgItemText(ID_ADD, pAPI->GetString(IDS_ADDCOLLECTION));
	SetDlgItemText(ID_UPDATE, pAPI->GetString(IDS_UPDATELOCALCOLLECTIONS));
	SetDlgItemText(ID_EXIT, pAPI->GetString(IDS_OK));

	m_List.GetScrollBar().SetIcons(pAPI->GetIcon(ICO_Up16), pAPI->GetIcon(ICO_Down16), 16, 16);
	m_info.SetColor(CLabelEx::COL_Bk, GetSysColor(COLOR_BTNFACE));
	m_info.SetSpecialFont(pAPI->GetFont(FONT_DialogsBold));

	m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	m_header.SetTitleText(pAPI->GetString(IDS_COLLECTIONLIST));
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(48, 48);
	m_header.CreateEx(NULL,NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER, 
		0,0,0,0,m_hWnd,0,0);

	//m_header.Init(this);
	//m_header.MoveCtrls(this);

	Refresh();
	CRect rc;
	GetClientRect(&rc);
	ResizeControls(rc.Width(), rc.Height());
	return TRUE;
}

void CCollectionsDlg::Refresh()
{
	PrgAPI* pAPI = PRGAPI();
	m_List.UpdateControl();
	UpdateInfo();
}

void CCollectionsDlg::UpdateInfo()
{
	PrgAPI* pAPI = PRGAPI();
	UINT totalSize = 0;
	UINT totalDuration = 0;
	UINT totalTrackCount = 0;
	CString info;
	const CollectionRecordCollection& crc = m_List.GetCollection();
	INT nPos = m_List.GetNextSelectedItem(-1);
	INT collections = 0;
	if (nPos == -1)
	{
		collections = crc.size();
		for (size_t i=0; i<crc.size(); i++)
		{
			CollectionRecord& cr = *(crc[i]);
			totalSize += cr.sumSize;
			totalDuration += cr.sumDuration;
			totalTrackCount += cr.trackCount;
		}
	}
	else
	{
		while (nPos != -1)
		{
			collections++;
			CollectionRecord& cr = *(crc[nPos]);
			totalSize += cr.sumSize;
			totalDuration += cr.sumDuration;
			totalTrackCount += cr.trackCount;
			nPos = m_List.GetNextSelectedItem(nPos);
		}
	}
	info.Format(
		_T("<s>%s</s> (")
		_T("%s:<s> %d</s> | ")	//Collections
		_T("%s:<s> %d</s> | ")	//TrackCount
		_T("%s:<s> %2.1f MB</s> | ")	//Size
		_T("%s:<s> %2.1f h</s>)")	//Duration
		,
		pAPI->GetString(IDS_COLLECTIONLIST),
		pAPI->GetString(IDS_COLLECTIONS), collections,
		pAPI->GetString(IDS_TRACKS), totalTrackCount,
		pAPI->GetString(IDS_SIZE), totalSize / 1024.0,
		pAPI->GetString(IDS_DURATION), totalDuration / 3600.0
		);

	m_info.SetWindowText(info);

}

BOOL CCollectionsDlg::OnStateChanged(UINT stateMessage)
{
	BOOL bHandled = TRUE;
	switch (stateMessage)
	{
	case SM_CollectionManagerEvent:
		m_List.UpdateControl();
		UpdateInfo();
		break;
	default:
		bHandled = FALSE;
	}
	return bHandled;
}

void CCollectionsDlg::OnBnClickedAdd()
{
	PRGAPI()->GetActionManager()->ShowAddNewCollectionDlg(this);
}

void CCollectionsDlg::OnBnClickedUpdate()
{
	PRGAPI()->GetCollectionManager()->RefreshLocalCollections(TRUE, FALSE, TRUE);
}

void CCollectionsDlg::OnBnClickedExit()
{
	OnCancel();
}

void CCollectionsDlg::OnSize(UINT nType, int cx, int cy)
{
	ResizeControls(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

void CCollectionsDlg::ResizeControls(int cx, int cy)
{
	const int marginX = 8;
	const int marginY = 8;
	const int infoHeight = 18;
	const int buttonHeight = 25;
	const int buttonWidth = 150;
	if (m_List.m_hWnd != 0)
	{
		m_header.SetWindowPos(NULL, 0, 0, cx, m_header.GetHeaderHeight(), SWP_NOZORDER);
		GetDlgItem(ID_ADD)->SetWindowPos(NULL, marginX, cy - marginY - buttonHeight, buttonWidth, buttonHeight, SWP_NOZORDER);
		GetDlgItem(ID_UPDATE)->SetWindowPos(NULL, marginX + buttonWidth + marginX, cy - marginY - buttonHeight, buttonWidth, buttonHeight, SWP_NOZORDER);
		GetDlgItem(ID_EXIT)->SetWindowPos(NULL, 2 * (marginX + buttonWidth) + marginX, cy - marginY - buttonHeight, buttonWidth, buttonHeight, SWP_NOZORDER);
		m_List.SetWindowPos(NULL, marginX, marginY + m_header.GetHeaderHeight(), cx - 2 * marginX, cy - 4 * marginY - infoHeight - buttonHeight - m_header.GetHeaderHeight(), SWP_NOZORDER);
		m_info.SetWindowPos(NULL, marginX, cy - 2 * marginY - buttonHeight - infoHeight, cx - 2 * marginX, infoHeight, SWP_NOZORDER);
	}
}



void CCollectionsDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize = CPoint(500, 250);
}
