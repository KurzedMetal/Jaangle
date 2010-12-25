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
#include "GuiControls/BCMenu.h"
#include "GuiControls/RectAlphaDrawer.h"
#include "TSTranslatable.h"
#include "TSSkinnable.h"
#include "TSConfigurable.h"
#include "TeenSpiritEnums.h"
#include "DataRecords.h"
#include "GuiControls/MultiPicWnd.h"
#include "FlexButton.h"
#include "StateManager.h"
class MediaPlayer;

class CColorEdit;

class CInfoCtrl : public CWnd, 
	public TSSkinnable, public TSTranslatable//, public TSConfigurable
{
	DECLARE_DYNAMIC(CInfoCtrl)

public:
	CInfoCtrl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoCtrl();

	BOOL Create(CWnd* pParentWnd, UINT nID);

	enum TEXTINFO
	{
		//TI_PicturePath = 0,
		TI_PicTitle = 0,
		TI_TextTitle,
		TI_MainText,
		TI_Last
	};
	void SetMediaPlayerMode(BOOL bMPMode)
	{
		m_bMediaPlayerMode = bMPMode;
	}

	BOOL SetActiveItem(FullTrackRecordSP& track);//, ItemTypeEnum desiredItemType, UINT delay = 0);
	//BOOL GetActiveItem(FullTrackRecordSP& track);
	const FullTrackRecordSP& CInfoCtrl::GetActiveItem();

	//FullTrackRecordSP& GetActiveItem();
	void SetStyle(ItemTypeEnum itStyle);
	ItemTypeEnum GetStyle();
	void Refresh(UINT delay = 0);

	enum ImageDisplayMode
	{
		IDM_OnePic,
		IDM_AllPic,
		IDM_AllPicFading,
		IDM_AllPicFadingParts,
		IDM_Last
	};
	void SetImageDisplayMode(ImageDisplayMode idm);
	ImageDisplayMode GetImageDisplayMode()				{return m_idm;}

	void EnableInfoType(InfoItemTypeEnum iit, BOOL bEnable);
	BOOL IsInfoTypeEnabled(InfoItemTypeEnum iit);

	virtual void ApplyTranslation(ITranslation& translation);
	virtual void ApplySkin(ISkin& skin);
	//virtual void UpdateConfiguration();


	const std::vector<InfoItemTypeEnum>& GetShowingItems()	{return m_showingItems;}

	HWND GetWindowForFocus();


	//---------------------------------------------------
	//ICtrlColors ---------------------------------------
	//---------------------------------------------------
public:
	void ActualRefresh();
	enum COLORS
	{
		COL_Bk,
		COL_BkText,
		COL_Text,
		COL_TextTitle,
		COL_Ouline,
		COL_Last
	};
	void SetColor(UINT idx, COLORREF value);
	COLORREF GetColor(UINT idx);
	//LPCTSTR GetColorName(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

protected:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point );

	DECLARE_MESSAGE_MAP()
private:
	BOOL MakeAlbumPage();
	BOOL MakeArtistPage();
	BOOL MakeTrackPage();
	BOOL SetNoInfoPage();
	BOOL MakeGenrePage();
	BOOL MakeYearPage();
	UINT DrawStuff(CDC& dc, CRect& rc, BOOL bCalcHeightOnly = FALSE);
	void SetTextInfo(TEXTINFO ti, LPCTSTR info);
	UINT AssignArtistImages();
	UINT AssignAlbumImages();
	BOOL AssignInfo(InfoItemTypeEnum iit);
	void Redraw();


private:
	//NEW VARS
	FullTrackRecordSP m_curFTRecord;
	ItemTypeEnum m_desiredItemType;
	BOOL m_bRequestArtistImage;
	BOOL m_bRequestAlbumImage;
	BOOL m_bRequestArtistInfo;
	BOOL m_bRequestAlbumInfo;
	BOOL m_InfoTypeEnabled[IIT_Last];

	//ItemTypeEnum m_actualItemType;
	//NEW VARS - END

	std::tstring m_Text[TI_Last];
	CBrush m_VariousItemsBrush;//Version 2.0
	INT m_ExtraTextStyle;
	CMultiPicWnd m_picture;
	BCMenu m_Menu;
	CColorEdit* m_pEdit;

	BOOL m_bMediaPlayerMode;

	CDC m_memDC;
	CBitmap m_memBitmap;
	CBitmap* m_oldBitmap;
	SIZE m_bmpSize;
	RectAlphaDrawer m_AlphaDrawer;
	ImageDisplayMode m_idm;
	CFont* m_pSmallFont;
	CFont* m_pBigFont;
	BOOL m_bNeedsRedraw;

	FlexButton m_button;

	std::vector<InfoItemTypeEnum> m_showingItems;


};
