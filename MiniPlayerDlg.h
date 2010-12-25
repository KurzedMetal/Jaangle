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

#include "FlexButton.h"
#include "Label.h"
#include "LabelEx.h"
#include "HTMLLite/PPTooltip.h"
#include "TSTranslatable.h"
#include "TSSkinnable.h"
#include "StateManager.h"
#include "SliderDlg.h"
#include "MediaSlider.h"

class ITSMenu;
class MediaPlayer;

class CMiniPlayerDlg : public CWnd, 
	public TSSkinnable, public TSTranslatable, public TSStateChangeListener, 
	public SliderDlgListener, public MediaSliderListener
{
	DECLARE_DYNAMIC(CMiniPlayerDlg)

public:
	CMiniPlayerDlg();   // standard constructor
	virtual ~CMiniPlayerDlg();

	BOOL Create(CWnd* pParent);
	//void SetTSDlg(CTeenSpiritDlg* pTSDlg)	{ASSERT(pTSDlg); m_pTSDlg = pTSDlg;}
	//void SetPlayer(MediaPlayer* pPlayer)	{ASSERT(pPlayer); m_pPlayer = pPlayer;}

	void Show();
	void Hide();

	void SaveOptions();
	void LoadOptions();

	void SetTransparency(BYTE trans);
	BYTE GetTransparency()				{return m_transparency;}

	void EnableTooltips(BOOL bEnable);
	BOOL IsToolTipsEnabled()			{return m_bTooltips;}

	void EnableExtraInfo(BOOL bEnable);
	BOOL IsExtraInfoEnabled()			{return m_bExtraInfo;}


	void SetMaxSize(INT maxSize);
	BOOL GetMaxSize()					{return m_maxSize;}

	void SetHeight(INT height);
	INT GetHeight()						{return m_height;}

	void SetForceOnTop(BOOL bEnable);
	BOOL GetForceOnTop()				{return m_bForceOnTop;}

	void SetForceOnTopDelay(UINT ms);
	UINT GetForceOnTopDelay()			{return m_forceOnTopDelay;}

	void SetDisableTransparencyOnMouseOver(BOOL bEnable)	{m_bDisableTransparencyOnMouseOver = bEnable;}
	BOOL GetDisableTransparencyOnMouseOver()				{return m_bDisableTransparencyOnMouseOver;}


	virtual void ApplyTranslation(ITranslation& translation);
	virtual void ApplySkin(ISkin& skin);
	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()				{return _T("CMiniPlayerDlg");}

	virtual void OnChangeSlider(INT newValue)			{SetTransparency(newValue);}
	virtual void OnMediaSliderClick(CMediaSlider* pMediaSlider);



// Dialog Data
	//enum { IDD = IDD_MINIPLAYER };

	//---------------------------------------------------
	//ICtrlColors ---------------------------------------
	//#include "ICtrlColors.h" // ,public ICtrlColors
	//---------------------------------------------------
public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	void Serialize(BOOL load);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	DECLARE_MESSAGE_MAP()
private:
	void OnNotifyDisplayTooltip(NMHDR * pNMHDR, LRESULT * result);
	void ResizeControl();
	void UpdateInfo(BOOL bForce);
	BOOL IsMouseOver();
	void SetRealTransparency(BYTE trans);

private:

	enum Timers
	{
		TMR_Update = 600,
		TMR_SpecialMessage,
		TMR_ForceOnTop,
		TMR_MouseOver,
		TMR_ResetShape,
		//TMR_ChangingVisibleState,
		TMR_Last
	};

	BOOL m_bTooltips;
	BOOL m_bExtraInfo;
	BOOL m_bMouseCaptured;
	CPoint m_ptLButtonDown;
	TCHAR m_Title[100];
	TCHAR m_Artist[100];
	DOUBLE m_totalSec;
	DOUBLE m_curSec;
	INT m_maxSize;
	INT m_height;
	//INT m_additionalHeight;
	INT GetExpandedHeight();
	BOOL m_bForceOnTop;
	UINT m_forceOnTopDelay;

	enum PlayButtons
	{
		BTN_Prev,
		BTN_Play,
		BTN_Pause,
		BTN_Stop,
		BTN_Next,
		BTN_Last
	};
	enum SystemButtons
	{
		SBTN_Find,
		SBTN_Menu,
		SBTN_Restore,
		SBTN_Last
	};	
	
	enum IconPlayButtons
	{
		IBTN_Type,
		IBTN_New,
		IBTN_Rank,
		IBTN_Rating,
		IBTN_Personal,
		IBTN_Lyrics,
		IBTN_Comment,
		IBTN_Last
	};




	FlexButton m_buttons[BTN_Last];
	FlexButton m_systemButtons[SBTN_Last];
	//FlexButton m_mute;
	CLabel m_Time;
	CLabelEx m_Info;
	static LPCTSTR SettingsCategory;
	//MediaPlayer* m_pPlayer;
	CBrush m_bkBrush;
	CPen m_borderPen;
	//CFont m_font;
	BYTE m_transparency;
	BYTE m_realTransparency;
	BOOL m_bSpecialMessage;
	DOUBLE m_LastMediaLength;
	CString m_TrackInfo;
	INT m_mainWidth;
	//INT m_ctrlHeight;
	CPPToolTip m_tooltip;
	CString m_iconInfo[10];
	CMediaSlider m_positionSlider;
	CMediaSlider m_volumeSlider;


	enum MenuItemsEnum
	{
		MI_First = 0x3000,
		MI_PlayAlbum,
		MI_OpenContainingFolder,
		MI_LocateAlbum,
		MI_ShowMainWindow,
		MI_Hide,
		
		MI_ForceOnTop,
		MI_SetTransparency,
		MI_ShowTooltips,
		MI_ShowExtraInfo,
		//MI_MaxSize,
		MI_DisableTransparencyOnMouseOver,

		MI_Last
	};


	enum MenusEnum
	{
		M_Main,
		M_Settings,
		M_Last
	};


	//enum VisibleState
	//{
	//	VS_Normal,
	//	VS_Expanding,
	//	VS_Shrinking,
	//	VS_Expanded,
	//	VS_Last
	//};
	//VisibleState m_visibleState;

	ITSMenu* m_pMenu[M_Last];
	ITSMenu& GetInternalMenu(MenusEnum mnu);
	void ResetAllMenus(BOOL bDelete);
	void ShowQuickSearch(LPCTSTR searchString);
	SliderDlg m_transparencyDlg;
	BOOL m_bDisableTransparencyOnMouseOver;
	BOOL m_bIsMouseOver;


};
