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
#ifndef _DockedWindow_h
#define _DockedWindow_h

class CDockedWindow
{
public:
	CDockedWindow():
m_bVisible(FALSE),
m_bDocked(TRUE),
m_hwndSelf(NULL),
m_hwndDocked(NULL),
m_hwndUndocked(NULL)
{}
	~CDockedWindow()	{}

	void SetWindow(HWND hwndSelf)				{m_hwndSelf = hwndSelf;}
	HWND GetWindow()							{return m_hwndSelf;}

	void SetDockedParentWindow(HWND hwndDocked)	{m_hwndDocked = hwndDocked;}
	void SetUnDockedWindow(HWND hwndUndocked)	{m_hwndUndocked = hwndUndocked;}

	BOOL IsOwner(HWND owner)
	{
		if (owner == m_hwndDocked && m_bDocked)
			return TRUE;
		if (owner == m_hwndUndocked && !m_bDocked)
			return TRUE;
		return FALSE;
	}

	void SetDocked(BOOL bDocked)
	{
		//ASSERT(::IsWindow(m_hwndSelf));
		//ASSERT(::IsWindow(m_hwndDocked));
		//ASSERT(::IsWindow(m_hwndUndocked));
		//::ShowWindow(m_hwndSelf,SW_HIDE);
		//::SetParent(m_hwndSelf, bDocked ? m_hwndDocked : m_hwndUndocked);
		//if (m_bVisible)
		//	::ShowWindow(m_hwndUndocked, bDocked ? SW_HIDE :SW_SHOW);
		//m_bDocked = bDocked;
		//::ShowWindow(m_hwndSelf,SW_SHOW);
		//RefreshParents();
		if (bDocked != m_bDocked)
		{
			m_bDocked = bDocked;
			if (IsVisible())
				UpdateState();
		}
	}
	BOOL IsDocked()								{return m_bDocked;}

	void SetVisible(BOOL bVisible)
	{
		//ASSERT(::IsWindow(m_hwndSelf));
		//ASSERT(::IsWindow(m_hwndDocked));
		//ASSERT(::IsWindow(m_hwndUndocked));
		//if (!m_bDocked)
		//	::ShowWindow(m_hwndUndocked, bVisible ? SW_SHOW : SW_HIDE);
		//::ShowWindow(m_hwndSelf, bVisible ? SW_SHOW : SW_HIDE);
		//m_bVisible = bVisible;
		//RefreshParents();
		if (bVisible != m_bVisible)
		{
			m_bVisible = bVisible;
			UpdateState();
		}
	}

	void UpdateState()
	{
		ASSERT(::IsWindow(m_hwndSelf));
		ASSERT(::IsWindow(m_hwndDocked));
		ASSERT(::IsWindow(m_hwndUndocked));
		//::ShowWindow(m_hwndSelf,SW_HIDE);
		if (m_bDocked)
		{
			::SetParent(m_hwndSelf, m_hwndDocked);
			::ShowWindow(m_hwndUndocked, SW_HIDE); 
		}
		else
		{
			::SetParent(m_hwndSelf, m_hwndUndocked);
			::ShowWindow(m_hwndUndocked, m_bVisible ? SW_SHOW : SW_HIDE); 
		}
		::ShowWindow(m_hwndSelf,m_bVisible ? SW_SHOW : SW_HIDE);
		RefreshParents();
	}
	BOOL IsVisible()							{return m_bVisible;}
private:
	void RefreshParents()
	{
		CRect rc;
		GetClientRect(m_hwndDocked, &rc);
		SendMessage(m_hwndDocked, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
		GetClientRect(m_hwndUndocked, &rc);
		SendMessage(m_hwndUndocked, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));}
private:
	BOOL m_bVisible;
	BOOL m_bDocked;
	HWND m_hwndSelf;
	HWND m_hwndDocked;
	HWND m_hwndUndocked;
	
};

#endif
