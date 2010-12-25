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

//class TreeNode;

//class TreeNodeCollection
//{
//public:
//	virtual ~TreeNodeCollection()				{}
//
//	virtual TreeNode* GetItem(INT idx)			= 0;
//	virtual UINT GetItemCount()					= 0;
//	virtual INT GetItemType(INT idx)			= 0;
//	virtual LPCTSTR GetItemString(INT idx)		= 0;
//};

#define TREE_NODE_ROOT_ID	0
#define INVALID_NODE_INDEX	0xffffffff

class TreeNode
{
public:
	TreeNode():bExpanded(FALSE),bExpandable(FALSE),pParent(FALSE)	{}
	virtual ~TreeNode()												{RemoveChildren();}

	//MainData
	BOOL bExpanded;
	BOOL bExpandable;
	TreeNode* pParent;

	//Virtual Data
	virtual INT GetType() const										{return TREE_NODE_ROOT_ID;}
	virtual LPCTSTR GetString() const								{ASSERT(0); return NULL;}

	//Calculated Data
	UINT GetDepth() const;
	INT GetLeafCount() const;
	TreeNode* GetTreeNodeByLeafIndex(INT index)  const;
	INT GetLeafIndexByTreeNode(TreeNode& item)  const;

	//===Child Actions
	void AppendChild(TreeNode* pNode);
	void InsertChild(UINT pos, TreeNode* pNode);
	void RemoveChild(UINT pos);
	void RemoveChildren();
	UINT GetChildCount() const;
	TreeNode* GetChild(UINT idx) const;
	UINT GetChildIndex(TreeNode* pNode);
private:
	std::vector<TreeNode*> m_pChildren;
};

