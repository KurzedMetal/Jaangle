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
#ifndef _DelegatedOleDropTarget_h_
#define _DelegatedOleDropTarget_h_

class ForwardOleDropTarget;

class DelegatedOleDropTarget
{
	friend ForwardOleDropTarget;
public:
	DelegatedOleDropTarget();
	virtual ~DelegatedOleDropTarget();

	BOOL RegisterAsDropTarget();
	void Revoke();

protected://Overridables
	virtual DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	virtual BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
	virtual void OnDragLeave ( CWnd* pWnd );

private:
	ForwardOleDropTarget* m_pDropTarget; 
};

class ForwardOleDropTarget: public COleDropTarget
{
public:
	ForwardOleDropTarget(DelegatedOleDropTarget* pMIOT);
	virtual ~ForwardOleDropTarget();

	DROPEFFECT OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	DROPEFFECT OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point );
	BOOL OnDrop ( CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point );
	void OnDragLeave ( CWnd* pWnd );

private:
	IDropTargetHelper* m_piDropHelper;
	DelegatedOleDropTarget* m_pMIOT;

};


#endif
