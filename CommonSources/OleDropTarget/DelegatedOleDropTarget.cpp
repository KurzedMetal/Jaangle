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
#include "DelegatedOleDropTarget.h"

//We Can t have Multiple Inheritance From CWnd & COleDropTarget as both 
//derives from CCmdTarget. This class delegates a CComOleTarget so it can be 
//used for multiple inheritance through a CWnd (CTreeCtrl - CListCtrl)

DelegatedOleDropTarget::DelegatedOleDropTarget():
m_pDropTarget(NULL)
{

}

DelegatedOleDropTarget::~DelegatedOleDropTarget()
{
	delete m_pDropTarget;
}

BOOL DelegatedOleDropTarget::RegisterAsDropTarget()
{
	if (m_pDropTarget == NULL)
		m_pDropTarget = new ForwardOleDropTarget(this);
	return m_pDropTarget->Register(dynamic_cast<CWnd*>(this));
}

void DelegatedOleDropTarget::Revoke()
{
	ASSERT(m_pDropTarget != NULL);//You shouldnt do this.. as you haven't register
	if (m_pDropTarget != NULL)
		m_pDropTarget->Revoke();
}


DROPEFFECT DelegatedOleDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
								DWORD dwKeyState, CPoint point )
{
	return DROPEFFECT_NONE;
}

DROPEFFECT DelegatedOleDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject,
							   DWORD dwKeyState, CPoint point )
{
	return DROPEFFECT_NONE;
}

BOOL DelegatedOleDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject,
					 DROPEFFECT dropEffect, CPoint point )
{
	return FALSE;
}

void DelegatedOleDropTarget::OnDragLeave ( CWnd* pWnd )
{

}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//------------------------------------  ForwardOleDropTarget  -------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------


ForwardOleDropTarget::ForwardOleDropTarget(DelegatedOleDropTarget* pMIOT):
m_pMIOT(pMIOT),
m_piDropHelper(NULL)
{
	ASSERT(m_pMIOT != NULL);
	//Needs 	OleInitialize(); !!!!! (OleUninitialize(); at end of CWinApp)
	HRESULT hr =CoCreateInstance ( CLSID_DragDropHelper, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IDropTargetHelper, 
		(void**) &m_piDropHelper );

	if (FAILED(hr))
		TRACE(_T("@1Special Effects For Drop Target Not available %d\n"), hr);

}

ForwardOleDropTarget::~ForwardOleDropTarget()
{
	if (m_piDropHelper != NULL)
		m_piDropHelper->Release();
}

DROPEFFECT ForwardOleDropTarget::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
						DWORD dwKeyState, CPoint point )
{
	//---------The following code should be in the client if he wants to accept files 
	//DROPEFFECT dwEffect = DROPEFFECT_NONE;
	//if ( NULL != pDataObject->GetGlobalData ( CF_HDROP ) )
	//	dwEffect = DROPEFFECT_COPY;
	//	return dwEffect;
	//---------END of Code
	DROPEFFECT dwEffect = m_pMIOT->OnDragEnter(pWnd, pDataObject, dwKeyState, point);

	// Call the DnD helper.
	// The DnD helper needs an IDataObject interface, so get one from
	// the COleDataObject.  Note that the FALSE param means that
	// GetIDataObject will not AddRef() the returned interface, so 
	// we do not Release() it.
	if (dwEffect != DROPEFFECT_NONE && m_piDropHelper != NULL)
	{
		IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
		m_piDropHelper->DragEnter(pWnd->GetSafeHwnd(), piDataObj, &point, dwEffect);
	}
	return dwEffect;

}

DROPEFFECT ForwardOleDropTarget::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject,
					   DWORD dwKeyState, CPoint point )
{
	//---------The following code should be in the client if he wants to accept files
	//DROPEFFECT dwEffect = DROPEFFECT_NONE;
	//if ( NULL != pDataObject->GetGlobalData ( CF_HDROP ) )
	//	dwEffect = DROPEFFECT_COPY;
	//	return dwEffect;
	//---------END of Code
	DROPEFFECT dwEffect = m_pMIOT->OnDragOver(pWnd, pDataObject, dwKeyState, point);
	if (dwEffect != DROPEFFECT_NONE && m_piDropHelper != NULL)
		m_piDropHelper->DragOver ( &point, dwEffect );
	return dwEffect;
}

BOOL ForwardOleDropTarget::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject,
			 DROPEFFECT dropEffect, CPoint point )
{
	//---------The following code should be in the client if he wants to accept files 
	//return ReadHdropData (pDataObject, &point);
	//---------END of Code
	BOOL bRet = m_pMIOT->OnDrop(pWnd, pDataObject, dropEffect, point);
	if (bRet && m_piDropHelper != NULL)
	{
		IDataObject* piDataObj = pDataObject->GetIDataObject ( FALSE ); 
		m_piDropHelper->Drop ( piDataObj, &point, dropEffect );
	}
	return bRet;

}

void ForwardOleDropTarget::OnDragLeave ( CWnd* pWnd )
{
	m_pMIOT->OnDragLeave(pWnd);
	if (m_piDropHelper != NULL)
		m_piDropHelper->DragLeave();
}
