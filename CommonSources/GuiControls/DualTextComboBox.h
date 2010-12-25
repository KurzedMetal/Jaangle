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
#ifndef __CDualTextComboBox__
#define __CDualTextComboBox__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )

#include <string>
#include <vector>

#ifndef tstring
#define tstring basic_string<TCHAR>
#endif

//This implementation is partial
//Some Combo box features are not implemented such as FindString - Sort etc..

class CDualTextComboBox : public CComboBox
{
public:
	CDualTextComboBox();

public:
	//Use AddItem instead of AddString
	//	LPCTSTR mainString: the main string
	//	LPCTSTR secondString: the sunsequent string with the different color
	//	HICON hIcon: A 16x16 masked icon. The user must free this resource
	int AddItem(LPCTSTR mainString, LPCTSTR secondString = 0, HICON hIcon = 0);
	void ResetContent();

	virtual ~CDualTextComboBox();


protected:
	class ItemData
	{
	public:
		ItemData():hIcon(NULL)	{}
		std::tstring mainString;
		std::tstring secondString;
		HICON hIcon;
	};

	ItemData* GetOrCreateItemData(int nItemIndex);

	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	virtual void MeasureItem(LPMEASUREITEMSTRUCT)				{}
	virtual int CompareItem(LPCOMPAREITEMSTRUCT)				{return 0;}
	afx_msg void OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct);
	virtual void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
private:
	//Hide Normal Handling 
	int AddString(LPCTSTR itemString)					{return 0;}
	int DeleteString(UINT nIndex)						{return 0;}
	int InsertString(int nIndex, LPCTSTR lpszString)	{return 0;}

	std::vector<ItemData> m_items;
};

#endif 
