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
#ifndef _CMainListCtrl_h_
#define _CMainListCtrl_h_

#include "GuiControls/BCMenu.h"
#include "TracksFilter.h"
#include "TSTranslatable.h"
#include "TSSkinnable.h"
#include "TSConfigurable.h"
#include "DataRecords.h"
#include "RecordCollectionSorter.h"

class BCMenu;
class IInfoProvider;

class CMainListCtrl : public CListCtrl,
	public ISkinnable, 
	public TSTranslatable/*, 
	public TSConfigurable*/
{
	DECLARE_DYNAMIC(CMainListCtrl)
public:
	CMainListCtrl();
	virtual ~CMainListCtrl();

	BOOL Create(DWORD style, CRect& rc, CWnd* pParent, UINT nID);

	void Update();
	void InvalidateInfoIcons();

	void GetSelectedTracks(FullTrackRecordCollection& col);
	
	FullTrackRecordCollection& GetFullTrackRecordCollection()			{return m_tracks;}
	void SetFullTrackRecordCollection(FullTrackRecordCollection& col)	{m_tracks = col;}

	//SortOptionCollection& GetSortOptionCollection()						{return m_SOC;}
	//void GetSortOptionCollection(SortOptionCollection& soc)				{m_SOC = soc;}

	INT GetItemCount();
	BOOL GetItem(FullTrackRecordSP& rec, INT idx);
	BOOL SelectItemByID(UINT TrackID);

	//We may have more than one Main List Control. We must keep each one's settings under a different key
	//void SetIdentifier(LPCTSTR id)//MAX 10 chars
	//{
	//	ASSERT(id != NULL);
	//	ASSERT(_tcslen(id)<=10);
	//	_tcsncpy(m_id, id, 10);
	//}
	UINT GetTrackID(int nItem);
	void OnSelectAll();
	void OnInvertSelection();
	void ClearSelection();


	void ShowInfoIcons(BOOL bEnable)	{m_bShowInfoIcons = bEnable;}
	BOOL IsShowInfoIconsEnabled()		{return m_bShowInfoIcons;}

	enum ColumnsEnum
	{
		CL_First,
		CL_Title = CL_First,
		CL_Controls,
		CL_Album,
		CL_Artist,
		CL_Rating,
		CL_TrackNo,
		CL_Path,
		CL_Collection,
		CL_Size,
		CL_Duration,
		CL_Bitrate,
		CL_Year,
		CL_Genre,
		CL_DateAdded,
		CL_Last
	};
	struct SColumnInfo
	{
		INT cx;
		INT iOrder;
		BOOL bVisible;
	};

	SColumnInfo* GetColumnInfo();
	void UpdateColumns();
	void ResetColumnInfo();


	SortOptionCollection& GetSortingInfo()			{return m_SOC;}
	void UpdateSorting();




public:
	virtual void ApplyTranslation(ITranslation& translation);
	virtual void ApplySkin(ISkin& skin);
	//=== TSConfigurable
	//virtual void UpdateConfiguration();

public:
	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_AltBk,
		COL_AltText,
		COL_SelBk,
		COL_SelText,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
	//virtual LPCTSTR GetColorName(UINT idx);
private:
	COLORREF m_colors[COL_Last];
	//---------------------------------------------------
	//---------------------------------------------------
	//---------------------------------------------------

	
private:
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/); 
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdfinditem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnHdnDividerdblclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:

	void ShowColumn(ColumnsEnum colName, BOOL bShow);
	void UpdateColumnInfo();
	ColumnsEnum SubItem2Column(INT iSubItem);
	INT Column2SubItem(ColumnsEnum col);
	void Init();
	//virtual void AddColumns();
	void DeleteMenus();
	void ShowProperties(UINT InitialPage);
	void NotifySelectedItemChanged();
	void AppendInfoProviderMenuItem(BCMenu& mnu, const IInfoProvider& IP, InfoItemTypeEnum iit);
	RecordSortOptionsEnum SubItem2RecordSortOption(INT subItem);
	INT RecordSortOptions2SubItem(RecordSortOptionsEnum rso);
	LPCTSTR FixName(LPCTSTR originalName);
	void GetRealSubItemRect(INT nItem, INT nSubItem, INT area, CRect& rcSubItem);
	void ResetMouseOver();
	void GetColorsFromItem(INT nItem, COLORREF& bkColor, COLORREF& textColor);
	void DrawControlsSubItem(CDC& dc, CRect& rcSubItem, COLORREF bkColor, COLORREF textColor, BOOL bMouseOver);
	void DrawRatingsSubItem(CDC& dc, CRect& rcSubItem, COLORREF bkColor, COLORREF textColor, BOOL bMouseOver, INT rating);

private:
	//TCHAR m_id[11];
	FullTrackRecordCollection m_tracks;
	FullTrackRecordCollection m_selectedTracks;
	SortOptionCollection m_SOC;
	CImageList m_LVMainList;//Empty Image List only for making the rows bigger..
	//TracksFilter m_Filter;
	BCMenu m_MainMenu;
	BCMenu m_RenamerMenu;
	BCMenu m_FileNameTaggerMenu;
	BCMenu m_CopyMenu;
	BCMenu m_ColumnsMenu;
	BCMenu m_ChangeCaseMenu;
	BCMenu m_AdvancedMenu;
	CBrush m_bkBrush;

	SColumnInfo m_columns[CL_Last];
	TCHAR cUnknown[100];
	struct ProviderRequest
	{
		std::tstring provID;
		InfoItemTypeEnum iit;
		ProviderRequest(LPCTSTR ProvID, InfoItemTypeEnum Iit):
		provID(ProvID), iit(Iit){}
	};
	std::vector<ProviderRequest> m_ProviderRequests;
	BOOL m_bDoNotUseDispOptimization;

	struct MouseOverDrawInfo
	{
		MouseOverDrawInfo():nItem(-1),nSubItem(0),xPos(0),bDrawn(FALSE){}
		INT nItem;
		INT nSubItem;
		INT xPos;
		BOOL bDrawn;
	} m_mouseOverDrawInfo;
	BOOL m_bReEntryLockCustomDraw;

	BOOL m_bUpdatingStateables;

	BOOL m_bShowInfoIcons;
};

#endif
