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

#include "TreeNode.h"
#include "FlexListCtrl.h"

class CFlexTreeCtrl : public CFlexListCtrl
{
public:
	CFlexTreeCtrl();
	virtual ~CFlexTreeCtrl();

	void ClearNodes();

	TreeNode& GetRootNode()	{return m_root;}
	void Expand(TreeNode& node, BOOL bExpand);
	BOOL SelectNode(TreeNode* pTN);

	void SetExpandButton(HICON hExpandedIcon, HICON hCollapsedIcon, INT cx, INT cy);
	void SetIndent(INT indent)			{m_indent = indent;}
	INT GetIndent()						{return m_indent;}

	TreeNode* GetSelectedNode();

	Gdiplus::Rect GetNodeRect(TreeNode& node);
	TreeNode* GetNodeByPoint(INT x, INT y);
	TreeNode* GetNodeByIndex(INT idx);
	INT GetIndexByNode(TreeNode* pTN);


	//TreeNode* GetNextSelectedNode(TreeNode* pPrevNode = NULL);
	//void SetNodeSelection(TreeNode& prevNode, BOOL bSelected);

	//void CenterNode(TreeNode& node);
	//void EnsureVisibleNode(TreeNode& node, BOOL bFullVisibility);

	struct DrawNodeInfo
	{
		DrawNodeInfo():bSelected(FALSE),bContinueDrawing(TRUE)		{}
		Gdiplus::Rect fullRC;
		Gdiplus::Rect itemRC;
		BOOL bSelected;
		BOOL bContinueDrawing;
	};
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected://New From CFlexTreeCtrl

	virtual void DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC);
	virtual void DrawNode(Gdiplus::Graphics& g, TreeNode& node, DrawNodeInfo& nfo)	{ASSERT(0);}
	virtual BOOL OnFillItems(TreeNode& node)					{ASSERT(0); return FALSE;}
	virtual INT OnFindItem(LPCTSTR str);
	virtual TreeNode* OnFindNode(LPCTSTR str)					{return NULL;}

private:
	INT GetIndentForNode(TreeNode& node);

	TreeNode m_root;
	HICON m_hExpandedIcon;
	HICON m_hCollapsedIcon;
	INT m_iconCX;
	INT m_iconCY;
	INT m_indent;
	Gdiplus::Bitmap* m_pBmpExpandedIcon;
	Gdiplus::Bitmap* m_pBmpCollapsedIcon; 

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


