//------------------------------------------------------------------------------
// File    : SSplitter.cpp
// Version : 1.1
// Date    : 20. January 2004
// Author  : Alexander Atamas
// Email   : atamas@mail.univ.kiev.ua
// Web     : 
// Systems : VC6.0 (Run under Window 98, Windows Nt)
// Remarks : based on Paul DiLascia's WinMgr code
//

// 
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it any of your 
// applications (Freeware, Shareware, Commercial). 
// All I ask is that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. I also wouldn't 
// mind if you sent me a copy of your application since I like to play with
// new stuff.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "SSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSSplitter

CSSplitter::CSSplitter():
m_pFPane(NULL),
m_pSPane(NULL),
m_nFConstr(100),
m_nSConstr(100),
m_bVertSplitter(TRUE),
m_bFHidden(FALSE),
m_bSHidden(FALSE),
m_bDragging(FALSE),
m_BarPos(0)
{
	m_nSplitterWidth		= 3;   // the width of slitter

	m_bMovingHorizSplitter	= FALSE;
	m_bDraggingHoriz		= FALSE;	
	m_bMovingVertSplitter	= FALSE;
	m_bDraggingVert			= FALSE;
	m_bMouseMoveSplittingMode	= FALSE;
}

CSSplitter::~CSSplitter()
{
}


BEGIN_MESSAGE_MAP(CSSplitter, CWnd)//CStatic)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSSplitter message handlers

BOOL CSSplitter::Create(
			DWORD dwStyle, 
			CWnd* pParentWnd, 
			CWnd* pFPane, 
			CWnd* pSPane,
			UINT nID,
			UINT nFConstr,
			UINT nSConstr,
			BOOL bVerticalSplitter,
			UINT SplitterWidth,
			COLORREF bkColor)
{
	ASSERT(pFPane != NULL);
	ASSERT(pSPane != NULL);
	ASSERT(pParentWnd != NULL);
	ASSERT(nID > 0);
	m_nID  = nID;
	m_pFPane = pFPane;
	m_pSPane = pSPane;
	m_nFConstr = nFConstr;
	m_nSConstr = nSConstr;
	m_pWndParent = pParentWnd;
	CWnd::Create(NULL, NULL, dwStyle, CRect(), pParentWnd, nID);
	m_pFPane->SetParent(this);
	m_pFPane->SetOwner(m_pWndParent);
	m_pSPane->SetParent(this);
	m_pSPane->SetOwner(m_pWndParent);
	m_bVertSplitter = bVerticalSplitter;
	m_nSplitterWidth = SplitterWidth;
	GetClientRect(&m_rcFPane);
	m_rcSpane = m_rcFPane;
	if (m_bFHidden)
		m_pFPane->ShowWindow(SW_HIDE);
	else if (m_bSHidden)
		m_pSPane->ShowWindow(SW_HIDE);
	MovePanes(2 * m_nFConstr);
	if (bkColor == -1)
		bkColor = GetSysColor(COLOR_BTNFACE);
	m_bkColor = bkColor;
	return TRUE;
}

void CSSplitter::MovePanes(UINT newSplitterPos)
{
	ASSERT(m_pFPane != NULL);
	ASSERT(m_pSPane != NULL);

	CRect rect;
	GetClientRect( &rect );
	ApplyConstraints(newSplitterPos, rect.Width());
	if (m_bVertSplitter)
	{
		m_pFPane->MoveWindow(rect.left, rect.top, newSplitterPos, rect.Height(), TRUE);
		m_pSPane->MoveWindow(newSplitterPos + m_nSplitterWidth, rect.top, rect.Width(), rect.Height(), TRUE);
	}
	else
	{
		m_pFPane->MoveWindow(rect.left, rect.top, rect.Width(), newSplitterPos, TRUE);
		m_pSPane->MoveWindow(rect.left, newSplitterPos + m_nSplitterWidth, rect.Width(), rect.Height(), TRUE);
	}
	m_SplitterPos = newSplitterPos;
	Invalidate();
}

void CSSplitter::ApplyConstraints(UINT& SplitterPos, UINT totalWidth)
{
	if (SplitterPos < m_nFConstr)
		SplitterPos = m_nFConstr;
	if (SplitterPos > totalWidth - m_nSConstr)
		SplitterPos = totalWidth - m_nSConstr;
}



BOOL CSSplitter::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= SS_NOTIFY; // to notify its parent of mouse events	
	return CWnd::PreCreateWindow(cs);
}


void CSSplitter::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	MovePanes(m_SplitterPos);
}

CRect CSSplitter::GetSplitterRect(UINT SplitterPos)
{
	CRect retRC;
	GetClientRect(&retRC);
	ApplyConstraints(SplitterPos, retRC.Width());
	if (m_bVertSplitter)
	{
		retRC.left += SplitterPos;
		retRC.right = retRC.left + m_nSplitterWidth;
	}
	else
	{
		retRC.top += SplitterPos;
		retRC.bottom = retRC.top + m_nSplitterWidth;
	}
	return retRC;
}

void CSSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bDragging = TRUE;
	if ( !m_bMouseMoveSplittingMode )
		DrawBar(point);					 
	SetCapture();
	m_hwndPrevFocusVert = ::SetFocus(m_hWnd);  
	::SetCursor( AfxGetApp()->LoadStandardCursor (LPCTSTR (m_bVertSplitter ? IDC_SIZEWE : IDC_SIZENS)));
	CWnd::OnLButtonDown(nFlags, point);
}

void CSSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDragging = FALSE;
	UINT newPos = m_BarPos;
	if ( !m_bMouseMoveSplittingMode )
		DrawBar(point, TRUE);					 
	ReleaseCapture();
	MovePanes(newPos);
	::SetFocus(m_hwndPrevFocusVert);
	CWnd::OnLButtonUp(nFlags, point);
}

void CSSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	::SetCursor( AfxGetApp()->LoadStandardCursor(LPCTSTR (m_bVertSplitter ? IDC_SIZEWE : IDC_SIZENS)));
	if (m_bDragging)
	{
		if (m_bMouseMoveSplittingMode)
		{
		}
		else
		{
			DrawBar(point);	
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CSSplitter::DrawBar(CPoint& p, BOOL bEraseOnly)
{
	CWindowDC dc(m_pWndParent);
	CRect rcParent, rcThis;
	m_pWndParent->GetWindowRect(&rcParent);
	GetWindowRect(&rcThis);
	int OffsetX = rcThis.left - rcParent.left;
	int OffsetY = rcThis.top - rcParent.top;

	
	//CClientDC dc(this);

	CBrush brush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = dc.SelectObject(&brush);
	CRect rc;
	if (m_BarPos > 0)
	{
		rc = GetSplitterRect(m_BarPos);
		rc.OffsetRect(OffsetX, OffsetY);
		dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
		m_BarPos = 0;
	}
	if (bEraseOnly)
		return;
	if (m_bVertSplitter)
		m_BarPos = p.x;
	else
		m_BarPos = p.y;

	rc = GetSplitterRect(m_BarPos);
	rc.OffsetRect(OffsetX, OffsetY);
	dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
	dc.SelectObject(pOldBrush);					 
}

void CSSplitter::CancelDrag()
{
	DrawBar(CPoint(), TRUE);								
	ReleaseCapture();						
	::SetFocus(m_hwndPrevFocusVert);		
	m_bDragging = FALSE;
}

BOOL CSSplitter::PreTranslateMessage(MSG* pMsg) 
{
	if ( (pMsg->message == WM_KEYDOWN)&&( pMsg->wParam == VK_ESCAPE ))
		CancelDrag();
	return CWnd::PreTranslateMessage(pMsg);
}

void CSSplitter::OnDestroy() 
{
	CWnd::OnDestroy();
}

void CSSplitter::ShowFPane(BOOL bShow)
{
	if (m_bFHidden == bShow)
		return;
	m_bFHidden = bShow;
	MovePanes(m_SplitterPos);
}
void CSSplitter::ShowSPane(BOOL bShow)
{
	if (m_bSHidden == bShow)
		return;
	m_bSHidden = bShow;
	MovePanes(m_SplitterPos);
}

void CSSplitter::SetSplitterPos(UINT nPos)
{
	MovePanes(nPos);
}
 
BOOL CSSplitter::OnEraseBkgnd(CDC* pDC)
{
	CGdiObject* oldPen = pDC->SelectStockObject(BLACK_PEN);

	pDC->FillSolidRect(&GetSplitterRect(m_SplitterPos), m_bkColor);
	//if (m_bVertSplitter)
	//{
	//	pDC->FillSolidRect(&m_rcVertSplitter, m_bkColor);
	//	//pDC->MoveTo(m_rcVertSplitter.left, m_rcVertSplitter.top);
	//	//pDC->LineTo(m_rcVertSplitter.left, m_rcVertSplitter.bottom);
	//	//pDC->MoveTo(m_rcVertSplitter.right, m_rcVertSplitter.top);
	//	//pDC->LineTo(m_rcVertSplitter.right, m_rcVertSplitter.bottom);
	//}
	//else
	//{
	//	pDC->FillSolidRect(&m_rcHorizSplitter, m_bkColor);
	//	pDC->MoveTo(m_rcHorizSplitter.left, m_rcHorizSplitter.top);
	//	pDC->LineTo(m_rcHorizSplitter.right, m_rcHorizSplitter.top);
	//	pDC->MoveTo(m_rcHorizSplitter.left, m_rcHorizSplitter.bottom);
	//	pDC->LineTo(m_rcHorizSplitter.right, m_rcHorizSplitter.bottom);
	//}
	pDC->SelectObject(oldPen);
	return TRUE;
}

