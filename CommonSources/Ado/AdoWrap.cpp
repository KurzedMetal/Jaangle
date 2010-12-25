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
#include "AdoWrap.h"
#include <math.h>//pow

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define ChunkSize 100

//-----------------------------------------------------------------------
//----------------------------------ADODatabase-------------------------------------
//-----------------------------------------------------------------------
ADODatabase::ADODatabase():
m_pConnection(NULL)
{}

ADODatabase::~ADODatabase()
{
	if (m_pConnection != NULL)
	{
		Close();
		m_pConnection.Release();
		m_pConnection = NULL;
	}
}

BOOL ADODatabase::Open(LPCTSTR connection, LPCTSTR userID, LPCTSTR pass, UINT timeOut, ConnectModeEnum mode)
{
	if (m_pConnection == NULL)
	{
		if (m_pConnection.CreateInstance(__uuidof(Connection)) != S_OK)
		{
			TRACE(_T("@1ADODatabase::Open(). CreateInstance Failed."));
			return FALSE;
		}
	}
	else
		Close();
	HRESULT hr = S_FALSE;
	try
	{
		if(timeOut != 0)
			m_pConnection->PutConnectionTimeout(timeOut);
		m_pConnection->CursorLocation = adUseServer;
		m_pConnection->PutMode(mode);
		hr = m_pConnection->Open(_bstr_t(connection), _bstr_t(userID), _bstr_t(pass), NULL);
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
	return hr == S_OK;
}


INT ADODatabase::Execute(LPCTSTR sExec)
{
	ASSERT(IsOpen());
	ASSERT(sExec != 0);
	ASSERT(sExec[0] != 0);
	try
	{
		_variant_t vRecords;
		m_pConnection->Execute(_bstr_t(sExec), &vRecords, adExecuteNoRecords);
		return vRecords.iVal;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		TRACE(_T("@1 ADODatabase::Execute SQL:'%s'\r\n"), sExec);
	}
	return -1;	
}

BOOL ADODatabase::BeginTransaction() 
{
	ASSERT(IsOpen());
	return m_pConnection->BeginTrans() == S_OK;
}
BOOL ADODatabase::CommitTransaction() 
{
	ASSERT(IsOpen());
	return m_pConnection->CommitTrans() == S_OK;
}
BOOL ADODatabase::RollbackTransaction() 
{
	ASSERT(IsOpen());
	return m_pConnection->RollbackTrans() == S_OK;
}
BOOL ADODatabase::IsOpen()
{
	if (m_pConnection == NULL)
		return FALSE;
	return m_pConnection->GetState() != adStateClosed;
}

void ADODatabase::Close()
{
	if (IsOpen())
		m_pConnection->Close();
}
//-----------------------------------------------------------------------
//----------------------------------ADORecordset-------------------------------------
//-----------------------------------------------------------------------
ADORecordset::ADORecordset():
m_pRecordset(NULL),
m_pConnection(NULL),
m_nEditStatus(dbNone),
m_nSearchDirection(adSearchForward)
{
	m_pRecordset.CreateInstance(__uuidof(Recordset));
}
ADORecordset::ADORecordset(ADODatabase* pAdoDatabase):
m_pRecordset(NULL),
m_pConnection(pAdoDatabase->GetActiveConnection()),
m_nEditStatus(dbNone),
m_nSearchDirection(adSearchForward)
{
	ASSERT(pAdoDatabase != NULL);
	m_pRecordset.CreateInstance(__uuidof(Recordset));
}
ADORecordset::~ADORecordset()
{
	Close();
	if(m_pRecordset)
		m_pRecordset.Release();
}

BOOL ADORecordset::AddNew()
{
	ASSERT(m_nEditStatus == dbNone);
	if(m_pRecordset->AddNew() != S_OK)
		return FALSE;
	m_nEditStatus = dbAddNew;
	return TRUE;
}

void ADORecordset::Edit()
{
	ASSERT(m_nEditStatus == dbNone);
	m_nEditStatus = dbEdit;
}

BOOL ADORecordset::Update()
{
	ASSERT(m_nEditStatus != dbNone);
	BOOL bret = FALSE;
	if(m_nEditStatus != dbNone)
	{
		try
		{
			if(m_pRecordset->Update() == S_OK)
				bret = TRUE;
		}
		catch(_com_error &e)
		{
			AdoWrap::HandleComError(e);
		}

		if(!bret)
		{
			TRACE(_T("@0CADORecordset::Update. Failed\r\n"));
			m_pRecordset->CancelUpdate();
		}
		m_nEditStatus = dbNone;
	}
	return bret;
}
BOOL ADORecordset::UpdateBatch()
{
	BOOL bret = TRUE;
	if(m_nEditStatus != dbNone)
	{
		try
		{
			if(m_pRecordset->UpdateBatch(adAffectAll) != S_OK)
				bret = FALSE;
		}
		catch(_com_error &e)
		{
			AdoWrap::HandleComError(e);
			bret = FALSE;
		}
		if(!bret)
			m_pRecordset->CancelUpdate();
		m_nEditStatus = dbNone;
	}
	return bret;
}

void ADORecordset::CancelUpdate()
{
	m_pRecordset->CancelUpdate();
	m_nEditStatus = dbNone;
}

BOOL ADORecordset::SetVariantFieldValue(const _variant_t index, _variant_t vtValue)
{
	ASSERT(index.vt == VT_UI2 || index.vt == VT_BSTR);//Check if UI4 is Valid
	if(m_nEditStatus == dbNone)
	{
		TRACE(_T("@1CADORecordset::SetFieldValue. Field is not Editable!!! First Call ->Edit()"));
		ASSERT(0);
		return FALSE;
	}
	try
	{
		FieldPtr f = m_pRecordset->Fields->GetItem(index);
		if (vtValue.vt == (VT_ARRAY|VT_UI1))
		{
			//TEST IS NEEDED
			f->AppendChunk(vtValue);
			return TRUE;
		}
		else if (vtValue.vt == VT_BSTR)
		{
			//TEST IS NEEDED
			UINT defSize = f->GetDefinedSize();
#ifdef _DEBUG
			UINT realSize = SysStringLen(vtValue.bstrVal);
			if (realSize >= defSize)
				int a = 0;//just for the breakpoint
#endif
			if (SysStringLen(vtValue.bstrVal) > defSize)
			{
				//BSTR oldbstr = vtValue.bstrVal;
				::SysReAllocStringLen(&vtValue.bstrVal, vtValue.bstrVal, defSize);
				//vtValue.bstrVal = newbstr;
			}
		}
		f->Value = vtValue;
	}
	catch(_com_error &e)
	{
		TRACE(_T("@1CADORecordset::SetVariantFieldValue.\n"));
		AdoWrap::HandleComError(e);
		return FALSE;
	}
	return TRUE;
}

BOOL ADORecordset::SetSystemTimeFieldValue(const _variant_t index, SYSTEMTIME& time)
{
	_variant_t vtFld;
	vtFld.vt = VT_DATE;
	SystemTimeToVariantTime(&time, &vtFld.date);
	return SetVariantFieldValue(index, vtFld);
}

BOOL ADORecordset::SetLPByteFieldValue(const _variant_t index, const BYTE* binData, ULONG nBytes)
{
	_variant_t vtFld;
	ByteDataToVariant(binData, nBytes, vtFld);
	return SetVariantFieldValue(index, vtFld);
}


BOOL ADORecordset::ByteDataToVariant(const BYTE* binData, ULONG nBytes, _variant_t& vtFld)
{
	ASSERT(binData != NULL);
	ASSERT(nBytes > 0);
	HRESULT hr;
	long lngOffset = 0;
	BYTE chData;
	SAFEARRAY FAR *psa = NULL;
	SAFEARRAYBOUND rgsabound[1];

	BOOL ret = TRUE;
	try
	{
		//Create a safe array to store the array of BYTES 
		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = nBytes;
		psa = SafeArrayCreate(VT_UI1,1,rgsabound);
		while(lngOffset < (long)nBytes)
		{
			chData	= binData[lngOffset];
			hr = SafeArrayPutElement(psa, &lngOffset, &chData);
			if(FAILED(hr)) 
			{
				ret	= FALSE;
				break;
			}
			lngOffset++;
		}
		if (ret)
		{
			lngOffset = 0;
			vtFld.vt = VT_ARRAY|VT_UI1;
			vtFld.parray = psa;
		}
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		ret = FALSE;
	}
	return ret;
}

BOOL ADORecordset::Find(LPCTSTR lpFind, SearchDirectionEnum nSearchDirection/* = adSearchForward*/)
{
	ASSERT(lpFind != NULL);
	ASSERT(lpFind[0] != 0);
	if (nSearchDirection != adSearchBackward)
		nSearchDirection = adSearchForward;
	m_strFind = lpFind;
	m_nSearchDirection = nSearchDirection;
	try 
	{
		if(m_nSearchDirection == adSearchForward)
		{
			m_pRecordset->Find(_bstr_t(m_strFind), 0, m_nSearchDirection, "");
			if(!IsEOF())
			{
				m_varBookFind = m_pRecordset->Bookmark;
				return TRUE;
			}
		}
		else
		{
			m_pRecordset->Find(_bstr_t(m_strFind), 0, m_nSearchDirection, "");
			if(!IsBOF())
			{
				m_varBookFind = m_pRecordset->Bookmark;
				return TRUE;
			}
		}
	}
	catch (_com_error &e) 
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
	return FALSE;
}

BOOL ADORecordset::FindNext()
{
	if(m_nSearchDirection == adSearchForward)
	{
		m_pRecordset->Find(_bstr_t(m_strFind), 1, adSearchForward, m_varBookFind);
		if(!IsEOF())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return TRUE;
		}
	}
	else
	{
		m_pRecordset->Find(_bstr_t(m_strFind), 1, adSearchBackward, m_varBookFind);
		if(!IsBOF())
		{
			m_varBookFind = m_pRecordset->Bookmark;
			return TRUE;
		}
	}
	return FALSE;
}

INT ADORecordset::GetRecordCount()
{
	UINT nRows = 0;
	try
	{
		nRows = m_pRecordset->GetRecordCount();
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return 0;	
	}
	return nRows;
}
INT ADORecordset::GetFieldCount()
{
	UINT nFields = 0;
	try
	{
		nFields = m_pRecordset->Fields->GetCount();
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return 0;	
	}
	return nFields;
}

BOOL ADORecordset::IsOpen()
{
	if(m_pRecordset.GetInterfacePtr() != NULL && IsConnectionOpen())
		return m_pRecordset->GetState() == adStateOpen;
	return FALSE;
}

void ADORecordset::Close()
{
	if(IsOpen())
	{
		if (m_nEditStatus != dbNone)
			CancelUpdate();
		m_pRecordset->PutSort(_T(""));
		m_pRecordset->Close();	
	}
}



BOOL ADORecordset::Open(LPCTSTR lpstrExec, 
						CursorTypeEnum CursorType, LockTypeEnum LockType, INT nOption)
{
	ASSERT(m_pConnection != NULL);
	ASSERT(m_pConnection->GetState() != adStateClosed);
	return Open(m_pConnection, lpstrExec, CursorType, LockType, nOption);
}

BOOL ADORecordset::Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec, 
						CursorTypeEnum CursorType, LockTypeEnum LockType, INT nOption)
{	
	Close();
	ASSERT(lpstrExec!=NULL);
	ASSERT(lpstrExec[0] != 0);
	if(m_pConnection == NULL)
		m_pConnection = mpdb;
	m_pRecordset->CursorLocation = adUseServer;
	try
	{
#ifdef _DEBUG
		TRACE(_T("@4 EXECUTE SQL:'%s'\r\n"), lpstrExec);
#endif
		m_pRecordset->Open(lpstrExec, _variant_t((IDispatch*)mpdb, TRUE), CursorType, LockType, nOption);
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		TRACE(_T("@0FAILED SQL:'%s'\r\n"), lpstrExec);
		return FALSE;
	}
	return IsOpen();
}

BOOL ADORecordset::SetAbsolutePosition(int nPosition)
{
	ASSERT(IsOpen());
	try
	{
		m_pRecordset->PutAbsolutePosition((enum PositionEnum)nPosition);
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		ASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL ADORecordset::GetVariantFieldValue(_variant_t index, _variant_t& vtValue)
{
	if (index.vt == VT_INT || index.vt == VT_UINT)
		index.vt = VT_I2;
	ASSERT(index.vt == VT_I2 || index.vt == VT_BSTR);//Check if VT_I4 *int)is Valid
	try
	{
		vtValue = m_pRecordset->Fields->GetItem(index)->Value;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
	return TRUE;
}
BOOL ADORecordset::GetStringFieldValue(const _variant_t index, std::tstring& value)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return 0;
	switch (vtValue.vt)
	{
	case VT_BSTR:
		value = vtValue.bstrVal;
		break;
	case VT_NULL:
	case VT_EMPTY:
		value.clear();
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

SHORT ADORecordset::GetSHORTFieldValue(const _variant_t index)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return 0;
	switch (vtValue.vt)
	{
	case VT_I2:
		return vtValue.iVal;
	case VT_NULL:
	case VT_EMPTY:
		return 0;
	}
	return 0;
}
INT ADORecordset::GetINTFieldValue(const _variant_t index)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return 0;
	switch (vtValue.vt)
	{
	case VT_I4:
		return vtValue.intVal;
	case VT_I2:
		return vtValue.iVal;//It is short I2 and you asked for an int I4 
	case VT_NULL:
	case VT_EMPTY:
		return 0;
	}
	return 0;
}
UINT ADORecordset::GetUINTFieldValue(const _variant_t index)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return 0;
	switch (vtValue.vt)
	{
	case VT_I4:
	case VT_UI4:
		return vtValue.uintVal;
	case VT_I2:
	case VT_UI2:
		return vtValue.uiVal;
	case VT_NULL:
	case VT_EMPTY:
		return 0;
	}
	return 0;

}
SYSTEMTIME ADORecordset::GetSYSTEMTIMEFieldValue(const _variant_t index)
{
	SYSTEMTIME ret;
	memset(&ret, 0, sizeof(SYSTEMTIME));
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return ret;
	if (vtValue.vt == VT_DATE)
		VariantTimeToSystemTime(vtValue.date, &ret);
	else
		ASSERT(0);//???? It may be VT_NULL with no problem
	return ret;

}
DOUBLE ADORecordset::GetDOUBLEFieldValue(const _variant_t index)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return 0;
	if (vtValue.vt == VT_R8)
		return vtValue.dblVal;
	return 0;

}
BOOL ADORecordset::GetBOOLFieldValue(const _variant_t index)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return 0;
	if (vtValue.vt == VT_BOOL)
		return vtValue.intVal != 0;
	return FALSE;
}
BOOL ADORecordset::Requery()
{
	if(IsOpen())
	{
		try
		{
			m_pRecordset->Requery(adOptionUnspecified);//adExecuteRecord);
		}
		catch(_com_error &e)
		{
			AdoWrap::HandleComError(e);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL ADORecordset::Resync()
{
	if(IsOpen())
	{
		try
		{
			m_pRecordset->Resync(adAffectAll, adResyncAllValues);
		}
		catch(_com_error &e)
		{
			AdoWrap::HandleComError(e);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL ADORecordset::IsFieldEmpty(const _variant_t index)
{
	_variant_t vtValue;
	if (!GetVariantFieldValue(index, vtValue))
		return FALSE;
	return vtValue.vt == VT_NULL || vtValue.vt == VT_EMPTY;
}






































//-----------------------------------------------------------------------
//----------------------------------AdoWrap-------------------------------------
//-----------------------------------------------------------------------

void AdoWrap::HandleComError(_com_error &e)
{
	CString ErrorStr;
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	ErrorStr.Format( _T("@1 AdoWrap::HandleComError\r\n\tCode = %08lx\r\n\tCode meaning = %s\r\n\tSource = %s\r\n\tDescription = %s\r\n"),
		e.Error(), e.ErrorMessage(), (LPCTSTR)bstrSource, (LPCTSTR)bstrDescription);
	TRACE(ErrorStr);
}

BOOL AdoWrap::CompactDatabase(LPCTSTR DBSource, LPCTSTR DbDestination, LPCTSTR password)
{
	try
	{
		CString strDatabaseSource;
		strDatabaseSource.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s"), DBSource, password);
		CString strDatabaseDestination;
		strDatabaseDestination.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s"), DbDestination, password);


		//CString strDatabaseDestination(DbDestination);
		JRO::IJetEnginePtr jet(__uuidof(JRO::JetEngine));		
		HRESULT hr = jet->CompactDatabase(strDatabaseSource.AllocSysString(), strDatabaseDestination.AllocSysString());
		//jet->CompactDatabase( "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=d:\\nwind2.mdb;Jet OLEDB:Database Password=test", 
		//	"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=d:\\abbc.mdb;" \ 
		//	"Jet OLEDB:Engine Type=4;Jet OLEDB:Database Password=test");

		
		return hr == S_OK;
	}
	catch(_com_error &e) 
	{
		TRACE(_T("@1AdoWrap::CompactDatabase. Error: %s\n"), e.ErrorMessage());
		return FALSE;
	} 
}






























BOOL ADORecordset::GetFieldInfo(const _variant_t index, ADOFieldInfo* fldInfo)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(index);
	memset(fldInfo, 0, sizeof(ADOFieldInfo));
	wcscpy(fldInfo->m_strName, (LPCTSTR)pField->GetName());
	fldInfo->m_lDefinedSize = pField->GetDefinedSize();
	fldInfo->m_nType = pField->GetType();
	fldInfo->m_lAttributes = pField->GetAttributes();
	if(!IsEOF())
		fldInfo->m_lSize = pField->GetActualSize();
	return TRUE;
}



BOOL ADORecordset::GetChunk(const _variant_t index, std::tstring& strValue)
{
	strValue.clear();
	FieldPtr pField = m_pRecordset->Fields->GetItem(index);
	LONG lngSize = pField->ActualSize;
	_variant_t varChunk;
	LONG lngOffSet = 0;
	while(lngOffSet < lngSize)
	{ 
		try
		{
			varChunk = pField->GetChunk(ChunkSize);
			strValue += varChunk.bstrVal;
			lngOffSet += ChunkSize;
		}
		catch(_com_error &e)
		{
			AdoWrap::HandleComError(e);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL ADORecordset::GetChunk(const _variant_t index, LPVOID lpData)
{
	FieldPtr pField = m_pRecordset->Fields->GetItem(index);
	_variant_t varChunk;    
	UCHAR chData;
	HRESULT hr;
	long lBytesCopied = 0;

	LONG lngSize = pField->ActualSize;
	LONG lngOffSet = 0;
	while(lngOffSet < lngSize)
	{ 
		try
		{
			varChunk = pField->GetChunk(ChunkSize);

			//Copy the data only upto the Actual Size of Field.  
            for(long lIndex = 0; lIndex <= (ChunkSize - 1); lIndex++)
            {
                hr= SafeArrayGetElement(varChunk.parray, &lIndex, &chData);
                if(SUCCEEDED(hr))
                {
                    //Take BYTE by BYTE and advance Memory Location
                    //hr = SafeArrayPutElement((SAFEARRAY FAR*)lpData, &lBytesCopied ,&chData); 
					((UCHAR*)lpData)[lBytesCopied] = chData;
                    lBytesCopied++;
                }
                else
                    break;
            }
			lngOffSet += ChunkSize;
		}
		catch(_com_error &e)
		{
			AdoWrap::HandleComError(e);
			return FALSE;
		}
	}

	lngOffSet = 0;
	return TRUE;
}

BOOL ADORecordset::Delete()
{
	if(m_pRecordset->Delete(adAffectCurrent) != S_OK)
		return FALSE;

	if(m_pRecordset->Update() != S_OK)
		return FALSE;
	
	m_nEditStatus = dbNone;
	return TRUE;
}



BOOL ADORecordset::SetFilter(LPCTSTR strFilter)
{
	ASSERT(IsOpen());
	
	try
	{
		m_pRecordset->PutFilter(strFilter);
		return TRUE;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
}

BOOL ADORecordset::SetSort(LPCTSTR strCriteria)
{
	ASSERT(IsOpen());
	
	try
	{
		m_pRecordset->PutSort(strCriteria);
		return TRUE;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
}

BOOL ADORecordset::SaveAsXML(LPCTSTR lpstrXMLFile)
{
	HRESULT hr;

	ASSERT(IsOpen());
	
	try
	{
		hr = m_pRecordset->Save(lpstrXMLFile, adPersistXML);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
	return TRUE;
}

BOOL ADORecordset::OpenXML(LPCTSTR lpstrXMLFile)
{
	HRESULT hr = S_OK;

	if(IsOpen())
		Close();

	try
	{
		hr = m_pRecordset->Open(lpstrXMLFile, "Provider=MSPersist;", adOpenForwardOnly, adLockOptimistic, adCmdFile);
		return hr == S_OK;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return FALSE;
	}
}












//--------------------------------ADOX-----------------------------------
//--------------------------------ADOX-----------------------------------
//--------------------------------ADOX-----------------------------------
//--------------------------------ADOX-----------------------------------
//--------------------------------ADOX-----------------------------------
//--------------------------------ADOX-----------------------------------


























///////////////////////////////////////////////////////
//
//  ADOXProcedure Class
//

ADOXUser::ADOXUser(ADOXCatalog* pCat)
{
	m_pCatalog = pCat->m_pCatalog;
	m_pUser = NULL;
	m_pUser.CreateInstance(__uuidof(User));
}

ADOXUser::~ADOXUser()
{
	m_pUser.Release();
	m_pCatalog = NULL;
	m_pUser = NULL;
}

bool ADOXUser::Open(LPCTSTR lpstrUserName)
{
	ASSERT(m_pCatalog != NULL);

	m_pUser = m_pCatalog->Users->GetItem(lpstrUserName);
	return m_pUser != NULL;
}

void ADOXUser::GetName(CString& strName)
{
	_variant_t vName;

	ASSERT(m_pUser != NULL);

	vName  = m_pUser->GetName();
	strName = vName.bstrVal;
}

bool ADOXUser::ChangePassword(LPCTSTR lpstrOldPassword, LPCTSTR lpstrNewPassword)
{
	ASSERT(m_pUser != NULL);

	m_pUser->ChangePassword(_bstr_t(lpstrOldPassword), _bstr_t(lpstrNewPassword));
	return true;

}

bool ADOXUser::Create(LPCTSTR lpstrUserName)
{
	try
	{
		m_pUser->PutName(lpstrUserName);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

///////////////////////////////////////////////////////
//
//  ADOXView Class
//

ADOXView::ADOXView(ADOXCatalog* pCat)
{
	m_pCatalog = pCat->m_pCatalog;
	m_pView = NULL;
	m_pView.CreateInstance(__uuidof(View));
}

ADOXView::~ADOXView()
{
	m_pView.Release();
	m_pCatalog = NULL;
	m_pView = NULL;
}

bool ADOXView::Open(LPCTSTR lpstrViewName)
{
	ASSERT(m_pCatalog != NULL);

	m_pView = m_pCatalog->Views->GetItem(lpstrViewName);
	return m_pView != NULL;
}

void ADOXView::GetName(CString& strName)
{
	_variant_t vName;

	ASSERT(m_pView != NULL);

	vName  = m_pView->GetName();
	strName = vName.bstrVal;
}

bool ADOXView::Create(CString strName, CString strCommand)
{
	HRESULT hr;
	ADODB::_CommandPtr pCommand = NULL;
	pCommand.CreateInstance(__uuidof(ADODB::Command));

	try
	{
		pCommand->put_CommandText(strCommand.AllocSysString());
		hr = m_pCatalog->Views->Append(_bstr_t(strName.GetBuffer(0)), pCommand);
		if(SUCCEEDED(hr))
		{
			m_pCatalog->Views->Refresh();
			m_pView = m_pCatalog->Views->GetItem(strName.GetBuffer(0));
			pCommand.Release();
			return true;
		}
		else
		{
			return false;
		}
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXView::GetCommand(CString &strCommand)
{
	_variant_t vCommand;
	ADODB::_CommandPtr pCommand = NULL;

	ASSERT(m_pView != NULL);

	pCommand.CreateInstance(__uuidof(ADODB::Command));

	pCommand  = m_pView->GetCommand();
	vCommand = pCommand->GetCommandText();
	strCommand = vCommand.bstrVal;
}


///////////////////////////////////////////////////////
//
//  ADOXProcedure Class
//

ADOXProcedure::ADOXProcedure(ADOXCatalog* pCat)
{
	m_pCatalog = pCat->m_pCatalog;
	m_pProcedure = NULL;
	m_pProcedure.CreateInstance(__uuidof(Procedure));
}

ADOXProcedure::~ADOXProcedure()
{
	m_pProcedure.Release();
	m_pCatalog = NULL;
	m_pProcedure = NULL;
}

bool ADOXProcedure::Open(LPCTSTR lpstrProcedureName)
{
	ASSERT(m_pCatalog != NULL);

	m_pProcedure = m_pCatalog->Procedures->GetItem(lpstrProcedureName);
	return m_pProcedure != NULL;
}

void ADOXProcedure::GetName(CString& strName)
{
	_variant_t vName;

	ASSERT(m_pProcedure != NULL);

	vName  = m_pProcedure->GetName();
	strName = vName.bstrVal;
}

bool ADOXProcedure::Create(CString strName, CString strCommand)
{
	HRESULT hr;
	ADODB::_CommandPtr pCommand = NULL;
	pCommand.CreateInstance(__uuidof(ADODB::Command));

	try
	{
		pCommand->put_CommandText(strCommand.AllocSysString());
		hr = m_pCatalog->Procedures->Append(_bstr_t(strName.GetBuffer(0)), pCommand);
		if(SUCCEEDED(hr))
		{
			m_pCatalog->Procedures->Refresh();
			m_pProcedure = m_pCatalog->Procedures->GetItem(strName.GetBuffer(0));
			pCommand.Release();
			return true;
		}
		else
		{
			return false;
		}
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXProcedure::GetCommand(CString &strCommand)
{
	_variant_t vCommand;
	ADODB::_CommandPtr pCommand = NULL;

	ASSERT(m_pProcedure != NULL);

	pCommand.CreateInstance(__uuidof(ADODB::Command));

	pCommand  = m_pProcedure->GetCommand();
	vCommand = pCommand->GetCommandText();
	strCommand = vCommand.bstrVal;
}


///////////////////////////////////////////////////////
//
// ADOXIndex Class
//

bool ADOXIndex::Create(LPCTSTR lpstrIndexName)
{
	try
	{
		m_pIndex->PutName(lpstrIndexName);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXIndex::AddField(LPCTSTR lpstrIndexName, enum DataType Type, int nLength)
{
	try
	{
		m_pIndex->Columns->Append(lpstrIndexName, (enum ADOX::DataTypeEnum) Type, nLength);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXIndex::SetPrimarKey(bool bPrimary)
{
	m_pIndex->PutPrimaryKey(bPrimary?-1:0);
	m_pIndex->PutUnique(bPrimary?-1:0);
}


///////////////////////////////////////////////////////
//
// ADOXTAble Class
//

/*ADOXTable::ADOXTable(ADOXCatalog* pCat)
{
//	::CoInitialize(NULL);
m_pTable == NULL;
m_pTable.CreateInstance(__uuidof(Table));
m_pCatalog = NULL;
m_pCatalog = pCat->m_pCatalog;
}
*/
ADOXTable::ADOXTable(ADOXCatalog* pCat, LPCTSTR lpstrTableName)
{
	//::CoInitialize(NULL);
	m_pTable == NULL;
	m_pTable.CreateInstance(__uuidof(Table));
	m_pCatalog = NULL;
	m_pCatalog = pCat->m_pCatalog;
	if(_tcslen(lpstrTableName) > 0)
		Open(lpstrTableName);
}

ADOXTable::ADOXTable(ADOXCatalog* pCat, int nTableIndex)
{
	//::CoInitialize(NULL);
	m_pTable == NULL;
	m_pTable.CreateInstance(__uuidof(Table));
	m_pCatalog = NULL;
	m_pCatalog = pCat->m_pCatalog;
	if( nTableIndex > -1)
		Open(nTableIndex);
}

ADOXTable::~ADOXTable()
{
	m_pTable.Release();
	m_pTable = NULL;
	//::CoUninitialize();
}

bool ADOXTable::Open(LPCTSTR lpstrTableName)
{
	ASSERT(m_pCatalog != NULL);

	m_pTable = m_pCatalog->Tables->GetItem(lpstrTableName);
	return m_pTable != NULL;
}

bool ADOXTable::Open(long nTableIndex)
{
	ASSERT(m_pCatalog != NULL);

	m_pTable = m_pCatalog->Tables->GetItem(nTableIndex);
	return m_pTable != NULL;
}

bool ADOXTable::Create(LPCTSTR lpstrTableName)
{
	try
	{
		m_pTable->PutName(lpstrTableName);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXTable::AddField(LPCTSTR lpstrFieldName, ADOX::DataTypeEnum Type, int nLength)
{
	try
	{
		m_pTable->Columns->Append(lpstrFieldName, Type, nLength);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXTable::AddIndex(ADOXIndex pIndex)
{
	try
	{
		m_pTable->Indexes->Append(_variant_t((IDispatch *)pIndex.m_pIndex));
		m_pCatalog->Tables->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXTable::DeleteField(LPCTSTR lpstrFieldName)
{
	try
	{
		m_pTable->Columns->Delete(lpstrFieldName);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXTable::GetName(CString& strTableName)
{
	_variant_t vName;

	ASSERT(m_pTable != NULL);

	vName  = m_pTable->GetName();
	strTableName = vName.bstrVal;
}

////////////////////////////////////////////////////////
//
// ADOXCAtalog Class
//


bool ADOXCatalog::CreateDatabase(LPCTSTR lpstrCreate)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	try
	{
		TRACE(_T("@4ADOXCatalog::CreateDatabase. Entering Create.."));
		m_pCatalog->Create(_bstr_t(lpstrCreate));	
		TRACE(_T("@4ADOXCatalog::CreateDatabase. Success!"));
		return true;
	}
	catch(_com_error &e)
	{
		TRACE(_T("@0ADOXCatalog::CreateDatabase. Fail!"));
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::Open(LPCTSTR lpstrConnection)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	ASSERT(m_pConn == NULL);
	HRESULT hr = m_pConn.CreateInstance(__uuidof(ADODB::Connection));
	try
	{
		m_pConn->Open(lpstrConnection, "", "", NULL);

		m_pCatalog->PutActiveConnection(variant_t((IDispatch *)m_pConn));
		//m_pCatalog->PutActiveConnection(_bstr_t(lpstrConnection));
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}

}

bool ADOXCatalog::AddTable(ADOXTable pTable)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	try
	{
		m_pCatalog->Tables->Append( _variant_t((IDispatch *)pTable.m_pTable));
		m_pCatalog->Tables->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXCatalog::GetTableName(long nTableIndex, CString &strTableName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return;
	ASSERT(nTableIndex >= 0 && nTableIndex < m_pCatalog->Tables->GetCount());

	strTableName = (LPCTSTR)m_pCatalog->Tables->GetItem(nTableIndex)->GetName();
}

bool ADOXCatalog::DeleteTable(LPCTSTR lpstrTableName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;

	try
	{
		m_pCatalog->Tables->Delete(lpstrTableName);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::DeleteTable(long nTableIndex)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	ASSERT(nTableIndex >= 0 && nTableIndex < m_pCatalog->Tables->GetCount());

	try
	{
		m_pCatalog->Tables->Delete(nTableIndex);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXCatalog::GetProcedureName(long nProcedure, CString &strProcedureName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return;
	ASSERT(nProcedure >= 0 && nProcedure < m_pCatalog->Procedures->GetCount());

	strProcedureName = (LPCTSTR)m_pCatalog->Procedures->GetItem(nProcedure)->GetName();
}

bool ADOXCatalog::DeleteProcedure(long nProcedure)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	ASSERT(nProcedure >= 0 && nProcedure < m_pCatalog->Procedures->GetCount());

	try
	{
		m_pCatalog->Procedures->Delete(nProcedure);
		m_pCatalog->Procedures->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::DeleteProcedure(LPCTSTR lpstrProcedureName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;

	try
	{
		m_pCatalog->Procedures->Delete(lpstrProcedureName);
		m_pCatalog->Procedures->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

long ADOXCatalog::GetViewCount()
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return -1;
	try
	{
		return m_pCatalog->Views->GetCount();
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return -1;
	}
}


void ADOXCatalog::GetViewName(long nViewIndex, CString &strViewName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return;
	ASSERT(nViewIndex >= 0 && nViewIndex < m_pCatalog->Views->GetCount());

	strViewName = (LPCTSTR)m_pCatalog->Views->GetItem(nViewIndex)->GetName();
}

bool ADOXCatalog::DeleteView(LPCTSTR lpstrViewName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;

	try
	{
		m_pCatalog->Views->Delete(lpstrViewName);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::DeleteView(long nViewIndex)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	ASSERT(nViewIndex >= 0 && nViewIndex < m_pCatalog->Views->GetCount());

	try
	{
		m_pCatalog->Views->Delete(nViewIndex);
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXCatalog::GetGroupName(long nGroupIndex, CString &strGroupName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return;
	ASSERT(nGroupIndex >= 0 && nGroupIndex < m_pCatalog->Groups->GetCount());

	strGroupName = (LPCTSTR)m_pCatalog->Groups->GetItem(nGroupIndex)->GetName();
}

bool ADOXCatalog::DeleteGroup(LPCTSTR lpstrGroupName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;

	try
	{
		m_pCatalog->Groups->Delete(lpstrGroupName);
		m_pCatalog->Groups->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::DeleteGroup(long nGroupIndex)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	ASSERT(nGroupIndex >= 0 && nGroupIndex < m_pCatalog->Groups->GetCount());

	try
	{
		m_pCatalog->Groups->Delete(nGroupIndex);
		m_pCatalog->Groups->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

void ADOXCatalog::GetUserName(long nUserIndex, CString &strUserName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return;
	ASSERT(nUserIndex >= 0 && nUserIndex < m_pCatalog->Users->GetCount());

	strUserName = (LPCTSTR)m_pCatalog->Users->GetItem(nUserIndex)->GetName();
}

bool ADOXCatalog::DeleteUser(LPCTSTR lpstrUserName)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;

	try
	{
		m_pCatalog->Users->Delete(lpstrUserName);
		m_pCatalog->Users->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::DeleteUser(long nUserIndex)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	ASSERT(nUserIndex >= 0 && nUserIndex < m_pCatalog->Users->GetCount());

	try
	{
		m_pCatalog->Users->Delete(nUserIndex);
		m_pCatalog->Users->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}

bool ADOXCatalog::AddUser(ADOXUser pUser, LPCTSTR lpstrPassword)
{
	ASSERT(m_pCatalog != NULL);
	if (m_pCatalog == NULL)
		return FALSE;
	try
	{
		m_pCatalog->Users->Append( _variant_t((IDispatch *)pUser.m_pUser), _bstr_t(lpstrPassword));
		m_pCatalog->Users->Refresh();
		return true;
	}
	catch(_com_error &e)
	{
		AdoWrap::HandleComError(e);
		return false;
	}
}



