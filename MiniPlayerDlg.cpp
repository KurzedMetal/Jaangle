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
#include "MiniPlayerDlg.h"
#include "AppSettings/AppSettings.h"
#include "PrgAPI.h"
#include "MediaPlayer.h"
#include "TeenSpiritDlg.h"
#include "ActionManager.h"
#include "MenuManager.h"
#include "SQLManager.h"
#include "QuickSearchDlg.h"

//=== 	SetWindowPos(&CWnd::wndTopMost, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);


LPCTSTR CMiniPlayerDlg::SettingsCategory = _T("MINIPLR");



#define ID_SPECIALMSG_TIMER_DELAY 5000 

#define ID_BUTTONSID		500
#define ID_SYSTEMBUTTONSID	600
#define ID_ICONBUTTONSID	250


IMPLEMENT_DYNAMIC(CMiniPlayerDlg, CWnd)

const int cMargin = 2;
const INT cPositionSliderHeight = 10;


CMiniPlayerDlg::CMiniPlayerDlg():
	m_bMouseCaptured(FALSE),
	m_transparency(128),
	m_bSpecialMessage(FALSE),
	m_LastMediaLength(-1.0),
	m_bTooltips(TRUE),
	m_bExtraInfo(TRUE),
	m_maxSize(500),
	m_height(26),
	m_mainWidth(0),
	m_forceOnTopDelay(10),
	m_bDisableTransparencyOnMouseOver(TRUE),
	m_realTransparency(0),
	m_bIsMouseOver(FALSE)/*,
	m_visibleState(VS_Normal),
	m_additionalHeight(0)*/
{
	m_colors[COL_Bk] = RGB(0,0,0);
	m_colors[COL_Text] = RGB(240,140,80);
	m_borderPen.CreatePen(PS_SOLID, 1, RGB(128,128,128));
	memset(&m_pMenu, 0, sizeof(m_pMenu));

}

CMiniPlayerDlg::~CMiniPlayerDlg()
{
	TRACE(_T("CMiniPlayerDlg::~CMiniPlayerDlg\r\n"));
	ResetAllMenus(TRUE);
	if (m_hWnd) 
		DestroyWindow();
}

//void CMiniPlayerDlg::DoDataExchange(CDataExchange* pDX)
//{
//	CWnd::DoDataExchange(pDX);
//	//DDX_Control(pDX, IDC_INFO, m_Info);
//	//DDX_Control(pDX, IDC_TIME, m_Time);
//}



BEGIN_MESSAGE_MAP(CMiniPlayerDlg, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY (UDM_TOOLTIP_DISPLAY, NULL, OnNotifyDisplayTooltip)
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

const int xMargin = 5;
const int cBtnWid = 26;
const int cSBtnWid = 20;


BOOL CMiniPlayerDlg::Create(CWnd* pParent)
{
	PrgAPI* pAPI = PRGAPI();
	CRect rc(100,100,200,200);

	if (CreateEx(WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, 
		_T("#32770"), 
		NULL, 
		WS_POPUP, 
		0, 0, 500, 500, 
		pParent ? pParent->GetSafeHwnd() : NULL, 
		0, 0))
	{
		SetTransparency(m_transparency);

		//=== INFO
		m_Info.Create(_T("STATIC"), 0, WS_CHILD, rc, this, 100);
		m_Info.SetColor(CLabelEx::COL_Erase, RGB(255,0,255));
		m_Info.SetColor(CLabelEx::COL_Fore, RGB(128,128,128));
		m_Info.SetAutoWidth(TRUE);
		m_Info.SetBorderType(CLabelEx::B_RoundRect, 5, 5);
		m_Info.SetMargins(3, 3);
		m_Info.SetSpecialFont(pAPI->GetFont(FONT_DialogsBold));


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
			m_buttons[i].ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
			m_buttons[i].SetComponents(FlexButton::COMP_Image);
			m_buttons[i].SetIcon(pAPI->GetIcon(Btn2Icon[i]), 24, 24);

		}

		const APP_ICONS SBtn2Icon[] = 
		{
			ICO_Find16,
			ICO_Options16,
			ICO_Restore16
		};


		for (int i = 0; i < SBTN_Last; i++)
		{
			VERIFY(m_systemButtons[i].Create(NULL, WS_VISIBLE | WS_CHILDWINDOW, rc, this, i + ID_SYSTEMBUTTONSID));
			m_systemButtons[i].ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
			m_systemButtons[i].SetComponents(FlexButton::COMP_Image);
			m_systemButtons[i].SetIcon(pAPI->GetIcon(SBtn2Icon[i]), 16, 16);
		}


		m_Time.Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, 200);

		m_tooltip.Create(this, TRUE);
		m_tooltip.AddTool(this);
		m_tooltip.SetBehaviour(PPTOOLTIP_MULTIPLE_SHOW | PPTOOLTIP_DISABLE_AUTOPOP );
		m_tooltip.SetNotify();
		m_tooltip.SetMaxTipWidth(300);
		//m_tooltip.SetDefaultSizes(TRUE);
		//m_tooltip.SetDelayTime(PPTOOLTIP_TIME_INITIAL, 1000);
		//m_tooltip.SetDelayTime(PPTOOLTIP_TIME_FADEIN, 25);
		//m_tooltip.SetTransparency(20);
		m_tooltip.SetCssStyles(_T("body {font-size: 10pt; color:black; font-family:Tahoma}")
			_T("h1 {font-size: 20pt; color:#1010a0; font-family:Tahoma; font-weight:bold}")
			_T("h2 {font-size: 14pt; color:black; font-family:Tahoma; font-weight:bold}"));


		m_positionSlider.Create(this, WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this, 210);
		m_volumeSlider.Create(this, WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this, 211);
		m_volumeSlider.SetPos(pAPI->GetMediaPlayer()->GetVolume());
		m_volumeSlider.SetMaxPos(100);
		//m_mute.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, ID_BUTTONSID + 50);




		ApplyTranslation(*pAPI->GetTranslationManager());
		ApplySkin(pAPI->GetSkinManager()->GetSkin());
		UpdateInfo(FALSE);
		Serialize(TRUE);
		SetTimer(TMR_Update, 1000, NULL);
		ResizeControl();

		if (pAPI->GetOption(OPT_MINIPLR_ForceOnTop))
		{
			INT delayTimer = pAPI->GetOption(OPT_MINIPLR_ForceOnTopDelay);
			if (delayTimer < 1)
				delayTimer = 1;
			else if (delayTimer > 1000)
				delayTimer = 1000;
			SetTimer(TMR_ForceOnTop, delayTimer * 1000, NULL); 
		}
		//SetTimer(TIMER_MOUSEOVER, 200, NULL);
		return TRUE;
	}
	else
	{
		DWORD dwLastError = GetLastError();
		TRACE(_T("QuickSearchDlg::Create Failed\r\n"));
	}
	return FALSE;
}

void CMiniPlayerDlg::Show()
{
	SetWindowPos(&CWnd::wndTopMost, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);
}

void CMiniPlayerDlg::Hide()
{
	ShowWindow(SW_HIDE);
}

BOOL CMiniPlayerDlg::IsMouseOver()
{
	CPoint p;
	GetCursorPos(&p);
	HWND wfp = ::WindowFromPoint(p);
	HWND owner = ::GetAncestor(wfp, GA_ROOTOWNER);
	return owner == m_hWnd;
}


void CMiniPlayerDlg::SetForceOnTop(BOOL bEnable)
{
	m_bForceOnTop = bEnable;
	if (bEnable)
		SetTimer(TMR_ForceOnTop, m_forceOnTopDelay * 1000, NULL);
	else
		KillTimer(TMR_ForceOnTop);
}

void CMiniPlayerDlg::SetForceOnTopDelay(UINT ms)
{
	m_forceOnTopDelay = ms;
	if (m_bForceOnTop)
	{
		KillTimer(TMR_ForceOnTop);
		SetTimer(TMR_ForceOnTop, m_forceOnTopDelay * 1000, NULL);	
	}

}

void CMiniPlayerDlg::ApplyTranslation(ITranslation& translation)
{
	ResetAllMenus(FALSE);
}

void CMiniPlayerDlg::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	PrgAPI* pAPI = PRGAPI();
	m_Info.SetFont(skin.GetFont(CSEC_MiniPlayer, FSTY_Normal));
	m_Info.SetSpecialFont(skin.GetFont(CSEC_MiniPlayer, FSTY_NormalBold));
	m_Time.SetFont(skin.GetFont(CSEC_MiniPlayer, FSTY_Normal));

	SetColor(COL_Bk, skin.GetColor(CSEC_MiniPlayer, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_MiniPlayer, CVAL_Text));
	//SetMaxSize(skin.GetIntParameter(CSEC_MiniPlayer, IPARAM_Width));
	SetHeight(skin.GetIntParameter(CSEC_MiniPlayer, IPARAM_Height));
}

void CMiniPlayerDlg::SetHeight(INT height)
{
	m_height = height;
}

void CMiniPlayerDlg::ResizeControl()
{
	int hei = m_height - 2 * cMargin;
	int curx = xMargin;//Startup
	for (int i = 0; i < BTN_Last; i++)
	{
		CRect rc(curx, cMargin, curx+cBtnWid, cMargin+hei);
		m_buttons[i].MoveWindow(&rc, TRUE);
		curx += cBtnWid + 1;
	}
	curx += 2;
	CSize sz;
	m_Time.CalculateSize(_T("000:00"), 6, sz);
	CRect rc;
	m_Time.MoveWindow(curx, cMargin, sz.cx, hei, TRUE);
	curx += sz.cx;
	curx += 2;

	for (int i = 0; i < SBTN_Last; i++)
	{
		CRect rc(curx, cMargin, curx+cSBtnWid, cMargin+hei);
		m_systemButtons[i].MoveWindow(&rc, TRUE);
		curx += cSBtnWid + 1;
	}
	curx += 1;
	m_mainWidth = curx + 2;

	INT volumeWidth = (m_mainWidth - 3 * xMargin) * 2 / 9;
	m_volumeSlider.MoveWindow(xMargin, m_height, volumeWidth, cPositionSliderHeight, TRUE);
	m_positionSlider.MoveWindow(2 * xMargin + volumeWidth, m_height, m_mainWidth - 3 * xMargin - volumeWidth, cPositionSliderHeight, TRUE);

	const INT cControlDistance = 3;
	m_Info.MoveWindow(m_mainWidth + cControlDistance, 0, m_maxSize - 2, m_height, FALSE);
	SetWindowPos(&CWnd::wndTopMost, 0, 0, curx + m_maxSize + cControlDistance, m_height, SWP_NOMOVE|SWP_NOACTIVATE);	//TopMost
}


void CMiniPlayerDlg::SetMaxSize(INT maxSize)
{
	m_maxSize = maxSize;
	ResizeControl();
}



BOOL CMiniPlayerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CMiniPlayerDlg::OnCommand"), wParam);
	BOOL bHandled = FALSE;
	PrgAPI* pAPI = PRGAPI();
	ActionManager* pAM = pAPI->GetActionManager();
	MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
	MenuManager* pMM = pAPI->GetMenuManager();
	if (pPlayer != NULL)
	{
		bHandled = TRUE;
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
		case ID_SYSTEMBUTTONSID + SBTN_Find:
			ShowQuickSearch(_T(""));
			break;
		case ID_SYSTEMBUTTONSID + SBTN_Menu:
			{
				CRect rc;
				GetDlgItem(wParam)->GetWindowRect(&rc);
				rc.left += rc.Width() / 2;
				rc.top += rc.Height() / 2;
				OnContextMenu(this, rc.TopLeft());
			}
			break;
		case ID_SYSTEMBUTTONSID + SBTN_Restore:
			pAM->ShowMainWindow();
			break;
		//case ID_BUTTONSID + 50:
		//	pPlayer->SetMute(!pPlayer->GetMute());
		//	break;
		default:
			bHandled = FALSE;
			break;
		}

	}
	if (!bHandled)
	{
		switch (wParam)
		{
		case MI_PlayAlbum:
			{
				INT pos = pPlayer->GetPlayListPos();
				MediaPlayListItem mpli;
				if (pos != -1 && pPlayer->GetPlayListItem(mpli, pos))
				{
					FullTrackRecordSP rec;
					SQLManager* pSM = pAPI->GetSQLManager();
					if (pSM->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						TracksFilter tf;
						tf.AlbumID.val = rec->track.albID;
						tf.AlbumID.match = NUMM_Exact;
						FullTrackRecordCollection col;
						if (pSM->GetFullTrackRecordCollectionByTracksFilter(col, tf))
						{
							SortOptionCollection scol;
							scol.ApplySortOption(SortOption(RSO_TrackNo, TRUE));
							scol.ApplySortOption(SortOption(RSO_Location, TRUE));
							RecordCollectionSorter::SortFullTrackRecordCollection(col, scol);
							pAM->Play(pPlayer, col);
						}
					}

				}
			}
			break;
		case MI_OpenContainingFolder:
			{
				INT pos = pPlayer->GetPlayListPos();
				MediaPlayListItem mpli;
				if (pos != -1 && pPlayer->GetPlayListItem(mpli, pos))
					pAM->OpenFolderToItem(mpli.url);
			}
			break;
		case MI_LocateAlbum:
			{
				INT pos = pPlayer->GetPlayListPos();
				MediaPlayListItem mpli;
				if (pPlayer->GetPlayListItem(mpli, pos))
				{
					SQLManager* pSM = pAPI->GetSQLManager();
					FullTrackRecordSP rec;
					if (pSM->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						pAPI->GetStateManager()->GetState().locateTrackID = rec->track.ID;
						pAPI->SendMessage(SM_LocateTrackRequest);
						pAM->ShowMainWindow();
					}
				}
			}
			break;
		case MI_Hide:
			Hide();
			break;
		case MI_ShowMainWindow:
			pAM->ShowMainWindow();
			break;
		case MI_SetTransparency:
			{
				if (m_transparencyDlg.GetSafeHwnd() == NULL)
				{
					m_transparencyDlg.SetListener(this);
					CPoint p;
					GetCursorPos(&p);
					INT cx = 300;
					INT cy = 46;
					m_transparencyDlg.Create(pAPI->GetString(IDS_TRANSPARENCY), 0, 255, m_transparency, p.x - cx / 2, p.y - cy / 2, cx, cy, this);
					m_transparencyDlg.ShowWindow(SW_SHOW);
				}
			}
			break;
		case MI_ForceOnTop:
			SetForceOnTop(!GetForceOnTop());
			break;
		case MI_ShowTooltips:
			EnableTooltips(!IsToolTipsEnabled());
			break;
		case MI_ShowExtraInfo:
			EnableExtraInfo(!IsExtraInfoEnabled());
			break;
		case MI_DisableTransparencyOnMouseOver:
			SetDisableTransparencyOnMouseOver(!GetDisableTransparencyOnMouseOver());
			break;
		default:
			{
				MenuCommandsEnum cmd = (MenuCommandsEnum) wParam;
				if (pMM->HandlePlayerCommands(*pPlayer, cmd, pPlayer->GetPlayListPos()) || 
					pMM->HandleGeneralCommands(cmd))
					m_LastMediaLength = 0;
				else
					CWnd::OnCommand(wParam, lParam);
			}
			break;
		}

	}
	return TRUE;
}

void CMiniPlayerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

}



void CMiniPlayerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseCaptured = TRUE;
	SetCapture();
	m_ptLButtonDown = point;
	CWnd::OnLButtonDown(nFlags, point);
}

void CMiniPlayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bMouseCaptured)
	{
		::ReleaseCapture();
		m_bMouseCaptured = FALSE;
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CMiniPlayerDlg::SetTransparency(BYTE trans)
{
	m_transparency = trans;
	SetRealTransparency(trans);
}

void CMiniPlayerDlg::SetRealTransparency(BYTE trans)
{
	if (m_realTransparency != trans)
	{
		m_realTransparency = trans;
		if (m_hWnd)
			SetLayeredWindowAttributes(RGB(255,0,255), m_realTransparency, LWA_COLORKEY|LWA_ALPHA);
	}
}

void CMiniPlayerDlg::EnableTooltips(BOOL bEnable)	
{
	m_bTooltips = bEnable;
}

void CMiniPlayerDlg::EnableExtraInfo(BOOL bEnable)	
{
	if (m_bExtraInfo != bEnable)
	{
		m_bExtraInfo = bEnable;
		m_LastMediaLength = -1;
	}
}

INT CMiniPlayerDlg::GetExpandedHeight()		
{
	return m_height + cPositionSliderHeight + 2 * cMargin;
}



void CMiniPlayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bIsMouseOver == FALSE)
	{
		m_bIsMouseOver = TRUE;
		if (m_bDisableTransparencyOnMouseOver)
			SetRealTransparency(255);
		SetTimer(TMR_MouseOver, 200, NULL);
		KillTimer(TMR_ResetShape);
		CRect rc;
		GetWindowRect(&rc);
		if (rc.Height() != GetExpandedHeight())
			SetWindowPos(NULL, 0, 0, rc.Width(), GetExpandedHeight(), SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);


		//switch (m_visibleState)
		//{
		//case VS_Normal:
		//	m_visibleState = VS_Expanding;
		//	SetTimer(TMR_ChangingVisibleState, 40, NULL);
		//	break;
		//case VS_Expanding:
		//	break;
		//case VS_Expanded:
		//	break;
		//case VS_Shrinking:
		//	m_visibleState = VS_Expanding;
		//	break;
		//}
	}

	if (m_bMouseCaptured)
	{
		const INT dockDistance = 12;
		CRect rectDlg;
		GetWindowRect(&rectDlg);
		int w = rectDlg.Width();
		int h = rectDlg.Height();
		rectDlg.left  += point.x - m_ptLButtonDown.x;
		INT screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);

		//CRect mainRect(rectDlg);
		//mainRect.right = mainRect.left;
		HMONITOR hMonitor = MonitorFromRect(rectDlg, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		//if (rectDlg.left < dockDistance)
		//	rectDlg.left = 0;
		//else if (rectDlg.left > screenWidth - m_ctrlWidth - dockDistance)
		//	rectDlg.left = screenWidth - m_ctrlWidth;
		//else if (mi.rcWork.right != screenWidth )

		if (abs(mi.rcWork.left - rectDlg.left) < dockDistance)
			rectDlg.left = mi.rcWork.left;
		else if (abs(mi.rcWork.right - rectDlg.left) < dockDistance)
			rectDlg.left = mi.rcWork.right;
		else if (rectDlg.left < dockDistance)
			rectDlg.left = 0;
		else if (rectDlg.left > screenWidth - dockDistance)
			rectDlg.left = screenWidth;


		rectDlg.right  = rectDlg.left + w;
		rectDlg.top   += point.y - m_ptLButtonDown.y;
		//if (rectDlg.top < dockDistance)
		//	rectDlg.top = 0;
		if (rectDlg.top - mi.rcWork.top < dockDistance)
			rectDlg.top = mi.rcWork.top;
		else if (mi.rcWork.bottom - rectDlg.top - h < dockDistance)
			rectDlg.top = mi.rcWork.bottom - h;


		rectDlg.bottom = rectDlg.top + h;

		MoveWindow(&rectDlg);
	}
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CMiniPlayerDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);
	//CBrush brush(RGB(255,0,255));
	pDC->FillSolidRect(&rect, RGB(255,0,255));



	rect.right = rect.left + m_mainWidth;
	if (m_bkBrush.m_hObject == 0)
		m_bkBrush.CreateSolidBrush(m_colors[COL_Bk]);
	CBrush* pOldBrush = pDC->SelectObject(&m_bkBrush);
	CPen* pOldPen = pDC->SelectObject(&m_borderPen);
	pDC->RoundRect(&rect, CPoint(5,5));
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
//	m_Picture.Show(pDC, CPoint(0,0), CPoint(m_Picture.m_Width, m_Picture.m_Height), 0, 0);
	return TRUE;//CDialog::OnEraseBkgnd(pDC);
}

void CMiniPlayerDlg::Serialize(BOOL load)
{
	AppSettings* params = PRGAPI()->GetAppSettings();
	if (load)
	{
		INT x,y;
		params->Read(SettingsCategory, _T("LEFT"), (int&) x, 400);
		params->Read(SettingsCategory, _T("TOP"), (int&) y, 0);
		SetWindowPos(&CWnd::wndTopMost, x,y,0,0,SWP_NOSIZE | SWP_NOACTIVATE);
	}
	else
	{
		CRect rc;
		GetWindowRect(&rc);
		params->Write(SettingsCategory, _T("LEFT"), (int&) rc.left);
		params->Write(SettingsCategory, _T("TOP"), (int&) rc.top);
	}
}


void CMiniPlayerDlg::OnDestroy()
{
	Serialize(FALSE);
	__super::OnDestroy();

}

void CMiniPlayerDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	MenuManager* pMM = PRGAPI()->GetMenuManager();
	MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();

	ITSMenu& main = GetInternalMenu(M_Main);

	main.CheckMenuItem(MI_ForceOnTop, GetForceOnTop());
	main.CheckMenuItem(MI_ShowTooltips, IsToolTipsEnabled());
	main.CheckMenuItem(MI_ShowExtraInfo, IsExtraInfoEnabled());
	main.CheckMenuItem(MI_DisableTransparencyOnMouseOver, GetDisableTransparencyOnMouseOver());

	pMM->PreparePlayerCommands(*pPlayer);
	main.TrackPopupMenu(point.x, point.y, this->GetSafeHwnd());
}

void CMiniPlayerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//ASSERT(0);//TODO
	//AfxGetMainWnd()->ShowWindow(SW_RESTORE);
	AfxGetMainWnd()->SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	//PRGAPI()->SendMessage(SM_ShowMainWindow);
	__super::OnLButtonDblClk(nFlags, point);
}

void CMiniPlayerDlg::UpdateInfo(BOOL bForce)
{
	PrgAPI* pAPI = PRGAPI();
	MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
	TCHAR newText[300];
	INT curIdx = pPlayer->GetPlayListPos();
	MediaPlayListItem mpli;
	if (curIdx >= 0 && pPlayer->GetPlayListItem(mpli, curIdx))
	{
		DOUBLE mediaLength = pPlayer->GetMediaLength();
		if (mpli.artist == NULL) mpli.artist = pAPI->GetString(IDS_UNKNOWN);
		if (mpli.title == NULL) mpli.title = mpli.url;
		if (mediaLength == 0.0)
			mediaLength = (DOUBLE)mpli.length;
		if (m_LastMediaLength != mediaLength || bForce)
		{
			m_LastMediaLength = mediaLength;
			m_TrackInfo.Format(_T("<s>%s</s> - %s"), mpli.title, mpli.artist);
			if (m_bExtraInfo)
			{
				PrgAPI* pAPI = PRGAPI();
				//SYSTEMTIME stl, stf;
				//UINT rank, hits;
				SQLManager::HistoryTrackStats stats;
				SQLManager* pSM = pAPI->GetSQLManager();
				if (pSM->GetHistoryTrackStats(mpli.artist, mpli.title,0,0,stats))
				{
					//if (rank < 100 && hits > 0)
					//{
					//	TCHAR bf[100];
					//	_sntprintf(bf, 100, _T(" [%s :%d]"), PRGAPI()->GetString(IDS_RANK), rank);
					//	m_TrackInfo += bf; 
					//}
				}
				if (stats.rank > 0)
				{
					TCHAR bf[20];
					_sntprintf(bf, 20, _T(" (#%d)"), stats.rank);
					m_TrackInfo += bf;
				}
				FullTrackRecordSP rec;
				if (pSM->GetFullTrackRecordByLocation(rec, mpli.url))
				{
					m_Info.SetIconProperties(16, 16);
					m_Info.ClearIcons();
					m_Info.AddIcon(pAPI->GetIconForTrackType(rec->track.trackType));
					m_iconInfo[m_Info.GetIconCount() - 1].Format(_T("<b>%s</b>: %s"), pAPI->GetString(IDS_TYPE), pAPI->GetStringForTrackType(rec->track.trackType));
					FLOAT fStars = Rating2Stars(rec->track.rating);
					if (fStars > 0.0 && fStars <= 1.0f)
						m_Info.AddIcon(pAPI->GetIcon(ICO_StarBad16));
					while (fStars > 1.0f)
					{
						fStars -= 1.0f;
						m_Info.AddIcon(pAPI->GetIcon(fStars >= .5f ? ICO_StarGold16 : ICO_StarGoldHalf16));
					}
					if (pAPI->IsTrackNew(rec->track.dateAdded))
					{
						m_Info.AddIcon(pAPI->GetIcon(ICO_New16));
						TCHAR bf[100];
						pAPI->FormatDate(bf, 100, rec->track.dateAdded);
						m_iconInfo[m_Info.GetIconCount() - 1].Format(_T("<b>%s</b>: %s"), pAPI->GetString(IDS_DATEADDED), bf);
					}
					if (stats.rank > 0 && stats.rank <= 100)
					{
						m_Info.AddIcon(pAPI->GetIconForRank(stats.rank));
						m_iconInfo[m_Info.GetIconCount() - 1].Format(_T("<b>%s</b>: %d<br><b>%s</b>: %d"), 
							pAPI->GetString(IDS_RANK), stats.rank,
							pAPI->GetString(IDS_HITS), stats.hits);
					}
					InfoItemTypeEnum iits[] = {IIT_TrackComment, IIT_TrackLyrics, IIT_TrackPersonal, IIT_TrackTablatures, IIT_Unknown};
					InfoRecord ir;
					INT i = 0;
					while (iits[i] != IIT_Unknown)
					{
						if (pSM->GetTrackInfoRecord(rec->track.ID, iits[i], ir))
						{
							m_Info.AddIcon(pAPI->GetIconForInfoItemType(iits[i]));
							m_iconInfo[m_Info.GetIconCount() - 1].Format(_T("<b>%s</b><br>%s"), 
								pAPI->GetStringForInfoItemType(iits[i]), 
								ir.name.c_str());
						}
						i++;
					}
				}

			}
			m_Info.Invalidate(TRUE);
			m_Info.ShowWindow(SW_SHOW);
			//=== Make it stay on top
			//SetWindowPos(&CWnd::wndTopMost, 0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOREPOSITION|SWP_SHOWWINDOW);
		}
		if (!m_bSpecialMessage)
		{
			//_sntprintf(newText, 300, _T("%s - %s"), pMi->artistName.c_str(), pMi->trackName.c_str());
			TCHAR bf[300];
			m_Info.GetWindowText(bf, 300);
			if (m_TrackInfo != bf)
				m_Info.SetWindowText(m_TrackInfo);
		}
		INT secsToFinish = INT(pPlayer->GetMediaLength() - pPlayer->GetMediaPos());
		if (secsToFinish > 0)
		{
			_sntprintf(newText, 300, _T("%0.2d:%0.2d"), INT(secsToFinish / 60), secsToFinish % 60);
			m_Time.SetWindowText(newText);
			if (m_positionSlider.GetMaxPos() != (INT)(pPlayer->GetMediaLength() * 1000))
				m_positionSlider.SetMaxPos((INT)(pPlayer->GetMediaLength() * 1000));
			if (m_positionSlider.GetPos() != (INT)(pPlayer->GetMediaPos() * 1000))
				m_positionSlider.SetPos(INT(pPlayer->GetMediaPos() * 1000));
		}
		else
		{
			m_Time.SetWindowText(_T(""));
			m_positionSlider.SetPos(0);
			m_positionSlider.SetMaxPos(0);
		}
	}
	else
	{
		m_Info.ShowWindow(SW_HIDE);
		//m_Info.SetWindowText(_T(""));
		m_Time.SetWindowText(_T(""));
		m_LastMediaLength = -1.0;
	}

}

void CMiniPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TMR_SpecialMessage:
		KillTimer(TMR_SpecialMessage);
		m_bSpecialMessage = FALSE;
		break;
	case TMR_Update:
		if (m_Info.m_hWnd)
			UpdateInfo(FALSE);
		break;
	case TMR_ForceOnTop:
		SetWindowPos(&CWnd::wndTopMost, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		break;
	case TMR_MouseOver:
		if (IsMouseOver() == FALSE)
		{
			KillTimer(TMR_MouseOver);
			m_bIsMouseOver = FALSE;
			SetTimer(TMR_ResetShape, 1000, NULL);
		}
		break;
	case TMR_ResetShape:
		{
			KillTimer(TMR_ResetShape);
			CRect rc;
			GetWindowRect(&rc);
			if (rc.Height() != m_height)
				SetWindowPos(NULL, 0, 0, rc.Width(), m_height, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
			if (m_bDisableTransparencyOnMouseOver)
				SetRealTransparency(m_transparency);
			//m_visibleState = VS_Shrinking;
			//SetTimer(TMR_ChangingVisibleState, 40, NULL);
		}
		break;
	//case TMR_ChangingVisibleState:
	//	{
	//		INT step = 5;
	//		switch (m_visibleState)
	//		{
	//		case VS_Expanding:
	//			m_additionalHeight += step;
	//			break;
	//		case VS_Shrinking:
	//			m_additionalHeight -= step;
	//			break;
	//		default:
	//			ASSERT(0);
	//		}
	//		INT newHeight = m_additionalHeight + m_height;
	//		if (newHeight >= GetExpandedHeight())
	//		{
	//			m_additionalHeight = GetExpandedHeight() - m_height;
	//			m_visibleState = VS_Expanded;
	//			KillTimer(TMR_ChangingVisibleState);
	//		}
	//		else if (newHeight <= m_height)
	//		{
	//			m_additionalHeight = 0;
	//			m_visibleState = VS_Normal;
	//			KillTimer(TMR_ChangingVisibleState);
	//		}
	//		CRect rc;
	//		GetWindowRect(&rc);
	//		SetWindowPos(NULL, 0, 0, rc.Width(), m_height + m_additionalHeight, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	//	}
	//	break;
	default:
		CWnd::OnTimer(nIDEvent);
	}
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CMiniPlayerDlg::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (m_hWnd != NULL)
		{
			if (idx == COL_Bk)
			{
				if (m_bkBrush.m_hObject)
					m_bkBrush.DeleteObject();
				m_Info.SetColor(CLabelEx::COL_Bk, value);
				m_Time.SetColor(CLabel::COL_Bk, value);
				for (int i = 0; i < BTN_Last; i++)
					m_buttons[i].SetColor(FlexButton::COL_Bk, value);
				for (int i = 0; i < SBTN_Last; i++)
					m_systemButtons[i].SetColor(FlexButton::COL_Bk, value);
				m_positionSlider.SetColor(CMediaSlider::COL_Bk, value);
				m_volumeSlider.SetColor(CMediaSlider::COL_Bk, value);
			}
			else if (idx==COL_Text)
			{
				m_Info.SetColor(CLabelEx::COL_Text, value);
				m_Time.SetColor(CLabel::COL_Text, value);
			}
		}
		if (m_hWnd)		
			Invalidate();

	}

}
COLORREF CMiniPlayerDlg::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

BOOL CMiniPlayerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	const INT diff = 3;
	MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
	INT newVol = pPlayer->GetVolume() + (zDelta > 0 ? diff : -diff);
	if (newVol < 0)
		newVol = 0;
	else if (newVol > 100)
		newVol = 100;

	pPlayer->SetVolume(newVol);
	PRGAPI()->SendMessage(SM_PlayerVolumeChanged);
	m_bSpecialMessage = TRUE;
	TCHAR bf[200];
	_sntprintf(bf, 200, _T("%s: %d%%"), PRGAPI()->GetString(IDS_VOLUME), newVol);
	m_Info.SetWindowText(bf);
	KillTimer(TMR_SpecialMessage);
	SetTimer(TMR_SpecialMessage, ID_SPECIALMSG_TIMER_DELAY, NULL);
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CMiniPlayerDlg::OnStateChanged(UINT stateMessages)
{
	BOOL bHandled = FALSE;
	switch (stateMessages)
	{
	case SM_TrackUpdated:
		UpdateInfo(TRUE);
		break;
	case SM_PlayerVolumeChanged:
		{
			PrgAPI* pAPI = PRGAPI();
			INT volume =  pAPI->GetMediaPlayer()->GetVolume();
			bHandled = TRUE;
			m_bSpecialMessage = TRUE;
			TCHAR bf[200];
			_sntprintf(bf, 200, _T("%s: %d%%"), pAPI->GetString(IDS_VOLUME), volume);
			m_Info.SetWindowText(bf);
			KillTimer(TMR_SpecialMessage);
			SetTimer(TMR_SpecialMessage, ID_SPECIALMSG_TIMER_DELAY, NULL);
			m_volumeSlider.SetPos(volume);



		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}


void CMiniPlayerDlg::OnNotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result)
{

	*result = 0;
	if (!m_bTooltips)
		return ;
	NM_PPTOOLTIP_DISPLAY * pNotify = (NM_PPTOOLTIP_DISPLAY*)pNMHDR;

	if (NULL == pNotify->hwndTool)
	{
		//Order to update a tooltip for a current Tooltip Help
		//He has not a handle of the window
		//If you want change tooltip's parameter than make it's here
	}
	else
	{
		PrgAPI* pAPI = PRGAPI();
		MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
		INT curIdx = pPlayer->GetPlayListPos();
		if (curIdx != -1)
		{
			CPoint curPoint;
			GetCursorPos(&curPoint);
			ScreenToClient(&curPoint);
			HWND curWnd = ::ChildWindowFromPoint(m_hWnd, curPoint);
			if (curWnd == m_Info.m_hWnd)
			{
				CPoint pt = *pNotify->pt;
				CRect rect, rcCtrl;
				GetWindowRect(&rcCtrl);
				pt -= rcCtrl.TopLeft();
				INT iconIdx = m_Info.GetIconIndexFromPoint(pt);
				if (iconIdx == -1)
				{
					MediaPlayListItem mpli;
					if (pPlayer->GetPlayListItem(mpli, curIdx))
						pAPI->GetActionManager()->GetToolTipText(mpli.url, pNotify->ti->sTooltip);
				}
				else
					pNotify->ti->sTooltip = m_iconInfo[iconIdx];
			}

		}
	}
}

void CMiniPlayerDlg::ShowQuickSearch(LPCTSTR searchString)
{
	QuickSearchDlg* pQS = PRGAPI()->GetQuickSearchDlg();
	pQS->SetSearchText(searchString);
	pQS->ShowRelative(this, QuickSearchDlg::RP_Outside);
	//PRGAPI()->GetQuickSearchDlg(searchString, rc.left, rc.top, bDownwards);

};

BOOL CMiniPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);
	switch (pMsg->message)
	{
	case WM_CHAR:
		{
			TCHAR bf[2];
			bf[0] = pMsg->wParam;
			switch (bf[0])
			{
			case 8:
			case 9:
			case 13:
			case 10:
			case 27://=== ESC
				break;
			default:
				bf[1] = 0;
				ShowQuickSearch(bf);
				return TRUE;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			MediaPlayer* pMP = PRGAPI()->GetMediaPlayer();
			switch (pMsg->wParam)
			{
			case VK_UP:
				pMP->Previous();
				return TRUE;
			case VK_DOWN:
				pMP->Next();
				return TRUE;
			case VK_LEFT:
				pMP->SetMediaPos(pMP->GetMediaPos() - 5.0);
				UpdateInfo(FALSE);
				return TRUE;
			case VK_RIGHT:
				pMP->SetMediaPos(pMP->GetMediaPos() + 5.0);
				UpdateInfo(FALSE);
				return TRUE;
			case 'F':
				if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				{
					CRect rc;
					GetWindowRect(&rc);
					rc.left += rc.Width() / 2 - 150;
					rc.top += rc.Height() / 2 - 150;
					ShowQuickSearch(_T(""));
					return TRUE;
				}
			}
		}
		break;
	}

	return __super::PreTranslateMessage(pMsg);
}

ITSMenu& CMiniPlayerDlg::GetInternalMenu(MenusEnum mnu)
{
	TRACEST(_T("CPlayListListCtrl::GetInternalMenu."), mnu);
	ASSERT(mnu < M_Last);
	PrgAPI* pAPI = PRGAPI();
	MenuManager* pMM = pAPI->GetMenuManager();
	if (m_pMenu[mnu] == NULL)
		m_pMenu[mnu] = pAPI->CreatePopupMenu();
	ITSMenu& curMenu = *m_pMenu[mnu];
	if (curMenu.GetInternalHandler() != NULL)
		return curMenu;
	curMenu.Create();
	switch (mnu)
	{
	case M_Main:
		{
			TCHAR bf[500];
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_LocateAlbum, (LPTSTR)pAPI->GetString(IDS_LOCATEALBUM), pAPI->GetIcon(ICO_Find16));
			_sntprintf(bf, 500, _T("%s: %s"), pAPI->GetString(IDS_PLAY), pAPI->GetString(IDS_ALBUM));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_PlayAlbum, bf);
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_OpenContainingFolder, (LPTSTR)pAPI->GetString(IDS_OPENCONTAININGFOLDER));
			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)pMM->GetMenu(MenuManager::MNU_PlayListModes).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_MODE));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)pMM->GetMenu(MenuManager::MNU_Rating).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_RATING), pAPI->GetIcon(ICO_StarGold16));
			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetInternalMenu(M_Settings).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_OPTIONS));
			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_ShowMainWindow, (LPTSTR)pAPI->GetString(IDS_SHOWMAINWINDOW));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_Hide, (LPTSTR)pAPI->GetString(IDS_HIDE));
			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_String, MENU_Exit, (LPTSTR)pAPI->GetString(IDS_EXIT));
		}
		break;
	case M_Settings:
		{
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_ForceOnTop, (LPTSTR)pAPI->GetString(IDS_FORCEONTOP));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_ShowTooltips, (LPTSTR)pAPI->GetString(IDS_TOOLTIPS));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_ShowExtraInfo, (LPTSTR)pAPI->GetString(IDS_EXTRAINFO));
			//curMenu.AppendMenu(ITSMenu::MIT_String, MI_MaxSize, (LPTSTR)pAPI->GetString(IDS_MAXSIZE));
			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_SetTransparency, (LPTSTR)pAPI->GetString(IDS_TRANSPARENCY));
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_DisableTransparencyOnMouseOver, (LPTSTR)pAPI->GetString(IDS_DISABLEONMOUSEOVER));
		}
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}

void CMiniPlayerDlg::ResetAllMenus(BOOL bDelete)
{
	for (int i = 0; i < M_Last; i++)
	{
		if (m_pMenu[i] != NULL)
		{
			m_pMenu[i]->Destroy();
			if (bDelete)
			{
				delete m_pMenu[i];
				m_pMenu[i] = NULL;
			}
		}
	}
}

void CMiniPlayerDlg::OnMediaSliderClick(CMediaSlider* pMediaSlider)
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

void CMiniPlayerDlg::SaveOptions()
{
	PrgAPI* pAPI = PRGAPI();
	pAPI->SetOption(OPT_MINIPLR_Transparency, GetTransparency());
	pAPI->SetOption(OPT_MINIPLR_ShowTooltips, IsToolTipsEnabled());
	pAPI->SetOption(OPT_MINIPLR_ShowExtraInfo, IsExtraInfoEnabled());
	pAPI->SetOption(OPT_MINIPLR_MaxSize, GetMaxSize());
	pAPI->SetOption(OPT_MINIPLR_ForceOnTop, GetForceOnTop());
	pAPI->SetOption(OPT_MINIPLR_ForceOnTopDelay, GetForceOnTopDelay());
	pAPI->SetOption(OPT_MINIPLR_DisableTransparencyOnMouseOver, GetDisableTransparencyOnMouseOver());
}

void CMiniPlayerDlg::LoadOptions()
{
	PrgAPI* pAPI = PRGAPI();
	SetTransparency(pAPI->GetOption(OPT_MINIPLR_Transparency));
	EnableTooltips(pAPI->GetOption(OPT_MINIPLR_ShowTooltips));
	EnableExtraInfo(pAPI->GetOption(OPT_MINIPLR_ShowExtraInfo));
	SetMaxSize(pAPI->GetOption(OPT_MINIPLR_MaxSize));
	SetForceOnTop(pAPI->GetOption(OPT_MINIPLR_ForceOnTop));
	SetForceOnTopDelay(pAPI->GetOption(OPT_MINIPLR_ForceOnTopDelay));
	SetDisableTransparencyOnMouseOver(pAPI->GetOption(OPT_MINIPLR_DisableTransparencyOnMouseOver));
}



