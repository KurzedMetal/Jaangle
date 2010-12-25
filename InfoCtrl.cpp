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
#include "InfoCtrl.h"

#include "InfoDownloadManager.h"
#include "AppSettings/AppSettings.h"
#include "ActionManager.h"
#include "PrgAPI.h"
#include "MediaPlayer.h"
#include "GuiControls/ColorEdit.h"
#include "LocalPictureManager.h"
#include "ShortcutManager.h"

#include "SQLManager.h"

#define TIMER_DELAY 100
#define TIMER_DELAY_DELAY 100
#define TIMER_FADE_REFRESH 120
#define TIMER_FADE_DELAY 40

#define TIMER_AUTODOWNLOAD 140
//#define TIMER_AUTODOWNLOAD_DELAY 3000


#define ID_SAVEPIC 100
#define ID_SAVETEXT 101
#define ID_OPENPIC 102
#define ID_EDITPIC 103
#define ID_COPYTEXT 104
#define ID_PROPERTIES 200
#define ID_TOGGLEPLAYER 300

IMPLEMENT_DYNAMIC(CInfoCtrl, CWnd)

CInfoCtrl::CInfoCtrl(CWnd* pParent /*=NULL*/):
m_ExtraTextStyle(0),
m_bMediaPlayerMode(FALSE),
m_pEdit(NULL),
m_bRequestArtistImage(NULL),
m_bRequestAlbumImage(NULL),
m_bRequestArtistInfo(NULL),
m_bRequestAlbumInfo(NULL),
m_bNeedsRedraw(TRUE),
m_desiredItemType(IT_Unknown)
{

	//Fade code
//	m_totalFadeSteps = 5;
//	m_curFadeStep = -1;//Initial Do NOT DO FADE AT THE BEGINNING

	m_bmpSize.cx = 0;
	m_bmpSize.cy = 0;
	m_oldBitmap = NULL;

	m_colors[COL_Bk] = RGB(205, 205, 220);
	m_colors[COL_BkText] = RGB(180,180,205);
	m_colors[COL_Text] = RGB(0, 0, 0);
	m_colors[COL_TextTitle] = RGB(150,150,220);
	m_VariousItemsBrush.CreateSolidBrush(m_colors[COL_BkText]);	//Version 2

	m_InfoTypeEnabled[IIT_ArtistBio] = TRUE;
	m_InfoTypeEnabled[IIT_AlbumReview] = TRUE;
	m_InfoTypeEnabled[IIT_TrackComment] = FALSE;
	m_InfoTypeEnabled[IIT_TrackLyrics] = TRUE;
	m_InfoTypeEnabled[IIT_TrackPersonal] = FALSE;
	m_InfoTypeEnabled[IIT_TrackTablatures] = FALSE;

	m_InfoTypeEnabled[IIT_AlbumPicture] = TRUE;
	m_InfoTypeEnabled[IIT_ArtistPicture] = TRUE;


}

CInfoCtrl::~CInfoCtrl()
{
	delete m_pEdit;
	m_pEdit = NULL;
	m_VariousItemsBrush.DeleteObject();	//Version 2.0
	if (m_Menu.m_hMenu != 0)
		m_Menu.DestroyMenu();

	if (m_memDC.m_hDC)
	{
		m_memDC.SelectObject(m_oldBitmap);
		m_memBitmap.DeleteObject();
		m_memDC.DeleteDC();

	}

}

HWND CInfoCtrl::GetWindowForFocus()				
{
	return m_pEdit == NULL ? NULL : m_pEdit->GetSafeHwnd();
}


BOOL CInfoCtrl::Create(CWnd* pParentWnd, UINT nID)
{
	if (CWnd::Create(NULL, NULL, WS_CHILD | WS_CLIPCHILDREN, CRect(0,0,0,0), pParentWnd, nID, NULL))
	{
		PrgAPI* pAPI = PRGAPI();

		CRect defRc(0, 0, 0, 0);
		m_pEdit = new CColorEdit;
		m_pEdit->Create(WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL | ES_READONLY, 
			defRc, this, 100);


		//ShortcutManager* pSCM = pAPI->GetShortcutManager();
		//pSCM->RegisterShortcutForFocus(VK_F5, m_pEdit->m_hWnd);

		m_picture.Create(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD, defRc, this, 200);
		GdiPlusPicDrawer::DrawParams& params = m_picture.GetDrawParams();
		params.zoomLockMode = GdiPlusPicDrawer::ZLM_FullImage;
		m_picture.SetTransitionTime(300);
		ApplySkin(pAPI->GetSkinManager()->GetSkin());
		ApplyTranslation(*pAPI->GetTranslationManager());
		ActualRefresh();
		//UpdateConfiguration();
		return TRUE;

	}
	return FALSE;
}




void CInfoCtrl::EnableInfoType(InfoItemTypeEnum iit, BOOL bEnable)
{
	ASSERT(iit>= IIT_Unknown && iit < IIT_Last);
	if (iit>= IIT_Unknown && iit < IIT_Last)
	{
		m_InfoTypeEnabled[iit] = bEnable;
	}
}

BOOL CInfoCtrl::IsInfoTypeEnabled(InfoItemTypeEnum iit)	
{
	return m_InfoTypeEnabled[iit];
}

BEGIN_MESSAGE_MAP(CInfoCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CInfoCtrl::ApplyTranslation(ITranslation& translation)
{
	//ActualRefresh();
}

void CInfoCtrl::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	PrgAPI* pAPI = PRGAPI();
	m_pSmallFont = skin.GetFont(CSEC_InfoControl, FSTY_Normal);
	m_pBigFont = skin.GetFont(CSEC_InfoControl, FSTY_Edit);

	SetColor(COL_Bk, skin.GetColor(CSEC_InfoControl, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_InfoControl, CVAL_Text));
	SetColor(COL_BkText, skin.GetColor(CSEC_InfoControl, CVAL_AltBk));
	SetColor(COL_TextTitle, skin.GetColor(CSEC_InfoControl, CVAL_AltText));
	SetColor(COL_Ouline, skin.GetColor(CSEC_InfoControl, CVAL_Outline));

	if (m_pEdit != NULL)
	{
		m_pEdit->SetFont(m_pBigFont);
		m_pEdit->SetBkColor(m_colors[COL_BkText]);
		m_pEdit->SetTextColor(m_colors[COL_Text]);
	}
	if (m_picture.m_hWnd != NULL)
	{
		m_picture.SetTitleFont(m_pSmallFont);
		m_picture.SetBkColor(m_colors[COL_Bk]);
		m_picture.SetTitleFont(m_pSmallFont);
	}
	m_bNeedsRedraw = TRUE;
	Invalidate();
}

//void CInfoCtrl::UpdateConfiguration()
//{
//	PrgAPI* pAPI = PRGAPI();
//	EnableInfoType(IIT_ArtistBio, pAPI->GetOption(OPT_IC_EnableArtistBio) != 0);
//	EnableInfoType(IIT_AlbumReview, pAPI->GetOption(OPT_IC_EnableAlbumReview) != 0);
//
//	EnableInfoType(IIT_TrackComment, pAPI->GetOption(OPT_IC_EnableTrackComment) != 0);
//	EnableInfoType(IIT_TrackLyrics, pAPI->GetOption(OPT_IC_EnableTrackLyrics) != 0);
//	EnableInfoType(IIT_TrackPersonal, pAPI->GetOption(OPT_IC_EnableTrackPersonal) != 0);
//	EnableInfoType(IIT_TrackTablatures, pAPI->GetOption(OPT_IC_EnableTrackPersonal) != 0);
//
//	EnableInfoType(IIT_ArtistPicture, pAPI->GetOption(OPT_IC_EnableArtistPicture) != 0);
//	EnableInfoType(IIT_AlbumPicture, pAPI->GetOption(OPT_IC_EnableAlbumPicture) != 0);
//	ActualRefresh();
//}

//void CInfoCtrl::OnStateChanged(UINT stateMessage)
//{
//	ASSERT(0);
//
//
//	if (stateMessage & SM_SectionChanged)
//	{
//
//	}
//	if (stateMessage & SM_TrackChanged)
//	{
//
//	}
//	if (stateMessage & SM_PictureChanged)
//	{
//
//	}
//	if (stateMessage & SM_InfoChanged)
//	{
//
//	}
//
//
//}



BOOL CInfoCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void CInfoCtrl::OnPaint()
{
	{
		CPaintDC _dc(this); // device context for painting
	}
	if (m_bNeedsRedraw)
	{
		m_bNeedsRedraw = FALSE;
		Redraw();
	}
	
	CClientDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &m_memDC, 0, 0, SRCCOPY);
}

//This is version 2--------------------------------------------------------
UINT CInfoCtrl::DrawStuff(CDC& dc, CRect& rc, BOOL bCalcHeightOnly)
{
	//ASSERT(m_bPlayerVisible == FALSE);
	//Consts
	const int cXMargin = 3;
	const int cYMargin = 3;
	const int cTitleHeight = 20;
	//return 0;

	dc.FillSolidRect(&rc, GetColor(COL_Bk));

	BOOL bPicAvailable = m_picture.GetCount() > 0;
	BOOL bTextAvailable = m_pEdit->GetWindowTextLength() > 0;

	//CRect drawingRC(rc);
	//drawingRC.DeflateRect(cXMargin, cYMargin);

	CRect picRC(rc);
	CRect textRC(rc);

	if (bPicAvailable && bTextAvailable)
	{
		picRC.right = rc.Width() / 2;
		textRC.left = rc.Width() / 2;
	} 
	if (bTextAvailable)
		textRC.right = textRC.left + min(550, textRC.Width());
	picRC.DeflateRect(cXMargin, cYMargin);
	textRC.DeflateRect(cXMargin, cYMargin);

	//Picture in left
	CFont* oldFont = dc.SelectObject(m_pSmallFont);
	dc.SetBkMode(TRANSPARENT);
	INT picWid = 0;
	if (bPicAvailable)
	{
		//GdiPlusPicDrawer pic;
		//pic.LoadFile(m_picture.GetCurrentImagePath());
		//pic.Draw(dc.m_hDC, rc);
		//m_pPicture->SetTitle(m_Text[TI_PicTitle].c_str());
		m_picture.SetWindowPos(NULL, picRC.left, picRC.top, picRC.Width(), picRC.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
	}
	else
		m_picture.ShowWindow(SW_HIDE);
	if (bTextAvailable)
	{
		//INT lastLeft = min(drawingRC.left + picWid + cXMargin, drawingRC.Width() / 2);
		INT txtWidth = min(550, textRC.Width());

		CBrush* pOldBrush = dc.SelectObject(&m_VariousItemsBrush);

		//CRect mainRC(drawingRC);
		//mainRC.left = lastLeft;
		//mainRC.right = lastLeft + txtWidth;
		CPen* pOldPen = (CPen*)dc.SelectStockObject(DC_PEN);
		dc.SetDCPenColor(m_colors[COL_Ouline]);
		dc.RoundRect(&textRC, CPoint(6,6));
		textRC.DeflateRect(8, 8);


		CRect titleRC(textRC);
		const int xInflation = 5;
		const int yInflation = 2;
		titleRC.DeflateRect(xInflation, yInflation);
		dc.DrawText(m_Text[TI_TextTitle].c_str(), -1, &titleRC, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS | DT_CALCRECT);
		m_AlphaDrawer.SetColor(RGB(0,0,0));
		dc.SetTextColor(m_colors[COL_TextTitle]);
		int titHeight = titleRC.Height();
		//titleRC.top = drawingRC.top + titHeight + 5;
		//titleRC.bottom = titleRC.top + titHeight;
		CRect borderRC(titleRC);
		borderRC.InflateRect(xInflation,yInflation);
		m_AlphaDrawer.Draw(dc, borderRC, 140);
		dc.DrawText(m_Text[TI_TextTitle].c_str(), -1, &titleRC, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS);
		m_pEdit->SetWindowPos(NULL, textRC.left, textRC.top + borderRC.Height() + 3, textRC.Width(), textRC.Height() - borderRC.Height(), SWP_SHOWWINDOW | SWP_NOZORDER);
		dc.SelectObject(pOldBrush);
		dc.SelectObject(pOldPen);
	}
	else
	{
		m_pEdit->ShowWindow(SW_HIDE);
	}

	dc.SelectObject(oldFont);
	return 0;//max(UINT(picHei), UINT(curTop + m_ScrollHelper.GetScrollPos().cy));
}

BOOL CInfoCtrl::SetActiveItem(FullTrackRecordSP& track)
{
	ASSERT(track->IsValid());
	if (track->IsValid())
	{
		m_curFTRecord = track;
		return TRUE;
	}
	return FALSE;
}

const FullTrackRecordSP& CInfoCtrl::GetActiveItem()
{
	if (m_curFTRecord.get() == NULL)
		INT a = 0;
	return m_curFTRecord;
}
void CInfoCtrl::SetStyle(ItemTypeEnum style)
{
	m_desiredItemType = style;
}
ItemTypeEnum CInfoCtrl::GetStyle()
{
	return m_desiredItemType;
}
void CInfoCtrl::Refresh(UINT delay/* = 0*/)
{
	if (delay == 0)
		ActualRefresh();
	else
	{
		KillTimer(TIMER_DELAY);
		SetTimer(TIMER_DELAY, delay, NULL);
	}
}


void CInfoCtrl::ActualRefresh()
{
	TRACEST(_T("CInfoCtrl::ActualRefresh"), m_desiredItemType);
	m_showingItems.clear();
	SetTextInfo(TI_MainText, _T(""));
	SetTextInfo(TI_TextTitle, _T(""));
	m_picture.ClearImages();
	m_bRequestArtistImage = FALSE;
	m_bRequestAlbumImage= FALSE;
	m_bRequestArtistInfo = FALSE;
	m_bRequestAlbumInfo = FALSE;
	if (m_curFTRecord.get() == NULL)
	{
		Invalidate(FALSE);
		return;
	}
	BOOL bRet = FALSE;
	switch (m_desiredItemType)
	{
	case IT_Artist:
		bRet = MakeArtistPage();
		break;
	case IT_Album:
		bRet = MakeAlbumPage();
		break;
	case IT_Track:
		bRet = MakeTrackPage();
		break;
	case IT_Genre:
		bRet = MakeGenrePage();
		break;
	case IT_Year:
		bRet = MakeYearPage();
		break;
	}
	if (!bRet)
		SetNoInfoPage();

	INT delay = PRGAPI()->GetOption(OPT_IM_AutoDlActivationDelay);
	if (delay < 1)
		delay = 1;
	if (delay > 60)
		delay = 60;
	KillTimer(TIMER_AUTODOWNLOAD);
	SetTimer(TIMER_AUTODOWNLOAD, delay * 1000, NULL);
	m_bNeedsRedraw = TRUE;
	Invalidate();
}

void CInfoCtrl::Redraw()
{
	if (m_memDC.m_hDC != NULL)
	{
		CRect rc;
		GetClientRect(&rc);
		DrawStuff(m_memDC, rc, FALSE);
	}
}


void CInfoCtrl::SetTextInfo(TEXTINFO ti, LPCTSTR info)
{
	ASSERT(ti>=0 && ti<TI_Last);
	if (ti!= TI_MainText)
		m_Text[ti] = info;
	else
	{
		CString modInfo(info);
		//=== Protect CRLF
		modInfo.Replace(_T("\r\n"), _T("#|"));
		//=== Make the replaces
		modInfo.Replace(_T("\r"), _T("\r\n"));
		modInfo.Replace(_T("\n"), _T("\r\n"));
		//=== Repair original CRLF
		modInfo.Replace(_T("#|"), _T("\r\n"));
		m_pEdit->SetWindowText(modInfo);
	}
}

void CInfoCtrl::OnSize(UINT nType, int cx, int cy)
{
	if (m_bmpSize.cx < cx || m_bmpSize.cy < cy)
	{
		m_bmpSize.cx = cx + 10;
		m_bmpSize.cy = cy + 10;
		CClientDC cdc(this);
		if (m_memBitmap.m_hObject == NULL)
		{
			m_memBitmap.CreateCompatibleBitmap(&cdc, m_bmpSize.cx, m_bmpSize.cy);
			m_memDC.CreateCompatibleDC(&cdc);
		}
		else
		{
			m_memDC.SelectObject(m_oldBitmap);
			m_memBitmap.DeleteObject();
			m_memBitmap.CreateCompatibleBitmap(&cdc, m_bmpSize.cx, m_bmpSize.cy);
		}
		m_oldBitmap = m_memDC.SelectObject(&m_memBitmap);
	}
	//Fade code end
	if (m_hWnd && cx > 0 && cy > 0)
		DrawStuff(m_memDC, CRect(0,0,cx,cy), FALSE);
	CWnd::OnSize(nType, cx, cy);
	//m_ScrollHelper.OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
}
BOOL CInfoCtrl::MakeGenrePage()
{
	ASSERT(m_curFTRecord.get() != NULL);
	m_ExtraTextStyle = 0;
	//Main Title ---------------------------------------
	SetTextInfo(TI_TextTitle, m_curFTRecord->genre.name.c_str());

	//Main Text ------------------------------------
	SetTextInfo(TI_MainText, _T(""));

	return TRUE;
}

BOOL CInfoCtrl::MakeYearPage()
{
	ASSERT(m_curFTRecord.get() != NULL);
	const UINT bfLen = 1000;
	TCHAR bf[bfLen];
	m_ExtraTextStyle = 0;
	//Title ---------------------------------------
	_sntprintf(bf, bfLen, _T("%d"), m_curFTRecord->track.year);
	SetTextInfo(TI_TextTitle, bf);
	//SubTitle ------------------------------------
	SetTextInfo(TI_MainText, _T(""));
	return TRUE;
}

void CInfoCtrl::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_DELAY:
		ActualRefresh();
		break;
	case TIMER_FADE_REFRESH:
		Invalidate();
		break;
	case TIMER_AUTODOWNLOAD:
		{
			if (m_curFTRecord.get() == NULL)
				return;
			TRACEST(_T("CInfoCtrl::OnTimer (TIMER_AUTODOWNLOAD) (p=type)"), m_desiredItemType);
			InfoDownloadManager* pIDM = PRGAPI()->GetInfoDownloadManager();
			if (m_bRequestArtistImage)
				pIDM->RequestArtistPic(m_curFTRecord->artist, NULL, FALSE, FALSE);
			if (m_bRequestArtistInfo)
				pIDM->RequestArtistInfo(m_curFTRecord->artist, NULL, FALSE, FALSE);
			if (m_bRequestAlbumImage)
				pIDM->RequestAlbumPic(m_curFTRecord->album, 
					m_curFTRecord->artist.name.c_str(), NULL, FALSE, FALSE);
			if (m_bRequestAlbumInfo)
				pIDM->RequestAlbumInfo(m_curFTRecord->album, 
					m_curFTRecord->artist.name.c_str(), NULL, FALSE, FALSE);
			break;
		}
		break;
	default:
		//It can happen if you derive It from CEDit
		//CEdit::OnTimer(nIDEvent);
		ASSERT(0);//?
	}
	KillTimer(nIDEvent);

}

BOOL CInfoCtrl::SetNoInfoPage()
{
	m_ExtraTextStyle = 0;
	SetTextInfo(TI_MainText, _T(""));
	return TRUE;
}

UINT CInfoCtrl::AssignArtistImages()
{
	if (m_curFTRecord.get() == NULL)
		return 0;
	if (!m_InfoTypeEnabled[IIT_ArtistPicture])
		return 0;
	if (m_curFTRecord->artist.name.empty())
		return 0;
	if (m_curFTRecord->artist.name[0] == '[')
		return 0;
	//TRACE(_T("@3 CInfoCtrl::AssignArtistImages '%s'\r\n"), m_curFTRecord->artist.name.c_str());
	PrgAPI* pAPI = PRGAPI();
	LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();
	std::tstringvector col;
	if (pLPM->GetArtistPictures(m_curFTRecord->artist, col))
	{
		m_showingItems.push_back(IIT_ArtistPicture);
		for (size_t i=0; i < col.size(); i++)
		{
			//TRACE(_T("@3 CInfoCtrl::AssignArtistImages Adding '%s'\r\n"), col[i].c_str());
			m_picture.AddImage(col[i].c_str(), m_curFTRecord->artist.name.c_str());
		}
	}
	m_bRequestArtistImage = (col.size() == 0);
	return col.size();
}
UINT CInfoCtrl::AssignAlbumImages()
{
	if (m_curFTRecord.get() == NULL)
		return 0;
	if (!m_InfoTypeEnabled[IIT_AlbumPicture])
		return 0;
	if (m_curFTRecord->album.name.empty())
		return 0;
	if (m_curFTRecord->album.name[0] == '[')
		return 0;
	PrgAPI* pAPI = PRGAPI();
	LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();
	std::tstringvector col;
	FullAlbumRecord rec;
	rec.artist = m_curFTRecord->artist;
	rec.album = m_curFTRecord->album;
	if (pLPM->GetAlbumPictures(rec, col))
	{
		m_showingItems.push_back(IIT_AlbumPicture);
		const INT bfLen = 500;
		TCHAR bf[bfLen];
		_sntprintf(bf, bfLen, _T("%s - %s"), 
			m_curFTRecord->artist.name.c_str(), 
			m_curFTRecord->album.name.c_str());
		for (size_t i=0; i < col.size(); i++)
		{
			//TRACE(_T("@3 CInfoCtrl::AssignAlbumImages Adding '%s'\r\n"), col[i].c_str());
			m_picture.AddImage(col[i].c_str(), bf);
		}
	}
	m_bRequestAlbumImage = (col.size() == 0);
	return col.size();
}

BOOL CInfoCtrl::AssignInfo(InfoItemTypeEnum iit)
{
	if (m_curFTRecord.get() == NULL)
		return FALSE;
	if (!m_InfoTypeEnabled[iit])
		return FALSE;
	UINT infoID = 0;
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM =pAPI->GetSQLManager();
	InfoRecord ir;
	const UINT bfLen = 1000;
	TCHAR bf[bfLen];
	bf[0] = 0;
	m_bRequestArtistInfo = FALSE;
	switch (iit)
	{
	case IIT_ArtistBio:
		if (pSM->GetArtistInfoRecord(m_curFTRecord->artist.ID, iit, ir))
		{
			m_showingItems.push_back(iit);
			_sntprintf(bf, bfLen, _T("%s (%s)"), 
				m_curFTRecord->artist.name.c_str(),
				pAPI->GetStringForInfoItemType(iit)
				);
		}
		else 
			m_bRequestArtistInfo = TRUE;
		break;
	case IIT_AlbumReview:
		if (pSM->GetAlbumInfoRecord(m_curFTRecord->album.ID, iit, ir))
		{
			m_showingItems.push_back(iit);
			_sntprintf(bf, bfLen, _T("%s - %s (%s)"), 
				m_curFTRecord->artist.name.c_str(),
				m_curFTRecord->album.name.c_str(),
				pAPI->GetStringForInfoItemType(iit)
				);
		}
		else 
			m_bRequestArtistInfo = TRUE;
		break;
	case IIT_TrackComment:
	case IIT_TrackLyrics:
	case IIT_TrackPersonal:
		if (pSM->GetTrackInfoRecord(m_curFTRecord->track.ID, iit, ir))
		{
			m_showingItems.push_back(iit);
			_sntprintf(bf, bfLen, _T("%s - %s (%s)"), 
				m_curFTRecord->artist.name.c_str(),
				m_curFTRecord->track.name.c_str(),
				pAPI->GetStringForInfoItemType(iit)
				);
		}
		else 
			m_bRequestArtistInfo = TRUE;
		break;
	default:
		return FALSE;
	}
	if (bf[0] != 0)
	{
		SetTextInfo(TI_TextTitle, bf);
		SetTextInfo(TI_MainText, ir.name.c_str());
	}
	return bf[0] != 0;
}

BOOL CInfoCtrl::MakeArtistPage()
{
	UINT images = AssignArtistImages();
	BOOL bInfo = AssignInfo(IIT_ArtistBio);
	return images > 0 || bInfo;
}

BOOL CInfoCtrl::MakeAlbumPage()
{
	UINT images = AssignAlbumImages();
	if (images == 0)
		images = AssignArtistImages();
	BOOL bInfo = AssignInfo(IIT_AlbumReview);
	m_bRequestAlbumInfo = !bInfo;
	if (!bInfo)
		bInfo = AssignInfo(IIT_ArtistBio);
	return images > 0 || bInfo;
}

BOOL CInfoCtrl::MakeTrackPage()
{
	UINT images = AssignAlbumImages();
	if (images == 0)
		images = AssignArtistImages();
	BOOL bInfo = AssignInfo(IIT_TrackLyrics);
	if (!bInfo)
		bInfo = AssignInfo(IIT_TrackComment);
	if (!bInfo)
		bInfo = AssignInfo(IIT_TrackPersonal);
	if (!bInfo)
		bInfo = AssignInfo(IIT_AlbumReview);
	if (!bInfo)
		bInfo = AssignInfo(IIT_ArtistBio);
	return images > 0 || bInfo;
}

int CInfoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CInfoCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	//This is needed for the WM_MOUSEWHEEL to work
	SetFocus();
}

void CInfoCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	TRACEST(_T("CInfoCtrl::OnContextMenu"));
	if (!(m_desiredItemType == IT_Artist || m_desiredItemType == IT_Album))
		return;
	PrgAPI* pLang = PRGAPI();
	if (m_Menu.m_hMenu == 0)
	{
		m_Menu.CreatePopupMenu();
		m_Menu.SetFont(pLang->GetFont(FONT_Dialogs));
		//m_Menu.AppendMenu(MF_STRING, ID_OPENPIC, (LPTSTR) pLang->GetString(IDS_OPENPICTURE));
		//m_Menu.AppendMenu(MF_STRING, ID_EDITPIC, (LPTSTR) pLang->GetString(IDS_EDITPICTURE));
		//m_Menu.AppendMenu(MF_STRING, ID_SAVEPIC, (LPTSTR) pLang->GetString(IDS_SAVEPICTURE));
		//m_Menu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		m_Menu.AppendMenu(MF_STRING, ID_SAVETEXT, (LPTSTR) pLang->GetString(IDS_SAVETEXT));
		m_Menu.AppendMenu(MF_STRING, ID_COPYTEXT, (LPTSTR) pLang->GetString(IDS_COPYTEXT));
		m_Menu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		m_Menu.AppendMenu(MF_STRING, ID_PROPERTIES, (LPTSTR) pLang->GetString(IDS_PROPERTIES));
		//m_Menu.AppendMenu(MF_SEPARATOR, NULL, NULL);
		//m_Menu.AppendMenu(MF_STRING, ID_TOGGLEPLAYER, (LPTSTR) pLang->GetString(IDS_SHOWVISUALIZATIONS));
	}
	//BOOL bPicExists = m_pPicture->GetPictureSize().cx > 0;
	//m_Menu.EnableMenuItem(ID_OPENPIC, bPicExists ? MF_ENABLED : MF_GRAYED);
	//m_Menu.EnableMenuItem(ID_EDITPIC, bPicExists ? MF_ENABLED : MF_GRAYED);
	//m_Menu.EnableMenuItem(ID_SAVEPIC, bPicExists ? MF_ENABLED : MF_GRAYED);
	m_Menu.EnableMenuItem(ID_SAVETEXT, 
		m_Text[TI_MainText].size() > 0 ? MF_ENABLED : MF_GRAYED);
	m_Menu.EnableMenuItem(ID_PROPERTIES, 
		(m_desiredItemType == IT_Artist || m_desiredItemType == IT_Album 
		|| m_desiredItemType == IT_Track)
		? MF_ENABLED : MF_GRAYED);
	m_Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
}

BOOL CInfoCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CInfoCtrl::OnCommand"), wParam);

	switch(wParam)
	{
	case ID_OPENPIC:
	case ID_EDITPIC:
	case ID_SAVEPIC:
		{
			LPCTSTR curPic = m_picture.GetCurrentImagePath();
			if (curPic != NULL)
			{
				switch (wParam)
				{
				case ID_OPENPIC:
					ShellExecute(NULL, NULL, curPic, NULL, NULL, SW_SHOWDEFAULT);
					break;
				case ID_EDITPIC:
					ShellExecute(NULL, _T("edit"), curPic, NULL, NULL, SW_SHOWDEFAULT);
					break;
				case ID_SAVEPIC:
					{
						const TCHAR *ft = _tcsrchr(curPic, '\\') + 1;
						CFileDialog fd(FALSE, NULL, ft);// ,,this);
						fd.m_ofn.Flags |= OFN_EXPLORER;
						if (fd.DoModal() == IDOK)
						{
							if (!CopyFile(curPic, fd.GetFileName(), FALSE))
								PRGAPI()->NotifyUser(IDS_OPERATIONFAILED, IDS_SAVEPICTURE, SEV_Warning);
						}
					}
					break;
				}
			}
		}
		break;
	case ID_SAVETEXT:
		{
			CFileDialog fd(FALSE, _T("*.txt"), _T("Export.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
				_T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*||"), this);
			fd.m_ofn.Flags |= OFN_EXPLORER;
			if (fd.DoModal() == IDOK)
			{
				CFile f;
				if (f.Open(fd.GetPathName(), CFile::modeCreate | CFile::modeWrite))
				{
					f.Write((LPCSTR)CT2CA(m_Text[TI_MainText].c_str()), m_Text[TI_MainText].size());
					f.Close();
				}
			}
		}
		break;
	case ID_COPYTEXT:
		{
			size_t len = m_Text[TI_MainText].size();
			if (len > 0)
			{
				if (OpenClipboard())
				{
					EmptyClipboard();
					HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, (len+1) * sizeof(TCHAR));
					TCHAR* pchData = (TCHAR*)GlobalLock(hClipboardData);
					_tcscpy(pchData, m_Text[TI_MainText].c_str());
					GlobalUnlock(hClipboardData);
#ifdef _UNICODE
					SetClipboardData(CF_UNICODETEXT, hClipboardData);
#else
					SetClipboardData(CF_TEXT, hClipboardData);
#endif
					CloseClipboard();
				}
			}
		}
		break;
	case ID_PROPERTIES:
		{
			if (m_curFTRecord.get() == NULL)
				return FALSE;
			ActionManager* pAm = PRGAPI()->GetActionManager();
			SQLManager* pSM = PRGAPI()->GetSQLManager();
			switch (m_desiredItemType)
			{
			case IT_Artist:
				{
					FullArtistRecordSP rec;
					if (pSM->GetFullArtistRecordByID(rec, m_curFTRecord->artist.ID))
						pAm->ShowArtistPropertiesDlg(rec, this, FALSE);
				}
				break;
			case IT_Album:
				{
					FullAlbumRecordSP rec;
					if (pSM->GetFullAlbumRecordByID(rec, m_curFTRecord->album.ID))
					{
						pAm->ShowAlbumPropertiesDlg(rec, this, FALSE);
					}
				}
				break;
			case IT_Track:
				pAm->ShowTrackPropertiesDlg(m_curFTRecord, this);
				break;
			default:
				ASSERT(0);
			}
		}
		break;
	//case ID_TOGGLEPLAYER:
	//	ShowPlayer(!m_bPlayerVisible);
	//	break;
	default:
		return __super::OnCommand(wParam, lParam);
	}
	return TRUE;
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------------------
//---------------------------------------------------

void CInfoCtrl::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (idx == COL_BkText)
		{
			if (m_pEdit != NULL)
				m_pEdit->SetBkColor(value);	//***TEMP Move this to SetColor
			if (m_VariousItemsBrush.m_hObject)
				m_VariousItemsBrush.DeleteObject();
			m_VariousItemsBrush.CreateSolidBrush(value);	//Version 2
		} 
		else if (idx == COL_Text)
		{
			if (m_pEdit != NULL)
				m_pEdit->SetTextColor(m_colors[COL_Text]);	
		}
		else if (idx == COL_Bk)
		{
			m_picture.SetBkColor(value);
		}
	}
}
COLORREF CInfoCtrl::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

void CInfoCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	LPCTSTR pic = m_picture.GetCurrentImagePath();
	if (pic != NULL)
		ShellExecute(NULL, NULL, pic, NULL, NULL, SW_SHOWMAXIMIZED);

}


//const TCHAR* const cColorNames[] = {
//	_T("Bk"),
//	_T("BkText"),
//	_T("BkTitle"),
//	_T("Text"),
//	_T("TextTitle"),
//	NULL
//};
//LPCTSTR CInfoCtrl::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
