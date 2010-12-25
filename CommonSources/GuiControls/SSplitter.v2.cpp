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
m_pWndParent(NULL),
m_pFPane(NULL),		
m_pSPane(NULL),		
m_nFConstr(100),		
m_nSConstr(100),			
m_bVertSplitter(TRUE),	
m_bFHidden(FALSE),
m_bSHidden(FALSE),
m_SplitterPos(0),
m_bDragging(FALSE),
m_BarPos(0),
m_DrawedBarPos(0),
m_hwndPrevFocus(0),
m_nSplitterWidth(3),
m_nID(0),
m_bMouseMoveSplittingMode(FALSE), 
m_bkColor(-1)
{
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
	ON_WM_PAINT()
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
	m_bVertSplitter = bVerticalSplitter;
	m_nSplitterWidth = SplitterWidth;
	m_SplitterPos = 2 * m_nFConstr;
	BOOL ret =  CWnd::Create(NULL, NULL, dwStyle, CRect(), pParentWnd, nID);
	if (ret)
	{
		m_pFPane->SetParent(this);
		m_pFPane->SetOwner(pParentWnd);
		m_pSPane->SetParent(this);
		m_pSPane->SetOwner(pParentWnd);

		if (m_bFHidden)
			m_pFPane->ShowWindow(SW_HIDE);
		else if (m_bSHidden)
			m_pSPane->ShowWindow(SW_HIDE);
		if (bkColor == -1)
			bkColor = GetSysColor(COLOR_BTNFACE);
		m_bkColor = bkColor;
	}
	return ret;
}

void CSSplitter::MovePanes()
{
	ASSERT(m_pFPane != NULL);
	ASSERT(m_pSPane != NULL);

	CRect rect;
	GetClientRect( &rect );
	if (rect.Width() == 0 || rect.Height() == 0)
		return;
	//ApplyConstraints(newSplitterPos, rect.Width());
	if (m_bVertSplitter)
	{
		m_pFPane->MoveWindow(rect.left, rect.top, m_SplitterPos, rect.Height(), TRUE);
		m_pSPane->MoveWindow(m_SplitterPos + m_nSplitterWidth, rect.top, rect.Width(), rect.Height(), TRUE);
	}
	else
	{
		m_pFPane->MoveWindow(rect.left, rect.top, rect.Width(), m_SplitterPos, TRUE);
		m_pSPane->MoveWindow(rect.left, m_SplitterPos + m_nSplitterWidth, rect.Width(), rect.Height(), TRUE);
	}
	InvalidateRect(GetSplitterRect(m_SplitterPos), TRUE);
	//m_pFPane->Invalidate();
	//m_pSPane->Invalidate();

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
	if (m_hWnd)
		MovePanes();
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
	m_DragStartPoint = point;
	m_BarPos = m_SplitterPos;
	if ( !m_bMouseMoveSplittingMode )
		DrawBar();					 
	SetCapture();
	m_hwndPrevFocus = ::SetFocus(m_hWnd);  
	::SetCursor( AfxGetApp()->LoadStandardCursor (LPCTSTR (m_bVertSplitter ? IDC_SIZEWE : IDC_SIZENS)));
	CWnd::OnLButtonDown(nFlags, point);
}

void CSSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDragging = FALSE;
	if ( !m_bMouseMoveSplittingMode )
		DrawBar(TRUE);	 
	ReleaseCapture();
	if (m_bVertSplitter)
		m_SplitterPos = m_SplitterPos + point.x - m_DragStartPoint.x;
	else
		m_SplitterPos = m_SplitterPos + point.y - m_DragStartPoint.y;
	MovePanes();
	::SetFocus(m_hwndPrevFocus);
	CWnd::OnLButtonUp(nFlags, point);
}

void CSSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	::SetCursor( AfxGetApp()->LoadStandardCursor(LPCTSTR (m_bVertSplitter ? IDC_SIZEWE : IDC_SIZENS)));
	if (m_bDragging)
	{
		
		if (m_bVertSplitter)
			m_BarPos = m_SplitterPos + point.x - m_DragStartPoint.x;
		else
			m_BarPos = m_SplitterPos + point.y - m_DragStartPoint.y;
		if (m_bMouseMoveSplittingMode)
		{
		}
		else
		{
			DrawBar();	
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CSSplitter::DrawBar(BOOL bEraseOnly)
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
	if (m_DrawedBarPos > 0)//Erase the previous
	{
		rc = GetSplitterRect(m_DrawedBarPos);
		rc.OffsetRect(OffsetX, OffsetY);
		dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
	}
	if (bEraseOnly)
		m_DrawedBarPos = 0;
	else
	{
		rc = GetSplitterRect(m_BarPos);
		rc.OffsetRect(OffsetX, OffsetY);
		dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
		dc.SelectObject(pOldBrush);	
		m_DrawedBarPos = m_BarPos;
	}
}

void CSSplitter::CancelDrag()
{
	DrawBar(TRUE);								
	ReleaseCapture();						
	::SetFocus(m_hwndPrevFocus);		
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
	MovePanes();
}
void CSSplitter::ShowSPane(BOOL bShow)
{
	if (m_bSHidden == bShow)
		return;
	m_bSHidden = bShow;
	MovePanes();
}

void CSSplitter::SetSplitterPos(UINT nPos)
{
	CRect rc;
	GetClientRect(&rc);
	ApplyConstraints(nPos, rc.Width());
	if (m_SplitterPos != nPos)
	{
		m_SplitterPos = nPos;
		MovePanes();
	}
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


void CSSplitter::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CGdiObject* oldPen = dc.SelectStockObject(BLACK_PEN);

	dc.FillSolidRect(&GetSplitterRect(m_SplitterPos), m_bkColor);
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
	dc.SelectObject(oldPen);

}
