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
#ifndef _SliderDlg_h_
#define _SliderDlg_h_

class SliderDlgListener
{
public:
	virtual void OnChangeSlider(INT newValue) = 0;
};

class SliderDlg : public CWnd
{
public:
	SliderDlg();
	virtual ~SliderDlg();

	void SetListener(SliderDlgListener* pListener)	{m_pListener = pListener;}
	BOOL Create(LPCTSTR title, INT minPos, INT maxPos, INT initialValue, INT x, INT y, INT cx, INT cy, CWnd* pParent);
	INT GetValue();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	//INT m_initialValue;
	SliderDlgListener* m_pListener;
	std::tstring m_title;
	CSliderCtrl m_slider;
};

#endif
