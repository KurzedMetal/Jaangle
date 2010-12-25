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
#include "FlexListCtrl.h"
#include "FlexDropHelper.h"
#include "ReEntryLock.h"

#define FLEXCTRL_CLASSNAME    _T("FlexListCtrl")
using namespace Gdiplus;

#define TIMER_SEARCHSTRING			0x50
#define TIMER_RESETSEARCHSTRING		0x51
#define TIMER_LBUTTONDOWNREPEAT		0x60

#define KEYBOARD_DELAY 500
#define KEYBOARD_SPEED 50




inline LRESULT CALLBACK FlexCtrlProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
}


IMPLEMENT_DYNAMIC(CFlexListCtrl, CWnd)

CFlexListCtrl::CFlexListCtrl():
m_itemHeight(20),
m_itemCount(0),
m_bitmapPos(0),
m_mouseOverItem(-1),
m_searchStringDelay(100),
m_searchStringResetDelay(1000),
m_pDropHelper(0),
m_bSingleSelection(TRUE),
m_selectedItem(-1),
m_scrollBarThickness(16),
m_bLockPaintReentry(FALSE),
m_flagsOnMouseDown(0),
m_nItemOnMouseDown(-1),
m_bMouseLButtonIsDown(FALSE)
{
	RegisterWindowClass();
	m_scrollBar.SetParent(this);
	m_scrollBar.SetScrollStep(m_itemHeight);
}

CFlexListCtrl::~CFlexListCtrl()
{
	delete m_pDropHelper;
}


BEGIN_MESSAGE_MAP(CFlexListCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_NCPAINT()
END_MESSAGE_MAP()


BOOL CFlexListCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, FLEXCTRL_CLASSNAME, &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_GLOBALCLASS;// CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;// FlexCtrlProcedure;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = NULL;
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = FLEXCTRL_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CFlexListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CWnd::Create(FLEXCTRL_CLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID) == TRUE)
	{
		PostCreate();
		return TRUE;
	}
	return FALSE;;
}

void SlideBitmap(Bitmap& bmp, INT bmpWidth, INT bmpHeight, INT slide)
{
	ASSERT(slide != 0 && bmpWidth != 0 && bmpHeight != 0);
	if (slide == 0 || slide >= bmpHeight || slide <= -bmpHeight || bmpWidth == 0)
		return;
	BitmapData bmpData;
	Rect rc(0,0,bmpWidth,bmpHeight);
	Status st = bmp.LockBits(&rc, 
		ImageLockModeRead | ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);
	if (st == Ok)
	{
		int stride = bmpData.Stride;
		UINT* pixels = (UINT*)bmpData.Scan0;
		if (slide > 0)
			memmove(&pixels[slide * (bmpData.Stride / 4)], &pixels[0],  bmpData.Stride * (bmpHeight - slide));
		else
			memmove(&pixels[0], &pixels[-slide * bmpData.Stride / 4],  bmpData.Stride * (bmpHeight + slide));
		bmp.UnlockBits(&bmpData);
	}
	else
	{
		TRACE(_T("@1 SlideBitmap. LockBits Failed\r\n"));
		ASSERT(0);
	}
}

void CFlexListCtrl::InvalidateViewRange(INT yPos1, INT length)
{
	INT nFirstItem = GetItemFromPoint(0,yPos1);
	if (nFirstItem != -1)
	{
		INT nLastItem = GetItemFromPoint(0, yPos1 + length);
		if (nLastItem == -1)
			nLastItem = m_itemCount - 1;
		for (int i = nFirstItem; i <= nLastItem; i++)
			InvalidateItem(i);
	}
}
BOOL CFlexListCtrl::OnEraseBkgnd(CDC* pDC)									
{
	//CRect rc;
	//GetClientRect(&rc);
	//pDC->FillSolidRect(&rc,RGB(255,0,0));
	return TRUE;
}


void CFlexListCtrl::OnPaint()
{
	CPaintDC _dc(this); // device context for painting
	//_dc.FillSolidRect(0,0,100,100,RGB(255,0,0));
	ReEntryLock lock(m_bLockPaintReentry);
	if (lock.WasAlreadyLocked())
	{
		TRACE(_T("@1 CFlexListCtrl::OnPaint Reentry\r\n"));
		return;
	}
	CRect _rc;
	GetClientRect(&_rc);
	//Convert to GDI+ Parameters
	Graphics g(_dc);
	Rect rc = FromRECT(_rc);
	if (m_scrollBar.HasScroll())
		rc.Width -= m_scrollBarThickness;
	Graphics* pdblg = m_dblBuffer.GetGraphics(rc.Width, rc.Height);
	if (pdblg == NULL)
	{
		TRACE(_T("@0 CFlexListCtrl::OnPaint Can't get dbl buffer\r\n\r\n"));
		return;
	}
	Graphics& dblg = *pdblg;
	if (m_bitmapPos != m_scrollBar.GetScrollPos())
	{
		INT slide = m_bitmapPos - m_scrollBar.GetScrollPos();
		if (abs(slide) < rc.Height)
		{
			if (slide > 0)
				InvalidateViewRange(0, slide);
			else
				InvalidateViewRange(rc.Height + slide, -slide);
			SlideBitmap(*m_dblBuffer.GetBitmap(), rc.Width, rc.Height, slide);
		}
		else
			InvalidateViewRange(0, rc.Height);
		m_bitmapPos = m_scrollBar.GetScrollPos();
	}
	if (m_bWindowInvalidated)
	{
		if (m_itemCount > 0)
		{
			std::set<UINT>::iterator it = m_invalidatedItemsSet.begin();
			for (; it!=m_invalidatedItemsSet.end(); it++)
			{
				if (*it < m_itemCount && IsItemVisible(*it, FALSE))
					DrawItem(dblg, *it, GetItemRect(*it));
			}
			Rect lastItemRC = GetItemRect(m_itemCount - 1);
			INT freeSpaceY = lastItemRC.Y + lastItemRC.Height;
			if (freeSpaceY < rc.GetBottom())
				DrawBackground(dblg, Rect(rc.X, freeSpaceY, rc.Width, rc.Height - freeSpaceY + 1));
		}
		else
		{
			DrawBackground(dblg, Rect(rc.X, rc.Y, rc.Width, rc.Height + 1));
		}
		m_invalidatedItemsSet.clear();
		m_bWindowInvalidated = FALSE;
	}
	m_dblBuffer.Render(g);

	//===Draw Mouse Over========================
	CPoint p;
	GetCursorPos(&p);
	m_mouseOverItem = -1;
	if (::WindowFromPoint(p) == m_hWnd)
	{
		ScreenToClient(&p);
		if (_rc.PtInRect(p) && !m_scrollBar.ContainsPoint(p.x, p.y) && !m_bMouseLButtonIsDown)
			m_mouseOverItem = GetItemFromPoint(p.x, p.y);

		if (m_mouseOverItem != -1)
			DrawMouseOver(g, m_mouseOverItem);
	}

	//===Draw ScrollBar========================
	if (m_scrollBar.HasScroll())
		m_scrollBar.Draw(g);

	//===Draw SearchString========================
	if (!m_searchString.empty())
		DrawQuickSearchString(g, m_searchString.c_str());
}

void CFlexListCtrl::DrawBackground(Graphics& g, const Rect& r)
{
	SolidBrush bkBrush(Color::MakeARGB(255, 230,230,230));
	g.FillRectangle(&bkBrush, r);
}

BOOL CFlexListCtrl::IsItemVisible(INT nItem, BOOL bFullVisibility)
{
	ASSERT(nItem >= 0 && nItem < (INT)GetItemCount());
	if (nItem >= 0 && nItem < (INT)GetItemCount())
	{
		INT yPos = nItem * m_itemHeight - m_scrollBar.GetScrollPos();
		RECT rc;
		GetClientRect(&rc);
		INT ctrlHeight = rc.bottom - rc.top;
		if (bFullVisibility)
		{
			if (yPos >= 0 && (yPos < ctrlHeight - (INT)m_itemHeight + 1))
				return TRUE;
		}
		else
		{
			if (yPos >= (1 -(INT) m_itemHeight) && (yPos < ctrlHeight))
				return TRUE;
		}
	}
	return FALSE;
}

void CFlexListCtrl::DrawMouseOver(Graphics& g, INT item)
{
	Rect r = GetItemRect(item);
	Pen p(Color::MakeARGB(255, 200,200,200));
	r.Width--;
	r.Height--;
	g.DrawRectangle(&p, r);
}

void CFlexListCtrl::ClearItems()
{
	m_selectedItems.clear();
	m_selectedItem = -1;
	m_itemCount = 0;
	Refresh();
}

void CFlexListCtrl::InsertItems(INT pos, INT itemCount)
{
	m_itemCount += itemCount;
	if (m_bSingleSelection)
	{
		if (pos <= m_selectedItem)
			m_selectedItem += itemCount;
	}
	else
		m_selectedItems.insert(m_selectedItems.begin() + pos, itemCount, FALSE);
	m_scrollBar.SetFullScroll(m_itemCount * m_itemHeight);
	Refresh();
}

void CFlexListCtrl::SwapItem(INT pos1, INT pos2)
{
	ASSERT(pos1 < (INT)m_itemCount && pos2 < (INT)m_itemCount);
	InvalidateItem(pos1);
	InvalidateItem(pos2);
	if (m_bSingleSelection)
	{
		if (pos1 == m_selectedItem)
			m_selectedItem = pos2;
		else if (pos2 == m_selectedItem)
			m_selectedItem = pos1;
	}
	else
	{
		BOOL bKeep = m_selectedItems[pos1];
		m_selectedItems[pos1] = m_selectedItems[pos2];
		m_selectedItems[pos2] = bKeep;
	}
	if (m_hWnd)Invalidate(FALSE);
}


void CFlexListCtrl::DeleteItems(INT pos, INT itemCount)
{
	ASSERT(pos < (INT)m_itemCount);
	m_itemCount -= itemCount;
	if (m_bSingleSelection)
	{
		if (pos < m_selectedItem) 
			m_selectedItem -= itemCount;
		else if (pos < m_selectedItem + itemCount)
			m_selectedItem = -1;
		if (m_selectedItem < 0)
			m_selectedItem = -1;
	}
	else
	{
		m_selectedItems.erase(m_selectedItems.begin() + pos, m_selectedItems.begin() + pos + itemCount);
	}
	m_scrollBar.SetFullScroll(m_itemCount * m_itemHeight);
	Refresh();
}

INT CFlexListCtrl::GetItemCount()
{
	return (INT) m_itemCount;	
}
void CFlexListCtrl::SetItemCount(INT nItemCount)
{
	m_itemCount = nItemCount;
	m_selectedItems.resize(m_itemCount, FALSE);
	m_scrollBar.SetFullScroll(m_itemCount * m_itemHeight);
	Refresh();
	m_selectedItem = -1;
}


Rect CFlexListCtrl::GetItemRect(INT nItem)
{
	CRect rc;
	GetClientRect(&rc);
	return Rect(rc.left, nItem * m_itemHeight - m_scrollBar.GetScrollPos(), rc.Width() - (m_scrollBar.HasScroll() ? m_scrollBarThickness : 0), m_itemHeight);
}

Gdiplus::Rect CFlexListCtrl::FromRECT(RECT& rc)
{
	return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}


INT CFlexListCtrl::GetItemFromPoint(INT x, INT y)
{
	INT nItem = ((m_scrollBar.GetScrollPos() + y) / m_itemHeight);
	if (nItem >= (INT)GetItemCount())
		return -1;
	return nItem;
}
INT CFlexListCtrl::GetNextVisibleItem(INT nPrevItem)
{
	if (nPrevItem == -1)
		return GetItemFromPoint(0, 0);
	nPrevItem++;
	if (nPrevItem < GetItemCount())
	{
		if (IsItemVisible(nPrevItem, FALSE))
			return nPrevItem;
	}
	return -1;
}

INT CFlexListCtrl::GetScrollPos()
{
	return m_scrollBar.GetScrollPos();
}

void CFlexListCtrl::SetScrollPos(INT pos)
{
	INT oldPos = GetScrollPos();
	m_scrollBar.SetScrollPos(pos);
	if (pos != GetScrollPos() && m_hWnd != NULL)
		Refresh();
}

BOOL CFlexListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	SetScrollPos(GetScrollPos() - zDelta / 2);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CFlexListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
}

void CFlexListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
}

void CFlexListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_scrollBar.SetScrollerRect(Gdiplus::Rect(cx - m_scrollBarThickness, 0, m_scrollBarThickness, cy));
	Refresh();
}

void CFlexListCtrl::SetSingleSelection(BOOL bEnable)
{
	if (m_bSingleSelection != bEnable)
	{
		INT curSel = GetNextSelectedItem();
		ClearSelection();
		m_bSingleSelection = bEnable;
		if (curSel != -1)
			SetItemSelection(curSel, TRUE);
	}
}

void CFlexListCtrl::SetItemHeight(UINT itemHeight)
{
	if (m_itemHeight != itemHeight)
	{
		m_itemHeight = itemHeight;
		m_scrollBar.SetScrollStep(m_itemHeight);
		m_scrollBar.SetFullScroll(m_itemCount * m_itemHeight);
		Refresh();
	}
}

void CFlexListCtrl::SetMouseOverItem(INT nItem)
{
	if (m_mouseOverItem != nItem)
	{
		m_mouseOverItem = nItem;
		Invalidate(FALSE);
	}
}

void CFlexListCtrl::CenterItem(INT nItem)
{
	ASSERT(nItem >= 0 && nItem < GetItemCount());
	CRect rc;
	GetClientRect(&rc);
	INT offset = rc.Height() / 2;
	if (offset == 0)
		offset = 2 * m_itemHeight;
	SetScrollPos(m_itemHeight * nItem - offset);
}

void CFlexListCtrl::EnsureVisible(INT nItem, BOOL bFullVisibility)
{
	if (!IsItemVisible(nItem, bFullVisibility))
	{
		INT newPos = m_itemHeight * nItem;
		INT curPos = GetScrollPos();
		if (curPos > newPos)
			SetScrollPos(newPos);
		else 
		{
			CRect rc;
			GetClientRect(&rc);
			SetScrollPos(newPos - rc.Height() + m_itemHeight);
		}
	}
}


void CFlexListCtrl::InvalidateItem(INT nItem)
{
	if (IsItemVisible(nItem, FALSE))
	{
		m_invalidatedItemsSet.insert(nItem);
		m_bWindowInvalidated = TRUE;
	}
}


void CFlexListCtrl::SetItemSelection(INT nItem, BOOL bSelected)
{
	ASSERT(nItem >= 0 && nItem < (INT)GetItemCount());
	if (nItem >= 0 && nItem < (INT)GetItemCount())
	{
		if (m_bSingleSelection )
		{
			if (bSelected && m_selectedItem != nItem)
			{
				ClearSelection();
				m_selectedItem = nItem;
				InvalidateItem(nItem);
				OnSelectionChanged();
				Invalidate(FALSE);
			}
		}
		else
		{
			if (IsItemSelected(nItem) != bSelected)
			{
				m_selectedItems[nItem] = bSelected;
				InvalidateItem(nItem);
				OnSelectionChanged();
				Invalidate(FALSE);
			}
		}
	}
}

void CFlexListCtrl::Refresh()
{
	if (m_hWnd)
	{
		CRect rc;
		GetClientRect(&rc);
		InvalidateViewRange(0, rc.Height());
		m_bWindowInvalidated = TRUE;
		SetScrollPos(GetScrollPos());
		if (m_hWnd)Invalidate(FALSE);
	}
}

BOOL CFlexListCtrl::PreTranslateMessage(MSG* pMsg)
{
	BOOL bHandled = FALSE;
	switch (pMsg->message)
	{
	case WM_KEYDOWN://We need this here because in a dialog based application CDialog handles
						//the VK_DOWN - VK_UP (change child control focus) and we 
						//don't get it in OnKeyDown Event. Also you must return TRUE if handled
		{
			INT curItem = GetNextSelectedItem();
			switch (pMsg->wParam)
			{
			case VK_UP:
				if (curItem > 0) 
				{
					if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0)
						ClearSelection();
					SetItemSelection(curItem - 1, TRUE);
					EnsureVisible(curItem - 1, TRUE);
				}
				bHandled = TRUE;
				break;
			case VK_DOWN:
				if (curItem < (INT)GetItemCount() - 1)
				{
					INT lastItem = curItem;
					while (lastItem != -1)
					{
						curItem = lastItem;
						lastItem = GetNextSelectedItem(curItem);
					}
					if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0)
						ClearSelection();
					SetItemSelection(curItem + 1, TRUE);
					EnsureVisible(curItem + 1, TRUE);
				}
				bHandled = TRUE;
				break;
			case VK_PRIOR:
				if (curItem > 0)
				{
					CRect rc;
					GetClientRect(&rc);
					INT h = GetItemHeight();
					INT newItem = curItem - rc.Height() / h;
					if (newItem < 0)
						newItem = 0;
					if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0)
					{
						ClearSelection();
						SetItemSelection(newItem, TRUE);
					}
					else
					{
						for (INT i = newItem; i < curItem; i++)
							SetItemSelection(i, TRUE);
					}
					EnsureVisible(newItem, TRUE);
				}
				break;
			case VK_NEXT:
				if (curItem < (INT)GetItemCount() - 1)
				{
					CRect rc;
					GetClientRect(&rc);
					INT h = GetItemHeight();
					INT newItem = curItem + rc.Height() / h;
					if (newItem > (INT)GetItemCount() - 1)
						newItem = (INT)GetItemCount() - 1;
					if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0)
					{
						ClearSelection();
						SetItemSelection(newItem, TRUE);
					}
					else
					{
						for (INT i = curItem + 1; i <= newItem; i++)
							SetItemSelection(i, TRUE);
					}
					EnsureVisible(newItem, TRUE);
				}
				break;
			}

		}
		break;
	case WM_CHAR:
		{
			BOOL bStringChanged = FALSE;
			UINT nChar = (UINT) pMsg->wParam;
			switch (nChar)
			{
			case 8:
				if (!m_searchString.empty())
				{
					bStringChanged = TRUE;
					m_searchString.erase(m_searchString.end() - 1);
				}
				break;
			default:
				bStringChanged = TRUE;
				m_searchString.push_back(TCHAR(nChar));
			}
			if (bStringChanged)
			{
				bHandled = TRUE;
				KillTimer(TIMER_SEARCHSTRING);
				SetTimer(TIMER_SEARCHSTRING, m_searchStringDelay, NULL);
				KillTimer(TIMER_RESETSEARCHSTRING);
				SetTimer(TIMER_RESETSEARCHSTRING, m_searchStringResetDelay, NULL);
				Invalidate(FALSE);
			}
		}
		break;
	}
	if (bHandled)
		return TRUE;
	return CWnd::PreTranslateMessage(pMsg);
}

void CFlexListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_SEARCHSTRING)
	{
		//CClientDC _dc(this);
		//Graphics g(_dc);
		//m_dblBuffer.Render(g);
		//if (!m_searchString.empty())
		//	DrawQuickSearchString(g, m_searchString.c_str());
		INT nItem = OnFindItem(m_searchString.c_str());
		if (nItem != -1)
		{
			ClearSelection();
			SetItemSelection(nItem, TRUE);
			CenterItem(nItem);
		}
		KillTimer(TIMER_SEARCHSTRING);
	}
	else if (nIDEvent == TIMER_RESETSEARCHSTRING)
	{
		m_searchString.clear();
		KillTimer(TIMER_RESETSEARCHSTRING);
		Invalidate(FALSE);
	}
	else if (nIDEvent == TIMER_LBUTTONDOWNREPEAT)
	{
		KillTimer(TIMER_LBUTTONDOWNREPEAT);
		SetTimer(TIMER_LBUTTONDOWNREPEAT, KEYBOARD_SPEED, NULL);
		OnLButtonDown(0xFFFFFFFF, m_lastLButtonClick);
	}
	CWnd::OnTimer(nIDEvent);
}

void CFlexListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	BOOL bHandledByScrollBar = m_scrollBar.OnMouseMove(point.x, point.y);


	CPoint sPoint(point);
	ClientToScreen(&sPoint);
	if (::WindowFromPoint(sPoint) == m_hWnd && rc.PtInRect(point))//May be out of the box and captured
	{
		if (bHandledByScrollBar || m_bMouseLButtonIsDown)
			SetMouseOverItem(-1);
		else
			SetMouseOverItem(GetItemFromPoint(point.x, point.y));
		if (::GetCapture() != m_hWnd)
			SetCapture();
	}
	else
	{
		if (!m_bMouseLButtonIsDown)
			ReleaseCapture();
		SetMouseOverItem(-1);
		//ASSERT(::GetCapture() == m_hWnd);
	}
	if (abs(point.x - m_lastLButtonClick.x) > 2 || abs(point.y - m_lastLButtonClick.y) > 2)
		KillTimer(TIMER_LBUTTONDOWNREPEAT);
	CWnd::OnMouseMove(nFlags, point);
}


void CFlexListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseLButtonIsDown = TRUE;
	if (!m_scrollBar.OnLButtonDown(point.x, point.y))
	{
		INT nItem = GetItemFromPoint(point.x, point.y);
		if (m_bSingleSelection)
		{
			if (nItem != -1)
				SetItemSelection(nItem, TRUE);
			else
				ClearSelection();
		}
		else
		{
			m_flagsOnMouseDown = nFlags;
			m_nItemOnMouseDown = nItem;
			if (nFlags & MK_CONTROL)
			{
			}
			else if (nFlags & MK_SHIFT)
			{
				if (nItem != -1)
				{
					INT firstSel = GetNextSelectedItem();
					if (firstSel == -1)
						SetItemSelection(nItem, TRUE);
					else
					{
						for (INT i = min(firstSel, nItem); i <= max(firstSel, nItem); i++)
							SetItemSelection(i, TRUE);
					}
				}
			}
			else 
			{
				ClearSelection();
				if (nItem != -1)
					SetItemSelection(nItem, TRUE);

				//if (nItem != -1 && !IsItemSelected(nItem))
				//{
				//	ClearSelection();
				//	SetItemSelection(nItem, TRUE);
				//}
			}
		}
		ItemMouseEvent ev;
		ev.activeItem = nItem;
		ev.type = IMET_LButtonDown;
		ev.xClient = point.x;
		ev.yClient = point.y;
		OnItemMouseEvent(ev);
	}
	SetFocus();
	if (nFlags == 0xFFFFFFFF)
		return;
	m_lastLButtonClick = point;
	SetTimer(TIMER_LBUTTONDOWNREPEAT, KEYBOARD_DELAY, NULL);
	CWnd::OnLButtonDown(nFlags, point);
}

void CFlexListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!m_scrollBar.OnLButtonDblClk(point.x, point.y))
	{
		ItemMouseEvent ev;
		ev.activeItem = GetItemFromPoint(point.x, point.y);
		ev.type = IMET_LButtonDblClick;
		ev.xClient = point.x;
		ev.yClient = point.y;
		OnItemMouseEvent(ev);
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CFlexListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseLButtonIsDown = FALSE;
	KillTimer(TIMER_LBUTTONDOWNREPEAT);
	if (!m_scrollBar.OnLButtonUp(point.x, point.y))
	{
		INT nItem = GetItemFromPoint(point.x, point.y);
		if (nItem == m_nItemOnMouseDown)
		{
			if (m_flagsOnMouseDown & MK_CONTROL)
			{
				if (nItem != -1)
					SetItemSelection(nItem, !IsItemSelected(nItem));
			}
			else if (m_flagsOnMouseDown & MK_SHIFT)
			{

			}
			else
			{
				if (GetSelectedItemCount() > 1)
					ClearSelection();
				if (nItem != -1)
					SetItemSelection(nItem, TRUE);
			}
		}
		ItemMouseEvent ev;
		ev.activeItem = nItem;
		ev.type = IMET_LButtonUp;
		ev.xClient = point.x;
		ev.yClient = point.y;
		OnItemMouseEvent(ev);

	}
	CRect rc;
	GetClientRect(&rc);	
	if (!rc.PtInRect(point))
	{
		ASSERT(::GetCapture() == m_hWnd);
		VERIFY(ReleaseCapture());
		SetMouseOverItem(-1);
	}
	CWnd::OnLButtonUp(nFlags, point);
}


void CFlexListCtrl::EnableDragDrop(BOOL bEnable)
{
	if (bEnable)
	{
		if (m_pDropHelper == NULL)
			m_pDropHelper = new FlexDropHelper(this);
		m_pDropHelper->EnableDragDrop(TRUE);
	}
	else
	{
		if (m_pDropHelper != NULL)
			m_pDropHelper->EnableDragDrop(FALSE);
	}
}

BOOL CFlexListCtrl::AcceptsFileDrop(CPoint& p)
{
	INT item = GetItemFromPoint(p.x, p.y);
	if (item != -1)
	{
		INT firstVisibleItem = GetItemFromPoint(0,0);
		CRect rc;
		GetClientRect(&rc);
		INT lastVisibleItem = GetItemFromPoint(0, rc.Height());
		if (item == firstVisibleItem && item != 0)
		{
			SetScrollPos(GetScrollPos() - m_itemHeight);
		}
		else if (item == lastVisibleItem && item < GetItemCount())
		{
			SetScrollPos(GetScrollPos() + m_itemHeight);
		}
		else
		{
			SetMouseOverItem(item);
		}

		//EnsureVisible(item, TRUE);

	}
	return ItemAcceptsFileDrop(p, item);
}

void CFlexListCtrl::ClearSelection()
{
	if (m_bSingleSelection)
	{
		if (m_selectedItem != -1)
		{
			InvalidateItem(m_selectedItem);
			m_selectedItem = -1;
		}	
	}
	else
	{
		for (size_t i = 0; i < m_selectedItems.size(); i++)
		{
			if (m_selectedItems[i])
			{
				InvalidateItem(i);
				m_selectedItems[i] = FALSE;
			}
		}
	}
	if (m_hWnd)Invalidate(FALSE);
}

INT CFlexListCtrl::GetNextSelectedItem(INT nPrevItem)
{
	if (m_bSingleSelection)
	{
		if (nPrevItem == -1)
			return m_selectedItem;
	}
	else
	{
		for (size_t i = nPrevItem + 1; i < m_selectedItems.size(); i++)
			if (m_selectedItems[i] == TRUE)
				return (INT)i;

	}
	return -1;
}

BOOL CFlexListCtrl::IsItemSelected(INT nItem)
{
	ASSERT(nItem != -1);
	if (m_bSingleSelection)
	{
		return nItem == m_selectedItem;
	}
	return m_selectedItems[nItem] == TRUE;
}

UINT CFlexListCtrl::GetSelectedItemCount()
{
	if (m_bSingleSelection)
		return m_selectedItem == -1 ? 0 : 1;
	UINT count = 0;
	std::vector<BOOL>::const_iterator it = m_selectedItems.begin();
	for (; it != m_selectedItems.end(); it++)
		if (*it == TRUE)
			count++;
	return count;
}

void CFlexListCtrl::OnNcPaint()
{
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & WS_BORDER) == WS_BORDER)
	{
		CWindowDC dc(this);
		CRect rc;
		GetWindowRect(&rc);
		rc.right = rc.Width();
		rc.bottom = rc.Height();
		rc.left = rc.top = 0;
		CPen* pOldPen = (CPen*) dc.SelectStockObject(DC_PEN);
		CBrush* pOldBrush = (CBrush*) dc.SelectStockObject(NULL_BRUSH);
		dc.SetDCPenColor(GetSysColor(COLOR_INACTIVECAPTION));
		dc.Rectangle(&rc);
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}

}
