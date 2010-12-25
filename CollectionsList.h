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
#ifndef _CCollectionsList_h_
#define _CCollectionsList_h_

#include "DataRecords.h"
#include "ICtrlColors.h"
#include "FlexListCtrl.h"
#include "ITSMenu.h"

class CCollectionsList : public CFlexListCtrl,public ICtrlColors
{
public:
	static const INT MsgIDLeftClick = WM_USER + 122;
	CCollectionsList();           // protected constructor used by dynamic creation
	virtual ~CCollectionsList();

	void UpdateControl();
	void GetSelectedRows(std::vector<UINT>& selectedItems);
	const CollectionRecordCollection& GetCollection()	{return m_CRC;}
	//void SetItemHeight(INT height)
	//{
	//	m_imageList.Create( height, height, ILC_COLOR4, 10, 10 ); 
	//	SetImageList( &m_imageList, LVSIL_SMALL );
	//	m_height = height;
	//}
	//INT GetItemHeight()
	//{
	//	return m_height;
	//}

	//---------------------------------------------------
	//ICtrlColors ---------------------------------------
	//#include "ICtrlColors.h" // ,public ICtrlColors
	//---------------------------------------------------
public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_TextBk,
		COL_TextBkAlt,
		COL_TextWorking,
		COL_TextSel,
		COL_TextSelBk,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	virtual LPCTSTR GetColorName(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
protected:
	virtual void DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC);
	virtual void DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r);

private:
	//void AddColumns();
	void InitMenus();

private:
	//std::vector<ColumnsEnum> m_columns;
	//BCMenu m_MainMenu;
	ITSMenu* m_pMainMenu;
	CollectionRecordCollection m_CRC;

	Gdiplus::Font* m_pMainTextFont;
	Gdiplus::Font* m_pTitleTextFont;

	Gdiplus::SolidBrush* m_pBkBrush;
	Gdiplus::SolidBrush* m_pTextBrush;
	Gdiplus::SolidBrush* m_pTextBkBrush;
	Gdiplus::SolidBrush* m_pTextSelBrush;
	Gdiplus::SolidBrush* m_pTextSelBkBrush;
	Gdiplus::SolidBrush* m_pTextAltBkBrush;
	Gdiplus::SolidBrush* m_pTextWorkingBrush;
	Gdiplus::Pen* m_pBorderPen;

	Gdiplus::StringFormat* m_pMainStringFormat;
	std::map<CollectionTypesEnum, Gdiplus::Bitmap*> m_pBitmaps;
};


#endif
