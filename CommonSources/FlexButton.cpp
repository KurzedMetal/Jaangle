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
#include "FlexButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef COLOR_HOTLIGHT
#define COLOR_HOTLIGHT 26
#endif

// Pointy finger cursor is defined on Win2000 and up
#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)
#endif

const INT cMargin = 4;
const INT cCheckBoxWidth = 18;
const INT cDropDownWidth = 18;



int FlexButton::m_nRefs = 0;                               // # FlexButton instances
HINSTANCE FlexButton::m_hMsimg32 = NULL;                   // handle to MSIMG32.dll
MSIMG32callBack FlexButton::m_dllGradientFillFunc = NULL;  // pointer to GradientFill() API

FlexButton::FlexButton():
m_textStyle(TS_SingleLineCenter),
m_bTracking(FALSE),
m_bChecked(FALSE),
m_bMultiClicked(FALSE),
m_bHandCursor(FALSE),
m_rgbTransparent(RGB(255,0,255)),
m_bDrawFocus(FALSE),
m_bButtonDown(FALSE),
m_bHot(TRUE),
m_hIcon(0),
m_hIconCheck(0),
m_hIconOwner(NULL),
m_hBitmap(NULL),
m_hBitmapOwner(NULL),
m_componentFlags(COMP_Text | COMP_Image),
m_nBitmapCx(0),
m_nBitmapCy(0),
m_nIconCx(0),
m_nIconCy(0)
{
	m_colors[COL_Bk] = GetSysColor(COLOR_BTNFACE);
	m_colors[COL_BkGrad] = RGB (128, 128, 128);
	m_colors[COL_Text] = GetSysColor(COLOR_BTNTEXT);

	m_colors[COL_Highlight] = GetSysColor (COLOR_BTNHIGHLIGHT);
	m_colors[COL_Shadow] = GetSysColor (COLOR_BTNSHADOW);

	m_colors[COL_Over] = RGB (255, 255, 255);
	m_colors[COL_OverGrad] = RGB (128, 128, 128);
	m_colors[COL_OverText] = RGB (0, 0, 0);

	m_colors[COL_Down] = m_colors[COL_Bk];
	m_colors[COL_DownGrad] = m_colors[COL_BkGrad];
	m_colors[COL_DownText] = m_colors[COL_Text];

	m_colors[COL_Checked] = GetSysColor (COLOR_3DLIGHT);
	m_colors[COL_CheckedGrad] = m_colors[COL_BkGrad];
	m_colors[COL_CheckedText] = m_colors[COL_Text];

	// Set default properties
	ApplyPredifinedButtonsStyle(PBS_PushButton);

	// Load MSIMG32.dll if possible
	if (m_nRefs == 0) 
	{
		ASSERT (m_hMsimg32 == NULL);
		m_hMsimg32 = LoadLibrary (_T("msimg32.dll"));
		if (m_hMsimg32 != NULL) 
		{
			m_dllGradientFillFunc = ((MSIMG32callBack) GetProcAddress (m_hMsimg32, "GradientFill"));
			ASSERT (m_dllGradientFillFunc != NULL);
		}
	}

	// Keep track of how many instances exist
	FlexButton::m_nRefs++;
}

//! Standard destructor.
FlexButton::~FlexButton()
{
	if (m_hIconOwner)
		DestroyIcon(m_hIconOwner);
	if (m_hBitmapOwner)
		DeleteObject(m_hBitmapOwner);
	// Free button group storage and MSIMG32.dll
	FlexButton::m_nRefs--;
	if (FlexButton::m_nRefs == 0) 
	{
		if (m_hMsimg32 != NULL) 
		{
			VERIFY (FreeLibrary (m_hMsimg32));
			m_dllGradientFillFunc = NULL;
			m_hMsimg32 = NULL;
		}
	}
}

BEGIN_MESSAGE_MAP(FlexButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

//! Sets the button's type.
//! @param  type    The button's type.
void FlexButton::ApplyPredifinedButtonsStyle(PredifinedButtonStyle style)
{
	//ST_Normal,
	//	ST_Down,
	//	ST_Over,
	//	ST_Checked,
	//	ST_Disabled,

	//BDE_Normal,
	//	BDE_Alter,
	//	BDE_Gradient,
	//	BDE_GradientAlter,
	//	BDE_Disabled,
	SetBackgroundDrawEffect(ST_Normal, BDE_Normal);
	SetBackgroundDrawEffect(ST_Down, BDE_Normal);
	SetBackgroundDrawEffect(ST_Over, BDE_Normal);
	SetBackgroundDrawEffect(ST_Checked, BDE_Normal);
	SetBackgroundDrawEffect(ST_Disabled, BDE_Disabled);

	//FDE_None,
	//	FDE_Normal,
	//	FDE_Down,
	SetFrameDrawEffect(ST_Normal, FDE_Normal);
	SetFrameDrawEffect(ST_Down, FDE_Down);
	SetFrameDrawEffect(ST_Over, FDE_Normal);
	SetFrameDrawEffect(ST_Checked, FDE_Down);
	SetFrameDrawEffect(ST_Disabled, FDE_Normal);

	//CBDE_Normal,
	//	CBDE_Down,
	//	CBDE_Disabled,
	SetCheckBoxDrawEffect(ST_Normal, CBDE_Normal);
	SetCheckBoxDrawEffect(ST_Down, CBDE_Down);
	SetCheckBoxDrawEffect(ST_Over, CBDE_Normal);
	SetCheckBoxDrawEffect(ST_Checked, CBDE_Normal);
	SetCheckBoxDrawEffect(ST_Disabled, CBDE_Disabled);



	//TDE_Normal,
	//	TDE_Over,
	//	TDE_Down,
	//	TDE_Disabled,
	//	TDE_Checked,
	SetTextDrawEffect(ST_Normal, TDE_Normal);
	SetTextDrawEffect(ST_Down, TDE_Down);
	SetTextDrawEffect(ST_Over, TDE_Normal);
	SetTextDrawEffect(ST_Checked, TDE_Checked);
	SetTextDrawEffect(ST_Disabled, TDE_Disabled);

	//IDE_Normal,
	//	IDE_Down,
	//	IDE_ShowCheckedIcon,
	//	IDE_ShowInvertedIcon,
	//	IDE_DrawOverlayIcon,
	//	IDE_Disabled,
	SetImageDrawEffect(ST_Normal, IDE_Normal);
	SetImageDrawEffect(ST_Down, IDE_Down);
	SetImageDrawEffect(ST_Over, IDE_Normal);
	SetImageDrawEffect(ST_Checked, IDE_Down);
	SetImageDrawEffect(ST_Disabled, IDE_Disabled);

	//DDDE_Normal,
	//	DDDE_Disabled,
	//	DDDE_Down,
	SetDropDownDrawEffect(ST_Normal, DDDE_Normal);
	SetDropDownDrawEffect(ST_Down, DDDE_Down);
	SetDropDownDrawEffect(ST_Over, DDDE_Over);
	SetDropDownDrawEffect(ST_Checked, DDDE_Down);
	SetDropDownDrawEffect(ST_Disabled, DDDE_Disabled);
	

	//SetDropDownDrawEffect(ST_Disabled, DDDE_Disabled);

	m_bChecked = FALSE;
	m_bCheckable = FALSE;
	m_bHandCursor = FALSE;
	switch (style) 
	{
	case PBS_PushButton:				//! Standard pushbutton
		break;
	case PBS_HotButton:
		SetFrameDrawEffect(ST_Normal, FDE_None);
		break;
	case PBS_HyperLink:
		SetBackgroundDrawEffect(ST_Normal, BDE_Normal);
		SetBackgroundDrawEffect(ST_Down, BDE_Normal);
		SetBackgroundDrawEffect(ST_Over, BDE_Normal);
		SetBackgroundDrawEffect(ST_Checked, BDE_Normal);

		SetFrameDrawEffect(ST_Normal, FDE_None);
		SetFrameDrawEffect(ST_Down, FDE_None);
		SetFrameDrawEffect(ST_Over, FDE_None);
		SetFrameDrawEffect(ST_Checked, FDE_None);
		SetFrameDrawEffect(ST_Disabled, FDE_None);

		SetImageDrawEffect(ST_Normal, IDE_Normal);
		SetImageDrawEffect(ST_Down, IDE_Normal);
		SetImageDrawEffect(ST_Over, IDE_Normal);
		SetImageDrawEffect(ST_Checked, IDE_Normal);

		SetTextDrawEffect(ST_Normal, TDE_Normal);
		SetTextDrawEffect(ST_Down, TDE_Normal);
		SetTextDrawEffect(ST_Over, TDE_Normal);
		SetTextDrawEffect(ST_Checked, TDE_Normal);
		m_bHandCursor = TRUE;

		SetColor(COL_Text, RGB(0,0,255));
		SetColor(COL_OverText, RGB(0,0,255));
		SetColor(COL_DownText, RGB(0,0,255));
		break;
	//case BT_PushButtonMulti:		//! Multi pushbutton with dropdown
	//	m_bMulti = TRUE;
	//	break;
	//case BT_CheckButton:			//! Standard check button
	//	m_bCheckButton = TRUE;
	//	break;
	//case BT_HyperLink:
	//	m_bHandCursor = TRUE;
	//	break;
	default:
		ASSERT(0);

	}
}

void FlexButton::SetBitmapId(INT nBitmapId)
{
	if (m_hBitmapOwner != NULL)
	{
		DeleteObject(m_hBitmapOwner);
		m_hBitmapOwner = NULL;
	}
	m_hBitmapOwner = ::LoadBitmap (AfxGetResourceHandle(), MAKEINTRESOURCE (nBitmapId));
	if (m_hBitmapOwner)
	{
		BITMAP  rBitmap;
		if (::GetObject(m_hBitmapOwner, sizeof(BITMAP), &rBitmap))
		{
			m_nBitmapCx = rBitmap.bmWidth;
			m_nBitmapCy = rBitmap.bmHeight;
		}
	}
}

void FlexButton::SetBitmap(HBITMAP hBitmap)
{
	if (hBitmap)
	{
		BITMAP  rBitmap;
		if (::GetObject(m_hBitmapOwner, sizeof(BITMAP), &rBitmap))
		{
			m_nBitmapCx = rBitmap.bmWidth;
			m_nBitmapCy = rBitmap.bmHeight;
		}
	}
}


void FlexButton::SetIconId(int nIconId, INT cx, INT cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	if (m_hIconOwner != NULL)
	{
		DestroyIcon(m_hIconOwner);
		m_hIconOwner = NULL;
	}
	m_hIconOwner = (HICON) ::LoadImage(AfxGetResourceHandle(),  MAKEINTRESOURCE ( nIconId ), IMAGE_ICON, m_nIconCx, m_nIconCy, LR_LOADTRANSPARENT);
	m_nIconCx = cx;
	m_nIconCy = cy;
}


void FlexButton::SetIcon(HICON hIcon, INT cx, INT cy)
{
	ASSERT(cx >= 0 && cy >= 0);
	m_nIconCx = cx;
	m_nIconCy = cy;
	m_hIcon = hIcon;
}


//! Displays a popup menu if the button type is pushButton, pushButtonDropDown,
//! hotPushButton or hotPushButtonDropDown.
//! @param    pMenu   The popup menu to be displayed.
//! @return   The menu selection.
int FlexButton::DisplayPopupMenu(CMenu* pMenu)
{
	Invalidate();
	if (pMenu != NULL) 
	{
		  CRect  rectButton;
		  GetWindowRect (&rectButton);
		  return (pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
			rectButton.left, rectButton.bottom, GetParent()));
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// FlexButton message handlers

void FlexButton::PreSubclassWindow() 
{
#ifdef _DEBUG
	// We really should be only subclassing a button control
	TCHAR buffer[255];
	GetClassName (m_hWnd, buffer, 255);
	ASSERT(_tcscmp(_T("Button"), buffer) == 0);
#endif
	// Check if it's a default button
	//if (GetStyle() & 0x0FL)
	//	m_bDefaultButton = true;
	// Make the button owner-drawn
	ModifyStyle (0x0FL, BS_OWNERDRAW, SWP_FRAMECHANGED);
	CButton::PreSubclassWindow();
}

void FlexButton::OnKillFocus(CWnd* pNewWnd)
{
	if (IsWindow(m_hWnd))
		Invalidate();
	CButton::OnKillFocus (pNewWnd);
}

void FlexButton::OnLButtonDown (UINT nFlags, CPoint point)
{
	if (MultiHitTest(point) == FALSE)
		m_bButtonDown = TRUE;
	else
		m_bMultiClicked = TRUE;
	//multiHitTest (point);   // sets m_bMultiClicked
	CButton::OnLButtonDown (nFlags, point);
	if (IsWindow(m_hWnd))
		Invalidate();
}

void FlexButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bButtonDown = FALSE;
	if (m_bCheckable && m_bMultiClicked == FALSE)
		m_bChecked = !m_bChecked;
	CButton::OnLButtonUp (nFlags, point);
	m_bMultiClicked = FALSE;
	if (IsWindow(m_hWnd))
		Invalidate();
}

void FlexButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bTracking) 
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		m_bTracking = _TrackMouseEvent(&tme);
		if (IsWindow(m_hWnd))
			Invalidate();
	}
	CButton::OnMouseMove(nFlags, point);
}

LRESULT FlexButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTracking = FALSE;
	m_bMultiClicked = FALSE;
	m_bButtonDown = FALSE;
	Invalidate();
	return 0;
}

BOOL FlexButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL FlexButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bHandCursor)
	{
		HCURSOR hCursor = ::LoadCursor (NULL, IDC_HAND);
		if (NULL != hCursor)
		{
			::SetCursor(hCursor);
			return TRUE;
		}
	}
	return CButton::OnSetCursor(pWnd, nHitTest, message);
}

void FlexButton::OnSetFocus(CWnd* pOldWnd)
{
	if (IsWindow(m_hWnd))
		Invalidate();
	CButton::OnSetFocus(pOldWnd);
}
//Alex - Calculates The size for a given text/icon
UINT FlexButton::CalcMinWidth() 
{
	INT width = 0;

	if (m_componentFlags & COMP_CheckBox)
		width += 2 * cMargin + cCheckBoxWidth;
	if (m_componentFlags & COMP_DropDown)
		width += cMargin + cDropDownWidth;
	if (m_componentFlags & COMP_Image)
	{
		UINT imageWidth = 0;
		if (m_nBitmapCx > 0)
			imageWidth += m_nBitmapCx;
		else if (m_nIconCx > 0)
			imageWidth += m_nIconCx;
		if (imageWidth > 0)
		{
			if (m_componentFlags != COMP_Image)
				imageWidth += cMargin;
			width += imageWidth;
		}
	}
	if (m_componentFlags & COMP_Text)
	{
		if (m_textStyle == TS_MultiLine)
			width += cMargin + 50;
		else
		{
			CString strCaption;
			GetWindowText (strCaption);
			CRect rc;
			GetClientRect(&rc);
			CClientDC dc(this);
			CFont* pOldFont = dc.SelectObject(GetFont());
			UINT dwFormat = DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_CALCRECT;
			::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
				&rc,
				dwFormat,
				NULL);
			dc.SelectObject(pOldFont);
			width += rc.Width() + 2 * cMargin;
		}
	}
	return width;
}

void FlexButton::AutoSize()
{
	UINT cx = CalcMinWidth();
	if (cx > 0 && GetParent() != NULL)
	{
		CRect rc;
		GetWindowRect(&rc);
		rc.right = rc.left + cx;
		SetWindowPos(NULL, 0,0,cx,rc.Height(), SWP_NOMOVE | SWP_NOZORDER);
	}
}


void FlexButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
	CRect rcItem(lpDrawItemStruct->rcItem);
	State state = ST_Normal;
	if (m_bChecked && m_bCheckable)
		state = ST_Checked;
	if (m_bTracking && m_bHot)
		state = ST_Over;
	if (m_bButtonDown)
		state = ST_Down;
	if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
		state = ST_Disabled;

	State imageState = state;
	if (state == ST_Over && m_bChecked && m_bCheckable)
		imageState = ST_Checked;


	DrawBackground(*pDC, rcItem, m_bkDrawEffect[state]);
	DrawFrame(*pDC, rcItem, m_frameDrawEffect[state]);
	CRect rcAvailable(rcItem);
	if (m_componentFlags & COMP_CheckBox)
	{
		CRect rc(rcItem);
		rc.left = cMargin;
		rc.right = rc.left + cCheckBoxWidth;
		DrawCheckBox(*pDC, rc, m_cbDrawEffect[state]);
		rcAvailable.left = rcItem.left + 2 * cMargin + cCheckBoxWidth;
	}
	if (m_componentFlags & COMP_DropDown)
	{
		CRect rc(rcItem);
		rc.left = rc.right - cDropDownWidth;
		DrawDropDown(*pDC, rc, m_dropDownDrawEffect[state]);
		rcAvailable.right = rcItem.right - cMargin - cDropDownWidth;
	}

	if ((m_componentFlags & COMP_Image) && (m_componentFlags & COMP_Text))
	{
		CRect rc(rcAvailable);
		rc.left += cMargin;
		INT curx = DrawImage(*pDC, rc, m_imageDrawEffect[imageState], FALSE);
		if (curx == 0)
			rc.left -= cMargin;
		rc.left += curx + cMargin;
		DrawCaption(*pDC, rc, m_textDrawEffect[state]);
	}
	else if (m_componentFlags & COMP_Image)
	{
		CRect rc(rcAvailable);
		if (m_componentFlags != COMP_Image)
			rc.left += cMargin;
		if (DrawImage(*pDC, rc, m_imageDrawEffect[imageState], TRUE) == 0)
			rc.left -= cMargin;
	}
	else if (m_componentFlags & COMP_Text)
	{
		CRect rc(rcAvailable);
		rc.left += cMargin;
		rc.right -= cMargin;
		DrawCaption(*pDC, rc, m_textDrawEffect[state]);
	}
	if (m_bDrawFocus && (lpDrawItemStruct->itemState & ODS_FOCUS) == ODS_FOCUS)
		DrawFocus(*pDC, rcItem);

}




void FlexButton::DrawBackground(CDC& dc, const CRect& rc, BackgroundDrawEffect effect)
{
	COLORREF bkColor = m_colors[COL_Bk];
	COLORREF bkColorGrad = m_colors[COL_BkGrad];
	switch (effect)
	{
	case BDE_None:
		break;
	case BDE_Disabled:
		//ASSERT(0);//TODO. Draw Normal for the moment
	case BDE_Normal:
		dc.FillSolidRect(&rc, m_colors[COL_Bk]);
		break;
	case BDE_Over:
		dc.FillSolidRect(&rc, m_colors[COL_Over]);
		break;
	case BDE_Down:
		dc.FillSolidRect(&rc, m_colors[COL_Down]);
		break;
	case BDE_Alter:
		dc.FillSolidRect(&rc, m_colors[COL_BkAlter]);
		break;
	case BDE_Gradient:
		DrawGradientRect(dc, rc, m_colors[COL_Bk], m_colors[COL_BkGrad], TRUE);
		break;
	case BDE_GradientAlter:
		DrawGradientRect(dc, rc, m_colors[COL_BkAlter], m_colors[COL_BkGradAlter], TRUE);
		break;
	default:
		ASSERT(0);
		break;
	}
}







/////////////////////////////////////////////////////////////////////////////
// FlexButton helper functions

//! Draws the button's frame
void FlexButton::DrawFrame(CDC& dc, const CRect& rc, FrameDrawEffect effect)
{
	switch (effect)
	{
	case FDE_Normal:
		dc.Draw3dRect (&rc, m_colors[COL_Highlight], m_colors[COL_Shadow]);
		break;
	case FDE_Down:
		dc.Draw3dRect (&rc, m_colors[COL_Shadow], m_colors[COL_Highlight]);
		break;
	case FDE_None:
		break;
	}
}

//! Draws the button's frame for a radio/checkbox button
void FlexButton::DrawCheckBox(CDC& dc, const CRect& rc, CheckBoxDrawEffect effect)
{
	// Draw frame
	UINT uFrameCtrl = DFCS_BUTTONCHECK;
	if (m_bChecked)
	uFrameCtrl |= DFCS_CHECKED;
	CRect rcItem(rc);
	if (effect == CBDE_Down)
		rcItem.OffsetRect(1,1);
	//if ((lpDrawItemStruct->itemState & ODS_SELECTED) == ODS_SELECTED)
	//   uFrameCtrl |= DFCS_PUSHED;
	//if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
	//   uFrameCtrl |= DFCS_INACTIVE;
	dc.DrawFrameControl(rcItem, DFC_BUTTON, uFrameCtrl);
}

//! Draws the button's bitmap
INT FlexButton::DrawImage(CDC& dc, const CRect& rc, ImageDrawEffect effect, BOOL bCentered)
{
	HBITMAP hBmp = m_hBitmapOwner;
	if (hBmp == NULL)
		hBmp = m_hBitmap;
	if (hBmp != NULL)
	{
		INT x = rc.left;
		if (bCentered)
			x = rc.left + (rc.Width() - m_nBitmapCx) / 2;

		INT y = rc.top + (rc.Height() - m_nBitmapCy) / 2;
		switch (effect)
		{
		case IDE_DrawOverlayIcon:
		case IDE_ShowCheckedIcon:
		case IDE_ShowInvertedIcon:
			ASSERT(0);
		case IDE_Normal:
			TransparentBlt (dc.m_hDC, x, y,
				m_nBitmapCx, m_nBitmapCy, hBmp, 0, 0, m_rgbTransparent, NULL);
			break;
		case IDE_Down:
			TransparentBlt (dc.m_hDC, x + 1, y + 1,
				m_nBitmapCx, m_nBitmapCy, hBmp, 0, 0, m_rgbTransparent, NULL);
			break;
		case IDE_Disabled:
			DisabledBlt (dc.m_hDC, x, y, m_nBitmapCx, m_nBitmapCy, hBmp, 0, 0);
			break;
		default:
			ASSERT(0);
			break;
		}
		return m_nBitmapCx;
	}

	HICON hIcon = m_hIconOwner;
	if (hIcon == NULL)
		hIcon = m_hIcon;
	if (hIcon != NULL)
	{
		INT x = rc.left;
		if (bCentered)
			x = rc.left + (rc.Width() - m_nIconCx) / 2;
		INT y = rc.top + (rc.Height() - m_nIconCy) / 2;
		switch (effect)
		{
		case IDE_Disabled:
			DrawState(dc.m_hDC, 0, 0, (LPARAM)hIcon, 0, x, y, m_nIconCx, m_nIconCy, DST_ICON | DSS_UNION | DSS_DISABLED);
			break;
		case IDE_Normal:
			DrawIconEx(dc.m_hDC, x, y, hIcon,
				m_nIconCx, m_nIconCy, 0, 0, DI_NORMAL);
			break;
		case IDE_Down:
			DrawIconEx(dc.m_hDC, x + 1, y + 1, hIcon,
				m_nIconCx, m_nIconCy, 0, 0, DI_NORMAL);
			break;
		case IDE_DrawOverlayIcon:
			DrawIconEx(dc.GetSafeHdc(), x, y, m_hIcon, m_nIconCx, m_nIconCy, 0, NULL, DI_NORMAL );
			if (m_hIconCheck != 0)
				DrawIconEx(dc.GetSafeHdc(), x, y, m_hIconCheck, m_nIconCx, m_nIconCy, 0, NULL, DI_NORMAL );
			break;
		case IDE_ShowCheckedIcon:
			if (m_hIconCheck != NULL)
				DrawIconEx(dc.GetSafeHdc(), x, y, m_hIconCheck, m_nIconCx, m_nIconCy, 0, NULL, DI_NORMAL );
			break;
		case IDE_ShowInvertedIcon:
			dc.BitBlt(x, y, m_nIconCx, m_nIconCy, &dc, 0,0,PATINVERT);
			DrawIconEx(dc.m_hDC, x, y, m_hIcon, m_nIconCx, m_nIconCy, 0, 0, DI_NORMAL);
			dc.BitBlt(x, y, m_nIconCx, m_nIconCy, &dc, 0,0,PATINVERT);
			break;
		default:
			ASSERT(0);
			break;
		}
		return m_nIconCx;
	}
	return 0;
}






//! Draws the button's caption
void FlexButton::DrawCaption(CDC& dc, const CRect& rc, TextDrawEffect effect)
{
	CString strCaption;
	GetWindowText (strCaption);

	// Determine drawing format
	DWORD  dwFormat = 0;
	switch (m_textStyle)
	{
	case TS_SingleLine:
		dwFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_LEFT;
		break;
	case TS_SingleLineCenter:
		dwFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_CENTER;
		break;
	case TS_MultiLine:
		dwFormat = DT_WORDBREAK | DT_LEFT | DT_VCENTER;
		break;
	}

	CFont* pOldFont = dc.SelectObject(GetFont());
	dc.SetBkMode(TRANSPARENT);
	CRect rectCaption(rc);
	switch (effect)
	{
	case TDE_Disabled:
		{
			::OffsetRect (&rectCaption, 1, 1);
			::SetTextColor (dc.m_hDC, ::GetSysColor (COLOR_3DHILIGHT));
			::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
				&rectCaption,
				dwFormat,
				NULL);

			::OffsetRect (&rectCaption, -1, -1);
			::SetTextColor(dc.m_hDC, ::GetSysColor (COLOR_GRAYTEXT));
			::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
				&rectCaption,
				dwFormat,
				NULL);
		}

		break;
	case TDE_Normal:
		dc.SetTextColor(m_colors[COL_Text]);
		::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
			&rectCaption,
			dwFormat,
			NULL);
		break;
	case TDE_Checked:
		dc.SetTextColor(m_colors[COL_CheckedText]);
		::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
			&rectCaption,
			dwFormat,
			NULL);
		break;
	case TDE_Down:
		rectCaption.OffsetRect(1, 1);
		dc.SetTextColor(m_colors[COL_DownText]);
		::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
			&rectCaption,
			dwFormat,
			NULL);
		break;
	case TDE_Over:
		dc.SetTextColor(m_colors[COL_OverText]);
		::DrawTextEx (dc.m_hDC, (LPTSTR)(LPCTSTR)strCaption, strCaption.GetLength(),
			&rectCaption,
			dwFormat,
			NULL);
		break;
	default:
		break;
	}
	dc.SelectObject (pOldFont);
}

//! Draws the button's drop-down indicator
void FlexButton::DrawDropDown(CDC& dc, const CRect& rc, DropDownDrawEffect effect)
{
	// Create pen of appropriate color
	COLORREF rgbPen = GetColor (COL_Text);
	switch (effect)
	{
	case DDDE_Disabled:
		rgbPen = ::GetSysColor (COLOR_GRAYTEXT);
		break;
	case DDDE_Over:
		rgbPen = m_colors[COL_OverText];
		break;
	}
	CPen pen;
	VERIFY (pen.CreatePen (PS_SOLID | PS_INSIDEFRAME, 1, rgbPen));

	// Set X and Y offsets
	int nOffsetX = 0;
	if (effect == DDDE_Down || m_bMultiClicked)
		nOffsetX = 1;
	int nOffsetY = nOffsetX;

	// Draw the indicator
	CPen* pOldPen = dc.SelectObject(&pen);

	INT nX = rc.left + (rc.Width() - 6 ) / 2 + nOffsetX;
	INT nY = rc.bottom - 3 - 5 + nOffsetY;

	for (INT i = 0; i < 6; i++)
	{
		if (i > 6 - i)
			break;
		dc.MoveTo(nX + i, nY + i);
		dc.LineTo(nX + 6 - i, nY + i);
	}

	dc.SelectObject(pOldPen);
	if (m_bMultiClicked) 
	{
		CRect rcTemp(rc);
		dc.DrawEdge (&rcTemp, BDR_SUNKENOUTER, BF_RECT);
	}
}

//! Draws the button's focus rectangle
void FlexButton::DrawFocus(CDC& dc, const CRect& rc)
{
	CRect rectButton(rc);
	rectButton.DeflateRect(4,4,4,4);
	::DrawFocusRect(dc.m_hDC, &rectButton);
}

//! Draws the border for a default button
//void FlexButton::drawDefaultBorder(CDC* pDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
//{
//	ASSERT (pDC != NULL);
//	ASSERT (lpDrawItemStruct != NULL);
//
//	//// Only pushbuttons can have a default border
//	//FlexButton::Type btnType = getType();
//	//if ((btnType != FlexButton::pushButton) &&
//	//(btnType != FlexButton::pushButtonDropDown) &&
//	//(btnType != FlexButton::pushButtonMulti))
//	//return;
//
//	// Nothing to do if the default border isn't required
//
//	// Nothing do if the button doesn't have focus or isn't the default button
//	if (((lpDrawItemStruct->itemState & ODS_FOCUS) == 0) || !m_bDefaultButton)
//		return;
//
//	COLORREF rgbBorder = GetSysColor (COLOR_3DDKSHADOW);
//	CBrush borderBrush (rgbBorder);
//	CRect innerRect = lpDrawItemStruct->rcItem;
//	if ((lpDrawItemStruct->itemState & ODS_SELECTED) == 0) 
//	{
//
//		COLORREF rgbBorder = GetSysColor (COLOR_3DDKSHADOW);
//		CPen borderGreyPen (PS_SOLID, 1, rgbBorder);
//		CPen* pOldPen = pDC->SelectObject (&borderGreyPen );
//
//		pDC->MoveTo (innerRect.right - 2, innerRect.top + 1);
//		pDC->LineTo (innerRect.right - 2, innerRect.bottom - 2);
//		pDC->LineTo (innerRect.left, innerRect.bottom - 2);
//
//		COLORREF rgbBrGrey = GetSysColor (COLOR_3DHILIGHT);
//		CPen brightGreyPen (PS_SOLID, 1, rgbBrGrey);
//		pOldPen = pDC->SelectObject (&brightGreyPen);
//
//		pDC->MoveTo (innerRect.right - 3, innerRect.top + 1);
//		pDC->LineTo (innerRect.left + 1, innerRect.top + 1);
//		pDC->LineTo (innerRect.left + 1, innerRect.bottom - 2);
//
//		COLORREF rgbDkGrey = GetSysColor (COLOR_BTNSHADOW);
//		CPen darkGreyPen (PS_SOLID, 1, rgbDkGrey);
//		pOldPen = pDC->SelectObject (&darkGreyPen);
//		pDC->MoveTo (innerRect.right - 3, innerRect.top + 2);
//		pDC->LineTo (innerRect.right - 3, innerRect.bottom - 3);
//		pDC->LineTo (innerRect.left + 1, innerRect.bottom - 3);
//		pDC->SelectObject (pOldPen);
//
//	} 
//	else 
//	{
//		innerRect.InflateRect (1, 1, -1, -1);
//		pDC->FrameRect (&innerRect, &borderBrush);
//	}
//	pDC->FrameRect(&lpDrawItemStruct->rcItem, &borderBrush);
//}

//
//  Purpose:
//    Copies a bitmap transparently onto the destination DC.
//    See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1749/
//
//  Returns:
//    None
//
void FlexButton::TransparentBlt
  (HDC      hdcDest,            // destination device context
   int      nXDest,             // x-coord of destination rectangle's upper-left corner
   int      nYDest,             // y-coord of destination rectangle's upper-left corner
   int      nWidth,             // width of destination rectangle
   int      nHeight,            // height of destination rectangle
   HBITMAP  hBitmap,            // source bitmap
   int      nXSrc,              // x-coord of source rectangle's upper-left corner
   int      nYSrc,              // y-coord of source rectangle's upper-left corner
   COLORREF colorTransparent,   // transparent color
   HPALETTE hPal)               // logical palette to be used with bitmap (may be NULL)
{
  CDC dc, memDC, maskDC, tempDC;
  dc.Attach( hdcDest );
  maskDC.CreateCompatibleDC(&dc);
  CBitmap maskBitmap;

  // These store return of SelectObject() calls
  CBitmap* pOldMemBmp = NULL;
  CBitmap* pOldMaskBmp = NULL;
  HBITMAP hOldTempBmp = NULL;

  memDC.CreateCompatibleDC (&dc);
  tempDC.CreateCompatibleDC (&dc);
  CBitmap bmpImage;
  bmpImage.CreateCompatibleBitmap (&dc, nWidth, nHeight);
  pOldMemBmp = memDC.SelectObject (&bmpImage);

  // Select and realize the palette
  if (dc.GetDeviceCaps (RASTERCAPS) & RC_PALETTE && hPal) {
     ::SelectPalette( dc, hPal, FALSE );
     dc.RealizePalette();
     ::SelectPalette( memDC, hPal, FALSE );
  }

  hOldTempBmp = (HBITMAP) ::SelectObject (tempDC.m_hDC, hBitmap);
  memDC.BitBlt (0, 0, nWidth, nHeight, &tempDC, nXSrc, nYSrc, SRCCOPY);

  // Create monochrome bitmap for the mask
  maskBitmap.CreateBitmap (nWidth, nHeight, 1, 1, NULL);
  pOldMaskBmp = maskDC.SelectObject (&maskBitmap);
  memDC.SetBkColor (colorTransparent);

  // Create the mask from the memory DC
  maskDC.BitBlt (0, 0, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);

  // Set the background in memDC to black. Using SRCPAINT with black
  // and any other color results in the other color, thus making
  // black the transparent color
  memDC.SetBkColor (RGB (0,0,0));
  memDC.SetTextColor (RGB (255,255,255));
  memDC.BitBlt (0, 0, nWidth, nHeight, &maskDC, 0, 0, SRCAND);

  // Set the foreground to black. See comment above.
  dc.SetBkColor (RGB (255,255,255));
  dc.SetTextColor (RGB (0,0,0));
  dc.BitBlt (nXDest, nYDest, nWidth, nHeight, &maskDC, 0, 0, SRCAND);

  // Combine the foreground with the background
  dc.BitBlt (nXDest, nYDest, nWidth, nHeight, &memDC, 0, 0, SRCPAINT);

  if (hOldTempBmp)
     ::SelectObject (tempDC.m_hDC, hOldTempBmp);
  if (pOldMaskBmp)
     maskDC.SelectObject (pOldMaskBmp);
  if (pOldMemBmp)
     memDC.SelectObject (pOldMemBmp);

  dc.Detach();
}

//
//  Purpose:
//    Draws a bitmap in a disabled mode onto the destination DC.
//    See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1699/
//
//  Returns:
//    None
//
void FlexButton::DisabledBlt
  (HDC      hdcDest,    // destination DC
   int      nXDest,     // x coord of destination rectangle's upper left corner
   int      nYDest,     // y coord of destination rectangle's upper left corner
   int      nWidth,     // width of destination rectangle
   int      nHeight,    // height of destination rectangle
   HBITMAP  hBitmap,    // source bitmap
   int      nXSrc,      // x-coord of source rectangle's upper-left corner
   int      nYSrc)      // y-coord of source rectangle's upper-left corner
{
  ASSERT (hdcDest && hBitmap);
  ASSERT (nWidth > 0 && nHeight > 0);

  // Create a generic DC for all BitBlts
  HDC hDC = CreateCompatibleDC (hdcDest);
  ASSERT (hDC);

  if (hDC) {

       // Create a DC for the monochrome DIB section
       HDC bwDC = CreateCompatibleDC(hDC);
       ASSERT (bwDC);

       if (bwDC) {

            // Create the monochrome DIB section with a black and white palette
            struct
            {
              BITMAPINFOHEADER  bmiHeader;
              RGBQUAD           bmiColors[2];
            } RGBBWBITMAPINFO = {
                                  {                               // a BITMAPINFOHEADER
                                    sizeof(BITMAPINFOHEADER),     // biSize
                                    nWidth,                       // biWidth;
                                    nHeight,                      // biHeight;
                                    1,                            // biPlanes;
                                    1,                            // biBitCount
                                    BI_RGB,                       // biCompression;
                                    0,                            // biSizeImage;
                                    0,                            // biXPelsPerMeter;
                                    0,                            // biYPelsPerMeter;
                                    0,                            // biClrUsed;
                                    0                             // biClrImportant;
                                  },
                                  {
                                    { 0x00, 0x00, 0x00, 0x00 },
                                    { 0xFF, 0xFF, 0xFF, 0x00 }
                                  }
                                };
            void*     pbitsBW;
            HBITMAP   hBitmapBW = CreateDIBSection (bwDC,
                                                    (LPBITMAPINFO) &RGBBWBITMAPINFO,
                                                    DIB_RGB_COLORS, &pbitsBW, NULL, 0);
            ASSERT (hBitmapBW);

            if (hBitmapBW) {
                 // Attach the monochrome DIB section and the bitmap to the DCs
                 SelectObject (bwDC, hBitmapBW);
                 SelectObject (hDC, hBitmap);

                 // BitBlt the bitmap into the monochrome DIB section
                 BitBlt (bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);

                 // Paint the destination rectangle in gray
                 FillRect (hdcDest,
                           CRect (nXDest, nYDest, nXDest + nWidth, nYDest + nHeight),
                           GetSysColorBrush (COLOR_3DFACE));

                 // BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
                 // The magic ROP comes from the Charles Petzold's book
                 HBRUSH hb = CreateSolidBrush (GetSysColor (COLOR_3DHILIGHT));
                 HBRUSH oldBrush = (HBRUSH) SelectObject (hdcDest, hb);
                 BitBlt (hdcDest, nXDest + 1, nYDest + 1, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);

                 // BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
                 hb = CreateSolidBrush (GetSysColor (COLOR_3DSHADOW));
                 DeleteObject (SelectObject(hdcDest, hb));
                 BitBlt (hdcDest, nXDest, nYDest, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
                 DeleteObject (SelectObject (hdcDest, oldBrush));
            }
            VERIFY (DeleteDC (bwDC));
       }
       VERIFY (DeleteDC(hDC));
  }
}

////! Draws the button's drop-down triangle.
//void FlexButton::drawDropDownIndicator
//  (HDC      hdcDest,            // destination DC
//   int      nX,                 // x coord of dropdown indicator's upper left vertex
//   int      nY,                 // y coord of dropdown indicator's upper left vertex
//   int      nWidthDropDown,     // width of indicator
//   int      nHeightDropDown)    // height of indicator
//{
//int   nCounter;   // generic counter
//
//  ASSERT (hdcDest);
//  for (nCounter = 0; (nCounter < nHeightDropDown); nCounter++) {
//
//      POINT   ptStart;    // starting point
//      int     nWidth;     // line width
//
//      ptStart.x = nX + nCounter;
//      ptStart.y = nY + nCounter;
//      nWidth = nWidthDropDown - nCounter;
//      if (nWidth < 0)
//         break;
//
//      ::MoveToEx (hdcDest, ptStart.x, ptStart.y, NULL);
//      ::LineTo (hdcDest, ptStart.x + nWidth - nCounter, ptStart.y);
//  }
//}

//! Checks if the mouse was clicked on multi part of button.
BOOL FlexButton::MultiHitTest(CPoint pt)
{
	if (m_componentFlags & COMP_DropDown)
	{
		CRect rect;
		GetClientRect(rect);
		rect.left = rect.right - cDropDownWidth;
		if (rect.PtInRect (pt))
			return TRUE;
	}
	return FALSE;
}

//! Draws a gradient rectangle.
void FlexButton::DrawGradientRect(CDC& dc, const CRect& rc, COLORREF cLeft, COLORREF cRight, BOOL bVertical)
{
	TRIVERTEX rcVertex[2];
	rcVertex[0].x = rc.left;
	rcVertex[0].y = rc.top;
	rcVertex[0].Red = (unsigned short) (GetRValue (cLeft) << 8);
	rcVertex[0].Green = (unsigned short) (GetGValue (cLeft) << 8);
	rcVertex[0].Blue = (unsigned short) (GetBValue (cLeft) << 8);
	rcVertex[0].Alpha = 0;

	rcVertex[1].x = rc.right;
	rcVertex[1].y = rc.bottom;
	rcVertex[1].Red = (unsigned short) (GetRValue (cRight) << 8);
	rcVertex[1].Green = (unsigned short) (GetGValue (cRight) << 8);
	rcVertex[1].Blue = (unsigned short) (GetBValue (cRight) << 8);
	rcVertex[1].Alpha = 0;
	GRADIENT_RECT gRect;
	gRect.UpperLeft = 0;
	gRect.LowerRight = 1;
	dc.GradientFill(rcVertex, 2, &gRect, 1,
		bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void FlexButton::SetColor(COLOR idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (m_hWnd)		Invalidate();
	}
}
COLORREF FlexButton::GetColor(COLOR idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

