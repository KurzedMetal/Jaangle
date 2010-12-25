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
#include "DataRecordsXmlSerializer.h"

//TODO: Using XMLLite + CSS. Meta bgale apo to MainListControl to DispInfo
//Check=== http://www.w3schools.com/xml/xml_display.asp

//TCHAR bf[MAX_PATH];
//_sntprintf(bf, MAX_PATH, _T("%s.html"), title);
//CFileDialog fd(FALSE, _T(".html"), bf, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
//			   _T("HTML Files (*.html)|*.html|All Files (*.*)|*.*||"), NULL);
//if (fd.DoModal() != IDOK)
//return FALSE;

#include <xmllite.h>
#pragma comment( lib, "xmllite.lib" )
#include "XmlLiteUtilities.h"

LPCTSTR DataRecordsXmlSerializer::SystemTime2String(LPTSTR buffer, SYSTEMTIME& st)
{
	_sntprintf(buffer, 100, _T("%d-%d-%d-%d-%d-%d"),
		st.wYear, st.wMonth, st.wDay, 
		st.wHour, st.wMinute, st.wSecond);
	return buffer;
}

void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, TrackRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("trackname"), NULL, rec.name.c_str());
	TCHAR bf[100];
	xmlWriter.WriteElementString(NULL, _T("trackdateadded"), NULL, SystemTime2String(bf, rec.dateAdded));
	xmlWriter.WriteElementString(NULL, _T("tracklocation"), NULL, rec.location.c_str());
	_itot(rec.bitrate, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("trackbitrate"), NULL, bf);
	_itot(rec.duration, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("trackduration"), NULL, bf);
	_itot(rec.size, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("tracksize"), NULL, bf);
	_itot(rec.trackNo, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("tracktrackNo"), NULL, bf);
	_itot(rec.year, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("trackyear"), NULL, bf);
	_itot(rec.rating, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("trackrating"), NULL, bf);
}
void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, AlbumRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("albumname"), NULL, rec.name.c_str());
	TCHAR bf[100];
	_itot(rec.rating, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("albumrating"), NULL, bf);
	_itot(rec.year, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("albumyear"), NULL, bf);
	if (rec.trackCount > 0)
	{
		_itot(rec.trackCount, bf, 10);
		xmlWriter.WriteElementString(NULL, _T("albumtrackcount"), NULL, bf);
	}
}
void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, ArtistRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("artistname"), NULL, rec.name.c_str());
	TCHAR bf[100];
	_itot(rec.rating, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("artistrating"), NULL, bf);
	if (rec.trackCount > 0)
	{
		_itot(rec.trackCount, bf, 10);
		xmlWriter.WriteElementString(NULL, _T("artisttrackcount"), NULL, bf);
	}
}
void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, GenreRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("genrename"), NULL, rec.name.c_str());
	if (rec.trackCount > 0)
	{
		TCHAR bf[100];
		_itot(rec.trackCount, bf, 10);
		xmlWriter.WriteElementString(NULL, _T("genretrackcount"), NULL, bf);
	}
}
void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, CollectionRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("collectionname"), NULL, rec.name.c_str());
	xmlWriter.WriteElementString(NULL, _T("collectionlocation"), NULL, rec.location.c_str());
	TCHAR bf[100];
	xmlWriter.WriteElementString(NULL, _T("collectiondateadded"), NULL, SystemTime2String(bf, rec.dateAdded));
	xmlWriter.WriteElementString(NULL, _T("collectiondateupdated"), NULL, SystemTime2String(bf, rec.dateUpdated));

	_itot(rec.serial, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("collectionserial"), NULL, bf);
	_itot(rec.type, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("collectiontype"), NULL, bf);
	if (rec.trackCount > 0)
	{
		_itot(rec.trackCount, bf, 10);
		xmlWriter.WriteElementString(NULL, _T("collectiontrackcount"), NULL, bf);
		_itot(rec.sumSize, bf, 10);
		xmlWriter.WriteElementString(NULL, _T("collectionsumsize"), NULL, bf);
		_itot(rec.sumDuration, bf, 10);
		xmlWriter.WriteElementString(NULL, _T("collectionsumduration"), NULL, bf);
	}
}

void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, HistLogRecord& rec)
{
	TCHAR bf[100];
	xmlWriter.WriteElementString(NULL, _T("dateadded"), NULL, SystemTime2String(bf, rec.dateAdded));
	_itot(rec.actID, bf, 10);
	xmlWriter.WriteElementString(NULL, _T("actiontype"), NULL, bf);
}
void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, HistArtistRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("artist"), NULL, rec.name.c_str());
}
void DataRecordsXmlSerializer::AppendXMLElement(IXmlWriter& xmlWriter, HistTrackRecord& rec)
{
	xmlWriter.WriteElementString(NULL, _T("title"), NULL, rec.name.c_str());
}

BOOL DataRecordsXmlSerializer::Export(LPCTSTR fileName, FullTrackRecordCollection& col)
{
	HRESULT hr;
	CComPtr<IStream> pOutFileStream;
	CComPtr<IXmlWriter> pWriter;

	//Open writeable output stream
	if (FAILED(hr = FileStream::OpenFile(fileName, &pOutFileStream, TRUE)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating file writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter),(void**) &pWriter, NULL)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating xml writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = pWriter->SetOutput(pOutFileStream)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error setting output for writer, error is %08.8lx"), hr);
		return FALSE;
	}
	pWriter->WriteStartElement(NULL, _T("fulltrackcollection"), NULL);
	FullTrackRecordCollection::iterator it = col.begin();
	while (it != col.end())
	{
		pWriter->WriteStartElement(NULL, _T("fulltrackrecord"), NULL);
		AppendXMLElement(*pWriter, (*it)->track);
		AppendXMLElement(*pWriter, (*it)->album);
		AppendXMLElement(*pWriter, (*it)->artist);
		AppendXMLElement(*pWriter, (*it)->genre);
		AppendXMLElement(*pWriter, (*it)->collection);
		pWriter->WriteEndElement();
		it++;
	}
	pWriter->WriteEndElement();
	pWriter.Release();
	pOutFileStream.Release();
	return TRUE;
}

BOOL DataRecordsXmlSerializer::Export(LPCTSTR fileName, FullHistLogRecordCollection& col)
{
	HRESULT hr;
	CComPtr<IStream> pOutFileStream;
	CComPtr<IXmlWriter> pWriter;

	//Open writeable output stream
	if (FAILED(hr = FileStream::OpenFile(fileName, &pOutFileStream, TRUE)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating file writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter),(void**) &pWriter, NULL)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating xml writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = pWriter->SetOutput(pOutFileStream)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error setting output for writer, error is %08.8lx"), hr);
		return FALSE;
	}
	pWriter->WriteStartElement(NULL, _T("fullhistlogrecordcollection"), NULL);
	FullHistLogRecordCollection::iterator it = col.begin();
	while (it != col.end())
	{
		pWriter->WriteStartElement(NULL, _T("fullhistlogrecord"), NULL);
		AppendXMLElement(*pWriter, (*it)->log);
		AppendXMLElement(*pWriter, (*it)->track);
		AppendXMLElement(*pWriter, (*it)->artist);
		pWriter->WriteEndElement();
		it++;
	}
	pWriter->WriteEndElement();
	pWriter.Release();
	pOutFileStream.Release();
	return TRUE;
}
BOOL DataRecordsXmlSerializer::Export(LPCTSTR fileName, FullHistTrackRecordCollection& col)
{
	HRESULT hr;
	CComPtr<IStream> pOutFileStream;
	CComPtr<IXmlWriter> pWriter;

	//Open writeable output stream
	if (FAILED(hr = FileStream::OpenFile(fileName, &pOutFileStream, TRUE)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating file writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter),(void**) &pWriter, NULL)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating xml writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = pWriter->SetOutput(pOutFileStream)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error setting output for writer, error is %08.8lx"), hr);
		return FALSE;
	}
	pWriter->WriteStartElement(NULL, _T("fullhisttrackrecordcollection"), NULL);
	FullHistTrackRecordCollection::iterator it = col.begin();
	while (it != col.end())
	{
		pWriter->WriteStartElement(NULL, _T("fullhisttrackrecord"), NULL);
		AppendXMLElement(*pWriter, (*it)->track);
		AppendXMLElement(*pWriter, (*it)->artist);
		if ((*it)->hits > 0)
		{
			TCHAR bf[100];
			_itot((*it)->hits, bf, 10);
			pWriter->WriteElementString(NULL, _T("hits"), NULL, bf);
			pWriter->WriteElementString(NULL, _T("firsttime"), NULL, 
				SystemTime2String(bf, (*it)->firstTime));
			pWriter->WriteElementString(NULL, _T("lasttime"), NULL, 
				SystemTime2String(bf, (*it)->lastTime));
		}
		pWriter->WriteEndElement();
		it++;
	}
	pWriter->WriteEndElement();
	pWriter.Release();
	pOutFileStream.Release();
	return TRUE;
}
BOOL DataRecordsXmlSerializer::Export(LPCTSTR fileName, FullHistArtistRecordCollection& col)
{
	HRESULT hr;
	CComPtr<IStream> pOutFileStream;
	CComPtr<IXmlWriter> pWriter;

	//Open writeable output stream
	if (FAILED(hr = FileStream::OpenFile(fileName, &pOutFileStream, TRUE)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating file writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter),(void**) &pWriter, NULL)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error creating xml writer, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = pWriter->SetOutput(pOutFileStream)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Export. Error setting output for writer, error is %08.8lx"), hr);
		return FALSE;
	}
	pWriter->WriteStartElement(NULL, _T("fullhistartistrecordcollection"), NULL);
	FullHistArtistRecordCollection::iterator it = col.begin();
	while (it != col.end())
	{
		pWriter->WriteStartElement(NULL, _T("fullhistartistrecord"), NULL);
		AppendXMLElement(*pWriter, (*it)->artist);
		if ((*it)->hits > 0)
		{
			TCHAR bf[100];
			_itot((*it)->hits, bf, 10);
			pWriter->WriteElementString(NULL, _T("hits"), NULL, bf);
			pWriter->WriteElementString(NULL, _T("firsttime"), NULL, 
				SystemTime2String(bf, (*it)->firstTime));
			pWriter->WriteElementString(NULL, _T("lasttime"), NULL, 
				SystemTime2String(bf, (*it)->lastTime));
		}
		pWriter->WriteEndElement();
		it++;
	}
	pWriter->WriteEndElement();
	pWriter.Release();
	pOutFileStream.Release();
	return TRUE;
}

BOOL DataRecordsXmlSerializer::Import(LPCTSTR fileName, FullTrackRecordCollection& col)
{
	HRESULT hr;
	CComPtr<IStream> pInFileStream;
	CComPtr<IXmlReader> pReader;

	//Open writeable output stream
	if (FAILED(hr = FileStream::OpenFile(fileName, &pInFileStream, FALSE)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Import. Error creating file reader, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader),(void**) &pReader, NULL)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Import. Error creating IXmlReader, error is %08.8lx"), hr);
		return FALSE;
	}

	if (FAILED(hr = pReader->SetInput(pInFileStream)))
	{
		TRACE(_T("@1 DataRecordsXmlSerializer::Import. Error setting Input for reader, error is %08.8lx"), hr);
		return FALSE;
	}
	XmlLiteReaderHelper hlp(*pReader);
	if (hlp.FindElement(_T("fulltrackcollection"), 0))
	{
		while (hlp.FindElement(_T("fulltrackrecord"), 1))
		{
			FullTrackRecord* pRec = new FullTrackRecord;
			XmlNodeType nodeType;
			const WCHAR* pwszLocalName = NULL;
			while (pReader->Read(&nodeType) == S_OK)
			{
				if (nodeType == XmlNodeType_Element)
				{
					if (pReader->GetLocalName(&pwszLocalName, NULL) == S_OK)
					{
						if (_tcsicmp(pwszLocalName, _T("trackname")) == 0)
							pRec->track.name = hlp.GetElementText();
						//else if (_tcsicmp(pwszLocalName, _T("trackdateadded")) == 0)
						//	pRec->track.dateAdded = hlp.GetElementText();
						else if (_tcsicmp(pwszLocalName, _T("tracklocation")) == 0)
							pRec->track.location = hlp.GetElementText();
						else if (_tcsicmp(pwszLocalName, _T("trackbitrate")) == 0)
							pRec->track.bitrate = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("trackduration")) == 0)
							pRec->track.duration = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("tracksize")) == 0)
							pRec->track.size = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("tracktrackNo")) == 0)
							pRec->track.trackNo = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("trackyear")) == 0)
							pRec->track.year = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("trackrating")) == 0)
							pRec->track.rating = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("albumname")) == 0)
							pRec->album.name = hlp.GetElementText();
						else if (_tcsicmp(pwszLocalName, _T("albumrating")) == 0)
							pRec->album.rating = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("albumyear")) == 0)
							pRec->album.year = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("artistname")) == 0)
							pRec->artist.name = hlp.GetElementText();
						else if (_tcsicmp(pwszLocalName, _T("artistrating")) == 0)
							pRec->artist.rating = _ttoi(hlp.GetElementText());
						else if (_tcsicmp(pwszLocalName, _T("genrename")) == 0)
							pRec->genre.name = hlp.GetElementText();

					}
				}
				else if (nodeType == XmlNodeType_EndElement)
				{
					UINT depth = 0;
					pReader->GetDepth(&depth);
					if (depth < 3)
						break;
				}
			}
			col.push_back(FullTrackRecordSP(pRec));
		}
	}
	pReader.Release();
	pInFileStream.Release();
	return TRUE;
}



