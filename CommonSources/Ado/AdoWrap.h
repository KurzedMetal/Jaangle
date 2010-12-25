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
#ifndef _AdoWrap_h_ 
#define _AdoWrap_h_

#pragma once
//#pragma warning (disable: 4146)
#import "C:\program files\common files\system\ado\msadox.dll"
#import "C:\program files\common files\system\ado\msado15.dll" rename("EOF","adoEOF")
#import "C:\Program Files\Common Files\System\ado\msjro.dll"
//#pragma warning (default: 4146)
#include "icrsint.h"
using namespace ADODB;


class ADODatabase
{
public:
	ADODatabase();
	virtual ~ADODatabase();
	//Open: Returns TRUE on success
	BOOL Open(LPCTSTR lpstrConnection, 
		LPCTSTR lpstrUserID = NULL, LPCTSTR lpstrPassword = NULL, 
		UINT timeOut = 0, ConnectModeEnum mode = adModeUnknown);
	void Close();
	BOOL IsOpen();
	//Execute: Returns -1 on Error. Records Affected on success
	INT Execute(LPCTSTR lpstrExec);

	_ConnectionPtr GetActiveConnection() {return m_pConnection;};

	BOOL BeginTransaction();
	BOOL CommitTransaction();
	BOOL RollbackTransaction();

private:
	_ConnectionPtr m_pConnection;
};

struct ADOFieldInfo
{
	TCHAR m_strName[30]; 
	short m_nType;
	long m_lSize; 
	long m_lDefinedSize;
	long m_lAttributes;
	short m_nOrdinalPosition;
	BOOL m_bRequired;   
	BOOL m_bAllowZeroLength; 
	long m_lCollatingOrder;  
};

class ADORecordset
{
public:
	enum cadoEditEnum
	{
		dbNone = 0,
		dbAddNew = 1,
		dbEdit = 2
	};

	enum cadoPositionEnum
	{

		positionUnknown = -1,
		positionBOF = -2,
		positionEOF = -3
	};

	ADORecordset();
	ADORecordset(ADODatabase* pAdoDatabase);
	virtual ~ADORecordset();


	// =================================CursorTypeEnum===================================
	//adOpenDynamic	:: Uses a dynamic cursor. Additions, changes, and deletions by other users
	//				are visible, and all types of movement through the Recordset are allowed, 
	//				except for bookmarks, if the provider doesn't support them.
	//adOpenForwardOnly :: Default. Uses a forward-only cursor. Identical to a static cursor, 
	//				except that you can only scroll forward through records. 
	//				This improves performance when you need to make only one pass through 
	//				a Recordset.
	//adOpenKeyset :: Uses a keyset cursor. Like a dynamic cursor, except that you can't see 
	//				records that other users add, although records that other users 
	//				delete are inaccessible from your Recordset. 
	//				Data changes by other users are still visible.
	//adOpenStatic :: Uses a static cursor, which is a static copy of a set of records 
	//				that you can use to find data or generate reports. 
	//				Additions, changes, or deletions by other users are not visible.
	//adOpenUnspecified :: Does not specify the type of cursor.
	// =================================LockTypeEnum===================================
	//adLockBatchOptimistic :: Indicates optimistic batch updates. 
	//				Required for batch update mode.
	//adLockOptimistic :: Indicates optimistic locking, record by record. 
	//				The provider uses optimistic locking, locking records only when 
	//				you call the Update method.
	//adLockPessimistic :: Indicates pessimistic locking, record by record. 
	//				The provider does what is necessary to ensure successful editing 
	//				of the records, usually by locking records at the data source 
	//				immediately after editing.
	//adLockReadOnly ::	Indicates read-only records. You cannot alter the data.
	//adLockUnspecified :: Does not specify a type of lock. For clones, the clone 
	//				is created with the same lock type as the original.
	//=====================================Options=====================================
	//====================CommandTypeEnum
	//adCmdUnspecified :: Does not specify the command type argument.
	//adCmdText :: Evaluates CommandText as a textual definition of a command 
	//				or stored procedure call.
	//adCmdTable :: Evaluates CommandText as a table name whose columns are all returned 
	//				by an internally generated SQL query.
	//adCmdStoredProc :: Evaluates CommandText as a stored procedure name.
	//adCmdUnknown :: Default. Indicates that the type of command in the CommandText property is not known.
	//adCmdFile :: Evaluates CommandText as the file name of a persistently stored Recordset. 
	//				Used with Recordset.Open or Requery only.
	//adCmdTableDirect :: Evaluates CommandText as a table name whose columns are all returned. 
	//				Used with Recordset.Open or Requery only. To use the Seek method, 
	//				the Recordset must be opened with adCmdTableDirect.
	//				This value cannot be combined with the ExecuteOptionEnum value adAsyncExecute.
	//===================ExecuteOptionEnum
	//adAsyncExecute :: Indicates that the command should execute asynchronously. 
	//				This value cannot be combined with the CommandTypeEnum value adCmdTableDirect.
	//adAsyncFetch :: Indicates that the remaining rows after the initial quantity specified 
	//				in the CacheSize property should be retrieved asynchronously.
	//adAsyncFetchNonBlocking :: Indicates that the main thread never blocks while retrieving. 
	//				If the requested row has not been retrieved, 
	//				the current row automatically moves to the end of the file.
	//				If you open a Recordset from a Stream containing a persistently stored 
	//				Recordset, adAsyncFetchNonBlocking will not have an effect; 
	//				the operation will be synchronous and blocking.
	//				adAsynchFetchNonBlocking has no effect when the adCmdTableDirect option 
	//				is used to open the Recordset.
	//adExecuteNoRecords :: Indicates that the command text is a command or stored procedure 
	//				that does not return rows (for example, a command that only inserts data). 
	//				If any rows are retrieved, they are discarded and not returned. 
	//				adExecuteNoRecords can only be passed as an optional parameter 
	//				to the Command or Connection Execute method.
	//adExecuteStream :: Indicates that the results of a command execution should be returned 
	//				as a stream. adExecuteStream can only be passed as an optional parameter 
	//				to the Command Execute method.
	//adExecuteRecord :: Indicates that the CommandText is a command or stored procedure 
	//				that returns a single row which should be returned as a Record object.
	//adOptionUnspecified :: Indicates that the command is unspecified.
	BOOL Open(LPCTSTR lpstrExec, 
		CursorTypeEnum CursorType = adOpenForwardOnly, LockTypeEnum LockType = adLockReadOnly, INT nOption = adCmdText);
	BOOL Open(_ConnectionPtr mpdb, LPCTSTR lpstrExec, 
		CursorTypeEnum CursorType = adOpenForwardOnly, LockTypeEnum LockType = adLockReadOnly, INT nOption = adCmdText);

	BOOL Requery();
	BOOL Resync();


	BOOL AddNew();
	void Edit();
	void CancelUpdate();
	BOOL Update();
	BOOL UpdateBatch();

	//Automatically handles:
	//Index: LPCTSTR, short 
	//vtValue: LPCTSTR, short, int, unsigned long, double, bool, _variant_t, _variant_t()"null"
	BOOL SetVariantFieldValue(const _variant_t index, _variant_t vtValue);
	BOOL SetSystemTimeFieldValue(const _variant_t index, SYSTEMTIME& time);
	BOOL SetLPByteFieldValue(const _variant_t index, const BYTE* binData, ULONG nBytes);

	BOOL GetVariantFieldValue(const _variant_t index, _variant_t& vtValue);
	BOOL GetStringFieldValue(const _variant_t index, std::tstring& value);
	SHORT GetSHORTFieldValue(const _variant_t index);
	INT GetINTFieldValue(const _variant_t index);
	UINT GetUINTFieldValue(const _variant_t index);
	SYSTEMTIME GetSYSTEMTIMEFieldValue(const _variant_t index);
	DOUBLE GetDOUBLEFieldValue(const _variant_t index);
	BOOL GetBOOLFieldValue(const _variant_t index);


	BOOL IsFieldEmpty(const _variant_t index);




	BOOL Find(LPCTSTR lpFind, SearchDirectionEnum nSearchDirection = adSearchForward);
	BOOL FindNext();

	//Returns -1 if the RS does not support RecordCounting
	INT GetRecordCount();
	INT GetFieldCount();

	BOOL IsOpen();
	void Close();

	BOOL SetAbsolutePosition(int nPosition);
	long GetAbsolutePosition()
	{return m_pRecordset->GetAbsolutePosition();};

	BOOL IsEOF()
	{return m_pRecordset->adoEOF == VARIANT_TRUE;};
	BOOL IsBOF()
	{return m_pRecordset->BOF == VARIANT_TRUE;};
	void MoveFirst() 
	{m_pRecordset->MoveFirst();};
	void MoveNext() 
	{m_pRecordset->MoveNext();};
	void MovePrevious() 
	{m_pRecordset->MovePrevious();};
	void MoveLast() 
	{m_pRecordset->MoveLast();};

	BOOL GetFieldInfo(const _variant_t index, ADOFieldInfo* fldInfo);
	BOOL GetChunk(const _variant_t index, std::tstring& strValue);
	BOOL GetChunk(const _variant_t index, LPVOID pData);
	BOOL IsConnectionOpen()
	{return m_pConnection != NULL && m_pConnection->GetState() != adStateClosed;};
	BOOL SetFilter(LPCTSTR strFilter);
	BOOL SetSort(LPCTSTR lpstrCriteria);
	BOOL SaveAsXML(LPCTSTR lpstrXMLFile);
	BOOL OpenXML(LPCTSTR lpstrXMLFile);

	BOOL Delete();




private:
	BOOL ByteDataToVariant(const BYTE* binData, ULONG nBytes, _variant_t& vtFld);

private:
	_RecordsetPtr m_pRecordset;
	_ConnectionPtr m_pConnection;
	cadoEditEnum m_nEditStatus;
	CString m_strFind;
	_variant_t m_varBookFind;
	SearchDirectionEnum m_nSearchDirection;
	BOOL GetFieldInfo(FieldPtr pField, ADOFieldInfo* fldInfo);
	BOOL GetChunk(FieldPtr pField, CString& strValue);
	BOOL GetChunk(FieldPtr pField, LPVOID lpData);
};

class AdoWrap
{
public:
	//Utilities
	static BOOL CompactDatabase(LPCTSTR DBSource, LPCTSTR DbDestination, LPCTSTR password);
	static void HandleComError(_com_error &e);
};


//---------------------------------------------ADOX

using namespace ADOX;

class ADOXIndex;
class ADOXCatalog;

class ADOXUser
{
public:
	ADOXUser(ADOXCatalog* pCat);
	~ADOXUser();
	void GetName(CString& strName);
	bool Create(LPCTSTR lpstrUserName);
	bool Open(LPCTSTR lpstrUserName);
	bool ChangePassword(LPCTSTR lpstrOldPassword, LPCTSTR lpstrNewPassword);

public:
	_UserPtr m_pUser;

protected:
	_CatalogPtr m_pCatalog;
};

class ADOXView
{
public:
	ADOXView(ADOXCatalog* pCat);
	~ADOXView();
	void GetCommand(CString& strCommand);
	void GetName(CString& strName);
	bool Open(LPCTSTR lpstrViewName);
	bool Create(CString strName, CString strCommand);

public:
	ViewPtr m_pView;

protected:
	_CatalogPtr m_pCatalog;
};

class ADOXProcedure
{
public:
	ADOXProcedure(ADOXCatalog* pCat);
	~ADOXProcedure();
	void GetName(CString& strName);
	bool Open(LPCTSTR lpstrProcedureName);
	bool Create(CString strName, CString strCommand);
	void GetCommand(CString &strCommand);

public:
	ProcedurePtr m_pProcedure;

protected:
	_CatalogPtr m_pCatalog;
};

class ADOXTable
{

public:

	_TablePtr m_pTable;

	//CADOXTable(CADOXCatalog* pCat);
	ADOXTable(ADOXCatalog* pCat, LPCTSTR lpstrTableName = _T(""));
	ADOXTable(ADOXCatalog* pCat, int nTableIndex);
	~ADOXTable();

	bool Create(LPCTSTR lpstrTableName);
	bool Open(LPCTSTR lpstrTableName);
	bool Open(long nTableIndex);
	bool AddField(LPCTSTR lpstrFieldName, enum ADOX::DataTypeEnum Type, int nLength = 0);
	bool AddIndex(ADOXIndex pIndex);
	bool DeleteField(LPCTSTR lpstrFieldName);
	void GetName(CString& strTableName);

protected:
	_CatalogPtr m_pCatalog;
};


class ADOXIndex
{
public:
	enum DataType
	{
		typeSmallInt = ADOX::adSmallInt,
		typeInteger = ADOX::adInteger,
		typeUnsignedTinyInt = ADOX::adUnsignedTinyInt,
		typeUnsignedSmallInt = ADOX::adUnsignedSmallInt,
		typeUnsignedInt = ADOX::adUnsignedInt,
		typeUnsignedBigInt = ADOX::adUnsignedBigInt,
		typeSingle = ADOX::adSingle,
		typeDouble = ADOX::adDouble,
		typeCurrency = ADOX::adCurrency,
		typeDecimal = ADOX::adDecimal,
		typeNumeric = ADOX::adNumeric,
		typeBoolean = ADOX::adBoolean,
		typeDate = ADOX::adDate,
		typeDBDate = ADOX::adDBDate,
		typeDBTime = ADOX::adDBTime,
		typeDBTimeStamp = ADOX::adDBTimeStamp,
		typeBSTR = ADOX::adBSTR,
		typeVarChar = ADOX::adVarChar,
		typeLongVarChar = ADOX::adLongVarChar,
		typeWChar = ADOX::adWChar,
		typeVarWChar = ADOX::adVarWChar,
		typeLongVarWChar = ADOX::adLongVarWChar,
		typeBinary = ADOX::adBinary,
		typeVarBinary = ADOX::adVarBinary,
		typeLongVarBinary = ADOX::adLongVarBinary,
		typeChapter = ADOX::adChapter,
		typeFileTime = ADOX::adFileTime,
		typePropVariant = ADOX::adPropVariant,
		typeVarNumeric = ADOX::adVarNumeric
	};

	_IndexPtr m_pIndex;

	ADOXIndex()
	{
		//::CoInitialize(NULL);
		m_pIndex = NULL;
		m_pIndex.CreateInstance(__uuidof(Index));
	}

	~ADOXIndex()
	{
		m_pIndex.Release();
		m_pIndex = NULL;
		//::CoUninitialize();
	}

	bool Create(LPCTSTR lpstrIndexName);
	bool AddField(LPCTSTR lpstrIndexName, enum DataType Type, int nLength = 0);
	void SetPrimarKey(bool bPrimary = true);
};


class ADOXCatalog
{
public:
	//static bool CompactAndRepairDatabase(LPCTSTR lpstrConnection);
	bool Open(LPCTSTR lpstrConnection);
	bool CreateDatabase(LPCTSTR lpstrCreate);
	bool AddTable(ADOXTable pTable);
	bool AddUser(ADOXUser pUser, LPCTSTR lpstrPassword);

	long GetProcedureCount()
	{return m_pCatalog->Procedures->GetCount();};
	long GetTableCount()
	{return m_pCatalog->Tables->GetCount();};
	long GetViewCount();
	long GetUserCount()
	{return m_pCatalog->Users->GetCount();};
	long GetGroupCount()
	{return m_pCatalog->Groups->GetCount();};

	void GetTableName(long nTableIndex, CString &strTableName);
	void GetProcedureName(long nProcedureIndex, CString &strProcedureName);
	void GetViewName(long nViewIndex, CString &strViewName);
	void GetUserName(long nUserIndex, CString &strUserName);
	void GetGroupName(long nGroupIndex, CString &strGroupName);
	bool DeleteTable(LPCTSTR lpstrTableName);
	bool DeleteTable(long nTableIndex);
	bool DeleteProcedure(long nProcedureIndex);
	bool DeleteProcedure(LPCTSTR lpstrProcedureName);
	bool DeleteView(LPCTSTR lpstrViewName);
	bool DeleteView(long nViewIndex);
	bool DeleteGroup(LPCTSTR lpstrGroupName);
	bool DeleteGroup(long nGroupIndex);
	bool DeleteUser(LPCTSTR lpstrUserName);
	bool DeleteUser(long nViewIndex);

	ADOXCatalog():
		m_pCatalog(NULL),
		m_pConn(NULL)
	{
		//::CoInitialize(NULL);
	}

	~ADOXCatalog()
	{
		if (m_pCatalog != NULL)
		{
			m_pCatalog.Release();
			m_pCatalog = NULL;
		}
		//::CoUninitialize();
	}

	_CatalogPtr m_pCatalog;

	BOOL Init()
	{
		if (m_pCatalog.CreateInstance(__uuidof(Catalog)) != S_OK)
		{
			TRACE(_T("CADOXCatalog::Init() CreateInstance Failed"));
			return FALSE;
		}
		return TRUE;
	}
private:
	ADODB::_ConnectionPtr m_pConn;


};




#endif
