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
#ifndef _CHotKeyCtrlAPI_h_
#define _CHotKeyCtrlAPI_h_

//This class has following features
//	1. Has compatible wModifier with the SetHotKeyAPI (No need to modify)
//	2. Notifies the parent when something changes with EM_CHANGE

class CHotKeyCtrlAPI : public CHotKeyCtrl
{
public:
	void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
	DWORD GetHotKey() const;
	void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

protected:
private:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


#endif
