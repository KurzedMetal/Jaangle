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
#include "FlexTreeCtrl.h"
#include "GdiplusUtilities.h"
using namespace Gdiplus;

CFlexTreeCtrl::CFlexTreeCtrl():
m_hExpandedIcon(NULL),
m_hCollapsedIcon(NULL),
m_pBmpExpandedIcon(NULL),
m_pBmpCollapsedIcon(NULL),
m_iconCX(0),
m_iconCY(0),
m_indent(18)
{
	m_root.bExpandable = TRUE;
}
CFlexTreeCtrl::~CFlexTreeCtrl()
{
	delete m_pBmpExpandedIcon;
	delete m_pBmpCollapsedIcon;
}


void CFlexTreeCtrl::ClearNodes()
{
	m_root.RemoveChildren();
	m_root.bExpanded = FALSE;
	ClearItems();
}


void CFlexTreeCtrl::Expand(TreeNode& node, BOOL bExpand)
{
	if (!node.bExpandable)
		return;
	if (node.bExpanded != bExpand)
	{
		INT activeItem = m_root.GetLeafIndexByTreeNode(node);
		if (bExpand)
		{
			if (node.GetChildCount() == 0)
				OnFillItems(node);
			node.bExpanded = TRUE;
			INT leafCount = node.GetLeafCount();
			InsertItems(activeItem + 1, leafCount);
		}
		else
		{
			BOOL bSelection = FALSE;
			INT leafCount = node.GetLeafCount();
			for (size_t i = activeItem; i < (UINT)(activeItem + leafCount); i++)
			{
				if (IsItemSelected((INT)i))
				{
					bSelection = TRUE;
					break;
				}
			}
			DeleteItems(activeItem, leafCount);
			if (bSelection)
				SetItemSelection(activeItem, TRUE);
			node.bExpanded = FALSE;
		}
		Refresh();
	}
}

BOOL CFlexTreeCtrl::SelectNode(TreeNode* pTN)
{
	INT nItem = GetIndexByNode(pTN);
	if (nItem != -1)
	{
		SetItemSelection(nItem, TRUE);
		CenterItem(nItem);
		return TRUE;
	}
	return FALSE;
}


INT CFlexTreeCtrl::GetIndentForNode(TreeNode& node)
{
	return m_indent * node.GetDepth();
}

void CFlexTreeCtrl::DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC)
{
	TreeNode* pTN = m_root.GetTreeNodeByLeafIndex(nItem);
	if (pTN == NULL)
		return;//No Items there. Don't draw Anything (DrawBackground only)
	DrawNodeInfo nfo;
	nfo.fullRC = itemRC;
	nfo.itemRC = itemRC;
	INT indent = GetIndentForNode(*pTN);
	nfo.itemRC.X += indent;
	nfo.itemRC.Width -= indent;
	nfo.bSelected = IsItemSelected(nItem);
	DrawNode(g, *pTN, nfo);
	if (pTN->bExpandable && nfo.bContinueDrawing)
	{
		g.DrawImage(pTN->bExpanded ? m_pBmpExpandedIcon : m_pBmpCollapsedIcon, 
			nfo.itemRC.X - (m_indent + m_iconCX) / 2,
			nfo.fullRC.Y + (m_indent - m_iconCY) / 2, 
			0,
			0,
			m_iconCX - 1, 
			m_iconCY - 1, 
			Gdiplus::UnitPixel);
	}

}


void CFlexTreeCtrl::SetExpandButton(HICON hExpandedIcon, HICON hCollapsedIcon, INT cx, INT cy)
{
	m_hExpandedIcon = hExpandedIcon;
	m_hCollapsedIcon = hCollapsedIcon;
	m_iconCX = cx;
	m_iconCY = cy;
	delete m_pBmpExpandedIcon;	m_pBmpExpandedIcon = NULL;
	delete m_pBmpCollapsedIcon;	m_pBmpCollapsedIcon = NULL;
	if (m_hCollapsedIcon != NULL)
		m_pBmpCollapsedIcon = GdiplusUtilities::FromHICON32(m_hCollapsedIcon);
	if (m_hExpandedIcon != NULL)
		m_pBmpExpandedIcon = GdiplusUtilities::FromHICON32(m_hExpandedIcon);
}

TreeNode* CFlexTreeCtrl::GetSelectedNode()
{
	INT selItem = GetNextSelectedItem();
	if (selItem != -1)
		return GetNodeByIndex(selItem);
	return NULL;
}

TreeNode* CFlexTreeCtrl::GetNodeByIndex(INT nItem)
{
	ASSERT(nItem >=-1 && nItem < (INT)GetItemCount());
	if (nItem >=0 && nItem < (INT)GetItemCount())
		return m_root.GetTreeNodeByLeafIndex(nItem);
	return NULL;
}

INT CFlexTreeCtrl::GetIndexByNode(TreeNode* pTN)
{
	ASSERT(pTN != NULL);
	if (pTN != NULL)
		return m_root.GetLeafIndexByTreeNode(*pTN);
	return -1;
}
//
//TreeNode* CFlexTreeCtrl::GetNextSelectedNode(TreeNode* pPrevNode)
//{
//	INT lastItem = -1;
//	if (pPrevNode != NULL)
//		lastItem = GetIndexByNode(pPrevNode);
//	return GetNodeByIndex(GetNextSelectedItem(lastItem));
//}
//
//void CFlexTreeCtrl::SetNodeSelection(TreeNode& prevNode, BOOL bSelected)
//{
//	SetItemSelection(GetIndexByNode(&prevNode), bSelected);
//}
//
//void CFlexTreeCtrl::CenterNode(TreeNode& node)
//{
//	
//}
//void CFlexTreeCtrl::EnsureVisibleNode(TreeNode& node, BOOL bFullVisibility)
//{
//
//}



TreeNode* CFlexTreeCtrl::GetNodeByPoint(INT x, INT y)
{
	INT nItem = GetItemFromPoint(x, y);
	if (nItem != -1)
		return GetNodeByIndex(nItem);
	return NULL;
}

//TreeNode* CFlexTreeCtrl::GetSelectedNode()
//{
//	INT item = GetSelectedItem();
//	if (item != -1)
//		return GetNodeByIndex(item);
//	return NULL;
//}

//void CFlexTreeCtrl::SetSelectedNode(TreeNode* pTN)
//{
//	if (pTN == NULL)
//		SetSelectedItem(-1);
//	SetSelectedItem(m_root.GetLeafIndexByTreeNode(*pTN));
//}


BOOL CFlexTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			BOOL bHandled = TRUE;
			switch (pMsg->wParam)
			{
			case VK_LEFT:
			case VK_RIGHT:
				{
					INT curItem = GetNextSelectedItem();
					TreeNode* pTN = GetNodeByIndex(curItem);
					if (pTN != NULL)
					{
						if (pMsg->wParam == VK_RIGHT)
						{
							if (pTN->bExpandable)
							{
								if (!pTN->bExpanded)
									Expand(*pTN, TRUE);
								else
								{
									ClearSelection();
									SetItemSelection(curItem + 1, TRUE);
								}
							}
						}
						else//VK_LEFT
						{
							if (pTN->bExpandable && pTN->bExpanded)
								Expand(*pTN, FALSE);
							else
							{
								if (pTN->pParent != NULL && pTN->pParent != &m_root)
								{
									Expand(*pTN->pParent, FALSE);
									ClearSelection();
									SetItemSelection(GetIndexByNode(pTN->pParent), TRUE);
								}
							}
						}
					}
				}
				break;
			default:
				bHandled = FALSE;
			}
			if (bHandled)
				return TRUE;
		}
		break;
	}
	return CFlexListCtrl::PreTranslateMessage(pMsg);
}


INT CFlexTreeCtrl::OnFindItem(LPCTSTR str)
{
	ASSERT(str!=NULL);
	if (str[0] != 0)
	{
		TreeNode* pTN = OnFindNode(str);
		if (pTN != NULL)
			return m_root.GetLeafIndexByTreeNode(*pTN);
	}
	return -1;
}

Rect CFlexTreeCtrl::GetNodeRect(TreeNode& node)
{
	INT nItem = m_root.GetLeafIndexByTreeNode(node);
	if (nItem > -1)
		return GetItemRect(nItem);
	return Rect();
}

BEGIN_MESSAGE_MAP(CFlexTreeCtrl, CFlexListCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CFlexTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
	INT curItem = GetItemFromPoint(point.x, point.y);
	TreeNode* pTN = GetNodeByIndex(curItem);
	if (pTN != NULL && pTN->bExpandable)
	{
		Rect r = GetNodeRect(*pTN);
		INT ind = GetIndentForNode(*pTN);
		if (point.x < ind && point.y >= r.Y && point.y <= r.GetBottom())
		{
			Expand(*pTN, !pTN->bExpanded);
			return;
		}
	}

	CFlexListCtrl::OnLButtonDown(nFlags, point);
}
