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
#include "TrayToolTipDlg.h"
#include "PrgAPI.h"
#include "SQLManager.h"
#include "TeenSpirit.h"
#include "LocalPictureManager.h"
#include "hdcUtils.h"
#include "QuickSearchDlg.h"
#include "GdiplusUtilities.h"
//#include "StateManager.h"

#define ID_BUTTONSID 200
#define ID_SYSTEMBUTTONSID 300
#define ID_CLOSEBUTTONID 400


const INT TrayToolTipDlg::s_marginWidth = 6;
const INT TrayToolTipDlg::s_normalButtonWidth = 24;
const INT TrayToolTipDlg::s_systemButtonWidth = 18;
const INT TrayToolTipDlg::s_distanceBetweenButtonsWidth = 40;
const INT TrayToolTipDlg::s_volumeWidth = 50;

const INT TrayToolTipDlg::s_marginHeight = 6;
const INT TrayToolTipDlg::s_internalMarginHeight = 2;
const INT TrayToolTipDlg::s_buttonHeight = 24;
const INT TrayToolTipDlg::s_sliderHeight = 12;





TrayToolTipDlg::TrayToolTipDlg():
m_transparency(240),
m_transparencyStep(20),
m_transparencyStepDelay(40),
m_realTransparency(0),
m_state(S_Hidden),
m_tickCountToClose(0),
m_bDataAreInvalid(TRUE),
m_maxWidth(500),
m_minWidth(200),
m_windowWidth(300),
//m_titleHeight(42),
m_pictureWidth(0)
{
	//m_colors[COL_Bk] = RGB(0xff,0xff,0xbf);
	m_colors[COL_Bk] = RGB(0xd0,0xd0,0xd0);
	m_colors[COL_Bk2] = RGB(0xd0,0xd0,0xa0);

	m_colors[COL_Transparency] = RGB(0xff,0x00,0xff);
	m_colors[COL_Border] = RGB(80,80,80);
	m_colors[COL_TitleText] = RGB(20,20,20);
	m_colors[COL_SubTitleText] = RGB(20,20,20);
	m_colors[COL_PictureBorder] = RGB(160, 160, 160);

	//m_titleHeight = 38;

	
	m_fontHeight[FNT_AppTitle] = 12;

	m_fontHeight[FNT_Title] = 18;
	m_fontHeight[FNT_Subtitle] = 18;
	m_fontHeight[FNT_Text] = 16;

}

TrayToolTipDlg::~TrayToolTipDlg()
{
	if (GetSafeHwnd())
		DestroyWindow();
}


BEGIN_MESSAGE_MAP(TrayToolTipDlg, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


void TrayToolTipDlg::ConfigureButton(FlexButton& button)
{
	button.SetComponents(FlexButton::COMP_Image);

	button.SetBackgroundDrawEffect(FlexButton::ST_Normal, FlexButton::BDE_None);
	button.SetBackgroundDrawEffect(FlexButton::ST_Down, FlexButton::BDE_None);
	button.SetBackgroundDrawEffect(FlexButton::ST_Over, FlexButton::BDE_None);

	button.SetFrameDrawEffect(FlexButton::ST_Normal, FlexButton::FDE_None);
	button.SetFrameDrawEffect(FlexButton::ST_Down, FlexButton::FDE_None);
	button.SetFrameDrawEffect(FlexButton::ST_Over, FlexButton::FDE_None);
	
	button.SetImageDrawEffect(FlexButton::ST_Normal, FlexButton::IDE_Normal);
	button.SetImageDrawEffect(FlexButton::ST_Down, FlexButton::IDE_Normal);
	button.SetImageDrawEffect(FlexButton::ST_Over, FlexButton::IDE_ShowInvertedIcon);

}

BOOL TrayToolTipDlg::Create(CWnd* pParent)
{
	//TRACEST(_T("TrayToolTipDlg::Create"));
	PrgAPI* pAPI = PRGAPI();
	HWND hParent = pParent ? pParent->GetSafeHwnd() : NULL;

	if (CreateEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED, 
		_T("#32770"), 
		NULL, 
		WS_POPUP, 
		0, 0, m_windowWidth, GetTotalHeight(), 
		hParent, 0, 0))
	{
		SetRealTransparency(0);
		CRect rc;//(m_minWidth - 18, 2, m_minWidth - 2 , 18);
		//m_closeButton.Create(0, WS_VISIBLE | WS_CHILD, rc, this, IDCANCEL);	

		//m_closeButton.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
		//m_closeButton.SetComponents(FlexButton::COMP_Image);
		//m_closeButton.SetIcon(pAPI->GetIcon(ICO_Close16), 16, 16);

		const APP_ICONS Btn2Icon[] = 
		{
			ICO_Prev24,
			ICO_Play24,
			ICO_Pause24,
			ICO_Stop24,
			ICO_Next24
		};

		ASSERT(sizeof(Btn2Icon)/sizeof(INT) == BTN_Last);
		for (int i = 0; i < BTN_Last; i++)
		{
			VERIFY(m_buttons[i].Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, rc, this, i + ID_BUTTONSID));
			ConfigureButton(m_buttons[i]);
			m_buttons[i].SetIcon(pAPI->GetIcon(Btn2Icon[i]), 24, 24);
		}



		const APP_ICONS SBtn2Icon[] = 
		{
			ICO_Find16
		};

		ASSERT(sizeof(SBtn2Icon)/sizeof(INT) == SBTN_Last);
		for (int i = 0; i < SBTN_Last; i++)
		{
			VERIFY(m_systemButtons[i].Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, rc, this, i + ID_SYSTEMBUTTONSID));
			ConfigureButton(m_systemButtons[i]);
			m_systemButtons[i].SetIcon(pAPI->GetIcon(SBtn2Icon[i]), 16, 16);
		}

		VERIFY(m_closeButton.Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, rc, this, ID_CLOSEBUTTONID));
		ConfigureButton(m_closeButton);
		m_closeButton.SetIcon(pAPI->GetIcon(ICO_Close16), 16, 16);

		




		m_positionSlider.Create(this, WS_VISIBLE | WS_CHILD, rc, this, 210);
		m_positionSlider.SetTransparentMode(TRUE);
		m_volumeSlider.Create(this, WS_VISIBLE | WS_CHILD, rc, this, 211);
		m_volumeSlider.SetPos(pAPI->GetMediaPlayer()->GetVolume());
		m_volumeSlider.SetMaxPos(100);
		m_volumeSlider.SetTransparentMode(TRUE);



		SetColor(COL_Bk, m_colors[COL_Bk]);
		SetColor(COL_Bk2, m_colors[COL_Bk2]);




		LOGFONT lf;
		lf.lfHeight = 13;
		lf.lfWidth = 0;
		lf.lfEscapement = 0; 
		lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = CLEARTYPE_QUALITY;//ANTIALIASED_QUALITY;// 
		lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		_tcscpy(lf.lfFaceName, _T("Tahoma"));


		lf.lfHeight = m_fontHeight[FNT_AppTitle];
		lf.lfWeight = FW_NORMAL;
		m_font[FNT_AppTitle].CreateFontIndirect(&lf);

		lf.lfHeight = m_fontHeight[FNT_Title];
		lf.lfWeight = FW_BOLD;
		//lf.lfWidth = lf.lfHeight / 3;
		m_font[FNT_Title].CreateFontIndirect(&lf);

		lf.lfHeight = m_fontHeight[FNT_Subtitle];
		lf.lfWeight = FW_NORMAL;
		//lf.lfWidth = lf.lfHeight / 3;
		m_font[FNT_Subtitle].CreateFontIndirect(&lf);

		lf.lfHeight = m_fontHeight[FNT_Text];
		//lf.lfWidth = lf.lfHeight / 3;
		m_font[FNT_Text].CreateFontIndirect(&lf);

		return TRUE;
	}
	else
	{
		DWORD dwLastError = GetLastError();
		//TRACE(_T("TrayToolTipDlg::Create Failed\r\n"));
	}
	return FALSE;
}

void TrayToolTipDlg::OnMediaChanged(BOOL bShowWindow)
{
	//TRACEST(_T("TrayToolTipDlg::OnMediaChanged"));
	m_bDataAreInvalid = TRUE;
	if (bShowWindow)
	{
		if (GetSafeHwnd() == NULL)
			Create(NULL);
		KillTimer(TMR_MediaChanged);
		if (IsWindowVisible())
			SetTimer(TMR_MediaChanged, 100, NULL);
		else
			SetTimer(TMR_MediaChanged, 2000, NULL);
	}
}


void TrayToolTipDlg::OnMouseOverTray(CPoint& p)
{
	m_cursorPos = p;
	Show(500);
}

BOOL TrayToolTipDlg::CalculateNewWindowLeftTop(INT& x, INT& y)
{
	RECT rcWorkArea = { 0 };
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWorkArea, 0 );

	x = rcWorkArea.right - m_windowWidth - 4;
	y = rcWorkArea.bottom - GetTotalHeight() - 4;


	//=== Get The Toolbar that holds the icons
	//HWND hWndTray = ::FindWindow(_T("Shell_TrayWnd"), NULL);
	//if (hWndTray == NULL)
	//{
	//	x = y = 10;
	//	return FALSE;
	//}
	//CRect rc;
	//::GetWindowRect(hWndTray, &rc);
	//if (rc.top < 5)
	//{
	//	x = rc.right - m_desiredWidth - 3;
	//	y = rc.bottom + 3;

	//}
	//else
	//{
	//	//=== Is at bottom (Normal)
	//	x = rc.right - m_desiredWidth - 3;
	//	y = rc.top - m_height - 3;
	//}
	return TRUE;
}


BOOL TrayToolTipDlg::Show(DWORD nAutoHideTimeOut)
{
	//TRACEST(_T("TrayToolTipDlg::Show"));
	TRACEST(_T("TrayToolTipDlg::Show nAutoHideTimeOut: "), nAutoHideTimeOut);
	INT x,y;
	if (!CalculateNewWindowLeftTop(x, y))
		return FALSE;
	//=== Calc the time that it is supposed to AutoHide
	DWORD newTickCountToClose = GetTickCount() + nAutoHideTimeOut;
	if (newTickCountToClose > m_tickCountToClose)
		m_tickCountToClose = newTickCountToClose;

	//=== If it is not created then create it now
	if (GetSafeHwnd() == NULL)
		Create(NULL);

	//=== Restart the Hide On Mouse out Timer
	KillTimer(TMR_HideOnMouse);
	SetTimer(TMR_HideOnMouse, 500, NULL);
	Invalidate(FALSE);
	SetWindowPos(NULL, x, y, m_windowWidth, GetTotalHeight(), SWP_NOACTIVATE | SWP_NOZORDER);
	switch (m_state)
	{
	case S_Showed:
	case S_Showing:
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		break;
	case S_Hiding:
		m_state = S_Showing;
		break;
	case S_Hidden:
		m_state = S_Showing;
		SetTimer(TMR_Transparency, m_transparencyStepDelay, NULL);
		SetTimer(TMR_UpdateSliders, 1000, NULL);
		UpdateSliders();
		break;
	}
	return TRUE;
}

void TrayToolTipDlg::Hide()
{
	if (GetSafeHwnd() == NULL)
		return;
	switch (m_state)
	{
	case S_Showed:
		TRACE(_T("TrayToolTipDlg::Hide S_Showed -> S_Hiding"));
		m_state = S_Hiding;
		SetTimer(TMR_Transparency, m_transparencyStepDelay, NULL);
		break;
	case S_Showing:
		TRACE(_T("TrayToolTipDlg::Hide S_Showing -> S_Hiding"));
		//SetTimer(TMR_Transparency, m_transparencyStepDelay, NULL);
		m_state = S_Hiding;
		break;
	case S_Hiding:
		//SetTimer(TMR_Transparency, m_transparencyStepDelay, NULL);
		break;
	case S_Hidden:
		//KillTimer(TMR_HideOnMouse);
		break;
	}
}

void TrayToolTipDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TMR_HideOnMouse:
		if (GetTickCount() > m_tickCountToClose)
		{
			CPoint pt;
			GetCursorPos(&pt);
			if (pt != m_cursorPos)
			{
				HWND hwnd = ::WindowFromPoint(pt);
				hwnd = ::GetAncestor(hwnd, GA_ROOT);
				if (hwnd != m_hWnd)
				{
					//TRACE(_T("TMR_HideOnMouse Mouse can be anywhere\r\n"));
					Hide();
				}
				else
				{
					TRACE(_T("TMR_HideOnMouse Point over window\r\n"));
					m_state = S_Showing;
					SetTimer(TMR_Transparency, m_transparencyStepDelay, NULL);
				}
			}
			else
			{
				//TRACE(_T("TMR_HideOnMouse Point over Initial Position\r\n"));
				m_state = S_Showing;
				SetTimer(TMR_Transparency, m_transparencyStepDelay, NULL);
			}
		}
		else
		{
			//TRACE(_T("TMR_HideOnMouse Not the right time\r\n"));
		}
		break;
	case TMR_Transparency:
		{
			INT newTrans = 0;
			switch (m_state)
			{
			case S_Hidden:
			case S_Showed:
				ASSERT(0);
				break;
			case S_Showing:
				newTrans = m_realTransparency + m_transparencyStep;
				if (newTrans > m_transparency)
				{
					newTrans = m_transparency;
					m_state = S_Showed;
					KillTimer(TMR_Transparency);
				}
				SetRealTransparency(newTrans);
				//TRACE(_T("TMR_Transparency Showing %d\r\n"), newTrans);
				break;
			case S_Hiding:
				newTrans = m_realTransparency - m_transparencyStep;
				if (newTrans < 0)
				{
					newTrans = 0;
					m_state = S_Hidden;
					//ShowWindow(SW_HIDE);
					KillTimer(TMR_Transparency);
					KillTimer(TMR_HideOnMouse);
				}
				SetRealTransparency(newTrans);
				//TRACE(_T("TMR_Transparency Hiding %d\r\n"), newTrans);
				break;
			}
		}
		break;
	case TMR_MediaChanged:
		KillTimer(TMR_MediaChanged);
		Show(5000);
		break;
	case TMR_UpdateSliders:
		if (IsWindowVisible())
			UpdateSliders();
		else
			KillTimer(TMR_UpdateSliders);
		break;
	default:
		CWnd::OnTimer(nIDEvent);
		break;
	}
}

void TrayToolTipDlg::UpdateSliders()
{
	MediaPlayer* pMP = PRGAPI()->GetMediaPlayer();
	INT volume = pMP->GetVolume();
	INT pos = (INT)(pMP->GetMediaPos() * 1000);
	m_volumeSlider.SetPos(volume);
	m_positionSlider.SetPos(pos);
	m_positionSlider.SetMaxPos((INT)(pMP->GetMediaLength()*1000));
}


//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void TrayToolTipDlg::SetColor(UINT idx, COLORREF value)
{
	//TRACEST(_T("TrayToolTipDlg::SetColor"));
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	switch (idx)
	{
	case COL_Bk:
		//m_closeButton.SetColor(FlexButton::COL_Bk, value);
		break;
	case COL_Bk2:
		for (int i = 0; i < BTN_Last; i++)
			m_buttons[i].SetColor(FlexButton::COL_Bk, value);
		for (int i = 0; i < SBTN_Last; i++)
			m_systemButtons[i].SetColor(FlexButton::COL_Bk, value);
		m_volumeSlider.SetColor(CMediaSlider::COL_Bk, value);
		m_positionSlider.SetColor(CMediaSlider::COL_Bk, value);
		break;
	}

	//else if (idx == COL_Text)
	//{
	//	//m_caption.SetColor(CLabelEx::COL_Text, value);
	//}
}
COLORREF TrayToolTipDlg::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}





BOOL TrayToolTipDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//TRACEST(_T("TrayToolTipDlg::OnCommand"));


	MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
	switch (wParam)
	{
	case ID_BUTTONSID + BTN_Prev:
		pPlayer->Previous();
		break;
	case ID_BUTTONSID + BTN_Play:
		pPlayer->Start();
		break;
	case ID_BUTTONSID + BTN_Pause:
		pPlayer->Pause();
		break;
	case ID_BUTTONSID + BTN_Next:
		pPlayer->Next();
		break;
	case ID_BUTTONSID + BTN_Stop:
		pPlayer->Stop();
		break;
	case ID_SYSTEMBUTTONSID + SBTN_QuickSearch:
		{
			PrgAPI* pAPI = PRGAPI();
			QuickSearchDlg* pQSDlg = pAPI->GetQuickSearchDlg();
			pQSDlg->SetSearchText(_T(""));
			pQSDlg->ShowRelative(NULL, QuickSearchDlg::RP_Centered);
		}
		break;
	case ID_CLOSEBUTTONID:
		SetRealTransparency(0);
		Hide();
		break;
	}

	return __super::OnCommand(wParam, lParam);
}


BOOL TrayToolTipDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void TrayToolTipDlg::ApplyTransparencyAndBorder(CDC& dc)
{
	CRect rect;
	GetClientRect(&rect);
	//=== Draw the border
	CRect rcBorder(rect);
	dc.SelectStockObject(NULL_BRUSH);
	dc.SelectStockObject(DC_PEN);
	dc.SetDCPenColor(m_colors[COL_Border]);
	rcBorder.right--;
	rcBorder.bottom--;
	dc.RoundRect(&rcBorder, CPoint(9,9));

	//=== Clip the window region
	CRgn rgn;
	rgn.CreateRoundRectRgn(rect.left, rect.top, rect.right, rect.bottom, 9, 9);
	int err = dc.SelectClipRgn(&rgn, RGN_XOR);
	ASSERT(err != ERROR);

	//=== Draw Transparent background
	dc.FillSolidRect(&rect, m_colors[COL_Transparency]);
}

void TrayToolTipDlg::Draw(CDC& dc)
{
	TRACEST(_T("TrayToolTipDlg::Draw"));

	CRect rcMemDC(0, 0, m_ff.GetWidth(), m_ff.GetHeight());

	PrgAPI* pAPI = PRGAPI();
	MediaPlayer* pMP = pAPI->GetMediaPlayer();

	//FullTrackRecordSP m_rec;
	MediaPlayListItem mpli;
	BOOL mpliIsValid = FALSE;
	INT playListPos = pMP->GetPlayListPos();
	if (playListPos != -1)
		mpliIsValid = pMP->GetPlayListItem(mpli, playListPos);

	CRect rcCalc(rcMemDC);
	dc.SetBkMode(TRANSPARENT);
	dc.SelectStockObject(NULL_BRUSH);
	dc.SelectStockObject(DC_PEN);
	INT maxTextWidth = 0;
	m_pictureWidth = 0;



	INT textLine1Width = 0;
	INT textLine2Width = 0;
	
	using namespace Gdiplus;
	Graphics graphics(dc.GetSafeHdc());
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	COLORREF outlineColor = RGB(220,220,220);
	INT r = GetRValue(m_colors[COL_TitleText]) + GetGValue(m_colors[COL_TitleText]) + GetBValue(m_colors[COL_TitleText]);
	if (r > (256 * 3) / 2)
		outlineColor = RGB(30,30,30);


	if (mpliIsValid == FALSE)
	{
		Gdiplus::SolidBrush br(GdiplusUtilities::COLORREF2Color(m_colors[COL_Bk]));
		graphics.FillRectangle(&br, GdiplusUtilities::RECT2GdiplusRect(rcMemDC));
		//rcMemDC.bottom = GetTotalHeight();
		//CFont* pOldFont = dc.SelectObject(&m_font[FNT_Title]);

		//dc.SetTextColor(m_colors[COL_TitleText]);
		//rcMemDC.right = m_minWidth;
		//dc.DrawText(CTeenSpiritApp::sAppTitle, -1, &rcMemDC, DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX | DT_CENTER);
		//dc.SelectObject(pOldFont);

		//maxTextWidth = rcCalc.Width();

		CRect rcItem = rcMemDC;
		rcItem.bottom = GetTotalHeight() - 30;
		rcItem.right = m_minWidth;
		LOGFONT lf;
		m_font[FNT_Title].GetLogFont(&lf);

		GdiplusUtilities::DrawTextOutline(graphics, CTeenSpiritApp::sAppTitle, _tcslen(CTeenSpiritApp::sAppTitle), &rcItem, 
			DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_CENTER, 
			lf, m_colors[COL_TitleText], outlineColor, 2);


	}
	else
	{
		SQLManager* pSM = pAPI->GetSQLManager();
		FullTrackRecordSP rec;
		BOOL bRecIsValid = pSM->GetFullTrackRecordByLocation(rec, mpli.url);

		if (bRecIsValid)
		{
			mpli.artist = rec->artist.name.c_str();
			mpli.title = rec->track.name.c_str();
			mpli.rating = rec->track.rating;
			if (mpli.length == 0)
				mpli.length = rec->track.duration;
		}
		


		//=== Picture

		if (bRecIsValid)
		{
			LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();

			SQLManager* pSM = pAPI->GetSQLManager();
			LPCTSTR imgPath = NULL;
			FullAlbumRecordSP albumRec;
			if (pSM->GetFullAlbumRecordByID(albumRec, rec->album.ID))
				imgPath = pLPM->GetMainAlbumPicture(*albumRec);
			if (imgPath == NULL)
				imgPath = pLPM->GetMainArtistPicture(albumRec->artist);

			if (imgPath != NULL)
			{
				GdiPlusPicDrawer pic;
				pic.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
				pic.GetDrawParams().positionY = 0;
				CRect rcPicture;
				if (pic.LoadFile(imgPath))
				{
					INT pictureHeight = rcMemDC.Height() - 2 * s_marginHeight;
					CSize sz = pic.GetPictureSize();
					m_pictureWidth = pictureHeight * sz.cx / sz.cy;
					if (m_pictureWidth > 2 * pictureHeight)
						m_pictureWidth = 2 * pictureHeight;
					else if (m_pictureWidth < pictureHeight)
						m_pictureWidth = pictureHeight;
					rcPicture.left = s_marginWidth;
					rcPicture.top = s_marginHeight;
					rcPicture.right = rcPicture.left + m_pictureWidth;
					rcPicture.bottom = rcPicture.top + pictureHeight;
					pic.Draw(graphics, GdiplusUtilities::RECT2GdiplusRect(rcMemDC));
					Gdiplus::SolidBrush br(GdiplusUtilities::COLORREF2Color(m_colors[COL_Bk], 50));
					graphics.FillRectangle(&br, GdiplusUtilities::RECT2GdiplusRect(rcMemDC));
					if (pic.Draw(graphics, GdiplusUtilities::RECT2GdiplusRect(rcPicture)))
					{
						dc.SelectStockObject(NULL_BRUSH);
						dc.SelectStockObject(DC_PEN);
						dc.SetDCPenColor(m_colors[COL_PictureBorder]);
						dc.Rectangle(&rcPicture);
						//INT shadowDist = 4;
						//COLORREF clrSshadow = DarkenColor(m_colors[COL_Bk2], .8);
						//dc.SelectStockObject(DC_BRUSH);
						//dc.SelectStockObject(NULL_PEN);
						//dc.SetDCBrushColor(clrSshadow);
						//dc.Rectangle(rcPicture.left + shadowDist, rcPicture.bottom, rcPicture.right + shadowDist, rcPicture.bottom + shadowDist);
						//dc.Rectangle(rcPicture.right, rcPicture.top + shadowDist, rcPicture.right + shadowDist, rcPicture.bottom + shadowDist);
					}
				}
			}
			else
			{
				Gdiplus::SolidBrush br(GdiplusUtilities::COLORREF2Color(m_colors[COL_Bk]));
				graphics.FillRectangle(&br, GdiplusUtilities::RECT2GdiplusRect(rcMemDC));
			}
		}

		INT totalFontHeight = m_fontHeight[FNT_Title] + m_fontHeight[FNT_Subtitle];// + m_fontHeight[FNT_Text];

		
		CRect rcItem(rcMemDC);
		LOGFONT lf;
		//=== AppTitle
		rcItem.top = s_internalMarginHeight;
		rcItem.left += s_marginWidth + m_pictureWidth + s_marginWidth;
		rcItem.right -= s_marginWidth;
		rcItem.bottom = rcItem.top + m_fontHeight[FNT_AppTitle];
		//m_font[FNT_AppTitle].GetLogFont(&lf);
		//GdiplusUtilities::DrawTextOutline(graphics, CTeenSpiritApp::sAppTitle, _tcslen(CTeenSpiritApp::sAppTitle), &rcItem, 
		//	DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX | DT_LEFT, 
		//	lf, m_colors[COL_TitleText], outlineColor, 2);

		//=== Title
		rcItem.top = 10;
		rcItem.bottom = rcItem.top + m_fontHeight[FNT_Title];
		m_font[FNT_Title].GetLogFont(&lf);
		//Gdiplus::RectF rf = GdiplusUtilities::DrawTextMeasure(graphics, mpli.title, _tcslen(mpli.title), lf);
		CRect rcCalc;
		GdiplusUtilities::DrawTextOutline(graphics, mpli.title, _tcslen(mpli.title), &rcItem, 
			DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX | DT_LEFT, 
			lf, m_colors[COL_TitleText], outlineColor, 2, FALSE, &rcCalc);
		INT textWidth = rcCalc.Width();
		maxTextWidth = textWidth;

		//=== SubTitle
		rcItem.top = rcItem.bottom + 3 * s_internalMarginHeight;
		rcItem.bottom = rcItem.top + m_fontHeight[FNT_Subtitle];
		m_font[FNT_Subtitle].GetLogFont(&lf);
		//rf = GdiplusUtilities::DrawTextMeasure(graphics, mpli.artist, _tcslen(mpli.artist), lf);
		GdiplusUtilities::DrawTextOutline(graphics, mpli.artist, _tcslen(mpli.artist), &rcItem, 
			DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX | DT_LEFT, 
			lf, m_colors[COL_SubTitleText], outlineColor, 2, FALSE, &rcCalc);
		textWidth = rcCalc.Width();
		if (textWidth > maxTextWidth)
			maxTextWidth = textWidth;

		maxTextWidth += 4 * s_marginWidth;

//		dc.SelectObject(pOldBrush);
//		dc.SelectObject(pOldPen);
	}
	INT buttonsWidth = SBTN_Last * s_systemButtonWidth + 
		s_distanceBetweenButtonsWidth + 
		BTN_Last * s_normalButtonWidth;
	if (buttonsWidth > maxTextWidth)
		maxTextWidth = buttonsWidth;


	INT desiredWith = s_marginWidth + m_pictureWidth + s_marginWidth + maxTextWidth + s_marginWidth;

	if (desiredWith > m_maxWidth)
		desiredWith = m_maxWidth;
	else if (desiredWith < m_minWidth)
		desiredWith = m_minWidth;
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	if (rcWindow.Width() != desiredWith)
	{
		m_windowWidth = desiredWith;
		SetWindowPos(NULL, rcWindow.right - desiredWith, rcWindow.top, m_windowWidth, GetTotalHeight(), SWP_NOACTIVATE | SWP_NOZORDER);
		
	}
	m_bDataAreInvalid = FALSE;

	//=== Text		20%
	//if (m_hits > 0)
	//{
	//	rcItem.top = rcItem.bottom;
	//	rcItem.bottom = rcItem.top + rc.Height() * m_fontHeight[FNT_Text] / totalFontHeight;
	//	dc.SelectObject(&m_font[FNT_Text]);
	//	TCHAR text[500];
	//	_sntprintf(text, 500, _T("%s: %d   %s: %d"), pAPI->GetString(IDS_RANK), m_rank, pAPI->GetString(IDS_HITS), m_hits);
	//	dc.DrawText(text, -1, &rcItem, DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER | DT_NOPREFIX | DT_LEFT);
	//}




}

void TrayToolTipDlg::OnPaint()
{
	CPaintDC _dc(this); // device context for painting
	if (m_realTransparency != 0)
	{
		TRACE(_T("TrayToolTipDlg::OnPaint()\r\n"));
		m_ff.SetSize(m_maxWidth, GetTotalHeight());
		m_ff.SetTargetDC(_dc);
		if (m_bDataAreInvalid)
			Draw(m_ff.GetMemDC());
		m_ff.Draw();
		ApplyTransparencyAndBorder(_dc);

	}
}

void TrayToolTipDlg::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_buttons[0].GetSafeHwnd())
	{
		//INT left = s_marginWidth;
		//INT top = s_marginHeight;
		//left += m_pictureWidth;
		//top += GetTitleHeight();




		INT leftButtons = cx - SBTN_Last * s_systemButtonWidth - s_marginWidth - BTN_Last * s_normalButtonWidth - s_distanceBetweenButtonsWidth;
		INT left = leftButtons;

		INT topSliders = cy - s_sliderHeight - s_marginHeight;
		INT top = topSliders - s_buttonHeight;
		//=== Place the normal buttons
		for (int i = 0; i < BTN_Last; i++)
		{
			m_buttons[i].MoveWindow(left, top, s_normalButtonWidth, s_buttonHeight, TRUE);
			left += s_normalButtonWidth;
		}
		left += s_distanceBetweenButtonsWidth;
		//=== Place the System buttons
		for (int i = 0; i < SBTN_Last; i++)
		{
			m_systemButtons[i].MoveWindow(left, top, s_systemButtonWidth, s_buttonHeight, TRUE);
			left += s_systemButtonWidth;
		}
		//=== Place the close button
		m_closeButton.MoveWindow(cx - 20, 4, 16, 16, TRUE);

		//=== Place the sliders
		left = leftButtons;
		top += s_buttonHeight;
		m_volumeSlider.MoveWindow(left, top, s_volumeWidth, s_sliderHeight, TRUE);
		left += s_volumeWidth + s_marginWidth;
		INT positionWidth = cx - left - s_marginWidth;
		m_positionSlider.MoveWindow(left, top, positionWidth, s_sliderHeight, TRUE);
	}

}

void TrayToolTipDlg::SetRealTransparency(INT transparency)
{
	//TRACEST(_T("TrayToolTipDlg::SetRealTransparency"));
	SetLayeredWindowAttributes(m_colors[COL_Transparency], transparency, LWA_COLORKEY|LWA_ALPHA);
	if (m_realTransparency == 0 && transparency > 0)
	{
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
		SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	}
	if (m_realTransparency > 0 && transparency == 0)
		ShowWindow(SW_HIDE);
	m_realTransparency = transparency;
}

void TrayToolTipDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	//TRACEST(_T("TrayToolTipDlg::OnLButtonDown"));
	//SetRealTransparency(0);
	//Hide();
	CWnd* wnd = AfxGetMainWnd();
	if (wnd->IsWindowVisible() == FALSE)
		wnd->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	CWnd::OnLButtonDown(nFlags, point);
}

void TrayToolTipDlg::OnMediaSliderClick(CMediaSlider* pMediaSlider)
{
	if (pMediaSlider == &m_positionSlider)
	{
		INT pos = m_positionSlider.GetPos();
		PRGAPI()->GetMediaPlayer()->SetMediaPos(pos / 1000.0);
	}
	else if (pMediaSlider == &m_volumeSlider)
	{
		INT pos = m_volumeSlider.GetPos();
		PRGAPI()->GetMediaPlayer()->SetVolume(pos);
		PRGAPI()->SendMessage(SM_PlayerVolumeChanged);

	}
}

BOOL TrayToolTipDlg::OnStateChanged(UINT stateMessage)
{
	//=== Show Tray ToolTip
	if (stateMessage == SM_MediaChanged)
	{
		OnMediaChanged(PRGAPI()->GetOption(OPT_GEN_ShowToolTipOnMediaChanged));
		return TRUE;
	}
	return FALSE;
}


