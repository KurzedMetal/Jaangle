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
#include "PlayerBarWnd.h"
#include "PrgAPI.h"
#include "ActionManager.h"
#include "MediaPlayer.h"
#include "InfoDownloadManager.h"
#include "CollectionManager.h"
#include "MenuManager.h"

//#define TIMER_MSGDELAY 100
#define TIMER_CHECKSTATUS 200
#define TIMER_CHECKSTATUS_DELAY 1000
#define TIMER_CHECKLEVEL 205
#define TIMER_CHECKLEVEL_DELAY 50

#define IDC_SLD_EQUALIZER 500




//IMPLEMENT_DYNAMIC(CPlayerBarWnd, CDialog)

const int cMargins = 1;
const int cBtnWidth = 34;
//const int cBtnHeight = 34;
const int cVolumeWidth = 70;
const int cEQSliderWidth = 8;
const int cLevelWidth = 8;
const int cSmallBtnWidth = 16;
const int cSliderHeight = 18;
const int cSeparator = 8;
//const int cTimeInfoWidth = 80;//m_timeInfo.CalculateWidth() + 5;
//const int cFullTimeInfoWidth = 40;

CPlayerBarWnd::CPlayerBarWnd(CWnd* pParent /*=NULL*/):
	m_pPlayer(NULL)
{
}

CPlayerBarWnd::~CPlayerBarWnd()
{
}

BEGIN_MESSAGE_MAP(CPlayerBarWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
//	ON_MESSAGE(WM_MEDIASLIDER_MSG, OnSliderMessage)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPlayerBarWnd message handlers

void CPlayerBarWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

BOOL CPlayerBarWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CWnd::Create(_T("STATIC"), NULL, dwStyle, rect, pParentWnd, nID) == FALSE)
		return FALSE;

	PrgAPI* pAPI = PRGAPI();

	m_previous.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Previous);
	m_play.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Play);
	m_pause.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Pause);
	m_stop.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Stop);
	m_next.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Next);

	m_mute.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Mute);

	m_crossfade.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_CrossFade);
	m_equalizer.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Equalizer);

	m_positionSlider.Create(this, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_Slider);
	m_positionSlider.SetTickInterval(60 * 1000);
	m_volumeSlider.Create(this, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_SldVolume);
	m_volMeterL.Create(WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_VolMeterL);
	m_volMeterR.Create(WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_VolMeterR);

	m_timeInfo.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_TimeInfo);
	m_fullTimeInfo.Create(NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_FullTimeInfo);
	m_playerInfo.Create(_T("STATIC"), NULL, WS_CHILDWINDOW | WS_VISIBLE, CRect(0,0,0,0), this, CTRL_PlayerInfo);


	//m_start.ApplyTypeStyle(FlexButton::BT_PushButton);
	//m_start.SetFrameStyle(FlexButton::FRS_Hot);
	//m_start.SetIcon(pAPI->GetIcon(ICO_Main), 48, 48);
	//m_start.SetTextStyle(FlexButton::TS_None);
	//m_start.SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);

	ConfigureButton(m_previous, pAPI->GetIcon(ICO_Prev24));
	ConfigureButton(m_play, pAPI->GetIcon(ICO_Play24));
	ConfigureButton(m_pause, pAPI->GetIcon(ICO_Pause24));
	ConfigureButton(m_stop, pAPI->GetIcon(ICO_Stop24));
	ConfigureButton(m_next, pAPI->GetIcon(ICO_Next24));


	m_mute.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	m_mute.SetImageDrawEffect(FlexButton::ST_Checked, FlexButton::IDE_ShowCheckedIcon);
	m_mute.SetFrameDrawEffect(FlexButton::ST_Checked, FlexButton::FDE_None);
	m_mute.SetFrameDrawEffect(FlexButton::ST_Over, FlexButton::FDE_Normal);
	//m_mute.SetCheckable(TRUE);
	m_mute.SetIcon(pAPI->GetIcon(ICO_Nomute16), 16, 16);
	//m_mute.SetCheckIcon(pAPI->GetIcon(ICO_Mute16));
	m_mute.SetComponents(FlexButton::COMP_Image);
	//m_mute.SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);


	m_crossfade.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	m_crossfade.SetImageDrawEffect(FlexButton::ST_Checked, FlexButton::IDE_ShowCheckedIcon);
	m_crossfade.SetFrameDrawEffect(FlexButton::ST_Checked, FlexButton::FDE_None);
	//m_crossfade.SetCheckable(TRUE);
	m_crossfade.SetIcon(pAPI->GetIcon(ICO_Crossfade16), 16, 16);
	//m_crossfade.SetCheckIcon(pAPI->GetIcon(ICO_CrossfadeAc16));
	m_crossfade.SetComponents(FlexButton::COMP_Image);
	//m_crossfade.SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);

	m_equalizer.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	m_equalizer.SetImageDrawEffect(FlexButton::ST_Checked, FlexButton::IDE_ShowCheckedIcon);
	m_equalizer.SetFrameDrawEffect(FlexButton::ST_Checked, FlexButton::FDE_None);
	//m_equalizer.SetCheckable(TRUE);
	m_equalizer.SetIcon(pAPI->GetIcon(ICO_Equalizer16), 16, 16);
	//m_equalizer.SetCheckIcon(pAPI->GetIcon(ICO_EqualizerAc16));
	m_equalizer.SetComponents(FlexButton::COMP_Image);
	//m_equalizer.SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);

	m_tooltip.Create(this, FALSE);
	m_tooltip.SetMaxTipWidth(400);

	for (int i = 0; i < cSldEqCount; i++)
	{
		m_sldEq[i].Create(this, WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_SLD_EQUALIZER + i);
		m_sldEq[i].SetCursorWidth(1);
		m_sldEq[i].SetHorizontal(FALSE);
	}




	//m_NetUsageText.SetOptions(CLabelEx::DO_AutoWidth);
	//CRect rc(0,0,0,0);


	//SetColor(COL_Bk, RGB(10,10,10));
	//SetColor(COL_Text, RGB(200,200,220));
	SetTimer(TIMER_CHECKSTATUS, TIMER_CHECKSTATUS_DELAY, NULL);
	SetTimer(TIMER_CHECKLEVEL, TIMER_CHECKLEVEL_DELAY, NULL);
	ApplyTranslation(*pAPI->GetTranslationManager());
	ApplySkin(pAPI->GetSkinManager()->GetSkin());
	//m_volumeSliderColorsAdapter.ConfigAdapter(&m_volumeSlider);
	//m_positionSliderColorsAdapter.ConfigAdapter(&m_positionSlider);
	//pAPI->RegisterColorControl(&m_volumeSliderColorsAdapter, _T("VolumeSlider"));
	//pAPI->RegisterColorControl(&m_positionSliderColorsAdapter, _T("PositionSlider"));
	m_volumeSlider.SetMaxPos(100);


	m_volMeterL.SetMaxMin(100, 0);
	m_volMeterL.ShowLines(FALSE);
	m_volMeterL.ShowBorder(FALSE);
	m_volMeterR.SetMaxMin(100, 0);
	m_volMeterR.ShowLines(FALSE);
	m_volMeterR.ShowBorder(FALSE);

	m_playerInfo.SetIconProperties(16, 16);

	SetTimeInfo(0,0);

	m_timeInfo.SetDrawStringParams(m_timeInfo.GetDrawStringParams() | DT_RIGHT);
	m_fullTimeInfo.SetDrawStringParams(m_fullTimeInfo.GetDrawStringParams() | DT_RIGHT);
	OnStateChanged(SM_PlayerVolumeChanged);
	OnStateChanged(SM_PlayerSettingsChanged);
	
	return TRUE;
}



void CPlayerBarWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	ResizePlayerControls();
}

BOOL CPlayerBarWnd::OnStateChanged(UINT stateMessage)
{
	if (m_pPlayer == NULL)
		return FALSE;
	BOOL bHandled = TRUE;
	switch (stateMessage)
	{
	case SM_PlayerVolumeChanged:
		if (m_volumeSlider.m_hWnd != 0)
		{
			PrgAPI* pAPI = PRGAPI();
			m_volumeSlider.SetPos(m_pPlayer->GetVolume());
			m_volumeSlider.Invalidate(FALSE);
			m_mute.SetIcon(pAPI->GetIcon(m_pPlayer->GetMute() ? ICO_Mute16 : ICO_Nomute16), 16, 16);
			//m_mute.SetCheck(pMP->GetMute());
			m_mute.Invalidate(TRUE);
		}
		break;
	case SM_PlayerSettingsChanged:
		if (m_volumeSlider.m_hWnd != 0)
		{
			PrgAPI* pAPI = PRGAPI();
			m_crossfade.SetIcon(
				pAPI->GetIcon((m_pPlayer->GetEngineConfig(MECO_Crossfade) == MEEM_Disabled) ? ICO_Crossfade16 : ICO_CrossfadeAc16), 16, 16);
			m_equalizer.SetIcon(
				pAPI->GetIcon((m_pPlayer->GetEngineConfig(MECO_EqualizerMode) == MEEM_Disabled) ? ICO_Equalizer16 : ICO_EqualizerAc16), 16, 16);
			//m_crossfade.SetCheck(!pMP->GetEngineConfig(MECO_Crossfade) == 0);
			//m_equalizer.SetCheck(!pMP->GetEngineConfig(MECO_EqualizerMode) == MEEM_Disabled);
			for (INT i = 0; i < cSldEqCount; i++)
			{
				m_sldEq[i].SetPos(100 - m_pPlayer->GetEngineConfig(MediaEngineConfigOption(MECO_EqualizerBandStart + i)));
			}
			ResizePlayerControls();
			Invalidate(TRUE);
		}
	default:
		bHandled = FALSE;
	}
	return bHandled;
}

void CPlayerBarWnd::ResizePlayerControls()
{
	if (m_play.m_hWnd == NULL)
		return;
	CRect rc;
	GetClientRect(&rc);
	rc.left = /*48 + 2 * */cMargins;
	//=== Both Lines - Buttons at the end
	INT lastPartWidth = cSeparator + cLevelWidth + cMargins + cLevelWidth + cSeparator +  2 * (cSmallBtnWidth + cMargins);
	if (m_pPlayer && m_pPlayer->GetEngineConfig(MECO_EqualizerMode) != 0)
		lastPartWidth += (-cMargins) + cSeparator + cSldEqCount * (1 + cEQSliderWidth) + cMargins;
	INT lastPartStartX = rc.right - lastPartWidth;

	INT curx = lastPartStartX + cSeparator;
	INT cury = rc.top + cMargins;
	m_volMeterL.MoveWindow(curx, cury + cMargins, cLevelWidth, rc.Height() - 4 * cMargins, FALSE);
	curx += cLevelWidth + cMargins;
	m_volMeterR.MoveWindow(curx, cury + cMargins, cLevelWidth, rc.Height() - 4 * cMargins, FALSE);
	curx += cLevelWidth + cSeparator;

	m_crossfade.MoveWindow(curx, cury, cSmallBtnWidth, cSmallBtnWidth, FALSE);
	curx += (cSmallBtnWidth + cMargins);
	m_equalizer.MoveWindow(curx, cury, cSmallBtnWidth, cSmallBtnWidth, FALSE);
	curx += (cSmallBtnWidth + cMargins);

	for (int i = 0; i < cSldEqCount; i++)
	{
		m_sldEq[i].MoveWindow(curx, cury, cEQSliderWidth, rc.Height() - 2 * cMargins, FALSE);
		curx += cEQSliderWidth;
	}


	//=== First Line - Buttons
	INT firstLineHeight = rc.Height() - 3 * cMargins - cSliderHeight;
	curx = rc.left;
	cury = cMargins;
	m_previous.MoveWindow(curx, cury, cBtnWidth, firstLineHeight, FALSE);
	curx += (cBtnWidth + cMargins);
	m_play.MoveWindow(curx, cury, cBtnWidth, firstLineHeight, FALSE);
	curx += (cBtnWidth + cMargins);
	m_pause.MoveWindow(curx, cury, cBtnWidth, firstLineHeight, FALSE);
	curx += (cBtnWidth + cMargins);
	m_stop.MoveWindow(curx, cury, cBtnWidth, firstLineHeight, FALSE);
	curx += (cBtnWidth + cMargins);
	m_next.MoveWindow(curx, cury, cBtnWidth, firstLineHeight, FALSE);
	curx += cBtnWidth;
	curx += cSeparator;
	//=== First Line - Track Info
	SIZE sz;
	m_timeInfo.CalculateSize(_T("0:00:00"), 7, sz);
	int cTimeInfoWidth = sz.cx;
	m_fullTimeInfo.CalculateSize(_T("0:00:00"), 7, sz);
	int cFullTimeInfoWidth = sz.cx;

	INT infoHeight = rc.Height() - cSliderHeight - 3 * cMargins;
	INT infoWidth = lastPartStartX - curx - cMargins - cTimeInfoWidth;
	m_playerInfo.MoveWindow(curx, cury, infoWidth, infoHeight, FALSE);
	//=== First Line - Time Info
	curx += infoWidth + cMargins;
	m_timeInfo.MoveWindow(curx, cury, cTimeInfoWidth, infoHeight, FALSE);


	//=== Second Line - Volume
	curx = rc.left + cMargins;
	cury = rc.bottom - cMargins - cSliderHeight;

	m_volumeSlider.MoveWindow(curx, cury, cVolumeWidth, cSliderHeight, FALSE);
	curx += cVolumeWidth + cMargins;
	m_mute.MoveWindow(curx, cury, cSliderHeight, cSliderHeight, FALSE);
	curx += cSliderHeight + cSeparator;

	//=== Second Line - Slider
	INT positionSliderWidth = lastPartStartX - curx - 2 * cMargins - cFullTimeInfoWidth;
	m_positionSlider.MoveWindow(curx, cury, positionSliderWidth, cSliderHeight, FALSE);

	//=== Second Line - cFullTimeInfo
	curx += positionSliderWidth + cMargins;
	m_fullTimeInfo.MoveWindow(curx, cury, cFullTimeInfoWidth, cSliderHeight, FALSE);
	Invalidate();


}

void CPlayerBarWnd::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;

	m_timeInfo.SetFont(skin.GetFont(CSEC_PlayerBar, FSTY_Time));
	m_fullTimeInfo.SetFont(skin.GetFont(CSEC_PlayerBar, FSTY_Length));
	m_playerInfo.SetFont(skin.GetFont(CSEC_PlayerBar, FSTY_Normal));
	m_playerInfo.SetSpecialFont(skin.GetFont(CSEC_PlayerBar, FSTY_NormalBold));

	SetColor(COL_Bk, skin.GetColor(CSEC_PlayerBar, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_PlayerBar, CVAL_Text));
	SetColor(COL_Slider1, skin.GetColor(CSEC_PlayerBar, CVAL_Slider1));
	SetColor(COL_Slider2, skin.GetColor(CSEC_PlayerBar, CVAL_Slider2));
	ResizePlayerControls();

}


void CPlayerBarWnd::ApplyTranslation(ITranslation& translation)
{
	PrgAPI* pAPI = PRGAPI();
	if (m_tooltip.m_hWnd)
	{
		LPCTSTR szKeyNumPad = pAPI->GetString(IDS_KEYNUMPAD);
		if (m_tooltip.GetToolCount() == 0)
		{
			m_tooltip.AddTool(&m_crossfade, _T(""));
			m_tooltip.AddTool(&m_equalizer, _T(""));
			TCHAR bf[1000];
			_sntprintf(bf, 1000, _T("%s (+:[%s+] / -:[%s-])"),
				pAPI->GetString(IDS_VOLUME), szKeyNumPad, szKeyNumPad);
			m_tooltip.AddTool(&m_volumeSlider, _T(""));
			m_tooltip.AddTool(&m_previous, _T(""));
			m_tooltip.AddTool(&m_play, _T(""));
			m_tooltip.AddTool(&m_pause, _T(""));
			m_tooltip.AddTool(&m_stop, _T(""));
			m_tooltip.AddTool(&m_next, _T(""));
			m_tooltip.AddTool(&m_positionSlider, _T(""));
			m_tooltip.AddTool(&m_mute, _T(""));
		}
		//m_tooltip.RemoveAllTools();
		m_tooltip.UpdateTipText(pAPI->GetString(IDS_CROSSFADE), &m_crossfade);
		m_tooltip.UpdateTipText(pAPI->GetString(IDS_EQUALIZER), &m_equalizer);
		m_tooltip.UpdateTipText(pAPI->GetString(IDS_MUTE), &m_mute);
		TCHAR bf[1000];
		_sntprintf(bf, 1000, _T("%s\r\n+ : [%s+]\r\n- : [%s-]"),
			pAPI->GetString(IDS_VOLUME), szKeyNumPad, szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_volumeSlider);
		_sntprintf(bf, 1000, _T("%s [%s8]"), pAPI->GetString(IDS_PREVIOUS),	szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_previous);
		_sntprintf(bf, 1000, _T("%s [%s5]"), pAPI->GetString(IDS_PLAY),	szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_play);
		_sntprintf(bf, 1000, _T("%s [%s5]"), pAPI->GetString(IDS_PAUSE),	szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_pause);
		_sntprintf(bf, 1000, _T("%s [%s0]"), pAPI->GetString(IDS_STOP),	szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_stop);
		_sntprintf(bf, 1000, _T("%s [%s2]"), pAPI->GetString(IDS_NEXT),	szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_next);
		_sntprintf(bf, 1000, _T("%s\r\n5s << : [%s4]\r\n5s >> : [%s6])"), 
			pAPI->GetString(IDS_TIMESLIDER), szKeyNumPad, szKeyNumPad);
		m_tooltip.UpdateTipText(bf, &m_positionSlider);

	}
	//m_timeInfo.SetFont(pAPI->GetFont(FONT_PlayerInfo));
	//m_fullTimeInfo.SetFont(pAPI->GetFont(FONT_Big));
	//m_playerInfo.SetFont(pAPI->GetFont(FONT_PlayerInfo));
	//m_playerInfo.SetSpecialFont(pAPI->GetFont(FONT_PlayerInfoBold));


	Invalidate(TRUE);
}


void CPlayerBarWnd::SetTimeInfo(DOUBLE pos, DOUBLE total)
{
	if (total > 0.0)
	{
		INT m1 = INT(pos / 60);
		INT s1 = INT(pos - m1 * 60);

		INT m2 = INT(total / 60);
		INT s2 = INT(total - m2 * 60);
		TCHAR bf[100];
		_sntprintf(bf, 100, _T("%0.2d:%0.2d"), m1, s1);
		UpdateWindowText(m_timeInfo, bf);
		_sntprintf(bf, 100, _T("%0.2d:%0.2d"), m2, s2);
		UpdateWindowText(m_fullTimeInfo, bf);
	}
	else
	{
		UpdateWindowText(m_timeInfo, _T(""));
		UpdateWindowText(m_fullTimeInfo, _T(""));
	}
}

void CPlayerBarWnd::UpdateWindowText(CWnd& wnd, LPCTSTR text)
{
	TCHAR wndText[100];
	wnd.GetWindowText(wndText, 100);
	if (_tcscmp(text, wndText) != 0)
		wnd.SetWindowText(text);
}

void CPlayerBarWnd::ConfigureButton(FlexButton& btn, HICON hIcon)
{
	btn.SetComponents(FlexButton::COMP_Image);
	btn.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	btn.SetIcon(hIcon, 24, 24);
	//btn.SetColor(FlexButton::COL_Bk, m_colors[COL_Bk]);
}

void CPlayerBarWnd::SetMediaPlayer(MediaPlayer* pPlayer)
{
	ASSERT(pPlayer != NULL);
	m_pPlayer = pPlayer;
	m_volumeSlider.SetPos(m_pPlayer->GetVolume());
	//m_Slider.SetMediaPlayer(pPlayer);
	//m_Volume.SetMediaPlayer(pPlayer);
	//m_Volume.Init(pPlayer->GetPlayerEngine(), 0, 100);
}


void CPlayerBarWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	//case TIMER_MSGDELAY:
	//	KillTimer(nIDEvent);
	//	m_InfoText.SetIcon(NULL, 0 ,0);
	//	m_InfoText.SetWindowText(_T(""));
	//	break;
	case TIMER_CHECKSTATUS:
		Update();
		break;
	case TIMER_CHECKLEVEL:
		if (m_pPlayer != NULL)
		{
			INT lev = m_pPlayer->GetEngineConfig(MECO_VolumeLevel);
			m_volMeterL.SetLevel(LOWORD(lev));
			m_volMeterR.SetLevel(HIWORD(lev));
		}
		break;
	default:
		ASSERT(0);
		__super::OnTimer(nIDEvent);
		break;
	}
}

void CPlayerBarWnd::Update()
{
	PrgAPI* pAPI = PRGAPI();
	if (m_pPlayer != NULL)
	{
		DOUBLE mediaPos = m_pPlayer->GetMediaPos();
		DOUBLE mediaLen = m_pPlayer->GetMediaLength();
		if (m_positionSlider.GetMaxPos() != mediaLen * 1000)
			m_positionSlider.SetMaxPos((INT)(mediaLen * 1000));
		if (m_positionSlider.GetPos() != mediaPos * 1000)
			m_positionSlider.SetPos(INT(mediaPos * 1000));

		//SetTimeInfo(mediaPos, mediaLen);

		if (mediaLen > 0.0)
		{
			TCHAR bf[100];
			INT m1 = INT(mediaPos / 60);
			INT s1 = INT(mediaPos - m1 * 60);
			_sntprintf(bf, 100, _T("%0.2d:%0.2d"), m1, s1);
			UpdateWindowText(m_timeInfo, bf);

			m1 = INT(mediaLen / 60);
			s1 = INT(mediaLen - m1 * 60);
			_sntprintf(bf, 100, _T("%0.2d:%0.2d"), m1, s1);
			//_sntprintf(bf, 100, _T("%0.2d:%0.2d [%d / %d]"), m1, s1, m_pPlayer->GetPlayListPos() + 1, m_pPlayer->GetPlayListCount());
			UpdateWindowText(m_fullTimeInfo, bf);
		}
		else
		{
			UpdateWindowText(m_timeInfo, _T(""));
			UpdateWindowText(m_fullTimeInfo, _T(""));
		}

		
		//if (m_pPlayer->GetEngineConfig(MECO_Crossfade) == 0)
		//{
		//	if (m_crossfade.IsChecked() == TRUE)
		//		m_crossfade.SetCheck(FALSE);
		//}
		//else
		//{
		//	if (m_crossfade.IsChecked() == FALSE)
		//		m_crossfade.SetCheck(TRUE);
		//}

		//if (m_pPlayer->GetEngineConfig(MECO_EqualizerMode) == )





		INT idx = m_pPlayer->GetPlayListPos();
		if (idx != -1)
		{
			MediaPlayListItem mpli;
			if (m_pPlayer->GetPlayListItem(mpli, idx))
			{
				if (mpli.artist == NULL) mpli.artist = pAPI->GetString(IDS_UNKNOWN);
				if (mpli.title == NULL) mpli.title = mpli.url;
				TCHAR bf[1000];
				_sntprintf(bf, 1000, _T("<s>%s</s> - %s"), mpli.title, mpli.artist);
				UpdateWindowText(m_playerInfo, bf);
				//PlayState ps = m_pPlayer->GetPlayState();
				//APP_ICONS icoResource = ICO_Stop24;
				//if (ps == PS_Playing)
				//	icoResource = ICO_Play16;
				//else if (ps == PS_Paused)
				//	icoResource = ICO_Pause24;
				//m_playerInfo.ClearIcons();
				//m_playerInfo.AddIcon(pAPI->GetIcon(icoResource));

			}
		}
		else
		{
			m_playerInfo.SetWindowText(_T(""));
			m_playerInfo.ClearIcons();
		}

	}
}

BOOL CPlayerBarWnd::PreTranslateMessage(MSG* pMsg)
{
	m_tooltip.RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

BOOL CPlayerBarWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CPlayerBarWnd::OnCommand"), wParam);
	if (m_pPlayer == 0 || HIWORD(wParam) != 0)
		return CWnd::OnCommand(wParam, lParam);
	switch (LOWORD(wParam))
	{
	case CTRL_Play:
		m_pPlayer->Start();
		break;
	case CTRL_Pause:
		m_pPlayer->Pause();
		break;
	case CTRL_Stop:
		m_pPlayer->Stop();
		break;
	case CTRL_Previous:
		m_pPlayer->Previous();
		break;
	case CTRL_Next:
		m_pPlayer->Next();
		break;
	case CTRL_Mute:
		m_pPlayer->SetMute(m_pPlayer->GetMute() ? FALSE : TRUE);
		PRGAPI()->GetStateManager()->SendMessage(SM_PlayerVolumeChanged);
		break;
	case CTRL_CrossFade:
		m_pPlayer->SetEngineConfig(MECO_Crossfade, 
			m_pPlayer->GetEngineConfig(MECO_Crossfade) ? 0 : 1);
		PRGAPI()->GetStateManager()->SendMessage(SM_PlayerSettingsChanged);
		break;
	case CTRL_Equalizer:
		m_pPlayer->SetEngineConfig(MECO_EqualizerMode, 
			m_pPlayer->GetEngineConfig(MECO_EqualizerMode) == MEEM_3Channel ? MEEM_Disabled : MEEM_3Channel);
		PRGAPI()->GetStateManager()->SendMessage(SM_PlayerSettingsChanged);
		ResizePlayerControls();
		break;
	//case IDC_START:
	//	{
	//		BCMenu& mnu = PRGAPI()->GetMenuManager()->GetMenu(MenuManager::MNU_StartMenu);
	//		CRect rc;
	//		GetWindowRect(&rc);
	//		mnu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_HORPOSANIMATION | TPM_VERNEGANIMATION,
	//			rc.left, rc.top, this);
	//	}
	//	break;
	default:
		PRGAPI()->GetMenuManager()->HandleGeneralCommands((MenuCommandsEnum)wParam);
	}
	return 0;

}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CPlayerBarWnd::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		if (idx == COL_Bk)
		{
			m_previous.SetColor(FlexButton::COL_Bk, value);
			m_play.SetColor(FlexButton::COL_Bk, value);
			m_pause.SetColor(FlexButton::COL_Bk, value);
			m_stop.SetColor(FlexButton::COL_Bk, value);
			m_next.SetColor(FlexButton::COL_Bk, value);
			m_mute.SetColor(FlexButton::COL_Bk, value);
			m_crossfade.SetColor(FlexButton::COL_Bk, value);
			m_equalizer.SetColor(FlexButton::COL_Bk, value);
			m_volMeterL.SetColor(CLevelMeter::COL_Bk, value);
			m_volMeterR.SetColor(CLevelMeter::COL_Bk, value);
			m_volumeSlider.SetColor(CMediaSlider::COL_Bk, value);
			m_positionSlider.SetColor(CMediaSlider::COL_Bk, value);
			m_timeInfo.SetColor(CLabel::COL_Bk, value);
			m_fullTimeInfo.SetColor(CLabel::COL_Bk, value);
			m_playerInfo.SetColor(CLabelEx::COL_Bk, value);
			for (int i = 0; i < cSldEqCount; i++)
				m_sldEq[i].SetColor(CMediaSlider::COL_Bk, value);

		}
		else if (idx == COL_Text)
		{
			m_timeInfo.SetColor(CLabel::COL_Text, value);
			m_fullTimeInfo.SetColor(CLabel::COL_Text, value);
			m_playerInfo.SetColor(CLabelEx::COL_Text, value);

			m_volMeterL.SetColor(CLevelMeter::COL_ActiveLow, value);
			m_volMeterR.SetColor(CLevelMeter::COL_ActiveLow, value);
			m_volMeterL.SetColor(CLevelMeter::COL_Lines, value);
			m_volMeterR.SetColor(CLevelMeter::COL_Lines, value);
		}
		else if (idx == COL_Slider1)
		{
			m_positionSlider.SetColor(CMediaSlider::COL_FirstPart, value);
			m_volumeSlider.SetColor(CMediaSlider::COL_FirstPart, value);
			for (int i = 0; i < cSldEqCount; i++)
				m_sldEq[i].SetColor(CMediaSlider::COL_SecondPart, value);
		}
		else if (idx == COL_Slider2)
		{
			m_positionSlider.SetColor(CMediaSlider::COL_SecondPart, value);
			m_volumeSlider.SetColor(CMediaSlider::COL_SecondPart, value);
			for (int i = 0; i < cSldEqCount; i++)
				m_sldEq[i].SetColor(CMediaSlider::COL_FirstPart, value);
		}
		m_colors[idx] = value;
		Invalidate(TRUE);
	}
}
COLORREF CPlayerBarWnd::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Bk"),
//	_T("Text"),
//	_T("Slider1"),
//	_T("Slider1"),
//	NULL
//};
//LPCTSTR CPlayerBarWnd::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
BOOL CPlayerBarWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, GetColor(COL_Bk));
	//DrawIconEx(pDC->m_hDC, cMargins, (rc.Height() - 48) - 2, PRGAPI()->GetIcon(ICO_Main), 48, 48, 0, 0, DI_NORMAL);
	return TRUE;//__super::OnEraseBkgnd(pDC);
}

void CPlayerBarWnd::OnMediaSliderClick(CMediaSlider* pMediaSplider)
{
	if (m_pPlayer == NULL)
		return;

	if (pMediaSplider == &m_positionSlider)
	{
		INT sldPos = m_positionSlider.GetPos();
		INT curPos = (INT)m_pPlayer->GetMediaPos();
		if ((sldPos / 1000) != curPos)
			m_pPlayer->SetMediaPos(sldPos / 1000.0);
	} 
	else if (pMediaSplider == &m_volumeSlider)
	{
		INT curVol = m_pPlayer->GetVolume();
		INT sldPos = m_volumeSlider.GetPos();
		if (curVol != sldPos)
		{
			m_pPlayer->SetVolume(sldPos);
			PRGAPI()->GetStateManager()->SendMessage(SM_PlayerVolumeChanged);
		}
	}
	else
	{
		for (INT i = 0; i < cSldEqCount; i++)
		{
			if (pMediaSplider == &m_sldEq[i])
			{
				m_pPlayer->SetEngineConfig(MediaEngineConfigOption(MECO_EqualizerBandStart + i), 
					100 - m_sldEq[i].GetPos());
				break;
			}
		}
	}
}
