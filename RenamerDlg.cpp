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
#include "RenamerDlg.h"
#include "PrgAPI.h"
#include "FileNameTagger.h"
#include "SQLManager.h"
#include "cStringUtils.h"


#define TIMER_UPDATE 100
// CRenamerDlg dialog

IMPLEMENT_DYNAMIC(CRenamerDlg, CDialog)

CRenamerDlg::CRenamerDlg(FullTrackRecordCollection& col, CWnd* pParent /*=NULL*/)
	: CDialog(CRenamerDlg::IDD, pParent), 
	m_col(col),
	m_bSomethingChanged(FALSE)
{
	ASSERT(col.size() != 0);
}

CRenamerDlg::~CRenamerDlg()
{
}

void CRenamerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_TAGS, m_lstTags);
	DDX_Control(pDX, IDC_CMB_CASING, m_cmbCasing);
	DDX_Control(pDX, IDC_CMB_PATTERN, m_cmbPattern);
}


BEGIN_MESSAGE_MAP(CRenamerDlg, CDialog)
	ON_BN_CLICKED(IDC_RENAME, &CRenamerDlg::OnBnClickedRename)
	ON_BN_CLICKED(IDC_SKIP, &CRenamerDlg::OnBnClickedSkip)
	ON_BN_CLICKED(IDC_ALL, &CRenamerDlg::OnBnClickedAll)
	ON_BN_CLICKED(IDC_CANCEL, &CRenamerDlg::OnBnClickedCancel)
	ON_CBN_EDITCHANGE(IDC_CMB_PATTERN, &CRenamerDlg::UpdatePreview)
	ON_CBN_SELCHANGE(IDC_CMB_CASING, &CRenamerDlg::UpdatePreviewDelayed)
	ON_CBN_SELCHANGE(IDC_CMB_PATTERN, &CRenamerDlg::UpdatePreviewDelayed)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CRenamerDlg message handlers

void CRenamerDlg::OnBnClickedRename()
{
	CString pat;
	GetDlgItemText(IDC_CMB_PATTERN, pat);
	if (pat.GetLength() == 0)
		return;
	if (m_col.size() > 0)
	{
		FullTrackRecordSP& rec = m_col[0];
		if (RenameTrack(rec, pat))
			OnBnClickedSkip();
		else
			PRGAPI()->MessageBox(m_hWnd, _T("Operation Failed"));
	}
}
BOOL CRenamerDlg::RenameTrack(FullTrackRecordSP& rec, LPCTSTR pat)
{
	TCHAR newPathName[MAX_PATH];
	TCHAR artist[100];
	TCHAR album[100];
	TCHAR title[100];
	_tcsncpy(artist, rec->artist.name.c_str(), 100);
	artist[99] = 0;
	_tcsncpy(album, rec->album.name.c_str(), 100);
	album[99] = 0;
	_tcsncpy(title, rec->track.name.c_str(), 100);
	title[99] = 0;
	INT curSel = m_cmbCasing.GetCurSel();
	switch (curSel)
	{
	case 1:
		_tcsproper(artist);
		_tcsproper(album);
		_tcsproper(title);
		break;
	case 2:
		_tcsupr(artist);
		_tcsupr(album);
		_tcsupr(title);
		break;
	case 3:
		_tcslwr(artist);
		_tcslwr(album);
		_tcslwr(title);
		break;
	}
	FileNameTagger fnTagger;
	if (fnTagger.CustomWrite(newPathName, 
		rec->track.location.c_str(),  
		pat,
		artist, 
		album, 
		rec->track.trackNo, 
		title, 
		rec->track.year))
	{
		m_bSomethingChanged = TRUE;
		rec->track.location = newPathName;
		return PRGAPI()->GetSQLManager()->UpdateTrackRecord(rec->track);
	}
	return FALSE;
}

void CRenamerDlg::OnBnClickedSkip()
{
	ASSERT(m_col.size() != 0);
	m_col.erase(m_col.begin());
	if (m_col.size() == 0)
		OnOK();
	else
		PrepareNextTrack();
}

void CRenamerDlg::OnBnClickedAll()
{
	CString pat;
	GetDlgItemText(IDC_CMB_PATTERN, pat);
	if (pat.GetLength() == 0)
		return;
	PrgAPI* pAPI = PRGAPI();
	for (size_t i = 0; i < m_col.size(); i++)
	{
		FullTrackRecordSP& rec = m_col[i];
		if (!RenameTrack(rec, pat))
		{
			TCHAR bf[1000];
			_sntprintf(bf, 1000, _T("%s: %s: '%s'"),
				pAPI->GetString(IDS_OPERATIONFAILED),
				pAPI->GetString(IDS_RENAME),
				rec->track.location.c_str());
			pAPI->NotifyUser(bf, NULL, SEV_Warning);
		}
	}
	OnOK();
}

void CRenamerDlg::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CRenamerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	PrgAPI* pAPI = PRGAPI();

	INT i = 0;
	while (TRUE)//FileNameTagger::GetWritePredifinedFormat(i))
	{
		LPCTSTR curPat = FileNameTagger::GetWritePredifinedFormat(i);
		if (curPat == NULL)
			break;
		i++;
		m_cmbPattern.AddString(curPat);
	}
	m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	m_header.SetTitleText(pAPI->GetString(IDS_RENAMER));
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(48, 48);
	m_header.Init(this);
	m_header.MoveCtrls(this);

	m_cmbCasing.AddString(pAPI->GetString(IDS_AUTOMATIC));
	m_cmbCasing.AddString(pAPI->GetString(IDS_PROPERCASE));
	m_cmbCasing.AddString(pAPI->GetString(IDS_ALLCAPS));
	m_cmbCasing.AddString(pAPI->GetString(IDS_NOCAPS));

	m_cmbCasing.SetCurSel(0);
	m_cmbPattern.SetCurSel(0);
	m_lstTags.InsertColumn(0, _T(""), 0, 20);
	m_lstTags.InsertColumn(1, _T(""), 0, 80);
	m_lstTags.InsertColumn(2, _T(""), 0, 200);
	PrepareNextTrack();

	SetDlgItemText(IDC_ST_SYMBOLS, pAPI->GetString(IDS_SYMBOLS));
	SetDlgItemText(IDC_ST_FILENAME, pAPI->GetString(IDS_FILE));
	SetDlgItemText(IDC_ST_PATTERN, pAPI->GetString(IDS_PATTERN));
	SetDlgItemText(IDC_ST_CASING, pAPI->GetString(IDS_CHANGECASE));
	SetDlgItemText(IDC_ST_PREVIEW, pAPI->GetString(IDS_PREVIEW));
	SetDlgItemText(IDC_RENAME, pAPI->GetString(IDS_RENAME));
	SetDlgItemText(IDC_SKIP, pAPI->GetString(IDS_SKIP));
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("%s (%s)"), pAPI->GetString(IDS_RENAME), pAPI->GetString(IDS_ALL));
	SetDlgItemText(IDC_ALL, bf);
	SetDlgItemText(IDC_CANCEL, pAPI->GetString(IDS_EXIT));
	SetWindowText(pAPI->GetString(IDS_RENAMER));


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRenamerDlg::InsertItem(INT rowID, LPCTSTR id, INT descID, LPCTSTR curVal)
{
	INT item = m_lstTags.InsertItem(rowID, id);
	INT ret = m_lstTags.SetItem(item, 1, LVIF_TEXT, PRGAPI()->GetString(descID), 0,0,0,0);
	ret = m_lstTags.SetItem(item, 2, LVIF_TEXT, curVal, 0,0,0,0);
}

BOOL CRenamerDlg::PrepareNextTrack()
{
	if (m_col.size() == 0)
		return FALSE;
	FullTrackRecordSP& rec = m_col[0];
	SetDlgItemText(IDC_EDT_FILENAME, rec->track.location.c_str());
	m_lstTags.DeleteAllItems();

	TCHAR bf[300];
	InsertItem(0, _T("$a"), IDS_ARTIST, rec->artist.name.c_str());
	InsertItem(1, _T("$l"), IDS_ALBUM, rec->album.name.c_str());
	_sntprintf(bf, 300, _T("%02d"), rec->track.trackNo);
	InsertItem(2, _T("$n"), IDS_TRACKNO, bf);
	InsertItem(3, _T("$t"), IDS_TITLE, rec->track.name.c_str());
	_sntprintf(bf, 300, _T("%d"), rec->track.year);
	InsertItem(4, _T("$y"), IDS_YEAR, bf);

	UpdatePreview();
	if (m_col.size() > 1)
		_sntprintf(bf, 300, _T("%d %s"), m_col.size(), PRGAPI()->GetString(IDS_TRACKS));
	else
		bf[0] = 0;
	SetDlgItemText(IDC_ST_TRACKS, bf);
	GetDlgItem(IDC_SKIP)->EnableWindow(m_col.size() > 1);
	GetDlgItem(IDC_ALL)->EnableWindow(m_col.size() > 1);
	//if (m_col)

	return TRUE;

}

void CRenamerDlg::UpdatePreviewDelayed()
{
	SetTimer(TIMER_UPDATE, 50, NULL);
}

void CRenamerDlg::UpdatePreview()
{
	TCHAR bf[MAX_PATH];
	BOOL ret = GetNewFileName(bf, MAX_PATH);
	if (ret)
		SetDlgItemText(IDC_EDT_PREVIEW, ret ? bf : PRGAPI()->GetString(IDS_FAILURE));
	GetDlgItem(IDC_RENAME)->EnableWindow(ret);
}

BOOL CRenamerDlg::GetNewFileName(LPTSTR bf, UINT bfLen)
{
	if (m_col.size() == 0)
		return FALSE;
	FullTrackRecordSP& rec = m_col[0];
	CString pat;
	GetDlgItemText(IDC_CMB_PATTERN, pat);
	TCHAR artist[100];
	TCHAR album[100];
	TCHAR title[100];
	_tcsncpy(artist, rec->artist.name.c_str(), 100);
	artist[99] = 0;
	_tcsncpy(album, rec->album.name.c_str(), 100);
	album[99] = 0;
	_tcsncpy(title, rec->track.name.c_str(), 100);
	title[99] = 0;
	INT curSel = m_cmbCasing.GetCurSel();
	switch (curSel)
	{
	case 1:
		_tcsproper(artist);
		_tcsproper(album);
		_tcsproper(title);
		break;
	case 2:
		_tcsupr(artist);
		_tcsupr(album);
		_tcsupr(title);
		break;
	case 3:
		_tcslwr(artist);
		_tcslwr(album);
		_tcslwr(title);
		break;
	}


	return FileNameTagger::TranslatePattern(bf, bfLen, pat, 
		artist,
		album,
		rec->track.trackNo,
		title,
		rec->track.year,
		NULL
		);

}

void CRenamerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_UPDATE)
	{
		KillTimer(nIDEvent);
		UpdatePreview();
	}

	CDialog::OnTimer(nIDEvent);
}
