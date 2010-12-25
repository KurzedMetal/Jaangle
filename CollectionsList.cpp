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
#include "CollectionsList.h"
#include "PrgAPI.h"
#include "SQLManager.h"
#include "ActionManager.h"
#include "CollectionManager.h"
#include "GdiplusUtilities.h"
#include "DataRecordsXmlSerializer.h"


#define ID_COL_IMPORTCOLLECTION 300
#define ID_COL_EXPORTCOLLECTION 301
#define ID_COL_MERGECOLLECTION 302
#define ID_COL_UPDATEL2	400
#define ID_COL_RENAME	401
#define ID_COL_REMOVE	402
#define ID_COL_ADDFOLDER	403
#define ID_COL_UPDATEL1 404


// CCollectionsList

CCollectionsList::CCollectionsList()
:m_pBkBrush(NULL),
m_pTextBrush(NULL),
m_pTextBkBrush(NULL),
m_pTextSelBrush(NULL),
m_pTextSelBkBrush(NULL),
m_pTextAltBkBrush(NULL),
m_pTextWorkingBrush(NULL),
m_pMainMenu(NULL)
{

	SetItemHeight(52);//36);
	SetSingleSelection(FALSE);

	LOGFONT lf;
	HDC hdc = ::GetDC(0);
	//	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	memset(&lf, 0, sizeof(LOGFONT));
	_tcscpy(lf.lfFaceName, _T("Tahoma"));
	lf.lfWeight = 400;
	lf.lfHeight = 13;
	m_pMainTextFont = new Gdiplus::Font(hdc, &lf);
	lf.lfWeight += 200;
	lf.lfHeight += 2;
	m_pTitleTextFont = new Gdiplus::Font(hdc, &lf);
	::ReleaseDC(0, hdc);

	SetColor(COL_Bk, RGB(255,255,255));
	SetColor(COL_Text, RGB(0,0,0));
	SetColor(COL_TextBk, RGB(255,255,255));
	SetColor(COL_TextBkAlt, RGB(240,240,240));
	SetColor(COL_TextWorking, RGB(150,0,0));
	SetColor(COL_TextSel, RGB(255,255,255));
	SetColor(COL_TextSelBk, RGB(0,0,150));

	m_pBorderPen = new Gdiplus::Pen(Gdiplus::Color::LightGray, 1);


	m_pMainStringFormat = new Gdiplus::StringFormat();
	m_pMainStringFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter);
	m_pMainStringFormat->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

}

CCollectionsList::~CCollectionsList()
{
	delete m_pBkBrush;
	delete m_pTextBrush;
	delete m_pTextBkBrush;
	delete m_pTextSelBrush;
	delete m_pTextSelBkBrush;
	delete m_pTextAltBkBrush;
	delete m_pTextWorkingBrush;
	delete m_pBorderPen;

	delete m_pMainStringFormat;
	delete m_pMainMenu;
}

void CCollectionsList::InitMenus()
{
	PrgAPI* pAPI = PRGAPI();
	if (m_pMainMenu == NULL)
		m_pMainMenu = pAPI->CreatePopupMenu();
	if (m_pMainMenu->GetInternalHandler())
		return;
	m_pMainMenu->Create();

	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_ADDFOLDER, (LPTSTR) pAPI->GetString(IDS_ADDCOLLECTION));
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("%s (%s)"), pAPI->GetString(IDS_UPDATE), pAPI->GetString(IDS_QUICK));
	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_UPDATEL1, bf);
	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_UPDATEL2, (LPTSTR) pAPI->GetString(IDS_UPDATE));
	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_RENAME, (LPTSTR) pAPI->GetString(IDS_RENAME));

	m_pMainMenu->AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);

	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_REMOVE, (LPTSTR) pAPI->GetString(IDS_REMOVE));

	m_pMainMenu->AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);

	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_IMPORTCOLLECTION, (LPTSTR) pAPI->GetString(IDS_IMPORTVIRTUALTRACKS));
	m_pMainMenu->AppendMenu(ITSMenu::MIT_String, ID_COL_EXPORTCOLLECTION, (LPTSTR) pAPI->GetString(IDS_EXPORTVIRTUALTRACKS));

}

void CCollectionsList::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint p = point;
	ScreenToClient(&point);
	INT item = GetItemFromPoint(point.x, point.y);
	if (item != -1 && !IsItemSelected(item))
	{
		ClearSelection();
		SetItemSelection(item, TRUE);
	}
	InitMenus();
	m_pMainMenu->EnableMenuItem(ID_COL_UPDATEL1, FALSE);
	m_pMainMenu->EnableMenuItem(ID_COL_UPDATEL2, FALSE);
	m_pMainMenu->EnableMenuItem(ID_COL_REMOVE, FALSE);
	m_pMainMenu->EnableMenuItem(ID_COL_RENAME, FALSE);
	if (item != -1)
	{
		m_pMainMenu->EnableMenuItem(ID_COL_RENAME, GetSelectedItemCount() == 1);
		m_pMainMenu->EnableMenuItem(ID_COL_REMOVE, TRUE);
		switch (m_CRC[item]->type)
		{
		case CTYPE_LocalFolder:
		case CTYPE_NetFolder:
		case CTYPE_Removable:
			m_pMainMenu->EnableMenuItem(ID_COL_UPDATEL1, TRUE);
			m_pMainMenu->EnableMenuItem(ID_COL_UPDATEL2, TRUE);
			break;
		case CTYPE_Media:
			break;
		case CTYPE_Virtual:
			break;
		case CTYPE_AudioCD:
			break;
		case CTYPE_HTTPLinks:
			break;
		default:
			ASSERT(0);
			break;
		}
	}

	m_pMainMenu->TrackPopupMenu(p.x, p.y, this->GetSafeHwnd());
}


void CCollectionsList::UpdateControl()
{
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	m_CRC.clear();
	pSM->GetCollectionRecordCollectionByTracksFilter(m_CRC, TracksFilter(), TRUE);
	if (GetItemCount() != m_CRC.size())
		SetItemCount(m_CRC.size());
	Refresh();
}

void CCollectionsList::DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC)
{
	PrgAPI* pAPI = PRGAPI();

	//Calculate Colors
	Gdiplus::Brush* pTextBrush = m_pTextBrush ;
	Gdiplus::Brush* pBkBrush = nItem % 2 ? m_pTextBkBrush : m_pTextAltBkBrush;
	if (IsItemSelected(nItem))
	{
		pTextBrush = m_pTextSelBrush;
		pBkBrush = m_pTextSelBkBrush;
	}
	g.FillRectangle(pBkBrush, itemRC);


	CollectionRecord& rec = *(m_CRC)[nItem];
	std::map<CollectionTypesEnum, Gdiplus::Bitmap*>::iterator it = m_pBitmaps.find(rec.type);
	Gdiplus::Bitmap* pBmpIcon = NULL;
	if (it == m_pBitmaps.end())
	{
		pBmpIcon = GdiplusUtilities::FromHICON32(pAPI->GetIconForCollectionType(rec.type));
		m_pBitmaps[rec.type] = pBmpIcon;
	}
	else
		pBmpIcon = (*it).second;

	INT xmargin = 1;
	INT ymargin = 1;
	g.DrawImage(pBmpIcon, itemRC.X + xmargin, itemRC.Y + ymargin, 0,0, 16, 16, Gdiplus::UnitPixel);

	BOOL bWorking = pAPI->GetCollectionManager()->IsCollectionPending(rec);

	if (bWorking)
		pTextBrush = m_pTextWorkingBrush;
	TCHAR sWorking[100];
	_sntprintf(sWorking, 100, _T("[%s] - "), pAPI->GetString(IDS_WORKING));
	TCHAR msg[1000];
	INT strLen = _sntprintf(msg, 1000, 
		_T("%s%s (%s) | %s: '%s' [0x%0.8X])"),
		bWorking ? sWorking : _T(""),
		rec.name.c_str(), 
		pAPI->GetStringForCollectionType(rec.type),
		pAPI->GetString(IDS_LOCATION), rec.location.c_str(), rec.serial);
	Gdiplus::Rect rcText(itemRC);
	rcText.X += 20;
	rcText.Width -= 22;
	rcText.Height = GetItemHeight() / 2;

	INT X = itemRC.X + 20;

	g.DrawString(msg, 
		strLen, 
		m_pTitleTextFont, 
		Gdiplus::RectF(
		(Gdiplus::REAL)(X), 
		(Gdiplus::REAL)(itemRC.Y), 
		(Gdiplus::REAL)(itemRC.GetRight() - X), 
		(Gdiplus::REAL)( 4 * itemRC.Height / 10)), 
		m_pMainStringFormat, 
		pTextBrush);

	//X = itemRC.X;
	TCHAR dateAddedStr[100];
	PrgAPI::FormatDate(dateAddedStr, 100, rec.dateAdded, FALSE);
	TCHAR dateUpdatedStr[100];
	PrgAPI::FormatDate(dateUpdatedStr, 100, rec.dateUpdated, FALSE);
	strLen = _sntprintf(msg, 1000, 
		_T("%s: %s | %s: %s"),
		pAPI->GetString(IDS_DATEADDED), dateAddedStr,
		pAPI->GetString(IDS_DATEUPDATED), dateUpdatedStr);
	g.DrawString(msg, 
		strLen, 
		m_pMainTextFont, 
		Gdiplus::RectF(
		(Gdiplus::REAL)(X), 
		(Gdiplus::REAL)(itemRC.Y + 4 * itemRC.Height / 10), 
		(Gdiplus::REAL)(itemRC.GetRight() - X), 
		(Gdiplus::REAL)(3 * itemRC.Height / 10)), 
		m_pMainStringFormat, 
		pTextBrush);

	strLen = _sntprintf(msg, 1000, 
		_T("%s: %d | %s: %2.1f MB | %s: %2.1f h"),
		pAPI->GetString(IDS_TRACKS), rec.trackCount,
		pAPI->GetString(IDS_SIZE), rec.sumSize / 1024.0,
		pAPI->GetString(IDS_DURATION), rec.sumDuration / 3600.0);
	g.DrawString(msg, 
		strLen, 
		m_pMainTextFont, 
		Gdiplus::RectF(
		(Gdiplus::REAL)(X), 
		(Gdiplus::REAL)(itemRC.Y + 7 * itemRC.Height / 10), 
		(Gdiplus::REAL)(itemRC.GetRight() - X), 
		(Gdiplus::REAL)(3 * itemRC.Height / 10)), 
		m_pMainStringFormat, 
		pTextBrush);
	g.DrawLine(m_pBorderPen, itemRC.GetLeft() + 1, itemRC.GetBottom() - 1, itemRC.GetRight() - 2, itemRC.GetBottom() - 1);

}

void CCollectionsList::DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r)
{
	g.FillRectangle(m_pBkBrush, r);
	if (r.Y == 0)
	{
		Gdiplus::StringFormat centeredStringFormat;
		centeredStringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		centeredStringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

		g.DrawString(PRGAPI()->GetString(IDS_EMPTY), 
			-1, 
			m_pMainTextFont, 
			Gdiplus::RectF(
			(Gdiplus::REAL)(r.X + 5), 
			(Gdiplus::REAL)(r.Y + 5), 
			(Gdiplus::REAL)(r.Width), 
			(Gdiplus::REAL)(r.Y + 30)), 
			&centeredStringFormat, 
			m_pTextBrush);
	}
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CCollectionsList::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	switch (idx)
	{
	case COL_Bk:
		delete m_pBkBrush;
		m_pBkBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	case COL_Text:
		delete m_pTextBrush;
		m_pTextBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	case COL_TextBk:
		delete m_pTextBkBrush;
		m_pTextBkBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	case COL_TextSel:
		delete m_pTextSelBrush;
		m_pTextSelBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	case COL_TextSelBk:
		delete m_pTextSelBkBrush;
		m_pTextSelBkBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	case COL_TextBkAlt:
		delete m_pTextAltBkBrush;
		m_pTextAltBkBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	case COL_TextWorking:
		delete m_pTextWorkingBrush;
		m_pTextWorkingBrush = new Gdiplus::SolidBrush(GdiplusUtilities::COLORREF2Color(m_colors[idx]));
		break;
	}
	if (m_hWnd)		
		Invalidate();


}
COLORREF CCollectionsList::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
const TCHAR* const cColorNames[] = {
	_T("Bk"),
	_T("Text"),
	_T("TextBk"),
	_T("TextSel"),
	_T("TextSelBk"),
	NULL
};


LPCTSTR CCollectionsList::GetColorName(UINT idx)
{
	ASSERT(idx <= COL_Last);
	if (idx < COL_Last)
		return cColorNames[idx];
	return NULL;
}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------



BOOL CCollectionsList::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CCollectionsList::OnCommand"), wParam);
	ASSERT(m_CRC.size() == GetItemCount());
	PrgAPI* pAPI = PRGAPI();
	CollectionManager* pCM = pAPI->GetCollectionManager();
	ActionManager* pAM = pAPI->GetActionManager();
	SQLManager* pSM = pAPI->GetSQLManager();
	INT nPos = GetNextSelectedItem(-1);
	switch (wParam)
	{
	case ID_COL_ADDFOLDER:
		pAM->ShowAddNewCollectionDlg(this);
		break;
	case ID_COL_IMPORTCOLLECTION:
		{
			CFileDialog fd(TRUE, _T(".xtc"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
				_T("Track Collection Files (*.xtc)|*.xtc|Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
			if (fd.DoModal() == IDOK)
			{
				FullTrackRecordCollection col;
				if (DataRecordsXmlSerializer::Import(fd.GetPathName(), col))
				{
					CollectionRecord cr;
					cr.location = fd.GetPathName();
					cr.name = fd.GetFileTitle();
					cr.serial = 0;
					cr.type = CTYPE_Virtual;
					if (pSM->AddNewCollectionRecord(cr))
						pCM->ImportFullTrackRecordCollection(cr, col);
				}
			}
		}
		break;
	case ID_COL_EXPORTCOLLECTION:
		{
			INT nPos = GetNextSelectedItem();
			if (nPos != -1)
			{
				CFileDialog fd(FALSE, _T(".xtc"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
					_T("Track Collection Files (*.xtc)|*.xtc|Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
				if (fd.DoModal() == IDOK)
				{
					FullTrackRecordCollection col;
					TracksFilter tf;
					tf.CollectionID.match = NUMM_Exact;
					tf.CollectionID.val = m_CRC[nPos]->ID;
					if (pSM->GetFullTrackRecordCollectionByTracksFilter(col, tf))
					{
						if (DataRecordsXmlSerializer::Export(fd.GetPathName(), col))
							pAPI->NotifyUser(IDS_SUCCESS);
						else
							pAPI->NotifyUser(IDS_FAILURE);
					}
				}
			}
		}
		break;
	case ID_COL_UPDATEL1:
	case ID_COL_UPDATEL2:
		while (nPos != -1)
		{
			pCM->UpdateCollection(m_CRC[nPos], TRUE, wParam == ID_COL_UPDATEL2, TRUE);
			nPos = GetNextSelectedItem(nPos);
		}
		break;
	case ID_COL_REMOVE:
		if (nPos != -1)
		{
			CollectionManager* pCM = pAPI->GetCollectionManager();
			while (nPos != -1)
			{
				//pSM->DeleteCollectionRecord(m_CRC[nPos]->ID, TRUE);
				pCM->RemoveCollection(m_CRC[nPos]->ID);
				nPos = GetNextSelectedItem(nPos);
			}
			//pAPI->SendMessage(SM_CollectionManagerEvent);
		}
		break;
	case ID_COL_RENAME:
		if (nPos != -1)
		{
			if (pAM->ShowRenameCollectionDlg(*m_CRC[nPos], this))
			{
				//pAPI->SendMessage(SM_DatabaseUpdated);
				//Refresh();
			}
		}
		break;
	default:
		return GetParent()->SendMessage(WM_COMMAND, wParam, lParam);
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CCollectionsList, CFlexListCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CCollectionsList::OnLButtonUp(UINT nFlags, CPoint point)
{
	GetParent()->PostMessage(MsgIDLeftClick);
	__super::OnLButtonUp(nFlags, point);
}
