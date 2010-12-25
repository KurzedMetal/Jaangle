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
#ifndef _IInfoProvider_h_
#define _IInfoProvider_h_

#include "IConfigurable.h"

class IInfoProvider:public IConfigurable
{
public:
	enum ServiceEnum
	{
		SRV_First,
		SRV_ArtistBio,
		SRV_ArtistImage,		// *
		SRV_AlbumReview,
		SRV_AlbumImage,			// *
		SRV_TrackLyrics,
		SRV_TrackTablatures,
		SRV_ArtistSimilar,		// **
		SRV_AlbumSimilar,		// **
		SRV_TrackSimilar,		// **
		SRV_TrackSyncedLyrics,	//Track Synced Lyrics
		SRV_TrackReview,		//Track Story - Review
		SRV_TrackInfo,			//Track Length, Year, Price
		SRV_AlbumInfo,			//Track List, Year, Price etc
		SRV_Last
	};

	struct Request
	{
		Request():service(SRV_First),title(NULL),album(NULL),artist(NULL){}
		Request(ServiceEnum srv):service(srv),title(NULL),album(NULL),artist(NULL){}
		ServiceEnum service;
		LPCTSTR title;
		LPCTSTR album;
		LPCTSTR artist;
		BOOL IsValid() const 
		{
			switch (service)
			{
			case SRV_ArtistBio:
			case SRV_ArtistImage:
			case SRV_ArtistSimilar:
				return artist != NULL && artist[0] != NULL;
			case SRV_AlbumReview:
			case SRV_AlbumImage:
			case SRV_AlbumSimilar:
			case SRV_AlbumInfo:
				return artist != NULL && artist[0] != NULL && album != NULL && album[0] != NULL;
			case SRV_TrackLyrics:
			case SRV_TrackTablatures:
			case SRV_TrackSimilar:
			case SRV_TrackSyncedLyrics:
			case SRV_TrackReview:
			case SRV_TrackInfo:
				return artist != NULL && artist[0] != NULL && title != NULL && title[0] != NULL;
			}
			return FALSE;
		}
	};

	struct Result
	{
		Result():service(SRV_First),main(NULL),additionalInfo(NULL)		{}
		ServiceEnum service;
		LPCTSTR main;
		LPCTSTR additionalInfo;
		BOOL IsValid() const 
		{
			return service > SRV_First && service < SRV_Last && main != NULL; 
		}
	};

	//		 *: Result is a path to a temp file
	//		**: Result is a string '|' separated list
	//	All Artist have 1 param (Artist)
	//	All Album have 2 param (Artist, Album)
	//	All Track have 2 param (Artist, Title)
	//enum ResultTypeEnum
	//{
	//	//RES_Type,
	//	RES_Main,
	//	RES_AdditionalInfo,
	//	RES_Last
	//};
public:
	virtual ~IInfoProvider()	{}

	//--------------------------------------------------------
	//INFO:		Clones (Duplicate the current item)
	//COMMENTS:	Retains all the configuration;
	virtual IInfoProvider* Clone() const					= 0;
	//--------------------------------------------------------
	//INFO: Initializes a request
	//PARAMS:
	//	params: Fill the appropriate structure with the required
	//			info for the 
	//RETURNS:	TRUE if the request is valid
	//			FALSE if the request is not valid
	//COMMENTS:	This function must not take long. (eg. must not get data from internet)
	//			Call NextResult to get the first result
	//USAGE: Request the info needed. Build the params using the InfoProviderHelper class
	//virtual BOOL OpenRequest(LPCTSTR requestString)							= 0;
	virtual BOOL OpenRequest(const Request& request)		= 0;
	//--------------------------------------------------------
	//INFO: Retrieves next or the first result.
	//RETURNS:	TRUE if there a result has been retrieved successfully
	//			FALSE if there are no more results
	//USAGE:	if returns true the call GetResult to get the data.
	//			You may call NextResult again until you get false
	virtual BOOL GetNextResult(Result& result)				= 0;
	virtual const Request& GetRequest()						= 0;

	virtual BOOL AddResult(const Result& result)			= 0;
	virtual BOOL DeleteResult()								= 0;


	//--------------------------------------------------------
	//INFO: Capabilities Function
	//PARAMS:
	//	type: One of the types that is supported by the provider
	//RETURNS:	TRUE if you can request the specified type
	//			FALSE if this type is not supported for requests
	virtual BOOL CanHandle(ServiceEnum service)	const 		= 0;
	enum ModuleInfo
	{
		IPI_ModuleFileName,		//Self Calculated
		IPI_UniqueID,			//Unique for this provider. All capitals. Version Independent
		IPI_Name,
		IPI_Author,
		IPI_VersionStr,
		IPI_VersionNum,
		IPI_Description,
		IPI_HomePage,
		IPI_Email,
		IPI_UpdateURL,
		IPI_XInfo,
		IPI_Last
	};
	//--------------------------------------------------------
	//Returns Information about the provider
	virtual LPCTSTR GetModuleInfo(ModuleInfo mi) const 		= 0;
	//--------------------------------------------------------
	virtual void SetInternetHandle(LPVOID hInternet)		= 0;
	virtual LPVOID GetInternetHandle() const				= 0;

	//--------------------------------------------------------
	//=== IConfigurable
	virtual void UpdateConfiguration()						{}

};

#endif
