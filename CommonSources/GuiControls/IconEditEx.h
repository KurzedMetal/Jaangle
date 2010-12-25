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
#ifndef _CIconEditEx_h_
#define _CIconEditEx_h_


//Single Line Edit Box with an Icon
//You need to set ES_MULTILINE style
//You can Set the Text Color / Bk Color
//You can also set a progress value (showing on top for 1px height)

class CIconEditEx : public CEdit
{
// Construction/destruction
public:
	CIconEditEx();
	virtual ~CIconEditEx();

// Operations
public:
	void SetIconDimentions(INT cx, INT cy);
	void SetIcon(HICON icon);
	void SetIcon(UINT iconres);
	void SetClearIcon(HICON icon);

	void SetEmptyMessage(LPCTSTR txt);

	enum Colors
	{
		COL_Bk,
		COL_Text,
		COL_Progress,
		COL_EmptyMessage,
		COL_Last
	};
	void SetColor(Colors col, COLORREF crColor);

	//Value should be between 0 - 0xFFFF
	//If > 0 it will show a text colored bar (2px) over the text
	void SetProgressValue(DOUBLE value);
	DOUBLE GetProgressValue()						{return m_progressValue;}

protected:
	BOOL PointOverClearIcon(CPoint& point);
	virtual void PreSubclassWindow();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
	afx_msg BOOL OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam); // Maps to WM_SETFONT
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor); // This Function Gets Called Every Time Your Window Gets Redrawn.
	DECLARE_MESSAGE_MAP()



private:
	void ReSize(int cx = 0, int cy = 0);
	void ClearIcon();
private:	// Attributes
	COLORREF m_colors[COL_Last];
	CString		m_textIfEmpty;
	HICON		m_hIcon;
	HICON		m_hClearIcon;
	HICON		m_hIconInternal;
	INT m_cx;
	INT m_cy;
	CBrush m_brBkgnd; // Holds Brush Color for the Edit Box
	DOUBLE m_progressValue;
};

#endif // !defined(AFX_ICONEDIT_H__AEA6DB87_1AB8_4BC0_8572_1A0ECB665F4A__INCLUDED_)
