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
#ifndef _CMainTreeCtrl_h_
#define _CMainTreeCtrl_h_

#include "GuiControls/BCMenu.h"
#include "TracksFilter.h"
#include "TeenSpiritEnums.h"

#include "TSTranslatable.h"
#include "TSSkinnable.h"
#include "FlexTreeCtrl.h"
//#include "ICtrlColors.h"
//#include "TSNodeCollections.h"
#include "StateManager.h"
#include "TSConfigurable.h"
#include "RecordCollectionSorter.h"

class MainTreeItemBitmapCache;
class MainTreeDrawerHelper;

class IInfoProvider;
//class CGdiPPicDrawer;

class CMainTreeCtrl : public CFlexTreeCtrl, /*public ISectionViewer, */
	public TSSkinnable, public TSTranslatable/*, public TSConfigurable*/
{
public:
	CMainTreeCtrl();
	virtual ~CMainTreeCtrl();
	//=== class ISectionViewer ===========================================
public:
	void Reset();
	void Update(BOOL bInvalidateData);
	void EnableAutoUpdates(BOOL bEnable);
	//const TracksFilter& GetTracksFilter()	{return m_tracksFilter;}

	void SetMode(SectionModesEnum smode);
	SectionModesEnum GetMode()					{return m_sectionMode;}

	void SetStyle(SectionStylesEnum style);
	SectionStylesEnum GetStyle()					{return m_style;}

	void SetFilter(SectionFiltersEnum sfilter);
	SectionFiltersEnum GetFilter()				{return m_sectionFilter;}


	static SectionTypeEnum GetChildForSectionType(SectionModesEnum smode, SectionTypeEnum st);

	void GroupSmallSections(BOOL bGroup);
	BOOL IsSmallSectionsGrouped()			{return m_bSmallSectionsGrouped;}

	void SetSmallSectionLimit(UINT value);
	UINT GetSmallSectionLimit()				{return m_smallSectionsLimit;}

	void SetSort(SectionTypeEnum type,SortOption so);
	SortOption GetSort(SectionTypeEnum type);

	SectionTypeEnum GetSelectedType();

	BOOL GetSelectedFullArtistRecordSP(FullArtistRecordSP& rec);
	BOOL GetSelectedFullAlbumRecordSP(FullAlbumRecordSP& rec );
	BOOL GetSelectedGenreRecordSP(GenreRecordSP& rec );
	BOOL GetSelectedCollectionRecordSP(CollectionRecordSP& rec );
	BOOL GetSelectedYearRecordSP(YearRecordSP& rec );
	BOOL GetSelectedMonthAddedRecordSP(MonthAddedRecordSP& rec );

	BOOL SelectNodeFromFullTrackRecord(FullTrackRecordSP& track, SectionTypeEnum desiredST);

	void ApplyTranslation(ITranslation& translation);
	void ApplySkin(ISkin& skin);

	BOOL m_bModifyState;
	//void UpdateConfiguration();

private:
	TracksFilter m_tracksFilter;
	BOOL m_bSmallSectionsGrouped;
	UINT m_smallSectionsLimit;
	SectionStylesEnum m_style;
	SectionModesEnum m_sectionMode;
	SectionFiltersEnum m_sectionFilter;

	SortOption m_sort[ST_Last];

public:
	enum COLORS
	{
		COL_Text,
		COL_TextBk1,
		COL_TextBk2,
		COL_TextSub,
		COL_TextSubBk1,
		COL_TextSubBk2,
		COL_TextSel,
		COL_TextSelBk1,
		COL_TextSelBk2,
		COL_Last
	};
	virtual void SetColor(UINT idx, COLORREF value);
	virtual COLORREF GetColor(UINT idx);
private:
	COLORREF m_colors[COL_Last];
protected:
	//=== class CFlexTreeCtrl ===========================================
	virtual BOOL OnFillItems(TreeNode& node);
	virtual void DrawNode(Gdiplus::Graphics& g, TreeNode& node, DrawNodeInfo& nfo);
	virtual void OnSelectionChanged();
	virtual TreeNode* OnFindNode(LPCTSTR str);
	virtual void DrawQuickSearchString(Gdiplus::Graphics& g, LPCTSTR searchString);
	void DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r);
	virtual void OnItemMouseEvent(const ItemMouseEvent& ev);
	virtual void PostCreate();


	//====== IFlexDropTarget
	virtual BOOL ItemAcceptsFileDrop(CPoint& p, INT nItem);
	virtual BOOL OnFileDrop(const std::vector<std::tstring>& files, CPoint& p);
	//=== class CFlexTreeCtrl =========================================== END
private:
	TreeNode* FindCompatibleNode(TreeNode* pParent, FullTrackRecordSP& rec, SectionTypeEnum desiredST);
	void ModifyState();

public:
	void GetTracksFilterForNode(const TreeNode& node, TracksFilter& filter);
	void SetNotificationDelay(UINT delay)	{m_notificationDelay = delay;}
	UINT GetNotificationDelay()				{return m_notificationDelay;}



	enum DrawNodeModeFlags
	{
		DNM_PicturesEnabled = 1,
		DNM_PicturesExtraWidth = 2,
		DNM_TextTwoLines = 4,
		DNM_OnlyNames = 8,
		//DNM_
		//DNM_OneLiner,			//Old Style
		//DNM_TwoLiner,			//New Style - No Pictures
		//DNM_TwoLinerEnh,		//New Style
		DNM_Last
	};
	struct GDIRes
	{
		GDIRes()
		{
			memset(this, 0, sizeof(GDIRes));
			fImageScale = 1.0f;
		}
		~GDIRes()	{}

		BOOL bShowImages;
		FLOAT fImageScale;
		INT nLines;
		BOOL bFullInformation;

		//UINT dnm;

		//HBRUSH expandableBrush;
		//COLORREF expandableTextColor;

		//HBRUSH notExpandableBrush;
		//COLORREF notExpandableTextColor;

		//HBRUSH selectedBrush;
		//COLORREF selectedTextColor;

		//CFont* pMainFont;
		//CFont* pTitleFont;
		//UINT dwStrFormat;

		//HICON hStarIcon;
		//HICON hAntiStarIcon; 
			
		//Gdiplus::Bitmap* m_pArtistBitmap;
		//Gdiplus::Bitmap* m_pAlbumBitmap;

		Gdiplus::Bitmap* pBitmap;

		MainTreeItemBitmapCache* pCache;
		MainTreeDrawerHelper* pDrawer;

	};


	//=== class ICtrlStrings ===========================================
protected:
//	virtual void ApplyTranslation(ITranslation& skin); 
	//=== class ICtrlStrings =========================================== END

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT nIDEvent);	
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	void DeleteMenus();
	void FixName(LPTSTR originalName, UINT maxLen);
	void AppendInfoProviderMenuItem(BCMenu& mnu, const IInfoProvider& IP, InfoItemTypeEnum iit);
private:
	BOOL GetFullArtistRecordSP(FullArtistRecordSP& rec, const TreeNode& node);
	BOOL GetFullAlbumRecordSP(FullAlbumRecordSP& rec, const TreeNode& node);
	BOOL GetGenreRecordSP(GenreRecordSP& rec, const TreeNode& node);
	BOOL GetCollectionRecordSP(CollectionRecordSP& rec, const TreeNode& node);
	BOOL GetYearRecordSP(YearRecordSP& rec, const TreeNode& node);
	BOOL GetMonthAddedRecordSP(MonthAddedRecordSP& rec, const TreeNode& node);
	BOOL ExecuteNode(AppCommandsEnum appCmd, const TreeNode& node);

	GDIRes m_gdiRes;
	BOOL m_actionsHappened;
	BCMenu m_ArtistMenu;
	BCMenu m_AlbumMenu;
	BCMenu m_GenericMenu;

	const TreeNode* m_pMenuItitiator;
	BOOL m_bInitialRefresh;
	UINT m_lastNotification;
	UINT m_notificationDelay;

	UINT m_LatestTimer;	//Used to make the FULL REFRESH UPDATES less frequent by time
	static LPCTSTR SettingsCategory;

	struct ProviderRequest
	{
		std::tstring provID;
		InfoItemTypeEnum iit;
		ProviderRequest(LPCTSTR ProvID, InfoItemTypeEnum Iit):
		provID(ProvID), iit(Iit){}
	};
	std::vector<ProviderRequest> m_ProviderRequests;
	BOOL m_bAutoUpdateIsEnabled;
	INT m_updatesPerformed;

};


#endif
