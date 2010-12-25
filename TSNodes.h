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

#include "VectorTreeNodeCollection.h"
#include "DataRecords.h"
#include "TeenSpiritEnums.h"
#include <vector>

//========================
//===ArtistsNodeCollection
//========================
class ArtistsNodeCollection:public TreeNodeCollection
{
public:
	ArtistsNodeCollection(FullArtistRecordCollection* pCol, 
		TreeNode* pParent, BOOL bExpandable, BOOL bVariousEnabled);
	virtual ~ArtistsNodeCollection();

	virtual TreeNode* GetItem(INT idx);
	virtual UINT GetItemCount()				{return (UINT)m_collection.size();}
	virtual INT GetItemType(INT idx);
	virtual LPCTSTR GetItemString(INT idx);



	//Extra Interface
	FullArtistRecordSP GetFullArtistRecord(INT idx);
	FullArtistRecordCollection* GetFullArtistRecordCollection()	{return m_pCol;}
	void SetFullArtistRecordCollection(FullArtistRecordCollection* pCol);

private:
	FullArtistRecordCollection* m_pCol;
	std::vector<TreeNode> m_collection;
	TreeNode* m_pParent;
	BOOL m_bExpandable;
	BOOL m_bVariousEnabled;
};

//========================
//===AlbumsNodeCollection
//========================
class AlbumsNodeCollection:public VectorTreeNodeCollection
{
public:
	AlbumsNodeCollection(FullAlbumRecordCollection* pCol, TreeNode* pParent, BOOL bExpandable, BOOL bVariousEnabled);
	virtual ~AlbumsNodeCollection();
	virtual INT GetItemType(INT idx);
	virtual LPCTSTR GetItemString(INT idx);
	//Extra Interface
	FullAlbumRecordSP GetFullAlbumRecord(INT idx);
	FullAlbumRecordCollection* GetFullAlbumRecordCollection()			{return m_pCol;}
	void SetFullAlbumRecordCollection(FullAlbumRecordCollection* pCol)	{m_pCol = pCol;}
private:
	FullAlbumRecordCollection* m_pCol;
	BOOL m_bVariousEnabled;
};



//========================
//===GenresNodeCollection
//========================
class GenresNodeCollection:public VectorTreeNodeCollection
{
public:
	GenresNodeCollection(GenreRecordCollection* pCol, TreeNode* pParent, BOOL bExpandable, BOOL bVariousEnabled);
	virtual ~GenresNodeCollection();
	virtual INT GetItemType(INT idx);
	virtual LPCTSTR GetItemString(INT idx);
	//Extra Interface
	GenreRecordSP GetGenreRecord(INT idx);
	GenreRecordCollection* GetGenreRecordCollection()			{return m_pCol;}
	void SetGenreRecordCollection(GenreRecordCollection* pCol)	{m_pCol = pCol;}
private:
	GenreRecordCollection* m_pCol;
	BOOL m_bVariousEnabled;
};

//========================
//===CollectionNodeCollection
//========================
class CollectionNodeCollection:public VectorTreeNodeCollection
{
public:
	CollectionNodeCollection(CollectionRecordCollection* pCol, TreeNode* pParent, BOOL bExpandable);
	virtual ~CollectionNodeCollection();
	virtual INT GetItemType(INT idx)		{return ST_Collection;}
	virtual LPCTSTR GetItemString(INT idx);
	//Extra Interface
	CollectionRecordSP GetCollectionRecord(INT idx);
	CollectionRecordCollection* GetCollectionRecordCollection()	{return m_pCol;}
	void SetCollectionRecordCollection(CollectionRecordCollection* pCol)	{m_pCol = pCol;}
private:
	CollectionRecordCollection* m_pCol;
};


//========================
//===YearNodeCollection
//========================
class YearNodeCollection:public VectorTreeNodeCollection
{
public:
	YearNodeCollection(YearRecordCollection* pCol, TreeNode* pParent, BOOL bExpandable, BOOL bVariousEnabled);
	virtual ~YearNodeCollection();
	virtual INT GetItemType(INT idx);
	virtual LPCTSTR GetItemString(INT idx);
	//Extra Interface
	YearRecordSP GetYearRecord(INT idx);
	YearRecordCollection* GetYearRecordCollection()	{return m_pCol;}
	void SetYearRecordCollection(YearRecordCollection* pCol)	{m_pCol = pCol;}
private:
	YearRecordCollection* m_pCol;
	BOOL m_bVariousEnabled;
	TCHAR m_yearStr[20];
};








