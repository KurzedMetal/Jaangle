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

#include "stdafx.h"
#include "TeenSpirit.h"
#include "MultiPicturePage.h"
#include "GuiControls/ImageImportDlg.h"
#include "PrgAPI.h"
#include "BitmapSaver.h"
#include "ActionManager.h"

#define ID_IMPORTFILE			1200
#define ID_CLIPBOARD			1201
#define ID_REMOVE				1202
#define ID_EDIT					1203
#define ID_OPENCONTAININGFOLDER 1204
#define ID_OPEN					1205

IMPLEMENT_DYNAMIC(CMultiPicturePage, CPropertyPage)

CMultiPicturePage::CMultiPicturePage(UINT nIDCaption)
	: CTSPropertyPage(CMultiPicturePage::IDD, nIDCaption),
	m_bInitialized(FALSE),
	m_curPic(-1),
	m_bSomethingChanged(FALSE)
{
}

CMultiPicturePage::~CMultiPicturePage()
{
	if (m_hWnd) DestroyWindow();
}

void CMultiPicturePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC, m_Pic);
}


BEGIN_MESSAGE_MAP(CMultiPicturePage, CPropertyPage)
//	ON_MESSAGE(UNM_HYPERLINK_CLICKED, NotifyHyperlinkClicked)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_RD_DEFAULT, &CMultiPicturePage::OnBnClickedRdDefault)
END_MESSAGE_MAP()

void CMultiPicturePage::Refresh()
{
	PrgAPI* pLang = PRGAPI();
	INT count = GetPictureCount();
	TCHAR msg[1000];
	msg[0] = 0;
	if (count == 0)
	{
		m_curPic = -1;
		m_Pic.ClearImage();
	}
	else
	{
		if (m_curPic < 0)
			m_curPic = 0;
		if (m_curPic >= count)
			m_curPic = count - 1;
		if (m_Pic.LoadFile(m_pics[m_curPic].path.c_str()))
		{
			if (m_pics[m_curPic].status == PS_Removed)
			{
				_sntprintf(msg, 1000, _T("%s"), 
					pLang->GetString(IDS_REMOVE));
			}
			else
			{
				CSize picSize = m_Pic.GetPictureSize();
				if (picSize.cx > 0)
				{
					_sntprintf(msg, 1000, _T("%s: %d x %d\r\n%s: %s"),
						pLang->GetString(IDS_SIZE),
						picSize.cx, picSize.cy,
						pLang->GetString(IDS_LOCATION),
						m_pics[m_curPic].path.c_str());
				}
				else
					_sntprintf(msg, 1000, _T("Invalid File\r\n%s: %s"), 
					pLang->GetString(IDS_LOCATION),
					m_pics[m_curPic].path.c_str());
			}
		}
		CheckDlgButton(IDC_RD_DEFAULT, m_pics[m_curPic].bDefault);


		//	curLen = _sntprintf(txt, 300, _T("<icon idres=%d width=16 height=16> %s:"),
		//		IDI_INFO, pLang->GetString(IDS_OPTIONS));
		//}
		//else
		//{
		//	curLen = _sntprintf(txt, txtLen, _T("<icon idres=%d width=16 height=16> ")
		//		_T("%s: %d x %d. %s"), 
		//		IDI_INFO, pLang->GetString(IDS_PICTUREDIMENSIONS), picSize.cx, picSize.cy, pLang->GetString(IDS_OPTIONS));
		//	curLen += _sntprintf(&txt[curLen], txtLen - curLen,
		//		_T("<br><t>� <a msg=\"3\">%s</a>."), pLang->GetString(IDS_DELETEPICTURE));
		//}
		//curLen += _sntprintf(&txt[curLen], txtLen - curLen,
		//	_T("<br><t>� <a msg=\"1\">%s</a>."), pLang->GetString(IDS_IMPORTDATADROMFILE));
		//curLen += _sntprintf(&txt[curLen], txtLen - curLen,
		//	_T("<br><t>� <a msg=\"4\">%s</a>."), pLang->GetString(IDS_PASTEPICTURE));
		//	//_tcsncat(txt, _T("<br><t>� <a msg=\"4\">Paste a Picture</a>."), _tcslen(txt) - txtLen);
		//txt[txtLen - 1] = 0;
		//m_Info.SetWindowText(txt);



	}
	SetDlgItemText(IDC_INFO, msg);
	m_buttons[B_Prev].EnableWindow(m_curPic > 0);
	m_buttons[B_Next].EnableWindow(m_curPic < count - 1);
	m_buttons[B_Actions].EnableWindow(m_curPic != -1);

	//CSize picSize = m_Pic.GetPictureSize();
	//if (picSize.cx == 0)
	//{
	//	curLen = _sntprintf(txt, 300, _T("<icon idres=%d width=16 height=16> %s:"),
	//		IDI_INFO, pLang->GetString(IDS_OPTIONS));
	//}
	//else
	//{
	//	curLen = _sntprintf(txt, txtLen, _T("<icon idres=%d width=16 height=16> ")
	//		_T("%s: %d x %d. %s"), 
	//		IDI_INFO, pLang->GetString(IDS_PICTUREDIMENSIONS), picSize.cx, picSize.cy, pLang->GetString(IDS_OPTIONS));
	//	curLen += _sntprintf(&txt[curLen], txtLen - curLen,
	//		_T("<br><t>� <a msg=\"3\">%s</a>."), pLang->GetString(IDS_DELETEPICTURE));
	//}
	//curLen += _sntprintf(&txt[curLen], txtLen - curLen,
	//	_T("<br><t>� <a msg=\"1\">%s</a>."), pLang->GetString(IDS_IMPORTDATADROMFILE));
	//curLen += _sntprintf(&txt[curLen], txtLen - curLen,
	//	_T("<br><t>� <a msg=\"4\">%s</a>."), pLang->GetString(IDS_PASTEPICTURE));
	//	//_tcsncat(txt, _T("<br><t>� <a msg=\"4\">Paste a Picture</a>."), _tcslen(txt) - txtLen);
	//txt[txtLen - 1] = 0;
	//m_Info.SetWindowText(txt);
	
}

struct BtnInfo
{
	INT resID;
	APP_ICONS resIconID;
	INT resStringID;
};
const BtnInfo Btn2BtnInfo[] =
{
	{IDC_ADD, ICO_Add16, IDS_IMPORT},
	{IDC_PREVIOUS, ICO_Back16, IDS_PREVIOUS},
	{IDC_NEXT, ICO_Forward16, IDS_NEXT},
	{IDC_ACTIONS, ICO_Last, IDS_OPTIONS}
};
BOOL CMultiPicturePage::OnInitDialog()
{
	PrgAPI* pAPI = PRGAPI();
	CPropertyPage::OnInitDialog();
	SetDlgItemText(IDC_GR_PICTURE, pAPI->GetString(IDS_PICTURE));
	SetDlgItemText(IDC_RD_DEFAULT, pAPI->GetString(IDS_DEFAULT));
	//m_Info.SetBkMode(TRANSPARENT);
	//m_Info.SetCallbackHyperlink(UNM_HYPERLINK_CLICKED);
	//m_PicturePathAtStart = PicturePath;
	m_bInitialized = TRUE;
	m_Pic.SetBkColor(RGB(0,0,0));
	GdiPlusPicDrawer::DrawParams& params = m_Pic.GetDrawParams();
	params.zoomLockMode = GdiPlusPicDrawer::ZLM_FullImage;

	ASSERT(sizeof(Btn2BtnInfo)/sizeof(BtnInfo) == B_Last);

	for (int i = 0; i < B_Last; i++)
	{
		m_buttons[i].SubclassDlgItem(Btn2BtnInfo[i].resID, this);
		pAPI->SetGlobalButtonStyle(m_buttons[i], Btn2BtnInfo[i].resIconID);
		if (Btn2BtnInfo[i].resStringID)
			m_buttons[i].SetWindowText(pAPI->GetString(Btn2BtnInfo[i].resStringID));
	}

	m_addMenu.CreatePopupMenu();
	m_addMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
	m_addMenu.AppendMenu(MF_STRING, ID_IMPORTFILE, (LPTSTR) pAPI->GetString(IDS_IMPORT));
	m_addMenu.AppendMenu(MF_STRING, ID_CLIPBOARD, (LPTSTR) pAPI->GetString(IDS_CLIPBOARD));
	m_actionMenu.CreatePopupMenu();
	m_actionMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
	m_actionMenu.AppendMenu(MF_STRING, ID_OPEN, (LPTSTR) pAPI->GetString(IDS_OPENPICTURE));
	m_actionMenu.AppendMenu(MF_STRING, ID_EDIT, (LPTSTR) pAPI->GetString(IDS_EDITPICTURE));
	m_actionMenu.AppendMenu(MF_STRING, ID_OPENCONTAININGFOLDER, (LPTSTR) pAPI->GetString(IDS_OPENCONTAININGFOLDER));
	m_actionMenu.AppendMenu(MF_SEPARATOR, 0, 0);
	m_actionMenu.AppendMenu(MF_STRING, ID_REMOVE, (LPTSTR) pAPI->GetString(IDS_REMOVE));
	Refresh();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)pAPI->GetFont(FONT_Dialogs)->m_hObject);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMultiPicturePage::AddPicture(const PicInfo& picInfo)
{
	PicInfoStore pic = {picInfo.path, picInfo.status, picInfo.bDefault};
	m_pics.push_back(pic);
}

BOOL CMultiPicturePage::GetPicture(UINT idx, PicInfo& picInfo)
{
	TRACEST(_T("CMultiPicturePage::GetPicture"));
	ASSERT(idx < GetPictureCount());
	if (idx < GetPictureCount())
	{
		picInfo.bDefault = m_pics[idx].bDefault;
		picInfo.path = m_pics[idx].path.c_str();
		picInfo.status = m_pics[idx].status;
		return TRUE;
	}
	return PS_Unknown;
}



//LRESULT CMultiPicturePage::NotifyHyperlinkClicked(WPARAM wParam, LPARAM lParam)
//{
//	int command = _ttoi((LPCTSTR)wParam);
//	switch (command)
//	{
//	case 1://Set a file
//		{
//			CImageImportDlg fd(NULL, 
//				NULL, 
//				NULL, 
//				_T("Picture Files (*.jpg;*.gif)|*.jpg;*.gif|All Files (*.*)|*.*||"), 
//				this);
//			fd.m_ofn.lpstrInitialDir = _T("");
//			fd.m_ofn.Flags |= OFN_EXPLORER;
//			if (fd.DoModal() == IDOK)
//			{
//				PicturePath = fd.GetPathName();
//				Refresh();
//			}
//		}
//		break;
//	case 4://Paste
//		{
//			if ( OpenClipboard() )
//			{
//				//Get the clipboard data
//				HBITMAP handle = (HBITMAP)GetClipboardData(CF_BITMAP);
//				if (handle)
//				{
//					TCHAR TempPath[MAX_PATH];
//					GetTempPath(MAX_PATH, TempPath);
//					_tcsncat(TempPath, _T("~tspst.jpg"), MAX_PATH);
//					if (SaveJpg(TempPath, handle))
//					{
//						PicturePath = TempPath;
//						Refresh();
//					}
//				}
//				CloseClipboard();
//				if (handle == 0)
//					PRGAPI()->MessageBox(m_hWnd, IDS_NOPICTUREINCLIPBOARD, 
//					NULL, MB_OK | MB_ICONINFORMATION);
//			}
//		}
//		break;
//	case 3://Delete
//		PicturePath = "";
//		Refresh();
//		break;
//	default:
//		ASSERT(FALSE);
//		break;
//	}
//
//	return 0;
//
//}
//------------------------------------------------------
//Clipboard Usage
//------------------------------------------------------


BOOL CMultiPicturePage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CMultiPicturePage::OnCommand \r\n"), wParam);
	switch (wParam)
	{
	case IDC_PREVIOUS:
		m_curPic--;
		Refresh();
		break;
	case IDC_NEXT:
		m_curPic++;
		Refresh();
		break;
	case IDC_ADD:
		{
			RECT rc;
			GetDlgItem(wParam)->GetWindowRect(&rc);
			m_addMenu.TrackPopupMenu(TPM_LEFTALIGN, rc.left, rc.bottom, this);
		}
		break;
	case IDC_ACTIONS:
		{
			RECT rc;
			GetDlgItem(wParam)->GetWindowRect(&rc);
			m_actionMenu.TrackPopupMenu(TPM_LEFTALIGN, rc.left, rc.bottom, this);
		}
		break;
	case ID_IMPORTFILE:
		{
			CImageImportDlg fd(NULL, 
				NULL, 
				NULL, 
				_T("Picture Files (*.jpg;*.gif)|*.jpg;*.gif|All Files (*.*)|*.*||"), 
				this);
			fd.m_ofn.lpstrInitialDir = _T("");
			fd.m_ofn.Flags |= OFN_EXPLORER;
			if (fd.DoModal() == IDOK)
			{
				PicInfoStore pic = {fd.GetPathName(), PS_AddedByFile, FALSE};
				m_pics.push_back(pic);
				m_curPic = m_pics.size() - 1;
				m_bSomethingChanged = TRUE;
				Refresh();
			}
		}
		break;
	case ID_CLIPBOARD:
		{
			BitmapSaver bs;
			bs.SetFormat(BitmapSaver::F_Jpg);
			TCHAR tmpPath[MAX_PATH], tmpFileName[MAX_PATH];
			GetTempPath(MAX_PATH, tmpPath);
			GetTempFileName(tmpPath, _T("ts~"), 0, tmpFileName);
			if (!bs.SaveClipboard(tmpFileName))
				PRGAPI()->NotifyUser(IDS_OPERATIONFAILED, 0, SEV_Warning);
			else
			{
				PicInfoStore pic = {tmpFileName, PS_AddedByClipboard, FALSE};
				m_pics.push_back(pic);
				m_curPic = m_pics.size() - 1;
				m_bSomethingChanged = TRUE;
				Refresh();
			}
		}
		break;
	case ID_REMOVE:
		if (m_curPic >= 0 && m_curPic < (INT)m_pics.size())
		{
			switch (m_pics[m_curPic].status)
			{
			case PS_Normal:
				m_bSomethingChanged = TRUE;
				m_pics[m_curPic].status = PS_Removed;
				break;
			case PS_Removed:
				break;//Nothing
			case PS_AddedByClipboard:
				DeleteFile(m_pics[m_curPic].path.c_str());//Delete only in this case (temp)
			case PS_AddedByFile:
				m_pics.erase(m_pics.begin() + m_curPic);//Remove from list either case
				break;
			}
			Refresh();
		}
		break;
	case ID_OPEN:
		ShellExecute(NULL, NULL, m_pics[m_curPic].path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
		break;
	case ID_EDIT:
		ShellExecute(NULL, _T("Edit"), m_pics[m_curPic].path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
		break;
	case ID_OPENCONTAININGFOLDER:
		ActionManager::OpenFolderToItem(m_pics[m_curPic].path.c_str());
		break;
	default:
		break;
	}

	return CTSPropertyPage::OnCommand(wParam, lParam);
}

void CMultiPicturePage::OnBnClickedRdDefault()
{
	if (m_curPic >= 0 && m_curPic < (INT)m_pics.size())
	{
		m_bSomethingChanged = TRUE;
		std::vector<PicInfoStore>::iterator it = m_pics.begin();
		for (; it != m_pics.end(); it++)
			it->bDefault = FALSE;
		m_pics[m_curPic].bDefault = TRUE;
	}
}
