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
#include "TracksSerializerDlg.h"
#include "PrgAPI.h"


// CTracksSerializerDlg dialog

IMPLEMENT_DYNAMIC(CTracksSerializerDlg, CDialog)

CTracksSerializerDlg::CTracksSerializerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTracksSerializerDlg::IDD, pParent)
{
	m_hIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_LOADTRANSPARENT);//LR_LOADTRANSPARENT LR_VGACOLOR

}

CTracksSerializerDlg::~CTracksSerializerDlg()
{
	DeleteObject(m_hIcon);
}

void CTracksSerializerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACKLIST, m_TrackList);
	DDX_Control(pDX, IDC_HEADER, m_description);
	DDX_Control(pDX, IDOK, m_btnOK);	
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CTracksSerializerDlg, CDialog)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_TRACKLIST, &CTracksSerializerDlg::OnLvnGetdispinfoTracklist)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TRACKLIST, &CTracksSerializerDlg::OnNMCustomdrawTracklist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRACKLIST, &CTracksSerializerDlg::OnLvnItemchangedTracklist)
END_MESSAGE_MAP()


// CTracksSerializerDlg message handlers

BOOL CTracksSerializerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PrgAPI* pAPI = PRGAPI();
	int titleResID = m_bImport ? IDS_IMPORTVIRTUALTRACKS : IDS_EXPORTVIRTUALTRACKS;
	int descResID = m_bImport ? IDS_IMPORTVIRTUALTRACKSINFO : IDS_EXPORTVIRTUALTRACKSINFO;
	int btnResID = m_bImport ? IDS_IMPORT : IDS_EXPORT;
	SetWindowText(pAPI->GetString(titleResID));
	m_description.SetWindowText(pAPI->GetString(descResID));
	GetDlgItem(IDOK)->SetWindowText(pAPI->GetString(btnResID));

	pAPI->SetGlobalButtonStyle(m_btnOK);
	pAPI->SetGlobalButtonStyle(m_btnCancel);

	m_TrackList.SetExtendedStyle(LVS_EX_BORDERSELECT | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);
	m_TrackList.SetItemCount(m_ItemsChecked.size());
	m_TrackList.InsertColumn(0, pAPI->GetString(IDS_LOCATION), LVCFMT_LEFT, 200);
	m_TrackList.InsertColumn(1, pAPI->GetString(IDS_TITLE), LVCFMT_LEFT, 100);
	m_TrackList.InsertColumn(2, pAPI->GetString(IDS_ARTIST), LVCFMT_LEFT, 100);
	m_TrackList.InsertColumn(3, pAPI->GetString(IDS_ALBUM), LVCFMT_LEFT, 100);
	m_TrackList.InsertColumn(4, pAPI->GetString(IDS_DURATION), LVCFMT_LEFT, 50);
	m_TrackList.InsertColumn(5, pAPI->GetString(IDS_GENRE), LVCFMT_LEFT, 100);


	SetIcon(m_hIcon, FALSE);
	m_description.SetColor(CLabelEx::COL_Bk, RGB(200,210,210));
	m_description.SetColor(CLabelEx::COL_Text, RGB(0,0,150));
	m_description.SetBorderType(CLabelEx::B_BottomLine);
	//m_description.SetWindowText(PRGAPI()->GetString(IDS_IMPORTLINKS_INFO1));
	//m_description.SetTextDrawOptions(CLabelEx::DO_Multiline);
	m_description.SetMargins(15,4);


	CRect rc;
	GetClientRect(&rc);
	PositionControls(rc.Width(), rc.Height());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTracksSerializerDlg::AddTrack(TagInfo& ti, LPCTSTR URL)
{
	m_ItemsChecked.push_back(TRUE);
	m_ItemsTagInfo.push_back(ti);
	m_ItemsURL.push_back(std::tstring(URL));
}


void CTracksSerializerDlg::OnLvnGetdispinfoTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	//Create a pointer to the item
	LV_ITEM* pItem= &(pDispInfo->item);

	//Which item number?
	int itemid = pItem->iItem;

	TCHAR bf[20];
	//Do the list need text information?
	if (pItem->mask & LVIF_TEXT)
	{
		LPCTSTR str = NULL;
		switch (pItem->iSubItem)
		{
		case 0:		str = m_ItemsURL[itemid].c_str();				break;
		case 1:		str = m_ItemsTagInfo[itemid].Title.c_str();		break;
		case 2:		str = m_ItemsTagInfo[itemid].Artist.c_str();	break;
		case 3:		str = m_ItemsTagInfo[itemid].Album.c_str();		break;
		case 4:		
			PrgAPI::FormatDuration(bf, 20, m_ItemsTagInfo[itemid].Length);
			str = bf;
			break;
		case 5:		str = m_ItemsTagInfo[itemid].Genre.c_str();		break;
		}


		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, str, pItem->cchTextMax);
	}

	//Do the list need image information?
	if( pItem->mask & LVIF_IMAGE) 
	{
		pItem->mask |= LVIF_STATE;
		pItem->stateMask = LVIS_STATEIMAGEMASK;
		if(m_ItemsChecked[itemid])
		{
			//Turn check box on
			pItem->state = INDEXTOSTATEIMAGEMASK(2);
		}
		else
		{
			//Turn check box off
			pItem->state = INDEXTOSTATEIMAGEMASK(1);
		}
	}

	*pResult = 0;

}

void CTracksSerializerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_TrackList.m_hWnd)
		PositionControls(cx, cy);

	
}

void CTracksSerializerDlg::PositionControls(int cx, int cy)
{
	if (m_description.m_hWnd != NULL)
	{
		const int headerHeight = 40;
		const int xMargin = 10;
		const int buttonHeight = 30;
		const int buttonWidth = 90;
		m_description.MoveWindow(0,0,cx,headerHeight);
		m_TrackList.MoveWindow(xMargin, headerHeight + xMargin, cx - 2 * xMargin, cy - headerHeight - buttonHeight - 3 * xMargin);
		GetDlgItem(IDOK)->MoveWindow(cx - 2 * (buttonWidth + xMargin), cy - xMargin - buttonHeight, buttonWidth, buttonHeight);
		GetDlgItem(IDCANCEL)->MoveWindow(cx - (buttonWidth + xMargin), cy - xMargin - buttonHeight, buttonWidth, buttonHeight);
	}
	CRect rc;
	m_TrackList.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.right = cx - rc.left;
	rc.bottom = cy - rc.left;
}

void CTracksSerializerDlg::GetTrack(IN UINT itemID, OUT TagInfo*& ti, OUT LPCTSTR& URL, OUT BOOL& bActive)
{
	ASSERT(itemID >= 0 && itemID < GetCount());
	if (itemID >= 0 && itemID < GetCount())
	{
		ti = &m_ItemsTagInfo[itemID];
		URL = m_ItemsURL[itemID].c_str();
		bActive = m_ItemsChecked[itemID];
	}	
}

UINT CTracksSerializerDlg::GetCount()
{
	return m_ItemsTagInfo.size();
}

void CTracksSerializerDlg::OnNMCustomdrawTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = CDRF_DODEFAULT;
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		pLVCD->clrTextBk = (pLVCD->nmcd.dwItemSpec & 1) ? RGB(230, 230, 255) : RGB(220, 220, 255);
		break;
	}
}

void CTracksSerializerDlg::OnLvnItemchangedTracklist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	*pResult = 0;

	if (pNMLV->iItem < 0)
		return;
	if (pNMLV->uOldState == pNMLV->uNewState)
		return;    // No change
	if (pNMLV->uNewState == 3)
		m_ItemsChecked[pNMLV->iItem ] = !m_ItemsChecked[pNMLV->iItem ];
}
