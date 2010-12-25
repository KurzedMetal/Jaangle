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


#include "TSTranslatable.h"
#include "TreePopSheet/TreePropSheet.h"
#include "GuiControls/FlexDialogHeader.h"


class CButtonX: public CButton
{
public:
	CButtonX()					{}
	virtual ~CButtonX()	{}
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnPaint();
};

class CTSPropertyPage;
using namespace TreePropSheet;
class COptionsSheet : public CTreePropSheet,public TSTranslatable
{

public:
	COptionsSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~COptionsSheet();
	virtual void ApplyTranslation(ITranslation& skin);

	void AddTSPropertyPage(CTSPropertyPage* page);
	void SetStartUpPage(UINT m_nIDCaption);

	FlexDialogHeader& GetHeaderControl()	{return m_header;}

protected:
	DECLARE_MESSAGE_MAP()
	virtual void AddPage(CPropertyPage* pPage)	{}//Overriden. Use AddTSPropertyPage
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnHelp();

private:
	std::vector<CTSPropertyPage*> m_pages;
	UINT m_nIDCaption;
	FlexDialogHeader m_header;

	CButtonX m_helpButton;

};


