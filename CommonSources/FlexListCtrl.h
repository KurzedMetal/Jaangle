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


#include "GdiPlusInstance.h"
#include <GdiPlus.h>
#include <string>
#include <vector>
#include "FlexScrollBar.h"
#include "FlexDblBuffer.h"
#include "IFlexDropTarget.h"

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class FlexDropHelper;

class CFlexListCtrl : public CWnd, public IFlexDropTarget
{
	friend class FlexDropTarget;
	DECLARE_DYNAMIC(CFlexListCtrl)

public:
	CFlexListCtrl();
	virtual ~CFlexListCtrl();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	virtual void ClearItems();
	virtual void SwapItem(INT pos1, INT pos2);
	virtual void InsertItems(INT pos, INT itemCount);
	virtual void DeleteItems(INT pos, INT itemCount);
	virtual INT GetItemCount();
	virtual void SetItemCount(INT nItemCount);

	Gdiplus::Rect GetItemRect(INT nItem);
	INT GetItemFromPoint(INT x, INT y);
	INT GetNextVisibleItem(INT nPrevItem = -1);
	BOOL IsItemVisible(INT nItem, BOOL bFullVisibility);
	//BOOL PointInClientArea(INT x, INT y);

	virtual void SetItemSelection(INT nItem, BOOL bSelected);
	virtual INT GetNextSelectedItem(INT nPrevItem = -1);
	virtual BOOL IsItemSelected(INT nItem);
	virtual UINT GetSelectedItemCount();
	virtual void ClearSelection();

	void CenterItem(INT nItem);
	void EnsureVisible(INT nItem, BOOL bFullVisibility);

	INT GetScrollPos();
	void SetScrollPos(INT pos);

	void Refresh();

	FlexScrollbar& GetScrollBar()										{return m_scrollBar;}

	//=== Configuration
	void SetSingleSelection(BOOL bEnable);
	BOOL GetSingleSelection()											{return m_bSingleSelection;}

	void SetItemHeight(UINT itemHeight);
	UINT GetItemHeight()												
	{
		return m_itemHeight;
	}
	void SetSearchStringResetDelay(UINT ms)								{m_searchStringResetDelay = ms;}
	UINT GetSearchStringResetDelay()									{return m_searchStringResetDelay;}
	void SetSearchStringDelay(UINT ms)									{m_searchStringDelay = ms;}
	UINT GetSearchStringDelay()											{return m_searchStringDelay;}

	void EnableDragDrop(BOOL bEnable);
	void IsDragDropEnabled();

	void InvalidateItem(INT nItem);

protected:
	virtual void DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC) = 0;			//Must be overridden
	virtual void DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r);
	virtual void DrawMouseOver(Gdiplus::Graphics& g, INT item);
	virtual void DrawQuickSearchString(Gdiplus::Graphics& g, LPCTSTR searchString)	{}
	virtual INT OnFindItem(LPCTSTR str)									{return -1;}
	virtual void OnSelectionChanged()									{}
	virtual void PostCreate()											{}

	
	enum ItemMouseEventType
	{
		IMET_Unknown,
		IMET_LButtonDown,
		IMET_LButtonUp,
		IMET_LButtonDblClick,
		IMET_Last
	};

	struct ItemMouseEvent
	{
		ItemMouseEvent():activeItem(-1),xClient(0),yClient(0),type(IMET_Unknown){}
		INT activeItem;
		INT xClient;
		INT yClient;
		UINT nFlags;
		ItemMouseEventType type;
	};
	virtual void OnItemMouseEvent(const ItemMouseEvent& ev)	{}


	//====== IFlexDropTarget
	virtual BOOL AcceptsFileDrop(CPoint& p);
	virtual BOOL OnFileDrop(const std::vector<std::tstring>& files, CPoint& p)	{ASSERT(0);return FALSE;}

	virtual BOOL ItemAcceptsFileDrop(CPoint& p, INT nItem)					{return FALSE;}


protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

private:
	void SetMouseOverItem(INT nItem);
	static BOOL RegisterWindowClass();
	inline static Gdiplus::Rect FromRECT(RECT& rc);
	void InvalidateViewRange(INT yPos1, INT length);

private:
	std::vector<UINT> m_invalidatedItems;
	std::set<UINT> m_invalidatedItemsSet;
	std::vector<BOOL> m_selectedItems;
	UINT m_itemCount;
	INT m_selectedItem;
	UINT m_itemHeight;
	GdiPlusInstance inst;
	FlexDblBuffer m_dblBuffer;

	BOOL m_bSingleSelection;
	//RangeValue m_invalidRange;
	INT m_bitmapPos;
	//BOOL m_bMouseCaptured;
	BOOL m_bMouseLButtonIsDown;
	INT m_mouseOverItem;
	std::tstring m_searchString;
	UINT m_searchStringDelay;
	UINT m_searchStringResetDelay;
	FlexScrollbar m_scrollBar;
	FlexDropHelper* m_pDropHelper;
	INT m_scrollBarThickness;
	CPoint m_lastLButtonClick;
	BOOL m_bLockPaintReentry;
	BOOL m_flagsOnMouseDown;
	INT m_nItemOnMouseDown;
	BOOL m_bWindowInvalidated;
public:
	afx_msg void OnNcPaint();
};


