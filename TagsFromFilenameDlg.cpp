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
#include "PrgAPI.h"
#include "TagsFromFilenameDlg.h"
#include "SQLManager.h"
#include "cStringUtils.h"
#include "FileNameTagger.h"
#include "ActionManager.h"

#define TIMER_UPDATE 100
// CTagsFromFilenameDlg dialog

IMPLEMENT_DYNAMIC(CTagsFromFilenameDlg, CDialog)

CTagsFromFilenameDlg::CTagsFromFilenameDlg(FullTrackRecordCollection& col, CWnd* pParent /*=NULL*/)
	: CDialog(CTagsFromFilenameDlg::IDD, pParent), 
	m_col(col),
	m_bSomethingChanged(FALSE)
{
	ASSERT(col.size() != 0);
}

CTagsFromFilenameDlg::~CTagsFromFilenameDlg()
{
}

void CTagsFromFilenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_TAGS, m_lstPreview);
	DDX_Control(pDX, IDC_CMB_CASING, m_cmbCasing);
	DDX_Control(pDX, IDC_CMB_PATTERN, m_cmbPattern);
}


BEGIN_MESSAGE_MAP(CTagsFromFilenameDlg, CDialog)
	ON_BN_CLICKED(IDC_GETTAGS, &CTagsFromFilenameDlg::OnBnClickedGetTags)
	ON_BN_CLICKED(IDC_SKIP, &CTagsFromFilenameDlg::OnBnClickedSkip)
	ON_BN_CLICKED(IDC_ALL, &CTagsFromFilenameDlg::OnBnClickedAll)
	ON_BN_CLICKED(IDC_CANCEL, &CTagsFromFilenameDlg::OnBnClickedCancel)
	ON_CBN_EDITCHANGE(IDC_CMB_PATTERN, &CTagsFromFilenameDlg::UpdatePreview)
	ON_CBN_SELCHANGE(IDC_CMB_CASING, &CTagsFromFilenameDlg::UpdatePreviewDelayed)
	ON_CBN_SELCHANGE(IDC_CMB_PATTERN, &CTagsFromFilenameDlg::UpdatePreviewDelayed)
	ON_BN_CLICKED(IDC_CHK_REMOVEUNDERSCORES, &CTagsFromFilenameDlg::UpdatePreview)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_AUTO, &CTagsFromFilenameDlg::OnBnClickedBtnAuto)
END_MESSAGE_MAP()


// CTagsFromFilenameDlg message handlers
void CTagsFromFilenameDlg::OnBnClickedGetTags()
{
	ASSERT(m_col.size() > 0);
	if (m_col.size() == 0)
	{
		TRACE(_T("@1 CTagsFromFilenameDlg::OnBnClickedGetTags. col is empty\r\n"));
		return;
	}
	PrgAPI* pAPI = PRGAPI();
	FullTrackRecordSP& rec = m_col[0];
	CString pat;
	GetDlgItemText(IDC_CMB_PATTERN, pat);
	if (UpdateTrack(rec, pat, IsDlgButtonChecked(IDC_CHK_WRITETAGS)))
	{
		m_bSomethingChanged = TRUE;
		OnBnClickedSkip();
	}
	else
		pAPI->MessageBox(m_hWnd, IDS_FAILURE);

}

BOOL CTagsFromFilenameDlg::UpdateTrack(FullTrackRecordSP& rec, LPCTSTR pat, BOOL bWriteTags)
{
	TagInfo ti;
	if (GetTagInfo(ti, rec->track.location.c_str(), pat))
	{
		PrgAPI* pAPI = PRGAPI();
		if (pAPI->GetSQLManager()->UpdateTrackFromTagInfo(rec->track.ID, 
			rec->collection.ID, rec->track.location.c_str(), ti))
		{
			if (bWriteTags)
				pAPI->GetActionManager()->TagTrack(rec->track.location.c_str(), ti);
			return TRUE;
		}
	}
	return FALSE;
}

void CTagsFromFilenameDlg::OnBnClickedSkip()
{
	ASSERT(m_col.size() != 0);
	m_col.erase(m_col.begin());
	if (m_col.size() == 0)
		OnOK();
	else
		PrepareNextTrack();
}

void CTagsFromFilenameDlg::OnBnClickedAll()
{
	CString pat;
	GetDlgItemText(IDC_CMB_PATTERN, pat);
	BOOL bWriteTags = IsDlgButtonChecked(IDC_CHK_WRITETAGS);
	for (size_t i = 0; i < m_col.size(); i++)
	{
		FullTrackRecordSP& rec = m_col[i];
		UpdateTrack(rec, pat, bWriteTags);
		m_bSomethingChanged = TRUE;
	}
	OnOK();
}

void CTagsFromFilenameDlg::OnBnClickedCancel()
{
	OnCancel();
}

BOOL CTagsFromFilenameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	PrgAPI* pAPI = PRGAPI();

	INT i = 0;
	while (TRUE)//FileNameTagger::GetWritePredifinedFormat(i))
	{
		LPCTSTR curPat = FileNameTagger::GetReadPredifinedFormat(i);
		if (curPat == NULL)
			break;
		i++;
		m_cmbPattern.AddString(curPat);
	}
	m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	m_header.SetTitleText(pAPI->GetString(IDS_GETTAGSFROMFILENAME));
	//m_header.SetDescText(pAPI->GetString(IDS_EMPTYCOLLECTION_DETAIL));
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(48, 48);
	m_header.Init(this);
	m_header.MoveCtrls(this);

	m_cmbCasing.AddString(pAPI->GetString(IDS_AUTOMATIC));
	m_cmbCasing.AddString(pAPI->GetString(IDS_PROPERCASE));
	m_cmbCasing.AddString(pAPI->GetString(IDS_ALLCAPS));
	m_cmbCasing.AddString(pAPI->GetString(IDS_NOCAPS));

	m_lstPreview.InsertColumn(0, _T(""), 0, 20);
	m_lstPreview.InsertColumn(1, _T(""), 0, 80);
	m_lstPreview.InsertColumn(2, _T(""), 0, 200);
	m_lstPreview.InsertItem(0, _T("$a"));
	m_lstPreview.SetItem(0, 1, LVIF_TEXT, pAPI->GetString(IDS_ARTIST), 0,0,0,0);
	m_lstPreview.InsertItem(1, _T("$l"));
	m_lstPreview.SetItem(1, 1, LVIF_TEXT, pAPI->GetString(IDS_ALBUM), 0,0,0,0);
	m_lstPreview.InsertItem(2, _T("$n"));
	m_lstPreview.SetItem(2, 1, LVIF_TEXT, pAPI->GetString(IDS_TRACKNO), 0,0,0,0);
	m_lstPreview.InsertItem(3, _T("$t"));
	m_lstPreview.SetItem(3, 1, LVIF_TEXT, pAPI->GetString(IDS_TITLE), 0,0,0,0);
	m_lstPreview.InsertItem(4, _T("$y"));
	m_lstPreview.SetItem(4, 1, LVIF_TEXT, pAPI->GetString(IDS_YEAR), 0,0,0,0);
	m_lstPreview.InsertItem(5, _T("$d"));
	m_lstPreview.SetItem(5, 1, LVIF_TEXT, pAPI->GetString(IDS_DISABLED), 0,0,0,0);



	m_cmbCasing.SetCurSel(0);
	m_cmbPattern.SetCurSel(0);
	PrepareNextTrack();

	SetDlgItemText(IDC_ST_FILENAME, pAPI->GetString(IDS_FILE));
	SetDlgItemText(IDC_ST_PATTERN, pAPI->GetString(IDS_PATTERN));
	SetDlgItemText(IDC_ST_CASING, pAPI->GetString(IDS_CHANGECASE));
	SetDlgItemText(IDC_ST_PREVIEW, pAPI->GetString(IDS_PREVIEW));
	SetDlgItemText(IDC_CHK_WRITETAGS, pAPI->GetString(IDS_WRITETAGS));
	SetDlgItemText(IDC_CHK_REMOVEUNDERSCORES, pAPI->GetString(IDS_REMOVEUNDERSCORES));

	

	SetDlgItemText(IDC_ALL, pAPI->GetString(IDS_ACCEPTALL));
	SetDlgItemText(IDC_ACCEPT, pAPI->GetString(IDS_ACCEPT));
	SetDlgItemText(IDC_SKIP, pAPI->GetString(IDS_SKIP));
	SetDlgItemText(IDC_CANCEL, pAPI->GetString(IDS_EXIT));
	SetWindowText(pAPI->GetString(IDS_GETTAGSFROMFILENAME));


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTagsFromFilenameDlg::PrepareNextTrack()
{
	if (m_col.size() == 0)
		return FALSE;
	SetDlgItemText(IDC_EDT_FILENAME, m_col[0]->track.location.c_str());
	UpdatePreview();
	GetDlgItem(IDC_SKIP)->EnableWindow(m_col.size() > 1);
	GetDlgItem(IDC_ALL)->EnableWindow(m_col.size() > 1);
	TCHAR bf[100];
	_sntprintf(bf, 100, _T("%d (%s)"), m_col.size(), PRGAPI()->GetString(IDS_TRACKS));
	SetDlgItemText(IDC_ST_TRACKS, bf);
	GetDlgItem(IDC_BTN_AUTO)->EnableWindow(TRUE);

	return TRUE;

}

void CTagsFromFilenameDlg::UpdatePreviewDelayed()
{
	SetTimer(TIMER_UPDATE, 50, NULL);
}

BOOL CTagsFromFilenameDlg::GetTagInfo(TagInfo& ti, LPCTSTR location, LPCTSTR pattern)
{
	FileNameTagger fnTagger;
	ti.validFields = fnTagger.CustomRead(location, ti, pattern);
	if (ti.validFields != 0)
	{
		const INT cMaxLen = 100;
		TCHAR artist[cMaxLen];
		TCHAR album[cMaxLen];
		TCHAR title[cMaxLen];
		_tcsncpy(artist, ti.Artist.c_str(), cMaxLen);
		artist[cMaxLen - 1] = 0;
		_tcsncpy(album, ti.Album.c_str(), cMaxLen);
		album[cMaxLen - 1] = 0;
		_tcsncpy(title, ti.Title.c_str(), cMaxLen);
		title[cMaxLen - 1] = 0;

		if (IsDlgButtonChecked(IDC_CHK_REMOVEUNDERSCORES))
		{
			_tcsReplaceInl(artist, NULL, _T("_"), _T(" "));
			_tcstrim(artist);
			_tcsReplaceInl(album, NULL, _T("_"), _T(" "));
			_tcstrim(album);
			_tcsReplaceInl(title, NULL, _T("_"), _T(" "));
			_tcstrim(title);
		}


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
		ti.Artist = artist;
		ti.Album = album;
		ti.Title = title;
		return TRUE;
	}
	return FALSE;
}

void CTagsFromFilenameDlg::UpdatePreview()
{
	if (m_col.size() == 0)
	{
		TRACE(_T("@1 CTagsFromFilenameDlg::UpdatePreview. col is empty\r\n"));
		return;
	}

	PrgAPI* pAPI = PRGAPI();
	CString pat;
	GetDlgItemText(IDC_CMB_PATTERN, pat);
	TagInfo ti;
	if (GetTagInfo(ti, m_col[0]->track.location.c_str(), pat))
	{
		TCHAR bf[300];
		INT nItem = 0;
		m_lstPreview.SetItem(0, 2, LVIF_TEXT, 
			(ti.validFields & TagInfo_Artist) ? ti.Artist.c_str() : _T(""), 0,0,0,0);
		m_lstPreview.SetItem(1, 2, LVIF_TEXT, 
			(ti.validFields & TagInfo_Album) ? ti.Album.c_str() : _T(""), 0,0,0,0);
		_sntprintf(bf, 300, _T("%02d"), ti.TrackNo);
		m_lstPreview.SetItem(2, 2, LVIF_TEXT, 
			(ti.validFields & TagInfo_TrackNo) ? bf : _T(""), 0,0,0,0);
		m_lstPreview.SetItem(3, 2, LVIF_TEXT, 
			(ti.validFields & TagInfo_Title) ? ti.Title.c_str() : _T(""), 0,0,0,0);
		_sntprintf(bf, 300, _T("%d"), ti.Year);
		m_lstPreview.SetItem(4, 2, LVIF_TEXT, 
			(ti.validFields & TagInfo_Year) ? bf : _T(""), 0,0,0,0);
	}
	else
	{
		for (INT i = 0; i < m_lstPreview.GetItemCount(); i++)
			m_lstPreview.SetItem(i, 2, LVIF_TEXT, _T(""), 0,0,0,0);
	}
}

void CTagsFromFilenameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_UPDATE)
	{
		KillTimer(nIDEvent);
		UpdatePreview();
	}

	CDialog::OnTimer(nIDEvent);
}

void CTagsFromFilenameDlg::OnBnClickedBtnAuto()
{
	if (m_col.size() == 0)
	{
		TRACE(_T("@1 CTagsFromFilenameDlg::OnBnClickedBtnAuto. col is empty\r\n"));
		return;
	}
	INT startSel = m_cmbPattern.GetCurSel();
	INT count = m_cmbPattern.GetCount();
	INT curSel = startSel + 1;
	while (TRUE)
	{
		if (curSel >= count)
			curSel = 0;
		if (curSel == startSel)
		{
			GetDlgItem(IDC_BTN_AUTO)->EnableWindow(FALSE);
			break;
		}
		CString pat;
		m_cmbPattern.GetLBText(curSel, pat);
		TagInfo ti;
		if (GetTagInfo(ti, m_col[0]->track.location.c_str(), pat))
		{
			m_cmbPattern.SetCurSel(curSel);
			UpdatePreview();
			break;
		}
		curSel++;
	}
}
