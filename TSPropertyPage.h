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
#ifndef _CTSPropertyPage_h_
#define _CTSPropertyPage_h_

#include "TSTranslatable.h"

class CTSPropertyPage : public CPropertyPage, public TSTranslatable
{
	DECLARE_DYNAMIC(CTSPropertyPage)

public:
	CTSPropertyPage(UINT nIDTemplate, UINT nIDCaption);
	virtual ~CTSPropertyPage();
	//UINT GetTitleResID()			{return m_resourceID;}
	virtual LPCTSTR GetTitle();
	//Used in Tree Sheet
	virtual void SetPPParent(const CTSPropertyPage* pTSPP)	{m_pPPParent = pTSPP;}
	virtual const CTSPropertyPage* GetPPParent() const		{return m_pPPParent;}

	virtual void ApplyTranslation(ITranslation& translation)	{}

	afx_msg void OnHelp();
	virtual LPCTSTR GetHelpID()						{return NULL;}

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


private:
	HBRUSH m_EditBrush;
	UINT m_resourceID;

private:

	//TRIPLE CLICK SUPPORT FOR ALL PROPERTY PAGES
	enum { LEFT=1, MIDDLE, RIGHT };		 // codes for which button
	static UINT IsDoubleClick(UINT msg)
	{
		return msg==WM_LBUTTONDBLCLK ? LEFT :
			msg==WM_MBUTTONDBLCLK ? MIDDLE :
			msg==WM_RBUTTONDBLCLK ? RIGHT : FALSE;
	}
	static UINT IsButtonDown(UINT msg)
	{
		return msg==WM_LBUTTONDOWN ? LEFT :
			msg==WM_MBUTTONDOWN ? MIDDLE :
			msg==WM_RBUTTONDOWN ? RIGHT : FALSE;
	}

	UINT m_uWhichButton;					// which button clicked?
	UINT m_uTimeLastDblClk;					// clock time of last double click
	UINT m_uClocksPerDblClk;				// max clocks for double-click
	CString m_Caption;
	const CTSPropertyPage* m_pPPParent;


};

#endif
