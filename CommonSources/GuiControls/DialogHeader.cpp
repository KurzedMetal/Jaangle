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
#include "DialogHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// DialogHeader.cpp : source file
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

CDialogHeader::CDialogHeader():m_bDraggable(TRUE)
{
	//Clear all display variables
	m_hIcon=NULL;
	m_sTitle="";
	m_sDesc="";

	//Set font size
	m_bFonts=FALSE;
	m_nTitleFontSize=DEFAULTTITLEFONTSIZE;
	m_nDescFontSize=DEFAULTDESCFONTSIZE;

	//Set default icon offset
	m_nIconOffset=DEFAULTICONOFFSET;

	//Set default title offset from icon
	m_nTitleOffset=DEFAULTTITLEOFFSET;

	//Set default desc offset
	m_nDescOffset=DEFAULTDESCOFFSET;
	
	//Set default text alignment
	m_uAlign=DEFAULTTEXTALIGN;

	//Set default background color
	m_cBackground=GetSysColor(COLOR_WINDOW);

	//Set default header height
	m_nHeaderHeight=DEFAULTHEADERHEIGHT;

	//Get the default system icon dims
	m_nIconWidth=::GetSystemMetrics(SM_CXICON);
	m_nIconHeight=::GetSystemMetrics(SM_CYICON);

	//Set default icon display mode
	m_bBlankIcon=DEFAULTBLANKICON;

	//Get system edge metrics
	m_nxEdge=::GetSystemMetrics(SM_CXEDGE);
//	m_nyEdge=::GetSystemMetrics(SM_CYEDGE);
	m_nyEdge=1;	//I think this edge looks better then system edge size
}

CDialogHeader::~CDialogHeader()
{
}


BEGIN_MESSAGE_MAP(CDialogHeader, CWnd)
	//{{AFX_MSG_MAP(CDialogHeader)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDialogHeader Operations

void CDialogHeader::Init(void* ptrWnd)
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
	SetHeaderHeight(m_nHeaderHeight);

	//Resize the window to accomodate the header
	pWnd->GetWindowRect(rect);
    pWnd->ScreenToClient(rect);
    pWnd->SetWindowPos(NULL,0,0,rect.Width(),rect.Height() + m_nHeaderHeight,
                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	//Create the header window
	CreateEx (NULL,NULL,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER, 
              -1,-1,rect.Width(),m_nHeaderHeight,pWnd->m_hWnd,0,0);

}

void CDialogHeader::MoveCtrls(void* ptrWnd)
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
			pwndChild->SetWindowPos(NULL,rect.left,rect.top + m_nHeaderHeight, 
                                    rect.Width(),rect.Height(),
									SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//Get the next child ctrl
		pwndChild = pwndChild->GetNextWindow();
	}
}

void CDialogHeader::SetIconDimensions(INT cx, INT cy)
{
	m_nIconWidth = cx;
	m_nIconHeight = cy;
}



/////////////////////////////////////////////////////////////////////////////
// CDialogHeader Access Functions

void CDialogHeader::SetBackgroundColor(const COLORREF color)
{
	//Set new background color
	m_cBackground=color;

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetHeaderHeight(const int height)
{
	//Variables
	int	minIcon=0;
	int	minText=0;
	int min=0;

	//Compute the min heder height to display icon/text properly
	minIcon=(2 * m_nIconOffset) + m_nIconHeight + m_nyEdge;
	minText=(2 * m_nIconOffset) + m_nTitleFontSize + m_nDescFontSize + 2 + m_nyEdge;
	
	if (minIcon < minText)
	{
		min=minText;
	}

	else
	{
		min=minIcon;
	}

	//Ensure height is at least the min
	if (height < min)
	{
		m_nHeaderHeight=min;
	}

	else
	{
		m_nHeaderHeight=height;
	}

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetIconOffset(const int offset)
{
	if (offset < 2)
	{
		m_nIconOffset=2;
	}

	else
	{
		m_nIconOffset=offset;
	}

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetTitleOffset(const int offset)
{
	if (offset < 2)
	{
		m_nTitleOffset=2;
	}

	else
	{
		m_nTitleOffset=offset;
	}

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetDescOffset(const int offset)
{
	m_nDescOffset=offset;

	//Repaint the header
	RepaintHeader();
}


void CDialogHeader::SetTextAlign(const UINT align)
{
	//Ensure valid alignment
	if ((align != DT_LEFT) && (align != DT_CENTER) && (align != DT_RIGHT))
	{
		m_uAlign=DT_LEFT;
	}

	else
	{
		m_uAlign=align;
	}

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetBlankIcon(const BOOL blank)
{
	m_bBlankIcon=blank;

	//Repaint the header
	RepaintHeader();
}



void CDialogHeader::SetDescText(const LPCTSTR text)
{
	//Set new description text
	m_sDesc=text;

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetFontSizes(const int TitleSize,const int DescSize)
{
	//Check for valid title font size
	if (TitleSize < 10)
	{
		m_nTitleFontSize=10;
	}

	else
	{
		m_nTitleFontSize=TitleSize;
	}

	//Check for valid desc font size
	if (DescSize < 10)
	{
		m_nTitleFontSize=10;
	}

	else
	{
		m_nDescFontSize=DescSize;
	}

	//Force update of fonts in the OnPaint handler
	m_bFonts=FALSE;

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetIconHandle(const HICON icon)
{
	//Set new header icon
	m_hIcon=icon;

	//Repaint the header
	RepaintHeader();
}


void CDialogHeader::SetTitleText(const LPCTSTR text)
{
	//Set new title text
	m_sTitle=text;

	//Repaint the header
	RepaintHeader();
}

void CDialogHeader::SetHeaderData(const HICON hIcon,
								  const LPCTSTR strTitle,
								  const LPCTSTR strDesc)
{
	//Set the header data members
	m_hIcon=hIcon;
	m_sTitle=strTitle;
	m_sDesc=strDesc;

	//Repaint the header
	RepaintHeader();
}



/////////////////////////////////////////////////////////////////////////////
// CDialogHeader Internal Functions

void CDialogHeader::UpdateFont(CFont& font,const LONG lfWeight,const int Size)
{
	//Ensure font object is deleted
	font.DeleteObject();
	
	//Create new font for header
    font.CreateFont(Size,0,0,0,lfWeight,0,
		0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,PROOF_QUALITY,
		VARIABLE_PITCH | 0x04 | FF_DONTCARE, (LPTSTR)_T("Tahoma"));
}

void CDialogHeader::RepaintHeader()
{
	if (NULL != m_hWnd)
	{
		Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDialogHeader message handlers

BOOL CDialogHeader::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	//Dont want to erase background since we will redraw it in on 
	//OnPaint handler
	return TRUE;
	
//	return CWnd::OnEraseBkgnd(pDC);
}

void CDialogHeader::OnPaint() 
{
	//Variables
	CPaintDC	dc(this);	//Device context for painting
	CRect		rect;		//General purpose rect
	CRect		text;		//Rect for enclosing text
	int			temp=0;		//Temp variable for right side of text rect


	//Ensure the fonts have been created
	if (FALSE == m_bFonts)
	{
		UpdateFont (m_NormalFont,FW_NORMAL,m_nDescFontSize);
		UpdateFont (m_BoldFont,FW_BOLD,m_nTitleFontSize);
	}

	//Get the header client rect
	GetClientRect(rect);

	//Draw 3d border around rect
	dc.Draw3dRect(rect,GetSysColor(COLOR_BTNHIGHLIGHT),GetSysColor(COLOR_BTNSHADOW));

	//Fill the rect with solid color
	//Leaving space for line at bottom of rect
	dc.FillSolidRect(rect.left,rect.top,rect.Width(),rect.Height() - m_nxEdge,
		             m_cBackground);

	//Draw icon if one is present
	if (NULL != m_hIcon)
	{
		DrawIconEx(dc.m_hDC, m_nIconOffset, m_nIconOffset, m_hIcon, m_nIconWidth, m_nIconHeight, 0, 0, DI_NORMAL);
		//dc.DrawIcon(m_nIconOffset,m_nIconOffset,m_hIcon);
	}
	
	//Draw title text in bold font on one line
	dc.SelectObject(&m_BoldFont);
	text.left=rect.left + m_nTitleOffset;
	if ((NULL != m_hIcon) || (NULL != m_bBlankIcon))
	{
		//If no icon but we want space then add icon space to left margin
		text.left+=m_nIconWidth + m_nIconOffset;
	}
	text.top=rect.top + m_nIconOffset;
	text.right=rect.right - m_nIconOffset - m_nyEdge;
	text.bottom=text.top + m_nTitleFontSize;
	temp=text.right;
	if (DT_CENTER == m_uAlign)
	{
		//If center align, make as much space on the right as the left
		//to make the text truly centered in the dialog
		text.right=text.right - (text.left - rect.left);
	}
	DrawText(dc.m_hDC,m_sTitle,m_sTitle.GetLength(),text,
	         m_uAlign | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL | DT_NOPREFIX);
//	text.right=temp;


	//Draw description text in normal front using
	//rest of available header client area
	dc.SelectObject (&m_NormalFont);
//	if (DT_CENTER == m_uAlign)
//	{
//		//If center align, make as much space on the right as the left
//		//to make the text truly centered in the dialog
//		text.right=text.right - (text.left - rect.left);
//	}

	if (m_uAlign == DT_LEFT)
	{
		//Add desc offset if both aligned left
		text.left=text.left + m_nDescOffset;
	}

	if (DT_RIGHT == m_uAlign)
	{
		//Subtract desc offset if both aligned right
		text.right=text.right - m_nDescOffset;
	}
	text.top=text.bottom;
	text.bottom=rect.bottom;
	DrawText(dc.m_hDC,m_sDesc,m_sDesc.GetLength(),text,
	         m_uAlign | DT_WORDBREAK | DT_END_ELLIPSIS | DT_EDITCONTROL | DT_NOPREFIX);
	
	// Do not call CWnd::OnPaint() for painting messages
}



void CDialogHeader::OnLButtonDown(UINT nFlags, CPoint point)
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

void CDialogHeader::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDraggable)
		ReleaseCapture();

	CWnd::OnLButtonUp(nFlags, point);
}

void CDialogHeader::OnMouseMove(UINT nFlags, CPoint point)
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
