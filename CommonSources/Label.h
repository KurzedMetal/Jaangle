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

class CLabel : public CWnd
{
	DECLARE_DYNAMIC(CLabel)

public:
	CLabel();
	virtual ~CLabel();

	BOOL Create(LPCTSTR caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_Last
	};
	virtual void SetColor(COLORS idx, COLORREF value);
	virtual COLORREF GetColor(COLORS idx);

	void SetDrawStringParams(DWORD params)			{m_drawStringParams = params;}
	DWORD GetDrawStringParams()						{return m_drawStringParams;}
	void CalculateSize(LPCTSTR text, int textLen, OUT SIZE& size);
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnGetText(WPARAM, LPARAM);
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_colors[COL_Last];
	DWORD m_drawStringParams;
	std::tstring m_text;
};


