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

#ifndef FlexButton_h
#define FlexButton_h

typedef UINT (CALLBACK* MSIMG32callBack)(HDC,CONST PTRIVERTEX,DWORD,CONST PVOID,DWORD,DWORD);

class FlexButton : public CButton
{
public:
	FlexButton();
	virtual ~FlexButton();

/////////////////////////////////////////////////////////////////////////////
// Attributes
public:
	enum PredifinedButtonStyle
	{
		PBS_PushButton,			//! Standard pushbutton
		PBS_HotButton,			//! On over draws a hot rectangle
		PBS_PushButtonMulti,	//! Multi pushbutton with dropdown
		PBS_CheckButton,		//! Standard check button
		PBS_HyperLink,			//! Standard check button
		PBS_Last
	};

	enum TextStyle
	{
		TS_SingleLine,			//! Single-line left-justified text
		TS_SingleLineCenter,	//! Single-line centered text
		TS_MultiLine,			//! Multi-line left-justified text
		TS_Last
	};

	enum BackgroundDrawEffect
	{
		BDE_Normal,
		BDE_Alter,
		BDE_Over,
		BDE_Down,
		BDE_Gradient,
		BDE_GradientAlter,
		BDE_Disabled,
		BDE_None,
		BDE_Last
	};

	enum FrameDrawEffect
	{
		FDE_None,
		FDE_Normal,
		FDE_Down,
		FDE_Last
	};

	enum CheckBoxDrawEffect
	{
		CBDE_Normal,
		CBDE_Down,
		CBDE_Disabled,
		CBDE_Last
	};

	enum TextDrawEffect
	{
		TDE_Normal,
		TDE_Over,
		TDE_Down,
		TDE_Disabled,
		TDE_Checked,
		TDE_Alter,
		TDE_Last
	};

	enum ImageDrawEffect
	{
		IDE_Normal,
		IDE_Down,
		IDE_ShowCheckedIcon,
		IDE_ShowInvertedIcon,
		IDE_DrawOverlayIcon,
		IDE_Disabled,
		IDE_Last
	};

	enum DropDownDrawEffect
	{
		DDDE_Normal,
		DDDE_Disabled,
		DDDE_Down,
		DDDE_Over,
		DDDE_Last
	};

	enum State
	{
		ST_Normal,
		ST_Down,
		ST_Over,
		ST_Checked,
		ST_Disabled,
		ST_Last
	};
/////////////////////////////////////////////////////////////////////////////
// Operations
public:
	enum Components
	{
		COMP_CheckBox	= 1,
		COMP_Image		= 2,
		COMP_Text		= 4,
		COMP_DropDown	= 8
	};
	void SetComponents(UINT componentFlags)		{m_componentFlags = componentFlags;}
	UINT GetComponentFlags()					{return m_componentFlags;}
	void SetHotButton(BOOL bHot)				{m_bHot = bHot;}
	BOOL IsHotButton()							{return m_bHot;}
	void SetCheck(BOOL bCheck)					{m_bChecked = bCheck;}
	BOOL IsChecked()							{return m_bChecked;}
	void SetCheckable(BOOL bCheckable)			{m_bCheckable = bCheckable;}
	BOOL IsCheckable()							{return m_bCheckable;}
	void SetDrawFocus(BOOL bDrawFocus)			{m_bDrawFocus = bDrawFocus;}
	BOOL IsDrawFocus()							{return m_bDrawFocus;}

	void SetFrameDrawEffect(State state, FrameDrawEffect effect)	{m_frameDrawEffect[state] = effect;}
	FrameDrawEffect GetFrameDrawEffect(State state)					{return m_frameDrawEffect[state];}
	void SetBackgroundDrawEffect(State state, BackgroundDrawEffect effect)	{m_bkDrawEffect[state] = effect;}
	BackgroundDrawEffect GetBackgroundDrawEffect(State state)				{return m_bkDrawEffect[state];}

	void SetCheckBoxDrawEffect(State state, CheckBoxDrawEffect effect)	{m_cbDrawEffect[state] = effect;}
	CheckBoxDrawEffect GetCheckBoxDrawEffect(State state)				{return m_cbDrawEffect[state];}

	
	
	
	void SetDropDownDrawEffect(State state, DropDownDrawEffect effect)	{m_dropDownDrawEffect[state] = effect;}
	DropDownDrawEffect GetDropDownDrawEffect(State state)				{return m_dropDownDrawEffect[state];}
	void SetImageDrawEffect(State state, ImageDrawEffect effect)	{m_imageDrawEffect[state] = effect;}
	ImageDrawEffect GetImageDrawEffect(State state)					{return m_imageDrawEffect[state];}
	void SetTextDrawEffect(State state, TextDrawEffect effect)		{m_textDrawEffect[state] = effect;}
	TextDrawEffect GetTextDrawEffect(State state)					{return m_textDrawEffect[state];}
	
	
	void SetTextStyle(TextStyle textStyle)		{m_textStyle = textStyle;}
	TextStyle GetTextStyle()					{return m_textStyle;}

	BOOL IsHandCursorEnabled()					{return m_bHandCursor;}
	BOOL SetHandCursor(BOOL bEnable)			{m_bHandCursor = bEnable;}


	void SetBitmapId(INT nBitmapId);
	void SetBitmap(HBITMAP hBitmap);
	void SetBitmapTransparentColor(COLORREF col)	{m_rgbTransparent = col;}
	void SetIconId(INT nIconId, INT cx, INT cy);
	void SetIcon(HICON hIcon, INT cx, INT cy);
	void SetCheckIcon(HICON hIconCheck)				{m_hIconCheck = hIconCheck;}


	//GetCheck()
private:
	UINT m_componentFlags;
	BOOL m_bHot;
	BOOL m_bChecked;
	BOOL m_bCheckable;
	BOOL m_bDrawFocus;
	BOOL m_bButtonDown;

	FrameDrawEffect m_frameDrawEffect[ST_Last];
	ImageDrawEffect m_imageDrawEffect[ST_Last];
	TextDrawEffect m_textDrawEffect[ST_Last];
	BackgroundDrawEffect m_bkDrawEffect[ST_Last];
	CheckBoxDrawEffect m_cbDrawEffect[ST_Last];
	DropDownDrawEffect m_dropDownDrawEffect[ST_Last];
	TextStyle m_textStyle;

public:
	void ApplyPredifinedButtonsStyle(PredifinedButtonStyle style);






	BOOL IsMultiClicked()					{return m_bMultiClicked;}
	void ResetMultiClicked()				{m_bMultiClicked = FALSE;}

	INT DisplayPopupMenu(CMenu* pMenu);

	UINT CalcMinWidth();
	void AutoSize();

/////////////////////////////////////////////////////////////////////////////
// Overrides

public:
	enum COLOR
	{
		COL_Bk,
		COL_BkGrad,			//Used only if Gradient is enabled
		COL_BkAlter,
		COL_BkGradAlter,	//Used only if Gradient is enabled
		COL_Text,
		COL_Highlight,
		COL_Shadow,
		COL_Over,
		COL_OverGrad,		//Used only if Gradient is enabled
		COL_OverText,
		COL_Down,
		COL_DownGrad ,		//Used only if Gradient is enabled
		COL_DownText,
		COL_Checked,
		COL_CheckedGrad ,	//Used only if Gradient is enabled
		COL_CheckedText,
		COL_Last
	};
	void SetColor(COLOR idx, COLORREF value);
	COLORREF GetColor(COLOR idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// Implementation
	// Generated message map functions
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//{{AFX_MSG(FlexButton)
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave (WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove (UINT nFlags, CPoint point);
	afx_msg void OnKillFocus (CWnd* pNewWnd);
	afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSetFocus (CWnd* pOldWnd);
	virtual void PreSubclassWindow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL m_bTracking;
	BOOL m_bMultiClicked;
	BOOL m_bHandCursor;
	COLORREF m_rgbTransparent;

	// Number of FlexButton instances.
	static int m_nRefs;

	//! Handle to msimg32.dll.
	static HINSTANCE m_hMsimg32;

	//! Pointer to gradient fill function in msimg32.dll.
	static MSIMG32callBack  m_dllGradientFillFunc;

	//UINT m_nIconID;
	INT m_nIconCx;
	INT m_nIconCy;
	INT m_nBitmapCx;
	INT m_nBitmapCy;
	HICON m_hIconOwner;
	HBITMAP m_hBitmap;
	HBITMAP m_hBitmapOwner;
	HICON m_hIcon;
	HICON m_hIconCheck;

/////////////////////////////////////////////////////////////////////////////
// Helper functions
protected:
	void DrawBackground(CDC& dc, const CRect& rc, BackgroundDrawEffect effect);
	void DrawFrame(CDC& dc, const CRect& rc, FrameDrawEffect effect);
	void DrawCheckBox(CDC& dc, const CRect& rc, CheckBoxDrawEffect effect);
	INT DrawImage(CDC& dc, const CRect& rc, ImageDrawEffect effect, BOOL bCentered);
	void DrawCaption(CDC& dc, const CRect& rc, TextDrawEffect effect);
	void DrawDropDown(CDC& dc, const CRect& rc, DropDownDrawEffect effect);
	void DrawFocus(CDC& dc, const CRect& rc);


	static void DrawGradientRect(CDC& pDC, const CRect& rc, COLORREF cLeft, COLORREF cRight, BOOL bVertical);
	//! Draws a bitmap with transparency color.
	//! See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1749/
	static void TransparentBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth,
		int nHeight, HBITMAP hBitmap, int nXSrc, int nYSrc,
		COLORREF colorTransparent, HPALETTE hPal);
	//! Draws a bitmap as disabled.
	//! See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1699/
	static void DisabledBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth,
	int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);


	//! Checks if the mouse was clicked on multi part of button.
	BOOL MultiHitTest(CPoint pt);


};

#endif

// End FlexButton.h
