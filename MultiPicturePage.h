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
#ifndef _CMultiPicturePage_h_
#define _CMultiPicturePage_h_

#include "GuiControls/PicWndEx.h"
#include "GuiControls/BCMenu.h"
#include "TSPropertyPage.h"
#include "FlexButton.h"
class CMultiPicturePage : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CMultiPicturePage)

public:
	enum PicStatus
	{
		PS_Unknown,
		PS_Normal,
		PS_Removed,
		PS_AddedByFile,
		PS_AddedByClipboard,
		PS_Last
	};

	struct PicInfo
	{
		LPCTSTR path;
		PicStatus status;
		BOOL bDefault;
	};

	CMultiPicturePage(UINT nIDCaption);
	virtual ~CMultiPicturePage();

	void AddPicture(const PicInfo& picInfo);
	BOOL GetPicture(UINT idx, PicInfo& picInfo);
	UINT GetPictureCount()	{return m_pics.size();}

	//void SetPicInfo(InformationManager::PICTURE_ITEM_TYPE pit, UINT ItemID);
	//void SetTitles(LPCTSTR Title, LPCTSTR SubTitle);
	//void EnableButtons(BOOL bOpenButton, BOOL bDownloadButton, BOOL bPasteButton);

// Dialog Data
	enum { IDD = IDD_PPPICTURE };
	virtual BOOL IsSomethingChanged()	{return m_bInitialized && m_bSomethingChanged;}

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
//	LRESULT NotifyHyperlinkClicked(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
private:
	void Refresh();
private:
	CPicWndEx m_Pic;
	enum Buttons
	{
		B_Add,
		B_Prev,
		B_Next,
		B_Actions,
		B_Last
	};
	FlexButton m_buttons[B_Last];
	BOOL m_bInitialized;
	BOOL m_bSomethingChanged;


	struct PicInfoStore
	{
		std::tstring path;
		PicStatus status;
		BOOL bDefault;
	};
	std::vector<PicInfoStore> m_pics;
	INT m_curPic;
	BCMenu m_actionMenu;
	BCMenu m_addMenu;
public:
	afx_msg void OnBnClickedRdDefault();
};

#endif
