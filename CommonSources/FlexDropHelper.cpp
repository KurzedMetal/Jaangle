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
#include "FlexDropHelper.h"
#include "FlexListCtrl.h"


FlexDropHelper::FlexDropHelper(IFlexDropTarget* pTarget):
m_pTarget(pTarget),
m_piDropHelper(NULL)
{
	ASSERT(m_pTarget != NULL); 
}
FlexDropHelper::~FlexDropHelper()
{
	EnableDragDrop(FALSE);
	if (m_piDropHelper != NULL)
		m_piDropHelper->Release();
}


BOOL FlexDropHelper::EnableDragDrop(BOOL bEnable)
{
	if (m_bIsEnabled == bEnable)
		return TRUE;
	if (bEnable)
	{
		if (!Register(dynamic_cast<CWnd*>(m_pTarget)))
		{
			ASSERT(FALSE);
			return FALSE;
		}
		if (m_piDropHelper == NULL)
		{
			HRESULT hr =CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER,
				IID_IDropTargetHelper, (void**) &m_piDropHelper);
			ASSERT(hr == S_OK);
		}
		m_bIsEnabled = TRUE;

	}
	else
	{
		Revoke();
		m_bIsEnabled = FALSE;
	}
	return TRUE;
}



DROPEFFECT FlexDropHelper::OnDragEnter ( CWnd* pWnd, COleDataObject* pDataObject,
											  DWORD dwKeyState, CPoint point )
{


	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	if (pDataObject->GetGlobalData(CF_HDROP) != NULL)
	{
		if (m_pTarget->AcceptsFileDrop(point))
			dwEffect = DROPEFFECT_MOVE;
	}
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

DROPEFFECT FlexDropHelper::OnDragOver ( CWnd* pWnd, COleDataObject* pDataObject,
											 DWORD dwKeyState, CPoint point )
{
	DROPEFFECT dwEffect = DROPEFFECT_NONE;
	if (pDataObject->GetGlobalData(CF_HDROP) != NULL)
	{
		if (m_pTarget->AcceptsFileDrop(point))
			dwEffect = DROPEFFECT_MOVE;
	}
	if (dwEffect != DROPEFFECT_NONE && m_piDropHelper != NULL)
		m_piDropHelper->DragOver ( &point, dwEffect );
	return dwEffect;
}

BOOL FlexDropHelper::OnDrop ( CWnd* pWnd, COleDataObject* pDataObject,
								   DROPEFFECT dropEffect, CPoint point )
{
	HGLOBAL hGlobalData = pDataObject->GetGlobalData(CF_HDROP);
	if (hGlobalData != NULL && m_pTarget->AcceptsFileDrop(point))
	{
		HDROP hdrop = (HDROP) GlobalLock(hGlobalData);
		if (hdrop == NULL)
			return FALSE;
		UINT uNumFiles = DragQueryFile(hdrop, -1, NULL, 0);
		TCHAR szNextFile[MAX_PATH];
		std::vector<std::tstring> files;
		for (UINT uFile = 0; uFile < uNumFiles; uFile++ )
		{
			if ( DragQueryFile ( hdrop, uFile, szNextFile, MAX_PATH ) > 0 )
			{
				files.push_back(szNextFile);
			}
		}
		GlobalUnlock(hGlobalData);
		if (m_piDropHelper != NULL)
		{
			IDataObject* piDataObj = pDataObject->GetIDataObject(FALSE); 
			m_piDropHelper->Drop (piDataObj, &point, dropEffect);
		}
		m_pTarget->OnFileDrop(files, point);
		return TRUE;
	}
	return FALSE;
}

void FlexDropHelper::OnDragLeave ( CWnd* pWnd )
{
	if (m_piDropHelper != NULL)
		m_piDropHelper->DragLeave();
}
