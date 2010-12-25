// DialogHeader.cpp : implementation file
//

#include "stdafx.h"
#include "FlexDialogHeader.h"


// FlexDialogHeader.cpp : source file
//******************************************************************************
//Author : Jason Gurgel
//EMail  : jmgurgel@attbi.com
//(C)    : 7-14-2002
//
//Purpose: Provide a professional looking header area for dialog controls
//       : that will display an icon, a title phrase in bold, and a
//       : description phrase. And also provide a custom background color
//       : and custom header height.
//		  
//Note   : This code was insipired by Mustafa Demirhan's article about hacking
//		 : the CPropterySheet. See link below. 
//		 : http://www.codeproject.com/property/hacking_the_cpropertyshee.asp#xx162207xx
//
//Disclaimer: This code is free to use as long as this information
//			: is included. The author takes no responsibility for
//			: the corretness of the code or any damage incured from it's use. 
//			: It is free so use at your own risk, and enjoy!
//			: Any comments,suggestions, or additions would be appericated.
//******************************************************************************


/////////////////////////////////////////////////////////////////////////////
// CDialogHeader

const INT cMargin = 5;
const INT cIndent = 12;

FlexDialogHeader::FlexDialogHeader():
m_bDraggable(TRUE),
m_pTitleFont(NULL),
m_pDescFont(NULL),
m_hIcon(NULL)
{
	//Set default background color
	m_bkColor = GetSysColor(COLOR_WINDOW);

	//Set default header height
	m_headerHeight = DEFAULTHEADERHEIGHT;

	//Get the default system icon dims
	m_cxIcon = ::GetSystemMetrics(SM_CXICON);
	m_cyIcon = ::GetSystemMetrics(SM_CYICON);

	//Get system edge metrics
	m_nxEdge=::GetSystemMetrics(SM_CXEDGE);
//	m_nyEdge=::GetSystemMetrics(SM_CYEDGE);
	m_nyEdge=1;	//I think this edge looks better then system edge size

	m_iconImage.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
}

FlexDialogHeader::~FlexDialogHeader()
{
}


BEGIN_MESSAGE_MAP(FlexDialogHeader, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDialogHeader Operations

void FlexDialogHeader::Init(void* ptrWnd)
{
	//Variables
	CRect rect;					//General purpose rect
	CWnd* pWnd=(CWnd*)ptrWnd;	//Ptr to calling window


	//Check for valid ptr
	if ((NULL == pWnd) || (NULL == pWnd->m_hWnd))
	{
		return;
	}

	//Ensure the height is at least a min to disp data
	SetHeaderHeight(m_headerHeight);

	//Resize the window to accomodate the header
	pWnd->GetWindowRect(rect);
    pWnd->ScreenToClient(rect);
    pWnd->SetWindowPos(NULL,0,0,rect.Width(),rect.Height() + m_headerHeight,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	//Create the header window
	CreateEx (NULL,NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER, 
              -1,-1,rect.Width(),m_headerHeight,pWnd->m_hWnd,0,0);

}

void FlexDialogHeader::MoveCtrls(void* ptrWnd)
{
	//Variables
	CRect rect;					//General purpose rect
	CWnd* pWnd=(CWnd*)ptrWnd;	//Ptr to calling window
	CWnd* pwndChild=NULL;		//Ptr to walk through dialog ctrl


	//Check for valid ptr
	if ((NULL == pWnd) || (NULL == pWnd->m_hWnd))
	{
		return;
	}

	//Move all dialog ctrls down to fit header
	pwndChild= pWnd->GetWindow(GW_CHILD);
	while(pwndChild)
	{
		//Too ensure we don't move the header ctrl down
		if (pwndChild != this)
		{

			//Get the child ctrl rect
			pwndChild->GetWindowRect(rect);
			ScreenToClient(rect); 

			//Move each child ctrl down to accomodate header
			pwndChild->SetWindowPos(NULL,rect.left,rect.top + m_headerHeight, 
                                    rect.Width(),rect.Height(),
									SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//Get the next child ctrl
		pwndChild = pwndChild->GetNextWindow();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDialogHeader message handlers

BOOL FlexDialogHeader::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void FlexDialogHeader::OnPaint() 
{
	//Variables
	CPaintDC	dc(this);	//Device context for painting
	CRect		rect;		//General purpose rect
	CRect		text;		//Rect for enclosing text


	GetClientRect(rect);

	//Draw 3d border around rect
	dc.Draw3dRect(rect,GetSysColor(COLOR_BTNHIGHLIGHT),GetSysColor(COLOR_BTNSHADOW));

	//Fill the rect with solid color
	//Leaving space for line at bottom of rect
	dc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height() - m_nxEdge, m_bkColor);

	//Draw icon if one is present
	if (m_iconImage.HasImage())
	{
		dc.FillSolidRect(cMargin - 1, cMargin - 1, m_cxIcon + 2, m_cyIcon + 2, RGB(128,128,128));
		m_iconImage.Draw(dc.m_hDC, CRect(cMargin, cMargin, cMargin + m_cxIcon, cMargin + m_cyIcon));
		text.left = rect.left + cMargin + m_cxIcon + cIndent;
	}
	else if (NULL != m_hIcon)
	{
		DrawIconEx(dc.m_hDC, cMargin, cMargin, m_hIcon, m_cxIcon, m_cyIcon, 0, 0, DI_NORMAL);
		text.left = rect.left + cMargin + m_cxIcon + cIndent;
	}
	else
		text.left = rect.left + cIndent;
	if (m_pTitleFont == NULL)
		m_pTitleFont = GetFont();
	if (m_pDescFont == NULL)
		m_pDescFont = GetFont();
	
	//Draw title text in bold font on one line
	dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(m_pTitleFont);

	CRect calcRC(rect);
	DrawText(dc.m_hDC, m_sTitle, m_sTitle.GetLength(), &calcRC, 
		DT_NOPREFIX | DT_EXPANDTABS | DT_SINGLELINE | DT_CALCRECT | DT_NOPREFIX);

	text.top = rect.top + cMargin;
	text.right = rect.right - cIndent - m_nyEdge;
	text.bottom = text.top + calcRC.Height() + cMargin;
	DrawText(dc.m_hDC, m_sTitle, m_sTitle.GetLength(), text, 
		DT_END_ELLIPSIS | DT_NOPREFIX | DT_EXPANDTABS | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);

	//Draw description text in normal front using
	//rest of available header client area
	dc.SelectObject (m_pDescFont);
	text.left += cMargin;
	text.top = text.bottom;
	text.bottom = rect.bottom;
	DrawText(dc.m_hDC,m_sDesc,m_sDesc.GetLength(),text,
	         DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL | DT_TOP | DT_NOPREFIX);
	
	dc.SelectObject(pOldFont);
}



void FlexDialogHeader::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bDraggable)
	{
		SetCapture();
		m_dragPoint = point;
		ClientToScreen(&m_dragPoint);
		//GetParent()->ScreenToClient(&m_dragPoint);
		//GetParent()->ClientToScreen(&m_dragPoint);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void FlexDialogHeader::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDraggable)
		ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void FlexDialogHeader::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDraggable)
	{
		if (::GetCapture() == m_hWnd)
		{
			CRect rc;
			GetParent()->GetWindowRect(&rc);
			ClientToScreen(&point);
			GetParent()->SetWindowPos(NULL,
				rc.left + point.x - m_dragPoint.x,
				rc.top + point.y - m_dragPoint.y,
				0, 
				0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
			m_dragPoint = point;
		}

	}

	CWnd::OnMouseMove(nFlags, point);
}
