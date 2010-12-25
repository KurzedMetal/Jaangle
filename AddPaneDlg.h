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

#include "PaneController.h"
#include "TeenSpirit.h"
#include "PrgAPI.h"
#include "GuiControls/FlexDialogHeader.h"


// CAddPaneDlg dialog

class CAddPaneDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddPaneDlg)

public:
	CAddPaneDlg(PaneController* pPaneController, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddPaneDlg();

	IPaneCreator* GetPaneCreator();
	PaneController::Position GetPosition()	{return m_position;}

// Dialog Data
	enum { IDD = IDD_ADDPANE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnLbnDblclkLstPanes();
	afx_msg void OnBnClickedReset();

	DECLARE_MESSAGE_MAP()

private:
	PaneController* m_pPaneControler;
	CListBox m_lstPanes;
	INT m_paneIndex;
	PaneController::Position m_position;
	FlexDialogHeader m_Header;

};
