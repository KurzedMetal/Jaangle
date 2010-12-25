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
#ifndef _CPlayerWnd_h_
#define _CPlayerWnd_h_

class CPlayerWindow
{
public:
	void SetParent(HWND newParent)
	{
		m_hWndOldParent = ::GetParent(m_hWnd);
		::SetParent(m_hWnd, newParent);
		CRect rc;
		GetClientRect(m_hWndOldParent, &rc);
		SendMessage(m_hWndOldParent, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
		GetClientRect(newParent, &rc);
		SendMessage(newParent, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()));
	}
	HWND m_hWnd;
	HWND m_hWndOldParent;
private:
};


#endif
