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
#ifndef _TrackListCtrl_h_
#define _TrackListCtrl_h_ 

#include "TSSkinnable.h"
#include "TSTranslatable.h"
#include "FlexListCtrl.h"
#include "TSConfigurable.h"
#include "DataRecords.h"

class MediaPlayer;
class ITSMenu;

class TrackListCtrl : public CFlexListCtrl
{
public:
	TrackListCtrl();
	virtual ~TrackListCtrl();

public:
	void SetFullTrackRecordCollection(FullTrackRecordCollection* pCollection)
	{
		m_pCollection = pCollection;
	}
	void Update();

	void SetMediaPlayer(MediaPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}

	void OnSelectAll();
	void OnInvertSelection();
	virtual void ApplyTranslation(ITranslation& skin);
	virtual void ApplySkin(ISkin& skin);

	//enum ConfigurationOptions
	//{
	//	C_ShowNumbers,
	//	C_Last
	//};
	//void SetConfigurationOption(ConfigurationOptions idx, INT value);
	//INT GetConfigurationOption(ConfigurationOptions idx);

public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_TextSel,
		COL_TextSelBk,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	//virtual LPCTSTR GetColorName(UINT idx);

	//=== TSConfigurable
	//virtual void UpdateConfiguration();
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

protected://Overrides (DelegatedOleDropTarget)
	//virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	//virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	//virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );

protected:
	virtual void DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC);
	virtual void DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r);
	virtual BOOL ItemAcceptsFileDrop(CPoint& p, INT nItem)	{return TRUE;}
	virtual void OnItemMouseEvent(const ItemMouseEvent& ev);
	virtual void PostCreate();

protected:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	//BOOL ReadHdropData(COleDataObject* pDataObject, CPoint* point = NULL);
	BOOL SwapRows(UINT row1,UINT row2);
private:
	MediaPlayer* m_pPlayer;
	CFont* m_pBoldFont;
	CFont* m_pNormalFont;
//	INT m_configOptions[C_Last];

	enum MenuItemsEnum
	{
		MI_First = 0x3000,
		MI_Play,
		MI_Enqueue,
		MI_InsertAfter,
		MI_LocateAlbum,
		MI_Last
	};
	enum MenusEnum
	{
		M_Main,
		M_Last
	};
	ITSMenu* m_pMenu[M_Last];
private:
	INT m_menuInitiator;
	FullTrackRecordCollection* m_pCollection;
	ITSMenu& GetInternalMenu(MenusEnum mnu);
	void ResetAllMenus(BOOL bDelete);
	BOOL m_bDrawPictures;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#endif
