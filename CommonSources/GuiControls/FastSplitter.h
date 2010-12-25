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
#ifndef _FASTSPLITTER_H_
#define _FASTSPLITTER_H_

//#include "ICtrlColors.h"

class CFastSplitter : public CWnd//, public ICtrlColors
{
public:
	CFastSplitter();
	virtual ~CFastSplitter();

	enum MODE
	{
		MOD_Normal,
		MOD_FirstMaximized,
		MOD_SecondMaximized,
		MOD_Last
	};

	BOOL Create(			
		DWORD dwStyle, 
		CWnd* pParentWnd, 
		CWnd* pFPane, 
		CWnd* pSPane,
		UINT nID,
		UINT nFConstraint,
		UINT nSConstraint,
		BOOL bVerticalSplitter,
		UINT SplitterWidth);

	BOOL Create(
		DWORD dwStyle, 
		CWnd* pParentWnd, 
		UINT nID);

	void SetMode(MODE mode);
	MODE GetMode()								{return m_mode;}
	//void SetFixedDistance(INT fixedDistance)	{m_fixedDistance = fixedDistance;}
	//INT GetFixedDistance()						{return m_fixedDistance;}
	//void ShowFPane(BOOL bShow);
	//void ShowSPane(BOOL bShow);
	BOOL IsVerticalSplitter()			{return m_bVertSplitter;}
	void SetSplitterPos(UINT nPos);
	UINT GetSplitterPos()				{return m_SplitterPos;}
	UINT GetRealSplitterPos()			{return m_RealSplitterPos;}

	BOOL IsLiveUpdate()					{return m_bLiveUpdate;}
	void LiveUpdate(BOOL bLiveUpdate)	{m_bLiveUpdate = bLiveUpdate;}

	void SetFirstPane(CWnd* pWnd)		{m_pFPane = pWnd;}
	void SetSecondPane(CWnd* pWnd)		{m_pSPane = pWnd;}
	CWnd* GetFirstPane()				{return m_pFPane;}
	CWnd* GetSecondPane()				{return m_pSPane;}


	void SetConstraintSize(UINT size)	{m_nFConstraint = m_nSConstraint = size;}
	void SetVerticalSplitter(BOOL bVertical)	{m_bVertSplitter = bVertical;}
	void SetSplitterSize(UINT size)				{m_nSplitterWidth = size;}

	void SetupWindows()							{MovePanes();}



	//---------------------------------------------------
	//ICtrlColors ---------------------------------------
	//#include "ICtrlColors.h" // ,public ICtrlColors
	//---------------------------------------------------
public:
	enum COLORS
	{
		COL_Normal,
		COL_Disabled,
		COL_Tracking,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	//virtual LPCTSTR GetColorName(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

// Overrides
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP() 

private:
	virtual void MovePanes();
	void DrawBar(BOOL bEraseOnly = FALSE);
	CRect GetSplitterRect(UINT SplitterPos, BOOL bWindowRelative = FALSE);
	void CancelDrag();
	void ApplyConstraints(UINT& SplitterPos, UINT Width, UINT Height);
	CRect GetRelativeRect();

private:
	CWnd*		m_pWndParent;
	CWnd*		m_pFPane;			//Left on vertical (Top on horizontal)
	CWnd*		m_pSPane;			//Right on vertical
	UINT		m_nFConstraint;			//MaxLeft on vertical
	UINT		m_nSConstraint;			//MaxRight on Verical
	BOOL		m_bVertSplitter;	//Otherwise it is Horizontal
	CRect		m_SplitterRect;

	UINT		m_SplitterPos;
	UINT		m_RealSplitterPos;
	BOOL		m_bDragging;
	UINT		m_BarPos;
	UINT		m_DrawedBarPos;
	CPoint		m_DragStartPoint;
	HWND		m_hwndPrevFocus;
	UINT		m_nSplitterWidth;
	UINT		m_nID;
	BOOL		m_bLiveUpdate; 
	CBrush		m_Brushes[COL_Last];
	MODE m_mode;
	BOOL m_bTracking;
};


#endif
