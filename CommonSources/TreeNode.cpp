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
#include "TreeNode.h"

void TreeNode::AppendChild(TreeNode* pNode)
{
	ASSERT(pNode != NULL);
	m_pChildren.push_back(pNode);	
}
void TreeNode::InsertChild(UINT pos, TreeNode* pNode)
{
	ASSERT(pNode != NULL);
	ASSERT(pos <= GetChildCount());
	m_pChildren.insert(m_pChildren.begin() + pos, pNode);	
}
void TreeNode::RemoveChild(UINT pos)
{
	ASSERT(pos < GetChildCount());
	if (pos >= 0 && pos < GetChildCount())
	{
		delete GetChild(pos);
		m_pChildren.erase(m_pChildren.begin() + pos);
	}
}
void TreeNode::RemoveChildren()
{
	for (size_t i = 0; i < m_pChildren.size(); i++)
		delete m_pChildren[i];
	m_pChildren.clear();
	bExpanded = FALSE;
}
UINT TreeNode::GetChildCount() const
{
	return m_pChildren.size();
}
TreeNode* TreeNode::GetChild(UINT idx) const
{
	if (idx < GetChildCount())
	{
		return m_pChildren[idx];
	}
	return NULL;
}

UINT TreeNode::GetChildIndex(TreeNode* pNode)
{
	for (UINT i = 0; i < m_pChildren.size(); i++)
	{
		if (pNode == m_pChildren[i])
			return i;
	}
	return INVALID_NODE_INDEX;
}


UINT TreeNode::GetDepth() const
{
	UINT depth = 0;
	TreeNode* par = pParent;
	while (par)
	{
		depth++;
		par = par->pParent;
	}
	return depth;
}


INT TreeNode::GetLeafCount() const
{
	INT items = 0;
	if (bExpanded)
	{
		ASSERT(bExpandable);
		UINT childs = GetChildCount();
		items += childs;
		for (UINT i = 0; i < childs; i++)
			items += GetChild(i)->GetLeafCount();
	}
	return items;
}

TreeNode* TreeNode::GetTreeNodeByLeafIndex(INT idx) const 
{
	ASSERT(idx >= 0);
	if (bExpanded)
	{
		ASSERT(bExpandable);
		UINT childs = GetChildCount();
		UINT nLeafIndex = 0;
		for (UINT i = 0; i < childs; i++)
		{
			TreeNode* pNode = GetChild(i);
			if (nLeafIndex == idx)
				return pNode;
			TreeNode* ret =	pNode->GetTreeNodeByLeafIndex(idx - nLeafIndex - 1);
			if (ret != NULL)
				return ret;
			nLeafIndex += pNode->GetLeafCount() + 1;
		}
	}
	return NULL;
}

INT TreeNode::GetLeafIndexByTreeNode(TreeNode& item) const
{
	if (bExpanded)
	{
		ASSERT(bExpandable);
		UINT childs = GetChildCount();
		UINT nLeafIndex = pParent == NULL ? 0 : 1;
		for (UINT i = 0; i < childs; i++)
		{
			TreeNode* pNode = GetChild(i);
			if (pNode == &item)
				return nLeafIndex;
			INT ret = pNode->GetLeafIndexByTreeNode(item);
			if (ret != -1)
				return nLeafIndex + ret;
			nLeafIndex += pNode->GetLeafCount() + 1;
		}
	}
	return - 1;
}
