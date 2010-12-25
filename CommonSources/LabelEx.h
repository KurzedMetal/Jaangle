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

// CLabelEx
//#define BORDERSTATIC_MAXTEXTLEN 300

class CLabelEx : public CWnd
{
	DECLARE_DYNAMIC(CLabelEx)

public:
	enum BORDER_TYPE
	{
		B_NoBorder,
		B_Rect,
		B_RoundRect,	//needs xParam1, xParam2,
		B_BottomLine,
		B_LeftLine,
		B_Last
	};
	CLabelEx();
	virtual ~CLabelEx();

	/*!
	* \brief
	* Sets the Border Type
	* 
	* \param bt
	* The Border
	* 
	* \param xParam1
	* Used in RoundRect Border Type
	* 
	* \param xParam2
	* Used in RoundRect Border Type
	*/
	void SetBorderType(BORDER_TYPE bt, INT xParam1 = 0, INT xParam2 = 0);
	void SetAutoWidth(BOOL bAutoWidth)				{m_bAutoWidth = bAutoWidth;}
	void SetMargins(INT xMargin, INT yMargin);
	INT CalculateWidth();

	virtual void SetIconProperties(INT cxIcon, INT cyIcon);
	virtual void AddIcon(HICON hIcon);
	virtual void ClearIcons();
	virtual INT GetIconCount();
	//If the cursor is over an icon it returns the icon index 0-1-2 ...
	//If not it returns -1
	virtual INT GetIconIndexFromPoint(CPoint& pt);

	/*!
	 * \brief
	 * Special Font Setting
	 * 
	 * \remarks
	 * You can make a set window text like this "test <s>special text</s> test"
	 * Special text will be drawed with the defaul setting but with the special font if provided	 * Write remarks for SetSpecialFont here.
	 */
	virtual void SetSpecialFont(CFont* pFont)	{m_pSpecialFont = pFont;}
	virtual CFont* GetSpecialFont()				{return m_pSpecialFont;}

	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_Erase,
		COL_Fore,
		COL_Last
	};
	virtual void SetColor(COLORS idx, COLORREF value);
	virtual COLORREF GetColor(COLORS idx);

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnGetText(WPARAM, LPARAM);
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

	void DrawString(CDC& dc, CRect& rc, BOOL bCalcOnly);
	INT CalculateWidthInternal(CDC& dc);
private:
	COLORREF m_colors[COL_Last];
	std::vector<HICON> m_hIcons;
	INT m_cxIcon;
	INT m_cyIcon;
	BOOL m_bEraseBk;
	BORDER_TYPE m_bt;
	INT m_xParam1;
	INT m_xParam2;
	INT m_xMargin;
	INT m_yMargin;
	CBrush m_bkBrush;
	CPen m_Pen;
	UINT m_ExtraStyle;
	std::tstring m_text;
	CFont* m_pSpecialFont;
	BOOL m_bAutoWidth;
	

};


