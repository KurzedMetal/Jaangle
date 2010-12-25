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

#include "MainTreeCtrl.h"
#include "PrgAPI.h"
#include "ActionManager.h"
#include "ServicesRegistrationSrv.h"
#include "InfoDownloadManager.h"
#include "MenuManager.h"
#include "SQLManager.h"
#include "InformationProviders/InfoProviderFactory.h"
#include "LocalPictureManager.h"
#include "GuiControls/GdiPlusPicDrawer.h"
#include "GdiplusUtilities.h"
#include "UserProgressDlg.h"
#include "PointerCache.h"
#include "CollectionManager.h"

#define TIMER_AUTOUPDATE	110
#define TIMER_AUTOUPDATE_DELAY 30000

//===================================================================
//===================================================================
//					Custom Nodes
//===================================================================
//===================================================================

static const int cMargin = 1;

class MainTreeDrawerHelper
{
public:
	MainTreeDrawerHelper()	{}
	COLORREF selectedTextColor;
	COLORREF expandableTextColor;
	COLORREF notExpandableTextColor;
	HBRUSH selectedBrush;
	HBRUSH expandableBrush;
	HBRUSH notExpandableBrush;
	CFont* pTitleFont;
	CFont* pMainFont;
	DWORD dwStrFormat;
	HICON hAntiStarIcon;
	HICON hStarIcon;
	BOOL bPicturesEnabled;

	virtual void Draw(Gdiplus::Graphics& g, 
		CFlexTreeCtrl::DrawNodeInfo& nfo,
		CMainTreeCtrl::GDIRes& res,
		InfoItemTypeEnum iit,
		UINT itemID,
		LPCTSTR firstPart,
		LPCTSTR secondPart,
		INT rating,
		BOOL bIsExpandable
		) const
	{
		HDC hdc = g.GetHDC();
		COLORREF textColor = bIsExpandable ? expandableTextColor : notExpandableTextColor;
		if (nfo.bSelected)
			textColor = selectedTextColor;
		HBRUSH bkBrush = bIsExpandable ? expandableBrush : notExpandableBrush;
		if (nfo.bSelected)
			bkBrush = selectedBrush;
		::FillRect(hdc, &CRect(nfo.fullRC.X, nfo.fullRC.Y, nfo.fullRC.GetRight(), nfo.fullRC.GetBottom()), bkBrush);
		
		HFONT oldFont = NULL;
		INT X = nfo.itemRC.X + cMargin;
		if (res.bShowImages)
		{
			FLOAT imageScale = res.fImageScale;
			if (iit == IIT_AlbumPicture)
				imageScale = 1.0f;
			INT imgHeight = nfo.itemRC.Height - 2 * cMargin;
			INT imgWidth = INT(imgHeight * imageScale);
			INT startX = X;
			INT startY = nfo.itemRC.Y + cMargin;
			INT endX = startX + imgWidth;
			INT endY = startY + imgHeight;

			//=== Image
			LocalPictureManager* pLM = PRGAPI()->GetLocalPictureManager();
			CRect rcImage(startX + 1, startY + 1, endX - 1, endY - 1);
			BOOL bRet = pLM->DrawThumbnail(iit, itemID, hdc, rcImage);
			if (!bRet)
				bRet = pLM->DrawDefaultThumbnail(iit, hdc, rcImage);
			X += imgWidth + cMargin;
			//=== Image Frame
			HBRUSH oldBr = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
			HPEN oldPen = (HPEN)::SelectObject(hdc, ::GetStockObject(BLACK_PEN));
			::Rectangle(hdc, startX, startY, endX, endY);
			::SelectObject(hdc, oldBr);
			::SelectObject(hdc, oldPen);
		}

		//===Draw Rating if available
		INT ratingWidth = DrawRating(hdc, rating, X, nfo.itemRC.Y + cMargin);
		if (res.nLines == 1)
		{
			X += ratingWidth;
			::SetBkMode(hdc, TRANSPARENT);
			::SetTextColor(hdc, textColor);
			oldFont = (HFONT)::SelectObject(hdc, pTitleFont->m_hObject);
			CRect rc(X, nfo.itemRC.Y, nfo.itemRC.GetRight() - cMargin, nfo.itemRC.GetBottom());
			CRect calcRC(rc);
			DrawText(hdc, firstPart, -1, &calcRC, dwStrFormat | DT_CALCRECT);
			DrawText(hdc, firstPart, -1, &rc, dwStrFormat);
			if (secondPart != NULL)
			{
				oldFont = (HFONT)::SelectObject(hdc, pMainFont->m_hObject);
				rc.left += calcRC.Width() + 10;
				DrawText(hdc, secondPart, -1, &rc, dwStrFormat | DT_RIGHT);
			}
		}
		else 
		{
			INT lineHeight = nfo.itemRC.Height / res.nLines;
			INT yStart = nfo.itemRC.Y + cMargin;
			INT xStart = ratingWidth + X;
			if (res.nLines > 2)
			{
				yStart += 18;//=== (Rating should be i a different line
				lineHeight = (nfo.itemRC.Height - 18) / (res.nLines - 1);
				xStart = X;
			}

			//===Draw First Line.
			::SetBkMode(hdc, TRANSPARENT);
			::SetTextColor(hdc, textColor);
			oldFont = (HFONT)::SelectObject(hdc, pTitleFont->m_hObject);
			DrawText(hdc, firstPart, -1, 
				&CRect(xStart, 
				yStart, 
				nfo.itemRC.GetRight(), 
				yStart + lineHeight), 
				dwStrFormat);

			if (secondPart)
			{
				yStart += lineHeight;
				::SelectObject(hdc, pMainFont->m_hObject);
				DrawText(hdc, 
					secondPart, 
					-1, 
					&CRect(X, 
					yStart, 
					nfo.itemRC.GetRight(), 
					nfo.itemRC.GetBottom()), 
					dwStrFormat);
			}
		}
		if (oldFont != NULL) 
			::SelectObject(hdc, oldFont);
		g.ReleaseHDC(hdc);
	}
private:
	INT DrawRating(HDC hdc, INT rating, INT x, INT y) const
	{
		if (rating > 0 && rating < 256)
		{
			FLOAT fStars = Rating2Stars(rating);
			INT ret = 0;
			if (fStars > 0.0f && fStars <=1.0f && hAntiStarIcon)
			{
				DrawIconEx(hdc, x, y, hAntiStarIcon, 16, 16, 0, 0, DI_NORMAL);
				ret = 16;
			}
			while (fStars > 1.0f && hStarIcon)
			{
				DrawIconEx(hdc, x + ret, y, hStarIcon, 16, 16, 0, 0, DI_NORMAL);
				fStars -= 1.0f;
				ret += 16;
			}
			return ret;
		}
		return 0;
	}
};

class TSNode: public TreeNode
{
public:
	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const		= 0;
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const						= 0;
	virtual void Draw(Gdiplus::Graphics& g, 
		CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const				= 0;
};

class ArtistNode: public TSNode	//--------------------------------------------
{
public:
	ArtistNode(TreeNode* parent/*, FullArtistRecordSP& rec*/)			
	{/*SetFullArtistRecord(rec);*/bExpandable = TRUE; pParent = parent;}
	virtual ~ArtistNode()						{}
	virtual INT GetType() const					{return m_rec.get() != 0 ? ST_Artist : - ST_Artist;}
	virtual LPCTSTR GetString() const								
	{
		if (m_rec.get() != NULL)
			return m_rec->artist.name.c_str();
		return _T("");
	}

	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		if (m_rec.get() == NULL)
		{
			filter.ArtistTrackCount.val = sv.GetSmallSectionLimit();
			filter.ArtistTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.ArtistID.val = m_rec->artist.ID;
			filter.ArtistID.match = NUMM_Exact;
		}
	}
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const
	{
		if (m_rec.get() == NULL)
			return FALSE;
		return m_rec->artist.ID == rec->artist.ID;
	}
	void SetFullArtistRecord(FullArtistRecordSP& rec)
	{
		ASSERT(rec.get() != NULL && rec->IsValid());
		m_rec = rec;
	}
	virtual void Draw(Gdiplus::Graphics& g, CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_rec.get() == NULL)
		{
			res.pDrawer->Draw(g, nfo, res, IIT_ArtistPicture, 0, pAPI->GetString(IDS_VARIOUS), NULL, 0, bExpandable);
			return;
		}
		LPCTSTR firstPart = m_rec->artist.name.c_str();
		if (_tcscmp(firstPart, TS_UnknownString) == 0)
			firstPart = pAPI->GetString(IDS_UNKNOWN);

		LPCTSTR secondPart = NULL;
		const INT cMaxLen = 300;
		TCHAR bf[cMaxLen];
		if (res.bFullInformation)
		{
			if (res.nLines == 1)
			{
				INT curLen = 0;
				if (m_rec->genre.ID	!= 1)
					curLen = _sntprintf(bf, cMaxLen, _T("%s | %d"),
					m_rec->genre.name.c_str(),
					m_rec->artist.trackCount);
				else
					curLen = _sntprintf(bf, cMaxLen, _T("%d"),
					m_rec->artist.trackCount);
				secondPart = bf;
			}
			else 
			{
				INT curLen = 0;
				if (m_rec->genre.ID	!= 1)
					curLen = _sntprintf(bf, 300, _T("%s: %s | %s: %d"), 
					pAPI->GetString(IDS_GENRE),
					m_rec->genre.name.c_str(),
					pAPI->GetString(IDS_TRACKS),
					m_rec->artist.trackCount);
				else
					curLen = _sntprintf(bf, 300, _T("%s: %d"), 
					pAPI->GetString(IDS_TRACKS),
					m_rec->artist.trackCount);
				secondPart = bf;
			}
		}
		res.pDrawer->Draw(g, nfo, res, IIT_ArtistPicture, m_rec->artist.ID, firstPart, secondPart, m_rec->artist.rating, bExpandable);

	}

	const FullArtistRecordSP& GetFullArtistRecord() const	{return m_rec;}
private:
	FullArtistRecordSP m_rec;
};

class AlbumNode: public TSNode	//--------------------------------------------
{
public:
	AlbumNode(TreeNode* parent)			
	{bExpandable = FALSE; pParent = parent;}
	virtual ~AlbumNode()						{}
	virtual INT GetType() const					{return m_rec.get() != 0 ? ST_Album : -ST_Album;}
	virtual LPCTSTR GetString() const								
	{
		if (m_rec.get() != NULL)
			return m_rec->album.name.c_str();
		return _T("");
	}

	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		if (m_rec.get() == NULL)
		{
			filter.AlbumTrackCount.val = sv.GetSmallSectionLimit();
			filter.AlbumTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.AlbumID.val = m_rec->album.ID;
			filter.AlbumID.match = NUMM_Exact;
		}
	}
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const
	{
		if (m_rec.get() == NULL)
			return FALSE;
		return m_rec->album.ID == rec->album.ID;
	}
	void SetFullAlbumRecord(FullAlbumRecordSP& rec)
	{
		ASSERT(rec.get() != NULL && rec->IsValid());
		m_rec = rec;
	}
	virtual void Draw(Gdiplus::Graphics& g, CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_rec.get() == NULL)
		{
			res.pDrawer->Draw(g, nfo, res, IIT_AlbumPicture, 0, pAPI->GetString(IDS_VARIOUS), NULL, 0, bExpandable);
			return;
		}
		//===Translate [Unknown]
		LPCTSTR firstPart = m_rec->album.name.c_str();
		if (_tcscmp(firstPart, TS_UnknownString) == 0)
			firstPart = pAPI->GetString(IDS_UNKNOWN);

		LPCTSTR secondPart = NULL;
		const INT cMaxLen = 300;
		TCHAR bf[cMaxLen];
		if (res.bFullInformation)
		{
			if (res.nLines == 1)
			{
				INT curLen = 0;
				if (m_rec->album.year > 1800)
					curLen = _sntprintf(bf, 300, _T("%d | %d"),
					m_rec->album.year,
					m_rec->album.trackCount);
				else
					curLen = _sntprintf(bf, 300, _T("%d"),
					m_rec->album.trackCount);
				secondPart = bf;
			}
			else
			{
				INT curLen = 0;
				if (m_rec->album.year > 1800)
					curLen = _sntprintf(bf, 300, _T("%s: %d | %s: %d"), 
					PRGAPI()->GetString(IDS_YEAR),
					m_rec->album.year,
					PRGAPI()->GetString(IDS_TRACKS),
					m_rec->album.trackCount);
				else
					curLen = _sntprintf(bf, 300, _T("%s: %d"), 
					PRGAPI()->GetString(IDS_TRACKS),
					m_rec->album.trackCount);
				secondPart = bf;
			}
		}
		res.pDrawer->Draw(g, nfo, res, IIT_AlbumPicture, m_rec->album.ID, firstPart, secondPart, m_rec->album.rating, bExpandable);
	}
	const FullAlbumRecordSP& GetFullAlbumRecord() const	{return m_rec;}
private:
	FullAlbumRecordSP m_rec;
};

class AlbumByNameNode:public AlbumNode
{
public:
	AlbumByNameNode(TreeNode* parent):	AlbumNode(parent)	{}		
	virtual ~AlbumByNameNode()								{}
	virtual INT GetType() const					{return GetFullAlbumRecord().get() != 0 ? ST_AlbumByName : -ST_AlbumByName;}
	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		const FullAlbumRecordSP& rec = GetFullAlbumRecord();
		if (rec.get() == NULL)
		{
			filter.AlbumTrackCount.val = sv.GetSmallSectionLimit();
			filter.AlbumTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.Album.val = rec->album.name;
			filter.Album.match = TXTM_Exact;
		}
	}

};

class GenreNode: public TSNode	//--------------------------------------------
{
public:
	GenreNode(TreeNode* parent/*, GenreRecordSP& rec*/)				
	{/*SetGenreRecord(rec);*/bExpandable = TRUE; pParent = parent;}
	virtual ~GenreNode()						{}
	virtual INT GetType() const					{return m_rec.get() != 0 ? ST_Genre : -ST_Genre;}
	virtual LPCTSTR GetString() const								
	{
		if (m_rec.get() != NULL)
			return m_rec->name.c_str();
		return _T("");
	}

	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		if (m_rec.get() == NULL)
		{
			filter.GenreTrackCount.val = sv.GetSmallSectionLimit();
			filter.GenreTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.GenreID.val = m_rec->ID;
			filter.GenreID.match = NUMM_Exact;
		}
	}
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const
	{
		if (m_rec.get() == NULL)
			return FALSE;
		return m_rec->ID == rec->genre.ID;
	}
	virtual void Draw(Gdiplus::Graphics& g, CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_rec.get() == NULL)
		{
			res.pDrawer->Draw(g, nfo, res, IIT_GenrePicture, 0, pAPI->GetString(IDS_VARIOUS), NULL, 0, bExpandable);
			return;
		}
		LPCTSTR firstPart = m_rec->name.c_str();
		if (_tcscmp(firstPart, TS_UnknownString) == 0)
			firstPart = pAPI->GetString(IDS_UNKNOWN);

		LPCTSTR secondPart = NULL;
		const INT cMaxLen = 300;
		TCHAR bf[cMaxLen];
		if (res.bFullInformation)
		{
			INT curLen = 0;
			secondPart = bf;
			if (res.nLines == 1)
				curLen = _sntprintf(bf, 300, _T("%d"), m_rec->trackCount);
			else if (res.nLines == 2)
				curLen = _sntprintf(bf, 300, _T("%s: %d"), pAPI->GetString(IDS_TRACKS),	m_rec->trackCount);
			else
				ASSERT(0);
		}
		res.pDrawer->Draw(g, nfo, res, IIT_GenrePicture, m_rec->ID, firstPart, secondPart, 0, bExpandable);
	}
	void SetGenreRecord(GenreRecordSP& rec)
	{
		ASSERT(rec.get() != NULL && rec->IsValid());
		m_rec = rec;
	}

	const GenreRecordSP& GetGenreRecord() const	{return m_rec;}
private:
	GenreRecordSP m_rec;
};

class CollectionNode: public TSNode	//--------------------------------------------
{
public:
	CollectionNode(TreeNode* parent, CollectionRecordSP& rec)		
	{SetCollectionRecord(rec);bExpandable = TRUE; pParent = parent;}
	virtual ~CollectionNode()					{}
	virtual INT GetType() const					{return m_rec.get() != 0 ? ST_Collection : - ST_Collection;}
	virtual LPCTSTR GetString() const								
	{
		if (m_rec.get() != NULL)
			return m_rec->name.c_str();
		return _T("");
	}

	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		if (m_rec.get() == NULL)
		{
			filter.CollectionTrackCount.val = sv.GetSmallSectionLimit();
			filter.CollectionTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.CollectionID.val = m_rec->ID;
			filter.CollectionID.match = NUMM_Exact;
		}
	}
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const
	{
		if (m_rec.get() == NULL)
			return FALSE;
		return m_rec->ID == rec->collection.ID;
	}
	virtual void Draw(Gdiplus::Graphics& g, CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_rec.get() == NULL)
		{
			res.pDrawer->Draw(g, nfo, res, IIT_CollectionPicture, 0, pAPI->GetString(IDS_VARIOUS), NULL, 0, bExpandable);
			return;
		}
		LPCTSTR firstPart = m_rec->name.c_str();
		if (_tcscmp(firstPart, TS_UnknownString) == 0)
			firstPart = pAPI->GetString(IDS_UNKNOWN);

		LPCTSTR secondPart = NULL;
		const INT cMaxLen = 300;
		TCHAR bf[cMaxLen];
		if (res.bFullInformation)
		{
			INT curLen = 0;
			secondPart = bf;
			if (res.nLines == 1)
				curLen = _sntprintf(bf, 300, _T("%d"), m_rec->trackCount);
			else if (res.nLines == 2)
				curLen = _sntprintf(bf, 300, _T("%s: %s (0x%X) | %s: %d"),
					pAPI->GetString(IDS_LOCATION),
					m_rec->location.c_str(),
					m_rec->serial,
					pAPI->GetString(IDS_TRACKS),
					m_rec->trackCount);
			else
				ASSERT(0);
		}
		res.pDrawer->Draw(g, nfo, res, IIT_CollectionPicture, m_rec->ID, firstPart, secondPart, 0, bExpandable);
	}
	void SetCollectionRecord(CollectionRecordSP& rec)
	{
		ASSERT(rec.get() != NULL && rec->IsValid());
		m_rec = rec;
	}
	const CollectionRecordSP& GetCollectionRecord() const	{return m_rec;}
private:
	CollectionRecordSP m_rec;
};

class YearNode: public TSNode	//--------------------------------------------
{
public:
	YearNode(TreeNode* parent/*, YearRecordSP& rec*/)					
	{/*SetYearRecord(rec);*/bExpandable = TRUE; pParent = parent;}
	virtual ~YearNode()					{}
	virtual INT GetType() const					{return m_rec.get() != 0 ? ST_Year : - ST_Year;}
	virtual LPCTSTR GetString() const								
	{
		if (m_rec.get() != NULL)
			return year;
		return _T("");
	}
	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		if (m_rec.get() == NULL)
		{
			filter.YearTrackCount.val = sv.GetSmallSectionLimit();
			filter.YearTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.Year.val = m_rec->year;
			filter.Year.match = NUMM_Exact;
		}
	}
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const
	{
		if (m_rec.get() == NULL)
			return FALSE;
		return m_rec->year == rec->track.year;
	}
	virtual void Draw(Gdiplus::Graphics& g, CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_rec.get() == NULL)
		{
			res.pDrawer->Draw(g, nfo, res, IIT_YearPicture, 0, pAPI->GetString(IDS_VARIOUS), NULL, 0, bExpandable);
			return;
		}
		TCHAR firstPart[100];
		_sntprintf(firstPart, 300, _T("%d"), m_rec->year);

		LPCTSTR secondPart = NULL;
		const INT cMaxLen = 300;
		TCHAR bf[cMaxLen];
		if (res.bFullInformation)
		{
			INT curLen = 0;
			secondPart = bf;
			if (res.nLines == 1)
				curLen = _sntprintf(bf, 300, _T("%d"), m_rec->trackCount);
			else if (res.nLines == 2)
				curLen = _sntprintf(bf, 300, _T("%s: %d"), pAPI->GetString(IDS_TRACKS),	m_rec->trackCount);
			else
				ASSERT(0);
		}
		res.pDrawer->Draw(g, nfo, res, IIT_YearPicture, m_rec->year, firstPart, secondPart, 0, bExpandable);
	}
	void SetYearRecord(YearRecordSP& rec)
	{
		ASSERT(rec.get() != NULL && rec->IsValid());
		if (rec->year > 0)
			_itot(rec->year, year, 10);
		else
		{
			_tcsncpy(year, PRGAPI()->GetString(IDS_UNKNOWN), 30);
			year[29] = 0;
		}
		m_rec = rec;
	}
	const YearRecordSP& GetYearRecord() const			{return m_rec;}
private:
	YearRecordSP m_rec;
	TCHAR year[30];
};

class MonthAddedNode: public TSNode	//--------------------------------------------
{
public:
	MonthAddedNode(TreeNode* parent/*, YearRecordSP& rec*/)					
	{/*SetYearRecord(rec);*/bExpandable = TRUE; pParent = parent;}
	virtual ~MonthAddedNode()					{}
	virtual INT GetType() const					{return m_rec.get() != 0 ? ST_MonthAdded : - ST_MonthAdded;}
	virtual LPCTSTR GetString() const								
	{
		if (m_rec.get() != NULL)
			return month;
		return _T("");
	}
	virtual void AppendTracksFilter(TracksFilter& filter, CMainTreeCtrl& sv) const
	{
		if (m_rec.get() == NULL)
		{
			filter.MonthAddedTrackCount.val = sv.GetSmallSectionLimit();
			filter.MonthAddedTrackCount.match = NUMM_Under;
		}
		else
		{
			filter.MonthAdded.val = m_rec->month;
			filter.MonthAdded.match = NUMM_Exact;
		}
	}
	virtual BOOL IsFullTrackContained(FullTrackRecordSP& rec) const
	{
		if (m_rec.get() == NULL)
			return FALSE;
		return (GetYear() == rec->track.dateAdded.wYear) && 
			(GetMonth() == rec->track.dateAdded.wMonth);
	}
	virtual void Draw(Gdiplus::Graphics& g, CFlexTreeCtrl::DrawNodeInfo& nfo, CMainTreeCtrl::GDIRes& res) const
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_rec.get() == NULL)
		{
			res.pDrawer->Draw(g, nfo, res, IIT_MonthPicture, 0, pAPI->GetString(IDS_VARIOUS), NULL, 0, bExpandable);
			return;
		}
		Bitmap* bmp = NULL;
		if (res.bShowImages)
			bmp = res.pBitmap;
		TCHAR firstPart[100];
		_sntprintf(firstPart, 300, _T("%d %d"), 
			GetYear(),
			GetMonth());

		LPCTSTR secondPart = NULL;
		const INT cMaxLen = 300;
		TCHAR bf[cMaxLen];
		if (res.bFullInformation)
		{
			INT curLen = 0;
			secondPart = bf;
			if (res.nLines == 1)
				curLen = _sntprintf(bf, 300, _T("%d"), m_rec->trackCount);
			else if (res.nLines == 2)
				curLen = _sntprintf(bf, 300, _T("%s: %d"), pAPI->GetString(IDS_TRACKS),	m_rec->trackCount);
			else
				ASSERT(0);
		}
		res.pDrawer->Draw(g, nfo, res, IIT_MonthPicture, m_rec->month, firstPart, secondPart, 0, bExpandable);
	}
	void SetMonthAddedRecord(MonthAddedRecordSP& rec)
	{
		ASSERT(rec.get() != NULL && rec->IsValid());
		m_rec = rec;
		_sntprintf(month, 30, _T("%d %d"), GetYear(), GetMonth());
	}
	const MonthAddedRecordSP& GetMonthAddedRecord() const			{return m_rec;}
private:
	INT GetMonth() const 
	{
		if (m_rec.get() != NULL)
			return (m_rec->month % 12) + 1;
		return 0;
	}
	INT GetYear() const 
	{
		if (m_rec.get() != NULL)
			return (m_rec->month / 12);
		return 0;
	}
	MonthAddedRecordSP m_rec;
	TCHAR month[30];
};





//===================================================================
//===================================================================
//					Main Control
//===================================================================
//===================================================================


#define REFRESH_DELAY_TIMER_ID 100
#define REFRESH_DELAY_TIMER_DELAY 300

#define ID_PROVIDERS 1000
//#define ID_INSERTRANDOMTRACKS 600

CMainTreeCtrl::CMainTreeCtrl():
m_bSmallSectionsGrouped(FALSE),
m_smallSectionsLimit(3),
m_sectionMode(SMODE_ArtistsAlbums),
m_sectionFilter(SFILT_All),
m_bInitialRefresh(TRUE),
m_lastNotification(0),
m_notificationDelay(200),
m_pMenuItitiator(NULL),
m_actionsHappened(FALSE),
m_style(SSTYL_1_Small),
m_bModifyState(TRUE),
m_bAutoUpdateIsEnabled(FALSE),
m_updatesPerformed(0)
{
	SetSort(ST_Artist, SortOption(RSO_Title,TRUE));
	SetSort(ST_Album, SortOption(RSO_Year,FALSE));
	SetSort(ST_Genre, SortOption(RSO_Title,TRUE));
	SetSort(ST_Collection, SortOption(RSO_Title,TRUE));
	SetSort(ST_Year, SortOption(RSO_Title,FALSE));
	SetSort(ST_MonthAdded, SortOption(RSO_Title,FALSE));


	PrgAPI* pAPI = PRGAPI();
	//HDC hdc = ::GetDC(0);
	//CFont* pFont = pAPI->GetFont(FONT_Main);
	//m_gdiRes.pMainFont = pFont;
	//m_gdiRes.pTitleFont = pAPI->GetFont(FONT_MainBold);

	m_gdiRes.pDrawer = new MainTreeDrawerHelper;

	m_gdiRes.pDrawer->expandableBrush = ::CreateSolidBrush(RGB(200, 200, 255));
	m_gdiRes.pDrawer->expandableTextColor = RGB(0, 0, 0);

//	m_gdiRes.pDrawer->notExpandableBrush = ::CreateSolidBrush(RGB(200, 200, 255));
//	m_gdiRes.pDrawer->notExpandableTextColor = RGB(0, 0, 0);

	m_gdiRes.pDrawer->selectedBrush = ::CreateSolidBrush(RGB(50, 50, 230));
	m_gdiRes.pDrawer->selectedTextColor = RGB(200, 200, 200);

	m_gdiRes.pDrawer->dwStrFormat = DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER;

	m_gdiRes.pDrawer->hStarIcon = pAPI->GetIcon(ICO_StarGold16);
	m_gdiRes.pDrawer->hAntiStarIcon = pAPI->GetIcon(ICO_StarBad16);

	//m_gdiRes.pBitmap = NULL;

	//m_gdiRes.pBitmap = new CGdiPPicDrawer;
	//m_gdiRes.pBitmap->LoadResourceID(IDR_PNG_ARTIST, _T("png"));
	//m_gdiRes.pBitmap->SetBkColor(RGB(0,0,0), 0);
	////m_gdiRes.m_pArtistBitmap->LoadResourceID(IDR_ARTIST, _T("jpg"));
	//m_gdiRes.m_pArtistBitmap->GetDrawParams().zoomLockMode = CGdiPPicDrawer::ZLM_FillArea;
	//m_gdiRes.m_pArtistBitmap->GetDrawParams().positionY = 0;

	//m_gdiRes.m_pAlbumBitmap = new CGdiPPicDrawer;
	//m_gdiRes.m_pAlbumBitmap->LoadResourceID(IDR_ALBUM, _T("jpg"));
	//m_gdiRes.m_pAlbumBitmap->GetDrawParams().zoomLockMode = CGdiPPicDrawer::ZLM_FillArea;

	m_gdiRes.bShowImages = TRUE;
	m_gdiRes.bFullInformation = TRUE;
	m_gdiRes.nLines = 2;
	SetStyle(SSTYL_3);

}

CMainTreeCtrl::~CMainTreeCtrl()
{
	DeleteMenus();
	if (m_hWnd) DestroyWindow();
	DeleteObject(m_gdiRes.pDrawer->selectedBrush);
	DeleteObject(m_gdiRes.pDrawer->expandableBrush);
	DeleteObject(m_gdiRes.pDrawer->notExpandableBrush);
	delete m_gdiRes.pDrawer;
}

void CMainTreeCtrl::Reset()
{
	TRACEST(_T("CMainTreeCtrl::Reset"));
	TreeNode* lastNode = GetNodeByIndex(GetNextSelectedItem());
	SectionTypeEnum lastST = ST_Unknown;
	BOOL bLastExpanded = FALSE;
	FullTrackRecordSP rec;
	INT lastScrollPos = GetScrollPos();
	if (lastNode != NULL)
	{
		//===Store LastItem
		lastST = (SectionTypeEnum) lastNode->GetType();
		bLastExpanded = lastNode->bExpanded;
		while (lastST != ST_Root)
		{
			TracksFilter tf;					
			GetTracksFilterForNode(*lastNode, tf);
			FullTrackRecordCollection col;
			PrgAPI* pAPI = PRGAPI();
			if (pAPI->GetSQLManager()->GetFullTrackRecordCollectionByTracksFilter(col, tf, 1))
			{
				if (col.size() == 1)
				{
					rec = col[0];
					break;
				}
				else
					TRACE(_T("@3 CMainTreeCtrl::Reset. Cant find tracks from tracks filter (1)\r\n"));

			}
			else
			{
				TRACE(_T("@3 CMainTreeCtrl::Reset. Cant find tracks from tracks filter (2)\r\n"));
			}
			lastNode = lastNode->pParent;
			if (lastNode == NULL)
			{
				ASSERT(0);
				break;
			}
			lastST = (SectionTypeEnum) lastNode->GetType();
			bLastExpanded = TRUE;//=== Set it to true as the current item does not exist anymore
		}
	}
	ClearItems();
	GetRootNode().RemoveChildren();
	GetRootNode().bExpanded = FALSE;
	Expand(GetRootNode(), TRUE);					//===ReExpand root

	BOOL bItemSelected = FALSE;
	if (rec.get() != NULL && lastST != ST_Unknown)	//===ReStore LastItem
		bItemSelected = SelectNodeFromFullTrackRecord(rec, lastST);
	if (bItemSelected)
	{
		INT idx = GetNextSelectedItem();
		if (idx != -1)
		{
			TreeNode* pTN = GetNodeByIndex(idx);
			if (pTN != NULL)
				Expand(*pTN, TRUE);
		}
	}
	else
	{
		if (lastScrollPos != 0)
		{
			SetScrollPos(lastScrollPos);
			INT vis = GetNextVisibleItem();
			if (vis != -1)
				SetItemSelection(vis + 1, TRUE);
			TRACE(_T("@3 CMainTreeCtrl::Reset. Locate using scrollPos\r\n"));
		}
		else
			TRACE(_T("@3 CMainTreeCtrl::Reset. No location\r\n"));
	}

	//if (!bItemSelected && GetItemCount() > 0)
	//	SetItemSelection(0, TRUE);
	Refresh();

}
void CMainTreeCtrl::Update(BOOL bInvalidateData)
{
	if (bInvalidateData)
		Reset();
	Refresh();
}

void CMainTreeCtrl::EnableAutoUpdates(BOOL bEnable)
{
	if (m_bAutoUpdateIsEnabled != bEnable)
	{
		m_bAutoUpdateIsEnabled = bEnable;
		m_updatesPerformed = 0;
		if (bEnable)
			SetTimer(TIMER_AUTOUPDATE, TIMER_AUTOUPDATE_DELAY, 0);
		else
			KillTimer(TIMER_AUTOUPDATE);
	}
}

SectionTypeEnum CMainTreeCtrl::GetSelectedType()
{
	INT nItem = GetNextSelectedItem();
	if (nItem > -1)
	{
		TreeNode* pTN = GetNodeByIndex(GetNextSelectedItem());
		if (pTN != NULL)
			return (SectionTypeEnum) pTN->GetType();
		ASSERT(0);
	}
	return ST_Unknown;
}

TreeNode* CMainTreeCtrl::FindCompatibleNode(TreeNode* pParent, FullTrackRecordSP& rec, SectionTypeEnum desiredST)
{
	ASSERT(pParent != NULL);
	UINT childCount = pParent->GetChildCount();
	for (UINT i = 0; i < childCount; i++)
	{
		TSNode* pNode = dynamic_cast<TSNode*> (pParent->GetChild(i));
		if (pNode->IsFullTrackContained(rec))
		//if (IsNodeCompatible(pNode, rec))
		{
			//Found Compatible. We will return something here
			if (pNode->GetType() != desiredST && pNode->bExpandable)
			{
				Expand(*pNode, TRUE);
				TreeNode* ret = FindCompatibleNode(pNode, rec, desiredST);
				if (ret != NULL)
					return ret;
			}
			return pNode;
		}
	}
	return NULL;
}

BOOL CMainTreeCtrl::SelectNodeFromFullTrackRecord(FullTrackRecordSP& rec, SectionTypeEnum desiredST)
{
	TRACEST(_T("CMainTreeCtrl::SelectNodeFromFullTrackRecord"), desiredST);
	ASSERT(rec->IsValid());
	TreeNode* pRoot = &GetRootNode();
	TreeNode* ret = FindCompatibleNode(pRoot, rec, desiredST);
	if (ret == NULL)
	{
		if (pRoot->GetChildCount() > 0)
		{
			TreeNode* pParent = pRoot->GetChild(0);
			if (pParent->GetType() < 0)
			{
				//Various collection is available. Search inside there
				Expand(*pParent, TRUE);
				ret = FindCompatibleNode(pParent, rec, desiredST);
			}
		}
	}
	if (ret != NULL)
	{
		INT nItem = GetIndexByNode(ret);
		if (nItem != -1)
		{
			SetItemSelection(nItem, TRUE);
			CenterItem(nItem);
		}
		return TRUE;
	}
	return FALSE;
}

void CMainTreeCtrl::GroupSmallSections(BOOL bGroup)
{
	if (bGroup != m_bSmallSectionsGrouped)
		m_bSmallSectionsGrouped = bGroup ;
}

void CMainTreeCtrl::SetSmallSectionLimit(UINT value)
{
	if (value != m_smallSectionsLimit)
		m_smallSectionsLimit = value;
}

BEGIN_MESSAGE_MAP(CMainTreeCtrl, CFlexTreeCtrl)
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

void CMainTreeCtrl::GetTracksFilterForNode(const TreeNode& node, TracksFilter& filter)
{
	filter = m_tracksFilter;
	const TreeNode* pFilterNode = &node;
	ASSERT(pFilterNode != NULL);
	if (pFilterNode == NULL)
		return;
	while (pFilterNode->pParent != NULL)
	{
		const TSNode* pTSNode = dynamic_cast<const TSNode*> (pFilterNode);
		if (pTSNode != NULL)
		{
			pTSNode->AppendTracksFilter(filter, *this);
			pFilterNode = pFilterNode->pParent;
			if (pFilterNode == NULL)
			{
				ASSERT(0);
				break;
			}
		}
		else
		{
			TRACE(_T("CMainTreeCtrl::GetTracksFilterForNode Cast failure\r\n"));
			ASSERT(0);
			break;
		}
	}
}


BOOL CMainTreeCtrl::OnFillItems(TreeNode& node)
{
	ASSERT(node.bExpandable);
	ASSERT(node.GetChildCount() == 0);
	if (node.bExpandable && node.GetChildCount() == 0)
	{
		TRACEST(_T("CMainTreeCtrl::OnFillItems"));
		node.bExpanded = TRUE;
		//===Application Objects
		PrgAPI* pAPI = PRGAPI();
		SQLManager* pSM = pAPI->GetSQLManager();

		//===Init
		BOOL bVarious = FALSE;
		INT nodeType = node.GetType();
		if (nodeType < 0)
		{
			bVarious = TRUE;
			//nodeType = -nodeType;
		}
		SectionTypeEnum stNode = (SectionTypeEnum) nodeType;
		SectionTypeEnum stChild = GetChildForSectionType(m_sectionMode, stNode);
		BOOL bExpandable = GetChildForSectionType(m_sectionMode, stChild) != ST_Unknown;
		if (bVarious)
			bExpandable = TRUE;

		//===Build the filter based on inheritance
		TracksFilter filter;
		GetTracksFilterForNode(node, filter);
		filter.MinNumTracks.val = m_smallSectionsLimit;
		filter.MinNumTracks.match = NUMM_Disabled;

		//===Fill the node
		SortOptionCollection soc;
		BOOL bAllowGrouping = m_bSmallSectionsGrouped && (stNode == ST_Root);
		switch(stChild)
		{
		case ST_Artist:
			{
				if (bVarious)
					filter.MinNumTracks.match = NUMM_Under;
				else if (bAllowGrouping)
				{
					node.AppendChild(new ArtistNode(&node));
					filter.MinNumTracks.match = NUMM_Over;
				}
				FullArtistRecordCollection recCol;
				if (pSM->GetFullArtistRecordCollectionByTracksFilter(recCol, filter))
				{
					soc.ApplySortOption(GetSort(ST_Artist));
					RecordCollectionSorter::SortFullArtistRecordCollection(recCol, soc);
					FullArtistRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						ArtistNode* pTN = new ArtistNode(&node);
						pTN->SetFullArtistRecord(*it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		case ST_Album:
			{
				if (bVarious)
					filter.MinNumTracks.match = NUMM_Under;
				else if (bAllowGrouping)
				{
					AlbumNode* pTN = new AlbumNode(&node);
					pTN->bExpandable = TRUE;
					node.AppendChild(pTN);
					filter.MinNumTracks.match = NUMM_Over;
				}
				FullAlbumRecordCollection recCol;
				if (pSM->GetFullAlbumRecordCollectionByTracksFilter(recCol, filter))
				{
					soc.ApplySortOption(GetSort(ST_Album));
					RecordCollectionSorter::SortFullAlbumRecordCollection(recCol, soc);
					FullAlbumRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						AlbumNode* pTN = new AlbumNode(&node);
						pTN->SetFullAlbumRecord(*it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		case ST_AlbumByName:
			{
				if (bVarious)
					filter.MinNumTracks.match = NUMM_Under;
				else if (bAllowGrouping)
				{
					AlbumByNameNode* pTN = new AlbumByNameNode(&node);
					pTN->bExpandable = TRUE;
					node.AppendChild(pTN);
					filter.MinNumTracks.match = NUMM_Over;
				}
				FullAlbumRecordCollection recCol;
				if (pSM->GetFullAlbumGPNRecordCollectionByTracksFilter(recCol, filter))
				{
					soc.ApplySortOption(GetSort(ST_Album));
					RecordCollectionSorter::SortFullAlbumRecordCollection(recCol, soc);
					FullAlbumRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						AlbumByNameNode* pTN = new AlbumByNameNode(&node);
						pTN->SetFullAlbumRecord(*it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		case ST_Genre:
			{
				if (bVarious)
					filter.MinNumTracks.match = NUMM_Under;
				else if (bAllowGrouping)
				{
					node.AppendChild(new GenreNode(&node));
					filter.MinNumTracks.match = NUMM_Over;
				}
				GenreRecordCollection recCol;
				if (pSM->GetGenreRecordCollectionByTracksFilter(recCol, filter, TRUE))
				{
					soc.ApplySortOption(GetSort(ST_Genre));
					RecordCollectionSorter::SortGenreRecordCollection(recCol, soc);
					GenreRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						GenreNode* pTN = new GenreNode(&node/*, *it*/);
						pTN->SetGenreRecord(*it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		case ST_Collection:
			{
				CollectionRecordCollection recCol;
				if (pSM->GetCollectionRecordCollectionByTracksFilter(recCol, filter, TRUE))
				{
					soc.ApplySortOption(GetSort(ST_Collection));
					RecordCollectionSorter::SortCollectionRecordCollection(recCol, soc);
					CollectionRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						TreeNode* pTN = new CollectionNode(&node, *it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		case ST_Year:
			{
				if (bVarious)
					filter.MinNumTracks.match = NUMM_Under;
				else if (bAllowGrouping)
				{
					node.AppendChild(new YearNode(&node));
					filter.MinNumTracks.match = NUMM_Over;
				}
				YearRecordCollection recCol;
				if (pSM->GetYearRecordCollectionByTracksFilter(recCol, filter, TRUE))
				{
					soc.ApplySortOption(GetSort(ST_Year));
					RecordCollectionSorter::SortYearRecordCollection(recCol, soc);
					YearRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						YearNode* pTN = new YearNode(&node/*, *it*/);
						pTN->SetYearRecord(*it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		case ST_MonthAdded:
			{
				if (bVarious)
					filter.MinNumTracks.match = NUMM_Under;
				else if (bAllowGrouping)
				{
					node.AppendChild(new MonthAddedNode(&node));
					filter.MinNumTracks.match = NUMM_Over;
				}
				MonthAddedRecordCollection recCol;
				if (pSM->GetMonthAddedRecordCollectionByTracksFilter(recCol, filter, TRUE))
				{
					soc.ApplySortOption(GetSort(ST_MonthAdded));
					RecordCollectionSorter::SortMonthAddedRecordCollection(recCol, soc);
					MonthAddedRecordCollection::iterator it = recCol.begin();
					for (;it != recCol.end(); it++)
					{
						MonthAddedNode* pTN = new MonthAddedNode(&node/*, *it*/);
						pTN->SetMonthAddedRecord(*it);
						pTN->bExpandable = bExpandable;
						node.AppendChild(pTN);
					}
				}
			}
			break;
		//case ST_Directory:
		//	{
		//		DirectoryRecordCollection recCol;
		//		if (pSM->GetDirectoryRecordCollectionByTracksFilter(recCol, filter, TRUE))
		//		{
		//			soc.ApplySortOption(GetSort(ST_MonthAdded));
		//			RecordCollectionSorter::SortMonthAddedRecordCollection(recCol, soc);
		//			DirectoryRecordCollection::iterator it = recCol.begin();
		//			for (;it != recCol.end(); it++)
		//			{
		//				DirectoryNode* pTN = new DirectoryNode(&node/*, *it*/);
		//				pTN->SetDirectoryRecord(*it);
		//				pTN->bExpandable = bExpandable;
		//				node.AppendChild(pTN);
		//			}
		//		}
		//	}
		//	break;
		case ST_Root:
		default:
			ASSERT(0);
			break;
		}
		return node.GetChildCount() > 0;
	}
	return FALSE;
}


BOOL CMainTreeCtrl::GetFullArtistRecordSP(FullArtistRecordSP& rec, const TreeNode& node)
{
	ASSERT((SectionTypeEnum)node.GetType() == ST_Artist);
	if ((SectionTypeEnum)node.GetType() == ST_Artist)
	{
		rec = ((ArtistNode*) (&node))->GetFullArtistRecord();
		return TRUE;
	}
	return FALSE;
}

BOOL CMainTreeCtrl::GetFullAlbumRecordSP(FullAlbumRecordSP& rec, const TreeNode& node)
{
	ASSERT((SectionTypeEnum)node.GetType() == ST_Album);
	if ((SectionTypeEnum)node.GetType() == ST_Album)
	{
		rec = ((AlbumNode*) (&node))->GetFullAlbumRecord();
		return TRUE;
	}
	return FALSE;
}
BOOL CMainTreeCtrl::GetGenreRecordSP(GenreRecordSP& rec, const TreeNode& node)
{
	ASSERT((SectionTypeEnum)node.GetType() == ST_Genre);
	if ((SectionTypeEnum)node.GetType() == ST_Genre)
	{
		rec = ((GenreNode*) (&node))->GetGenreRecord();
		return TRUE;
	}
	return FALSE;
}

BOOL CMainTreeCtrl::GetCollectionRecordSP(CollectionRecordSP& rec, const TreeNode& node)
{
	ASSERT((SectionTypeEnum)node.GetType() == ST_Collection);
	if ((SectionTypeEnum)node.GetType() == ST_Collection)
	{
		rec = ((CollectionNode*) (&node))->GetCollectionRecord();
		return TRUE;
	}
	return FALSE;
}

BOOL CMainTreeCtrl::GetYearRecordSP(YearRecordSP& rec, const TreeNode& node)
{
	ASSERT((SectionTypeEnum)node.GetType() == ST_Year);
	if ((SectionTypeEnum)node.GetType() == ST_Year)
	{
		rec = ((YearNode*) (&node))->GetYearRecord();
		return TRUE;
	}
	return FALSE;
}

BOOL CMainTreeCtrl::GetMonthAddedRecordSP(MonthAddedRecordSP& rec, const TreeNode& node)
{
	ASSERT((SectionTypeEnum)node.GetType() == ST_MonthAdded);
	if ((SectionTypeEnum)node.GetType() == ST_MonthAdded)
	{
		rec = ((MonthAddedNode*) (&node))->GetMonthAddedRecord();
		return TRUE;
	}
	return FALSE;
}
void CMainTreeCtrl::DrawNode(Gdiplus::Graphics& g, TreeNode& node, DrawNodeInfo& nfo)
{
	const TSNode* pNode = dynamic_cast<const TSNode*>(&node);
	ASSERT(pNode != NULL);
	pNode->Draw(g, nfo, m_gdiRes);
}
void CMainTreeCtrl::DrawBackground(Graphics& g, const Rect& r)
{
	SolidBrush bkBrush(GdiplusUtilities::COLORREF2Color(m_colors[COL_TextBk1]));
	g.FillRectangle(&bkBrush, r);
}


void CMainTreeCtrl::OnItemMouseEvent(const ItemMouseEvent& ev)
{
	switch (ev.type)
	{
	case IMET_LButtonDblClick:
		{
			TreeNode* tn = GetNodeByIndex(ev.activeItem);
			if (tn != NULL)
			{
				if (tn->bExpandable)
					Expand(*tn, !tn->bExpanded);
				else
					ExecuteNode(ACMD_DefaultPlayerAction, *tn);
			}
			break;
		}
	default:
		CFlexTreeCtrl::OnItemMouseEvent(ev);
	}
}


void CMainTreeCtrl::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		DeleteMenus();
		SetStyle(GetStyle());
		//Refresh();
	}
}

void CMainTreeCtrl::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;

	//=== Colors
	SetColor(COL_Text, skin.GetColor(CSEC_TreeCtrl, CVAL_Text));
	SetColor(COL_TextBk1, skin.GetColor(CSEC_TreeCtrl, CVAL_Bk));
	SetColor(COL_TextSub, skin.GetColor(CSEC_TreeCtrl, CVAL_SubText));
	SetColor(COL_TextSubBk1, skin.GetColor(CSEC_TreeCtrl, CVAL_SubBk));
	SetColor(COL_TextSel, skin.GetColor(CSEC_TreeCtrl, CVAL_SelText));
	SetColor(COL_TextSelBk1, skin.GetColor(CSEC_TreeCtrl, CVAL_SelBk));

	SetStyle(m_style);

//COL_Text,
//COL_TextBk1,
//COL_TextBk2,
//COL_TextSub,
//COL_TextSubBk1,
//COL_TextSubBk2,
//COL_TextSel,
//COL_TextSelBk1,
//COL_TextSelBk2,


}

//void CMainTreeCtrl::UpdateConfiguration()
//{
//
//	PrgAPI* pAPI = PRGAPI();
//	SetMode((SectionModesEnum)pAPI->GetOption(OPT_TREE_Mode));
//	GroupSmallSections(pAPI->GetOption(OPT_TREE_GroupingEnabled) != 0);
//	SetSmallSectionLimit(pAPI->GetOption(OPT_TREE_GroupingValue));
//	SetStyle((SectionStylesEnum)pAPI->GetOption(OPT_TREE_Style));
//
//	INT rso = pAPI->GetOption(OPT_TREE_SortArtist);
//	SetSort(ST_Artist, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
//	rso = pAPI->GetOption(OPT_TREE_SortAlbum);
//	SetSort(ST_Album, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
//	rso = pAPI->GetOption(OPT_TREE_SortGenre);
//	SetSort(ST_Genre, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
//	rso = pAPI->GetOption(OPT_TREE_SortCollection);
//	SetSort(ST_Collection, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
//	rso = pAPI->GetOption(OPT_TREE_SortYear);
//	SetSort(ST_Year, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
//	rso = pAPI->GetOption(OPT_TREE_SortMonthAdded);
//	SetSort(ST_MonthAdded, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
//
//	SetFilter((SectionFiltersEnum)pAPI->GetOption(OPT_TREE_Filter));
//
//}


void CMainTreeCtrl::DeleteMenus()
{
	if (m_ArtistMenu.m_hMenu)	m_ArtistMenu.DestroyMenu();
	if (m_AlbumMenu.m_hMenu)	m_AlbumMenu.DestroyMenu();
	if (m_GenericMenu.m_hMenu)	m_GenericMenu.DestroyMenu();
	m_ProviderRequests.clear();
}

void CMainTreeCtrl::DrawQuickSearchString(Gdiplus::Graphics& g, LPCTSTR searchString)
{
	CRect rc;
	GetClientRect(&rc);
	const INT margin = 10;
	rc.left += margin;
	rc.right -= 3 * margin;
	rc.top += margin;
	rc.bottom = rc.top + 20;
	if (rc.Width() > 50)
	{
		Gdiplus::SolidBrush br(Gdiplus::Color::MakeARGB(150, 0, 50, 0));
		g.FillRectangle(&br, rc.left, rc.top, rc.Width(), rc.Height());
		HDC hdc = g.GetHDC();
		//Gdiplus::SolidBrush textBr(Gdiplus::Color::MakeARGB(255, 255,255,255));

		::SetBkMode(hdc, TRANSPARENT);
		::SetTextColor(hdc, RGB(255,255,255));
		HFONT oldFont = (HFONT)::SelectObject(hdc, m_gdiRes.pDrawer->pTitleFont->m_hObject);
		DrawText(hdc, searchString, -1, 
			&rc, m_gdiRes.pDrawer->dwStrFormat);
		//DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER);
		::SelectObject(hdc, oldFont);
		g.ReleaseHDC(hdc);

	//	g.DrawString(searchString, 
	//		-1, 
	//		m_gdiRes.pMainTextFont, 
	//		Gdiplus::RectF(
	//		(Gdiplus::REAL)rc.left, 
	//		(Gdiplus::REAL)rc.top, 
	//		(Gdiplus::REAL)rc.Width(), 
	//		(Gdiplus::REAL)rc.Height()), 
	//		m_gdiRes.pMainStringFormat, 
	//		&textBr);
	}
}



void CMainTreeCtrl::SetMode(SectionModesEnum smode)
{
	ASSERT(smode > SMODE_First && smode < SMODE_Last);
	if (m_sectionMode != smode)
		m_sectionMode = smode;
}

void CMainTreeCtrl::SetStyle(SectionStylesEnum style)
{
	ASSERT(style > SSTYL_First && style < SSTYL_Last);
	INT oldScrollPos = GetScrollPos();
	INT oldItemHeight = GetItemHeight();
	m_style = style;
	TSSkin& skin = (TSSkin&) PRGAPI()->GetSkinManager()->GetSkin();
	INT rfs = skin.GetRelativeFontSize();
	m_gdiRes.nLines = 2;
	m_gdiRes.bShowImages = TRUE;
	m_gdiRes.bFullInformation = TRUE;
	m_gdiRes.fImageScale = 1.0f;
	switch (m_style)
	{
	case SSTYL_1_Small:
		SetItemHeight(18 + rfs);		
		m_gdiRes.nLines = 1;
		m_gdiRes.bShowImages = FALSE;
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_NormalBold);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Normal);
		break;
	case SSTYL_2:
		SetItemHeight(32 + 2 * rfs);	
		m_gdiRes.bShowImages = FALSE;
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_NormalBold);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Normal);
		break;
	case SSTYL_3:
		SetItemHeight(32 + 2 * rfs);	
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_NormalBold);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Normal);
		break;
	case SSTYL_4:
		m_gdiRes.nLines = 3;
		SetItemHeight(64 + 2 * rfs);	
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Big);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Normal);
		break;
	case SSTYL_5:
		m_gdiRes.nLines = 3;
		SetItemHeight(64 + 2 * rfs);	
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_BigBold);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Big);
		m_gdiRes.fImageScale = 1.4f; 
		break;
	case SSTYL_6:
		m_gdiRes.nLines = 4;
		SetItemHeight(96 + 2 * rfs);	
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_VeryBig);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Big);
		break;
	case SSTYL_7:
		SetItemHeight(32 + rfs);	
		m_gdiRes.nLines = 1;
		m_gdiRes.bFullInformation = FALSE;
		m_gdiRes.pDrawer->pTitleFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Big);
		m_gdiRes.pDrawer->pMainFont = skin.GetFont(CSEC_TreeCtrl, FSTY_Big);
		break;
	default:
		ASSERT(0);
		break;
	}
	SetScrollPos(oldScrollPos * GetItemHeight() / oldItemHeight);
	Refresh();
}


void CMainTreeCtrl::SetFilter(SectionFiltersEnum sfilter)
{
	ASSERT(sfilter > SFILT_First && sfilter < SFILT_Last);
	if (m_sectionFilter != sfilter)
	{	
		m_tracksFilter = TracksFilter();
		switch (sfilter)
		{
		case SFILT_Audio:
			m_tracksFilter.TrackType.match = NUMM_Under;
			m_tracksFilter.TrackType.val = 99;
			break;
		case SFILT_Video:
			m_tracksFilter.TrackType.match = NUMM_Over;
			m_tracksFilter.TrackType.val = 99;
			break;
		case SFILT_New:
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				DOUBLE dtime;
				SystemTimeToVariantTime(&st, &dtime);
				dtime -= PRGAPI()->GetOption(OPT_GEN_DaysMeansNew);
				VariantTimeToSystemTime(dtime, &st);
				m_tracksFilter.DateAdded.val = st;
				m_tracksFilter.DateAdded.match = DATM_After;
			}
			break;
		}
		m_sectionFilter = sfilter;
	}
}


const SectionTypeEnum ArtAlb[] = {ST_Root, ST_Artist, ST_Album, ST_Unknown};
const SectionTypeEnum ArtGen[] = {ST_Root, ST_Artist, ST_Genre, ST_Unknown};
const SectionTypeEnum ArtYea[] = {ST_Root, ST_Artist, ST_Year, ST_Unknown};

const SectionTypeEnum GenArtAlb[] = {ST_Root, ST_Genre, ST_Artist, ST_Album, ST_Unknown};
const SectionTypeEnum GenYea[] = {ST_Root, ST_Genre, ST_Year, ST_Unknown};

const SectionTypeEnum ColArtAlb[] = {ST_Root, ST_Collection, ST_Artist, ST_Album, ST_Unknown};
const SectionTypeEnum ColGenArtAlb[] = {ST_Root, ST_Collection, ST_Genre, ST_Artist, ST_Album, ST_Unknown};

const SectionTypeEnum YeaAlb[] = {ST_Root, ST_Year, ST_AlbumByName, ST_Unknown};
const SectionTypeEnum YeaArtAlb[] = {ST_Root, ST_Year, ST_Artist, ST_Album, ST_Unknown};

const SectionTypeEnum AlbBN[] = {ST_Root, ST_AlbumByName, ST_Unknown};

const SectionTypeEnum MonArtAlb[] = {ST_Root, ST_MonthAdded, ST_Artist, ST_Album, ST_Unknown};

const SectionTypeEnum ColAlb[] = {ST_Root, ST_Collection, ST_AlbumByName, ST_Unknown};


SectionTypeEnum CMainTreeCtrl::GetChildForSectionType(SectionModesEnum smode, SectionTypeEnum st)
{
	if ((INT) st < 0)
		return SectionTypeEnum(- INT(st));
	const SectionTypeEnum* pCurEnum = ArtAlb;
	switch (smode)
	{
	case SMODE_ArtistsAlbums:
		pCurEnum = ArtAlb;
		break;
	case SMODE_ArtistsGenres:
		pCurEnum = ArtGen;
		break;
	case SMODE_ArtistsYears:
		pCurEnum = ArtYea;
		break;
	case SMODE_GenreArtistsAlbums:
		pCurEnum = GenArtAlb;
		break;
	case SMODE_GenreYears:
		pCurEnum = GenYea;
		break;
	case SMODE_CollectionsArtistsAlbums:
		pCurEnum = ColArtAlb;
		break;
	case SMODE_CollectionsGenresArtistsAlbums:
		pCurEnum = ColGenArtAlb;
		break;
	case SMODE_YearAlbums:
		pCurEnum = YeaAlb;
		break;
	case SMODE_YearArtistsAlbums:
		pCurEnum = YeaArtAlb;
		break;
	case SMODE_AlbumsByName:
		pCurEnum = AlbBN;
		break;
	case SMODE_MonthAddedArtistAlbum:
		pCurEnum = MonArtAlb;
		break;
	case SMODE_CollectionsAlbums:
		pCurEnum = ColAlb;
		break;
	//case SMODE_Directories:
	//	return ST_Directory;
	//	break;
	default:
		ASSERT(0);
		break;
	}
	while (*pCurEnum != ST_Unknown)
	{
		if (*pCurEnum == st)
		{
			pCurEnum++;
			return *pCurEnum;
		}
		pCurEnum++;
	}
	ASSERT(0);
	return ST_Unknown;
}

void CMainTreeCtrl::SetSort(SectionTypeEnum type,SortOption so)
{
	ASSERT(type > ST_Unknown && type < ST_Last);
	if (type > ST_Unknown && type < ST_Last)
		m_sort[type] = so;
}

SortOption CMainTreeCtrl::GetSort(SectionTypeEnum type)
{
	ASSERT(type > ST_Unknown && type < ST_Last);
	if (type > ST_Unknown && type < ST_Last)
		return m_sort[type];
	return SortOption();
}



BOOL CMainTreeCtrl::GetSelectedFullArtistRecordSP(FullArtistRecordSP& rec)
{
	TreeNode* tn = GetNodeByIndex(GetNextSelectedItem());
	ASSERT(tn!=NULL);
	if (tn == NULL)	return FALSE;
	return GetFullArtistRecordSP(rec, *tn);
}
BOOL CMainTreeCtrl::GetSelectedFullAlbumRecordSP(FullAlbumRecordSP& rec )
{
	TreeNode* tn = GetNodeByIndex(GetNextSelectedItem());
	ASSERT(tn!=NULL);
	if (tn == NULL)	return FALSE;
	return GetFullAlbumRecordSP(rec, *tn);
}
BOOL CMainTreeCtrl::GetSelectedGenreRecordSP(GenreRecordSP& rec )
{
	TreeNode* tn = GetNodeByIndex(GetNextSelectedItem());
	ASSERT(tn!=NULL);
	if (tn == NULL)	return FALSE;
	return GetGenreRecordSP(rec, *tn);
}
BOOL CMainTreeCtrl::GetSelectedCollectionRecordSP(CollectionRecordSP& rec )
{
	TreeNode* tn = GetNodeByIndex(GetNextSelectedItem());
	ASSERT(tn!=NULL);
	if (tn == NULL)	return FALSE;
	return GetCollectionRecordSP(rec, *tn);
}
BOOL CMainTreeCtrl::GetSelectedYearRecordSP(YearRecordSP& rec )
{
	TreeNode* tn = GetNodeByIndex(GetNextSelectedItem());
	ASSERT(tn!=NULL);
	if (tn == NULL)	return FALSE;
	return GetYearRecordSP(rec, *tn);
}
BOOL CMainTreeCtrl::GetSelectedMonthAddedRecordSP(MonthAddedRecordSP& rec)
{
	TreeNode* tn = GetNodeByIndex(GetNextSelectedItem());
	ASSERT(tn!=NULL);
	if (tn == NULL)	return FALSE;
	return GetMonthAddedRecordSP(rec, *tn);
}

void CMainTreeCtrl::OnSelectionChanged()
{
	UINT curTick = GetTickCount();
	if (curTick - m_lastNotification > 1000)
	{
		ModifyState();
	}
	else
	{
		KillTimer(REFRESH_DELAY_TIMER_ID);
		SetTimer(REFRESH_DELAY_TIMER_ID, 
			m_notificationDelay / (curTick - m_lastNotification < m_notificationDelay ? 1 : 10), 
			NULL);
	}
	m_lastNotification = curTick;
}

void CMainTreeCtrl::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case REFRESH_DELAY_TIMER_ID:
		KillTimer(REFRESH_DELAY_TIMER_ID);
		ModifyState();
		return;
	case TIMER_AUTOUPDATE:
		{
			CollectionManager* pCM = PRGAPI()->GetCollectionManager();
			CollectionManager::State state = pCM->GetState();
			if (state == CollectionManager::S_Idle)
				KillTimer(TIMER_AUTOUPDATE);
			else
			{
				INT updates = pCM->GetProgress().updateActionsPerformed;
				if (updates != m_updatesPerformed)
					Update(TRUE);
			}
		}
		break;
	}
	__super::OnTimer(nIDEvent);
		
}

void CMainTreeCtrl::ModifyState()
{
	if (m_bModifyState == FALSE)
		return;
	TreeNode* pNode = GetNodeByIndex(GetNextSelectedItem());
	if (pNode != NULL)
	{
		StateManager* pSM = PRGAPI()->GetStateManager();
		TSState& state = pSM->GetState();
		GetTracksFilterForNode(*pNode, state.activeTracksFilter);
//		state.sectionSelectionStyle = TSState::SSS_Normal;
		switch (GetSelectedType())
		{
		case ST_Artist:
			state.activeItemType = IT_Artist;
			break;
		case ST_Album:
		case ST_AlbumByName:
			state.activeItemType = IT_Album;
			break;
		case ST_Year:
			state.activeItemType = IT_Year;
			break;
		case ST_Genre:
			state.activeItemType = IT_Genre;
			break;
		case ST_Collection:
			state.activeItemType = IT_Collection;
			break;
		default:
			state.activeItemType = IT_Various;
			break;
		}
		PRGAPI()->SendMessage(SM_CurrentSectionChanged);
	}
}

void CMainTreeCtrl::AppendInfoProviderMenuItem(BCMenu& mnu, const IInfoProvider& IP, InfoItemTypeEnum iit)
{
	PrgAPI* pAPI = PRGAPI();
	m_ProviderRequests.push_back(ProviderRequest(IP.GetModuleInfo(IInfoProvider::IPI_UniqueID), iit));
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("\"%s\" - %s"), IP.GetModuleInfo(IInfoProvider::IPI_Name), pAPI->GetStringForInfoItemType(iit));
	mnu.AppendMenu(MF_STRING, ID_PROVIDERS + m_ProviderRequests.size() - 1, bf, pAPI->GetIconForInfoItemType(iit));
}



BOOL CMainTreeCtrl::ExecuteNode(AppCommandsEnum appCmd, const TreeNode& node)
{
	TRACEST(_T("CMainTreeCtrl::ExecuteNode (p=appCmd)"), appCmd);
	PrgAPI* pAPI = PRGAPI();
	switch (appCmd)
	{
	case ACMD_DefaultPlayerAction:
	case ACMD_Play:
	case ACMD_Enqueue:
		{
			TracksFilter tf;
			GetTracksFilterForNode(node, tf);
			FullTrackRecordCollection tracks;
			if (pAPI->GetSQLManager()->GetFullTrackRecordCollectionByTracksFilter(tracks, tf))
			{
				ASSERT(tracks.size() > 0);
				MediaPlayer* pMP = pAPI->GetMediaPlayer();
				if (tracks.size() > 0)
				{
					SortOptionCollection soc;
					soc.ApplySortOption(SortOption(RSO_TrackNo, TRUE));
					soc.ApplySortOption(SortOption(RSO_Location, TRUE));
					RecordCollectionSorter::SortFullTrackRecordCollection(tracks, soc);
					if (appCmd == ACMD_Enqueue)
						pAPI->GetActionManager()->Enqueue(pMP, tracks);
					else if (appCmd == ACMD_Play)
						pAPI->GetActionManager()->Play(pMP, tracks);
					else
						pAPI->GetActionManager()->DefaultPlayerAction(pMP, tracks);
				}
			}
		}
		break;
	case ACMD_PurchaseInfo:
		{
			SectionTypeEnum st = ST_Unknown;
			st = (SectionTypeEnum) node.GetType();
			switch (st)
			{
			case ST_Artist:
				{
					FullArtistRecordSP rec;
					if (GetFullArtistRecordSP(rec, node))
						pAPI->GetActionManager()->GoToArtistBuyPage(rec->artist.name.c_str());
				}
				break;
			case ST_Album:
				{
					FullAlbumRecordSP rec;
					if (GetFullAlbumRecordSP(rec, node))
						pAPI->GetActionManager()->GoToAlbumBuyPage(rec->artist.name.c_str(), rec->album.name.c_str());
				}
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		break;
	case ACMD_Properties:
		{
			SectionTypeEnum st = (SectionTypeEnum) node.GetType();
			switch (st)
			{
			case ST_Artist:
				{
					FullArtistRecordSP rec;
					if (GetFullArtistRecordSP(rec, node))
					{
						//=== Detect Changes
						//UINT oldArtID = rec->artist.ID;
						//UINT oldGenID = rec->genre.ID;
						if (pAPI->GetActionManager()->ShowArtistPropertiesDlg(rec, this, TRUE))
						{
							//if (oldArtID != rec->artist.ID || oldGenID != rec->genre.ID)
							//	Update(TRUE);//=== Full Refresh
							//else
							//	Update(FALSE);
							Update(TRUE);//=== Full Refresh
						}
					}
				}
				break;
			case ST_Album:
				{
					FullAlbumRecordSP rec;
					if (GetFullAlbumRecordSP(rec, node))
					{
						//=== Detect Changes
						//UINT oldArtID = rec->artist.ID;
						//UINT oldAlbID = rec->album.ID;
						if (pAPI->GetActionManager()->ShowAlbumPropertiesDlg(rec, this, TRUE))
						{
							//if (oldArtID != rec->artist.ID || oldAlbID != rec->album.ID)
							//	Update(TRUE);//=== Full Refresh
							//else
							//	Update(FALSE);
							Update(TRUE);
						}
					}
				}
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		break;
	case ACMD_InsertRandomTracks:
		{
			ActionManager* pAM = pAPI->GetActionManager();
			TracksFilter tf;
			GetTracksFilterForNode(node, tf);
			FullTrackRecordCollection col;
			pAM->GetRandomTrackCollection(col, tf, 5);
			if (col.size() > 0)
				pAM->Enqueue(pAPI->GetMediaPlayer(), col);
		}
		break;
	default:
		{
			ServicesRegistrationSrv* srv = pAPI->GetServicesRegistrationSrv();
			if (appCmd >= MENU_WebServicesHolder && appCmd < MENU_WebServicesHolder + (INT)srv->GetServicesCount())
			{
				ServicesExecuteInfo sei;
				SectionTypeEnum st = (SectionTypeEnum) node.GetType();
				switch (st)
				{
				case ST_Artist:
					{
						FullArtistRecordSP rec;
						if (GetFullArtistRecordSP(rec, node))
						{
							sei.artist = rec->artist.name.c_str();
							ServiceInfo si;
							if (srv->GetServiceInfo(appCmd - MENU_WebServicesHolder, si))
								srv->Execute(si, sei, srv->GetWebViewer());
						}
					}
					break;
				case ST_Album:
					{
						FullAlbumRecordSP rec;
						if (GetFullAlbumRecordSP(rec, node))
						{
							sei.artist = rec->artist.name.c_str();
							sei.album = rec->album.name.c_str();
							ServiceInfo si;
							if (srv->GetServiceInfo(appCmd - MENU_WebServicesHolder, si))
								srv->Execute(si, sei, srv->GetWebViewer());
						}
					}
					break;
				default:
					ASSERT(0);
					break;
				}
			}
			else if (appCmd >= ID_PROVIDERS && appCmd < ID_PROVIDERS + (INT)m_ProviderRequests.size())
			{
				InfoDownloadManager* pIDM = pAPI->GetInfoDownloadManager();
				InfoItemTypeEnum iit = m_ProviderRequests[appCmd - ID_PROVIDERS].iit;
				SectionTypeEnum st = (SectionTypeEnum) node.GetType();
				switch (iit)
				{
				case IIT_ArtistBio:
				case IIT_ArtistPicture:
					{
						ASSERT(st == ST_Artist);
						FullArtistRecordSP rec;
						if (GetFullArtistRecordSP(rec, node))
						{
							if (iit == IIT_ArtistBio)
								pIDM->RequestArtistInfo(rec->artist, 
								m_ProviderRequests[appCmd - ID_PROVIDERS].provID.c_str(), TRUE);
							else
								pIDM->RequestArtistPic(rec->artist, 
								m_ProviderRequests[appCmd - ID_PROVIDERS].provID.c_str(), TRUE);
						}
					}
					break;
				case IIT_AlbumReview:
				case IIT_AlbumPicture:
					{
						ASSERT(st == ST_Album);
						FullAlbumRecordSP rec;
						if (GetFullAlbumRecordSP(rec, node))
						{
							if (iit == IIT_AlbumReview)
								pIDM->RequestAlbumInfo(rec->album, rec->artist.name.c_str(),
								m_ProviderRequests[appCmd - ID_PROVIDERS].provID.c_str(), 
								TRUE);
							else
								pIDM->RequestAlbumPic(rec->album, rec->artist.name.c_str(),
								m_ProviderRequests[appCmd - ID_PROVIDERS].provID.c_str(), 
								TRUE);
						}
					}
					break;
				default:
					ASSERT(0);
					break;
				}
			}
			else if (appCmd >= MENU_RatingHolder && appCmd <= MENU_RatingHolder+5)
			{
				SectionTypeEnum st = (SectionTypeEnum) node.GetType();
				INT Rating = Stars2Rating((FLOAT)(appCmd - MENU_RatingHolder));
				SQLManager* pSM = PRGAPI()->GetSQLManager();
				switch (st)
				{
				case ST_Artist:
					{
						FullArtistRecordSP rec;
						if (GetFullArtistRecordSP(rec, node))
						{
							if (rec->artist.rating != Rating)
							{
								rec->artist.rating = Rating;
								pSM->UpdateArtistRecord(rec->artist, FALSE);
								Update(FALSE);
							}
						}
					}
					break;
				case ST_Album:
					{
						FullAlbumRecordSP rec;
						if (GetFullAlbumRecordSP(rec, node))
						{
							if (rec->album.rating != Rating)
							{
								rec->album.rating = Rating;
								pSM->UpdateAlbumRecord(rec->album, FALSE);
								Update(FALSE);
							}
						}
					}
					break;
				default:
					ASSERT(FALSE);
				}
				Invalidate();
			}
			else
			{
				TRACE(_T("@1 CMainTreeCtrl::ExecuteNode. Unknown Command\n"));
				ASSERT(0);
			}
		}
	}
	return TRUE;
}



BOOL CMainTreeCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CMainTreeCtrl::OnCommand"), wParam);
	ASSERT(m_pMenuItitiator != NULL);
	if (m_pMenuItitiator != NULL)
		ExecuteNode((AppCommandsEnum) wParam, *m_pMenuItitiator);

	return __super::OnCommand(wParam, lParam);
}

TreeNode* CMainTreeCtrl::OnFindNode(LPCTSTR str)
{
	ASSERT(str != NULL);
	if (str == NULL || str[0] == 0)
		return NULL;
	TRACEST(_T("CMainTreeCtrl::OnFindNode"), str);
	INT strSize = _tcslen(str);
	INT itemCount = GetItemCount();
	INT startItem = GetNextSelectedItem();
	if (startItem == -1)
		return NULL;
	INT curItem = startItem;
	while (TRUE)
	{
		curItem++;
		if (curItem >= itemCount)
			curItem = 0;
		if (curItem == startItem)
			return NULL;//Not found
		TreeNode* curNode = GetNodeByIndex(curItem);
		ASSERT(curNode != NULL);
		if (curNode != NULL)
		{
			if (_tcsnicmp(str, curNode->GetString(), strSize) == 0)
				return curNode;
		}	
	}
	return NULL;
}

BOOL CMainTreeCtrl::ItemAcceptsFileDrop(CPoint& p, INT nItem)
{
	TreeNode* node = GetNodeByIndex(nItem);
	if (node != NULL)
	{
		SectionTypeEnum nt = (SectionTypeEnum) node->GetType();
		switch (nt)
		{
		case ST_Artist:
		case ST_Album:
		case ST_Genre:
		case ST_Year:
			return TRUE;
		}
	}
	return FALSE;

}
BOOL CMainTreeCtrl::OnFileDrop(const std::vector<std::tstring>& files, CPoint& p)
{
	TRACEST(_T("CMainTreeCtrl::OnFileDrop"));
	ASSERT(!files.empty());
	TreeNode* node = GetNodeByPoint(p.x, p.y);
	if (node != NULL)
	{
		PrgAPI* pAPI = PRGAPI();
		SQLManager* pSM = pAPI->GetSQLManager();
		ActionManager* pAM = pAPI->GetActionManager();


		//=== Get The required changes in a TagInfo structure
		TagInfo ti;
		SectionTypeEnum nt = (SectionTypeEnum) node->GetType();
		while (nt != ST_Root)
		{
			switch (nt)
			{
			case ST_Artist:
				{
					FullArtistRecordSP artRec;
					if (GetFullArtistRecordSP(artRec, *node))
					{
						ti.validFields |= TagInfo_Artist;
						ti.Artist = artRec->artist.name;
					}
				}
				break;
			case ST_Album:
				{
					FullAlbumRecordSP albRec;
					if (GetFullAlbumRecordSP(albRec, *node))
					{
						ti.validFields |= TagInfo_Album;
						ti.Album = albRec->album.name;
					}
				}
				break;
			case ST_Genre:
				{
					GenreRecordSP genRec;
					if (GetGenreRecordSP(genRec, *node))
					{
						ti.validFields |= TagInfo_Genre;
						ti.Genre = genRec->name;
					}
				}
				break;
			case ST_Year:
				{
					YearRecordSP yeaRec;
					if (GetYearRecordSP(yeaRec, *node))
					{
						ti.validFields |= TagInfo_Year;
						ti.Year = yeaRec->year;
					}
				}
				break;
			}
			node = node->pParent;
			nt = (SectionTypeEnum) node->GetType();
		}
		if (ti.validFields == TagInfo_None)
		{
			TRACE(_T("@1 CMainTreeCtrl::OnFileDrop Nothing to do\r\n"));
			return TRUE;//===Nothing to do
		}

		HWND parent = GetAncestor(GA_ROOT)->m_hWnd;
		UINT total = files.size();
		UINT pos = 0;
		FullTrackRecordCollection col;
		CUserProgressDlg dlg(CWnd::FromHandle(parent));
		dlg.SetTitle(pAPI->GetString(IDS_TAGGING));
		for (std::vector<std::tstring>::const_iterator itFiles = files.begin();itFiles != files.end(); itFiles++)
		{
			col.clear();
			LPCTSTR fileName = (*itFiles).c_str();
			dlg.SetMessage(fileName);
			dlg.SetProgress(pos++ / (FLOAT)total);
			dlg.Show();
			if (pSM->GetFullTrackRecordCollectionByLocation(col, fileName) && !col.empty())
			{
				FullTrackRecordCollection::iterator it = col.begin();
				for (; it!= col.end(); it++)
				{
					FullTrackRecordSP& rec = *it;
					pSM->UpdateTrackFromTagInfo(rec->track.ID, rec->track.colID, fileName, ti);
					pAM->TagTrack(fileName, ti);
				}

			}
			else
			{
				TRACE(_T("@1 CMainTreeCtrl::OnFileDrop File Not Found: %s\r\n"), fileName);
			}
		}
		pAPI->SendMessage(SM_DatabaseUpdated);
	}
	return TRUE;
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CMainTreeCtrl::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		switch (idx)
		{
		case COL_Text:
			m_gdiRes.pDrawer->expandableTextColor = value;
			break;
		case COL_TextBk1:
			DeleteObject(m_gdiRes.pDrawer->expandableBrush);
			m_gdiRes.pDrawer->expandableBrush = CreateSolidBrush(value);
			break;
		case COL_TextBk2://Unused
			break;
		case COL_TextSub:
			m_gdiRes.pDrawer->notExpandableTextColor = value;
			break;
		case COL_TextSubBk1:
			DeleteObject(m_gdiRes.pDrawer->notExpandableBrush);
			m_gdiRes.pDrawer->notExpandableBrush = CreateSolidBrush(value);
			break;
		case COL_TextSubBk2://Unused
			break;
		case COL_TextSel:
			m_gdiRes.pDrawer->selectedTextColor = value;

			break;
		case COL_TextSelBk1:
			DeleteObject(m_gdiRes.pDrawer->selectedBrush);
			m_gdiRes.pDrawer->selectedBrush = CreateSolidBrush(value);
			break;
		case COL_TextSelBk2://Unused
			break;
		}
		Refresh();
	}
}
COLORREF CMainTreeCtrl::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Text"),
//	_T("TextBk1"),
//	_T("TextBk2"),
//	_T("TextSub"),
//	_T("TextSubBk1"),
//	_T("TextSubBk2"),
//	_T("TextSel"),
//	_T("TextSelBk1"),
//	_T("TextSelBk2"),
//	NULL
//};
//
//
//LPCTSTR CMainTreeCtrl::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

void CMainTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	TRACEST(_T("CMainTreeCtrl::OnRButtonUp"));
	m_pMenuItitiator = GetNodeByPoint(point.x, point.y);
	if (m_pMenuItitiator == NULL)
	{
		TRACE(_T("@3 CMainTreeCtrl::OnRButtonUp. Empty Space\r\n"));
		return;
	}
	LPCTSTR ret = m_pMenuItitiator->GetString();
	if (ret == 0)
		return;
	SectionTypeEnum nt = ST_Unknown;
	PrgAPI* pAPI = PRGAPI();
	ITSMenu& mnuRatings = pAPI->GetMenuManager()->GetMenu(MenuManager::MNU_Rating);
	ClientToScreen(&point);
	if (ret[0] != '[')
		nt = (SectionTypeEnum) m_pMenuItitiator->GetType();
	switch (nt)
	{
	case ST_Artist:
		if (m_ArtistMenu.m_hMenu == NULL)
		{
			ITSMenu& mnuArtistServicesMenu = pAPI->GetMenuManager()->GetMenu(MenuManager::MNU_ArtistServices);
			m_ArtistMenu.CreatePopupMenu();
			m_ArtistMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
			m_ArtistMenu.SetIconSize(16,16);
			TCHAR bf[1000];
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PLAY), pAPI->GetString(IDS_KEYENTER));
			m_ArtistMenu.AppendMenu(MF_STRING, ACMD_Play, bf, pAPI->GetIcon(ICO_Play16));
			_sntprintf(bf, 1000, _T("%s\t%s + %s"), pAPI->GetString(IDS_ENQUEUE), pAPI->GetString(IDS_KEYSHIFT), pAPI->GetString(IDS_KEYENTER));
			m_ArtistMenu.AppendMenu(MF_STRING, ACMD_Enqueue, bf, pAPI->GetIcon(ICO_Add16));
			m_ArtistMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_INSERTRANDOMTRACKS), pAPI->GetString(IDS_KEYINSERT));
			m_ArtistMenu.AppendMenu(MF_POPUP, ACMD_InsertRandomTracks, bf);
			m_ArtistMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			m_ArtistMenu.AppendMenu(MF_POPUP, (UINT) mnuArtistServicesMenu.GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_SERVICES), pAPI->GetIcon(ICO_Providers16));
			m_ArtistMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			InfoProviderFactory* pIPF = pAPI->GetInfoProviderFactory();
			INT itCount = m_ProviderRequests.size();
			INT ipIdx = 0;
			while (pIPF->GetItemByIndex(ipIdx) != NULL)
			{
				if (pIPF->ForPublicUse(ipIdx))
				{
					const IInfoProvider* pIP = pIPF->GetItemByIndex(ipIdx);
					if (pIP->CanHandle(IInfoProvider::SRV_ArtistBio))
						AppendInfoProviderMenuItem(m_ArtistMenu, *pIP, IIT_ArtistBio);
					if (pIP->CanHandle(IInfoProvider::SRV_ArtistImage))
						AppendInfoProviderMenuItem(m_ArtistMenu, *pIP, IIT_ArtistPicture);
				}
				ipIdx++;
			}

			if (itCount != m_ProviderRequests.size())
			{
				m_ArtistMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			}
			m_ArtistMenu.AppendMenu(MF_POPUP, (UINT) mnuRatings.GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_RATING), pAPI->GetIcon(ICO_StarGold16));
			_sntprintf(bf, 1000, _T("%s - %s"), pAPI->GetString(IDS_MOREALBUMS), pAPI->GetString(IDS_PURCHASEINFO));
			m_ArtistMenu.AppendMenu(MF_POPUP, ACMD_PurchaseInfo, 
				bf, pAPI->GetIcon(ICO_Buy16));
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PROPERTIES), pAPI->GetString(IDS_KEYHOME));
			m_ArtistMenu.AppendMenu(MF_STRING, ACMD_Properties, bf, pAPI->GetIcon(ICO_Properties16));
		}
		pAPI->GetMenuManager()->PrepareMenu(MenuManager::MNU_ArtistServices);
		m_ArtistMenu.TrackPopupMenu(NULL, point.x, point.y, this);
		break;
	case ST_Album:
		if (m_AlbumMenu.m_hMenu == NULL)
		{
			TCHAR bf[1000];
			ITSMenu& mnuAlbumServicesMenu = pAPI->GetMenuManager()->GetMenu(MenuManager::MNU_AlbumServices);
			m_AlbumMenu.CreatePopupMenu();
			m_AlbumMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
			m_AlbumMenu.SetIconSize(16,16);
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PLAY), pAPI->GetString(IDS_KEYENTER));
			m_AlbumMenu.AppendMenu(MF_STRING, ACMD_Play, bf, pAPI->GetIcon(ICO_Play16));
			_sntprintf(bf, 1000, _T("%s\t%s + %s"), pAPI->GetString(IDS_ENQUEUE), pAPI->GetString(IDS_KEYSHIFT), pAPI->GetString(IDS_KEYENTER));
			m_AlbumMenu.AppendMenu(MF_STRING, ACMD_Enqueue, bf, pAPI->GetIcon(ICO_Add16));
			m_AlbumMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_INSERTRANDOMTRACKS), pAPI->GetString(IDS_KEYINSERT));
			m_AlbumMenu.AppendMenu(MF_POPUP, ACMD_InsertRandomTracks, bf);
			m_AlbumMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			m_AlbumMenu.AppendMenu(MF_POPUP, (UINT) mnuAlbumServicesMenu.GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_SERVICES), pAPI->GetIcon(ICO_Providers16));
			m_AlbumMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			InfoProviderFactory* pIPF = pAPI->GetInfoProviderFactory();
			INT itCount = m_ProviderRequests.size();
			INT ipIdx = 0;
			while (pIPF->GetItemByIndex(ipIdx) != NULL)
			{
				if (pIPF->ForPublicUse(ipIdx))
				{
					const IInfoProvider* pIP = pIPF->GetItemByIndex(ipIdx);
					if (pIP->CanHandle(IInfoProvider::SRV_AlbumReview))
						AppendInfoProviderMenuItem(m_AlbumMenu, *pIP, IIT_AlbumReview);
					if (pIP->CanHandle(IInfoProvider::SRV_AlbumImage))
						AppendInfoProviderMenuItem(m_AlbumMenu, *pIP, IIT_AlbumPicture);
				}
				ipIdx++;
			}
			if (itCount != m_ProviderRequests.size())
			{
				m_AlbumMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			}
			m_AlbumMenu.AppendMenu(MF_POPUP, (UINT) mnuRatings.GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_RATING), pAPI->GetIcon(ICO_StarGold16));
			_sntprintf(bf, 500, _T("%s - %s - %s"), 
				pAPI->GetString(IDS_PREVIEW), 
				pAPI->GetString(IDS_SIMILAR), 
				pAPI->GetString(IDS_PURCHASEINFO));
			m_AlbumMenu.AppendMenu(MF_POPUP, ACMD_PurchaseInfo, 
				bf, pAPI->GetIcon(ICO_Buy16));
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PROPERTIES), pAPI->GetString(IDS_KEYHOME));
			m_AlbumMenu.AppendMenu(MF_STRING, ACMD_Properties, bf, pAPI->GetIcon(ICO_Properties16));
			//m_AlbumMenu.LoadToolbar(IDR_MAINLISTNORMAL);
		}
		pAPI->GetMenuManager()->PrepareMenu(MenuManager::MNU_AlbumServices);
		m_AlbumMenu.TrackPopupMenu(NULL, point.x, point.y, this);
		break;
	default:
		if (m_GenericMenu.m_hMenu == NULL)
		{
			m_GenericMenu.CreatePopupMenu();
			m_GenericMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
			m_GenericMenu.SetIconSize(16,16);
			TCHAR bf[1000];
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PLAY), pAPI->GetString(IDS_KEYENTER));
			m_GenericMenu.AppendMenu(MF_STRING, ACMD_Play, bf, pAPI->GetIcon(ICO_Play16));
			_sntprintf(bf, 1000, _T("%s\t%s + %s"), pAPI->GetString(IDS_ENQUEUE), pAPI->GetString(IDS_KEYSHIFT), pAPI->GetString(IDS_KEYENTER));
			m_GenericMenu.AppendMenu(MF_STRING, ACMD_Enqueue, bf, pAPI->GetIcon(ICO_Add16));
			m_GenericMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
			_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_INSERTRANDOMTRACKS), pAPI->GetString(IDS_KEYINSERT));
			m_GenericMenu.AppendMenu(MF_POPUP, ACMD_InsertRandomTracks, bf);
		}
		m_GenericMenu.TrackPopupMenu(NULL, point.x, point.y, this);
	}


	__super::OnRButtonUp(nFlags, point);
}


BOOL CMainTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			//TRACE(_T("WM_KEYDOWN: %d - %d\r\n"), pMsg->wParam, pMsg->lParam);
			INT nItem = GetNextSelectedItem();
			if (nItem != -1)
			{
				TreeNode* pTN = GetNodeByIndex(nItem);
				if (pTN != NULL)
				{
					BOOL bHandled = TRUE;
					switch (pMsg->wParam)
					{
					case VK_RETURN:
						ExecuteNode(ACMD_Play, *pTN);
						break;
					case VK_BACK:
						ExecuteNode(ACMD_Enqueue, *pTN);
						break;
					case VK_INSERT:
						ExecuteNode(ACMD_InsertRandomTracks, *pTN);
						break;
					case VK_HOME:
						ExecuteNode(ACMD_Properties, *pTN);
						break;
					default:
						bHandled = FALSE;
					}
					if (bHandled)
						return TRUE;
				}
			}

		}
		break;
	case WM_CHAR:
		{
			BOOL bHandled = TRUE;
			switch (pMsg->wParam)
			{
			case VK_RETURN:
			case VK_BACK:
			case VK_INSERT:
			case VK_HOME:
			default:
				bHandled = FALSE;
			}
			if (bHandled)
				return TRUE;
		}
		break;
	}
	return CFlexTreeCtrl::PreTranslateMessage(pMsg);
}

void CMainTreeCtrl::PostCreate()
{
	PrgAPI* pAPI = PRGAPI();
	ApplyTranslation(*pAPI->GetTranslationManager());
	ApplySkin(pAPI->GetSkinManager()->GetSkin());
}
