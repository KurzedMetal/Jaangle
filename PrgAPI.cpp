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
#include "PrgAPI.h"

#include "TeenSpirit.h"
#include "ObjFactory.h"
#include "AppSettings/AppSettings.h"
#include "LocalPictureManager.h"
#include "InfoDownloadManager.h"
#include "ActionManager.h"
#include "ServicesRegistrationSrv.h"
#include "TeenSpiritPlayer.h"
#include "FileTagger.h"
#include "ResourceUtils.h"
#include "SkinManager.h"
#include "MenuManager.h"
#include "SQLManager.h"
#include "CollectionManager.h"
#include "UpdateChecker.h"
#include "FlexButton.h"
#include "InformationProviders/InfoProviderFactory.h"
#include "MessageDlg.h"
#include "InfoCtrl.h"
#include "HotKeyManager.h"
#include "ShortcutManager.h"
#include "ConfigurationManager.h"
#include "LastFMServices.h"
#include "Trackers/MultiTracker.h"
#include "UserProgressDlg.h"
#include "TSOnlineServices.h"
#include "VersionNo.h"
#include "MediaPlayerUtilities.h"
#include "ApplicationMonitor.h"
#include "AppMonitor.h"
#include "Language/LanguageManager.h"
#include "ExceptionHandler.h"
#include "BassInstance.h"//Is needed for initial bass configuration (Init)
#include "TranslationManager.h"
#include "StateManager.h"
#include "MainTreeCtrl.h"
#include "TSMenu.h"
#include "TrayToolTipDlg.h"
#include "MiniPlayerDlg.h"
#include "QuickSearchDlg.h"
#include "SearchDlg.h"


//=== For 0.99 Version
//#define USE_YOUTUBEENGINE


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//	DatabasePath=L:\TS-storage\db\
//	StoragePath=L:\TS-storage\pix\


UINT cDefaultBaseFontSize = 14;
LPCTSTR cDefaultBaseFontName = _T("Tahoma");

LPCTSTR const cGeneral =			_T("General");
LPCTSTR const cPlayerSettings =		_T("Player");
LPCTSTR const cInfoMgrSettings =	_T("InfoManager");
LPCTSTR const cInfoControlSettings =_T("InfoControl");
LPCTSTR const cPaneSettings	=		_T("Panes");
LPCTSTR const cTagManager =			_T("TagManager");
LPCTSTR const cViewerPane =			_T("ViewerPane");
LPCTSTR const cMiniPlayer =			_T("MiniPlayer");
LPCTSTR const cTree	=				_T("Tree");
LPCTSTR const cTrackList =			_T("TrackList");
LPCTSTR const cPlayList =			_T("PlayList");


LPCTSTR const cLastFM =				_T("LastFM");

//LPCTSTR const SettingsCategory = _T("Main");
LPCTSTR const cOptionLanguage =		_T("Language");
LPCTSTR const cOptionSkin =			_T("Skin");

LPCTSTR const cOptionUser =			_T("User");
LPCTSTR const cOptionPass =			_T("Pass");

LPCTSTR const cErrStr	=			_T("ErrorString");

struct OptInfo
{
	AppOptionsEnum optID;
	LPCTSTR valueName;
	LPCTSTR category;
	INT defVal;
};
struct StringOptInfo
{
	INT resID;
	LPCTSTR valueName;
	LPCTSTR category;
	LPCTSTR defVal;
};


const OptInfo Option2Info[] =
{

	{OPT_GEN_Debugging, _T("Debugging"), cGeneral, 0},					//OPT_GEN_Debugging
	{OPT_GEN_NotifyMSN, _T("NotifyMSN"), cGeneral, 1},	//OPT_GEN_NotifyMSN
	{OPT_GEN_LastFMScrobbler, _T("LastFMScrobbler"), cGeneral, 0},		//OPT_GEN_LastFMScrobbler,
	{OPT_GEN_NotifyHistory, _T("NotifyHistory"), cGeneral, 1},			//OPT_GEN_NotifyHistory,

	{OPT_GEN_CheckForUpdates, _T("CheckForUpdates"), cGeneral, 1},				//OPT_GEN_CheckForUpdates
	{OPT_GEN_DaysBetweenCheck, _T("DaysBetweenCheck"), cGeneral, 5},	//OPT_GEN_DaysBetweenCheck
	{OPT_GEN_CheckLastCheck, _T("CheckLastCheck"), cGeneral, 0},			//OPT_GEN_CheckLastCheck
	{OPT_GEN_DaysMeansNew, _T("DaysMeansNew"), cGeneral, 30},				//OPT_GEN_DaysMeansNew
	{OPT_GEN_LastVersion, _T("LastVersion"), cGeneral, 0},		//OPT_GEN_LastVersion

	{OPT_GEN_AutoUpdateDB, _T("AutoUpdateDB"), cGeneral, 0},			//,
	{OPT_GEN_AutoUpdateDBDelay, _T("AutoUpdateDBDelay"), cGeneral, 300},	//OPT_GEN_AutoUpdateDelay,(sec)
	{OPT_GEN_MinimizeToTray, _T("MinimizeToTray"), cGeneral, 1},		//OPT_GEN_MinimizeToTray
	{OPT_GEN_MultimediaKeysActive, _T("MultimediaKeysActive"), cGeneral, 1},	//OPT_GEN_MultimediaKeysActive
	{OPT_GEN_UseNativeOnlineServices, _T("UseNativeOnlineServices"), cGeneral, -1},	//OPT_GEN_UseTSOnlineServices

	{OPT_GEN_RelativeFontSize, _T("RelativeFontSize"), cGeneral, 0},		//OPT_GEN_RelativeFontSize
	{OPT_GEN_ForceClearTypeFonts, _T("ForceClearTypeFonts"), cGeneral, 1},	//OPT_GEN_ForceClearTypeFonts
	{OPT_GEN_ShowToolTipOnMediaChanged, _T("ShowToolTipOnMediaChanged"), cGeneral, 1},
	//{0, _T("PaneLayoutStyle"), cPaneSettings, 0},	//OPT_PANES_PaneLayoutStyle

	{OPT_TAG_WriteComments, _T("WriteComments"), cTagManager, 1},		//,
	{OPT_TAG_WriteLyrics, _T("WriteLyrics"), cTagManager, 1},		//OPT_TAG_WriteLyrics,
	{OPT_TAG_WriteRating, _T("WriteRating"), cTagManager, 0},		//OPT_TAG_WriteRating,
	{OPT_TAG_SafetyOption, _T("SafetyOption"), cTagManager, 50},			//OPT_TAG_SafetyOption
	{OPT_TAG_SafetyOptionManual, _T("SafetyOptionManual"), cTagManager, 100},	//OPT_TAG_SafetyOptionManual

	{OPT_TAG_MP3, _T("mp3"), cTagManager, 3},		//OPT_TAG_MP3,
	{OPT_TAG_OGG, _T("ogg"), cTagManager, 3},		//OPT_TAG_OGG,
	{OPT_TAG_OGA, _T("oga"), cTagManager, 3},		//OPT_TAG_OGA,
	{OPT_TAG_FLAC, _T("flac"), cTagManager, 3},	//OPT_TAG_FLAC,
	{OPT_TAG_MPC, _T("mpc"), cTagManager, 3},		//OPT_TAG_MPC,
	{OPT_TAG_WV, _T("wv"), cTagManager, 3},		//OPT_TAG_WV,
	{OPT_TAG_SPX, _T("spx"), cTagManager, 3},		//OPT_TAG_SPX,
	{OPT_TAG_TTA, _T("tta"), cTagManager, 3},		//OPT_TAG_TTA,
	{OPT_TAG_M4A, _T("m4a"), cTagManager, 0},		//OPT_TAG_M4A,
	{OPT_TAG_WMA, _T("wma"), cTagManager, 1},		//OPT_TAG_WMA,
	{OPT_TAG_AIF, _T("aif"), cTagManager, 3},		//OPT_TAG_AIF,
	{OPT_TAG_WAV, _T("wav"), cTagManager, 3},		//OPT_TAG_WAV,

	//{IDS_SHUFFLE, cPlayerSettings, 0},						//OPT_PL_Shuffle
	//{IDS_LOOP, cPlayerSettings, 0},							//OPT_PL_Loop,
	//{IDS_AUTOCONTINUE, cPlayerSettings, 1},					//OPT_PL_AutoContinue,

	{OPT_PL_PlayMode, _T("PlayMode"), cPlayerSettings, 3},				//OPT_PL_PlayMode,
	{OPT_PL_AutoContinueMode, _T("AutoContinueMode"), cPlayerSettings, RTM_All},	//OPT_PL_AutoContinueMode,
	{OPT_PL_Volume, _T("Volume"), cPlayerSettings, 90},					//OPT_PL_Volume,
	{OPT_PL_RememberLastTrack, _T("RememberLastTrack"), cPlayerSettings, 1},			//OPT_PL_RememberLastTrack,
	{OPT_PL_LastTrack, _T("LastTrack"), cPlayerSettings, 0},			//OPT_PL_LastTrackIndex,
	{OPT_PL_RememberLastTime, _T("RememberLastTime"), cPlayerSettings, 0},				//,
	{OPT_PL_LastTime, _T("LastTime"), cPlayerSettings, 0},				//OPT_PL_LastTime,
	{OPT_PL_AutoPlay, _T("AutoPlay"), cPlayerSettings, 0},					//OPT_PL_AutoPlay,
	{OPT_PL_Crossfade, _T("Crossfade"), cPlayerSettings, 1},		//OPT_PL_Crossfade,
	{OPT_PL_CrossfadeAtStart, _T("CrossfadeAtStart"), cPlayerSettings, 0},			//OPT_PL_CrossfadeAtStart,
	{OPT_PL_Mute, _T("Mute"), cPlayerSettings, 0},
	{OPT_PL_DefaultAction, _T("DefaultAction"), cPlayerSettings, MPA_Play},

	

	{OPT_PL_CrossfadeAtEnd, _T("CrossfadeAtEnd"), cPlayerSettings, 3000},		//OPT_PL_CrossfadeAtEnd,
	{OPT_PL_EqualizerMode, _T("EqualizerMode"), cPlayerSettings, 0},		//OPT_PL_EqualizerMode,
	{OPT_PL_UseSampleFloat, _T("UseSampleFloat"), cPlayerSettings, 1},	//OPT_PL_UseSampleFloat,
	{OPT_PL_UseFloatDSP, _T("UseFloatDSP"), cPlayerSettings, 1},		//OPT_PL_UseFloatDSP,

	{OPT_IM_AutoDlArtistBio, _T("AutoDlArtistBio"), cInfoMgrSettings, 1},		//OPT_IM_AutoDlArtistBio,
	{OPT_IM_AutoDlAlbumReview, _T("AutoDlAlbumReview"), cInfoMgrSettings, 1},		//OPT_IM_AutoDlAlbumReview,
	{OPT_IM_AutoDlLyrics, _T("AutoDlLyrics"), cInfoMgrSettings, 0},			//OPT_IM_AutoDlLyrics,
	{OPT_IM_AutoDlAlbumPicture, _T("AutoDlAlbumPicture"), cInfoMgrSettings, 1},	//OPT_IM_AutoDlArtistPicture,
	{OPT_IM_AutoDlArtistPicture, _T("AutoDlArtistPicture"), cInfoMgrSettings, 1},		//OPT_IM_AutoDlAlbumPicture,
	{OPT_IM_AutoDlActivationDelay, _T("AutoDlActivationDelay"), cInfoMgrSettings, 3},	//OPT_IM_AutoDlActivationDelay
	{OPT_IM_UseFolderImages, _T("UseFolderImages"), cInfoMgrSettings, 1},		//OPT_IM_UseFolderImages


	{OPT_MINIPLR_ShowAtStartup, _T("ShowAtStartup"), cMiniPlayer, 0},				//OPT_MINIPLR_ShowAtStartup
	{OPT_MINIPLR_Automatic, _T("Automatic"), cMiniPlayer, 1},
	{OPT_MINIPLR_ForceOnTop, _T("ForceOntTop"), cMiniPlayer, 0},
	{OPT_MINIPLR_ForceOnTopDelay, _T("ForceOntTopDelay"), cMiniPlayer, 15},
	{OPT_MINIPLR_Transparency, _T("Transparency"), cMiniPlayer, 140},	//OPT_MINIPLR_Transparency,
	{OPT_MINIPLR_ShowTooltips, _T("ShowTooltips"), cMiniPlayer, 1},			//OPT_MINIPLR_ShowTooltips,
	{OPT_MINIPLR_ShowExtraInfo, _T("ShowExtraInfo"), cMiniPlayer, 1},			//OPT_MINIPLR_ShowExtraInfo,
	{OPT_MINIPLR_MaxSize, _T("MaxSize"), cMiniPlayer, 400},			//OPT_MINIPLR_MaxSize
	{OPT_MINIPLR_DisableTransparencyOnMouseOver, _T("DisableTransparencyOnMouseOver"), cMiniPlayer, 1},			//OPT_MINIPLR_MaxSize

	

	//{IDS_NUMBEROFTRACKS, 0, cTree, 1},			//OPT_TREE_ShowNumTracks,
	//{IDS_YEAR, 0, cTree, 1},					//OPT_TREE_ShowYear,
	//{IDS_ITEMGROUPING, 0, cTree, 0},			//OPT_TREE_GroupingEnabled,
	//{IDS_MINNUMOFTRACKS, 0, cTree, 3},			//OPT_TREE_GroupingNumber,
	//{IDS_MODE, 0, cTree, SMODE_ArtistsAlbums},	//OPT_TREE_Mode
	//{IDS_ARTIST, 0, cTree, RSO_Title},			//OPT_TREE_SortArtist,
	//{IDS_ALBUM, 0, cTree, RSO_Year},			//OPT_TREE_SortAlbum,
	//{IDS_GENRE, 0, cTree, RSO_Title},			//OPT_TREE_SortGenre,
	//{IDS_COLLECTION, 0, cTree, RSO_Title},		//OPT_TREE_SortCollection,
	//{IDS_YEAR, 0, cTree, RSO_Title},			//OPT_TREE_SortYear,
	//{IDS_MONTH, 0, cTree, RSO_Title},			//OPT_TREE_SortMonth,
	//{IDS_FILTER, 0, cTree, SFILT_All},			//OPT_TREE_Filter,
	//{IDS_STYLE, 0, cTree, SSTYL_3},				//OPT_TREE_Style,
	//{0, _T("LastSectionID"), cTree, 0},			//OPT_TREE_LastSectionID,
	//{0, _T("LastSectionType"), cTree, 0},		//OPT_TREE_LastSectionType,


	//{IDS_SHOWINFOICONS, 0, cTrackList, 1},		//OPT_TRACKLIST_ShowInfoIcons

	//{IDS_SHOWNUMBERS, 0, cPlayList, 0},			//OPT_PL_ShowNumbers


	{OPT_Last,0,0,0}
};

const StringOptInfo StringOption2Info[] =
{
	{0, _T("AppMonitoring"), cGeneral, NULL},				//SOPT_GEN_Monitoring
	{0, _T("PaneSorting"), cPaneSettings, NULL},			//SOPT_PANES_Sorting

	{0,0,0,0}

};




PrgAPI* PrgAPI::m_pInst = NULL;

PrgAPI* PrgAPI::Instance()
{
	if (m_pInst == NULL)
	{
		m_pInst = new PrgAPI;
		m_pInst->Init();
	}
	return m_pInst;
}
void PrgAPI::Shutdown()
{
	if (m_pInst == NULL)
		return;
	delete m_pInst;	
}

void PrgAPI::SaveSettings()
{
	TRACEST(_T("PrgAPI::SaveSettings."));
	AppSettings* pAppSettings = GetAppSettings();

	TCHAR monitor[500];
	GetAppMonitor()->Save(monitor, 500);
	SetStringOption(SOPT_GEN_Monitoring, monitor);

	TranslationManager* pLM = GetTranslationManager();
	pAppSettings->Write(cGeneral, cOptionLanguage, pLM->GetTranslationPath());
	SkinManager* pSM = GetSkinManager();
	pAppSettings->Write(cGeneral, cOptionSkin, pSM->GetSkin().GetSkinPath());

	SetOption(OPT_GEN_RelativeFontSize, pSM->GetSkin().GetRelativeFontSize());

	if (m_pInfoProviderFactory != NULL)
		m_pInfoProviderFactory->SaveSettings();
	if (m_pHotKeyManager != NULL)
		m_pHotKeyManager->SaveSettings();
	if (m_pTrayToolTipDlg != NULL)
	{
		//=== Save Settings
	}
	if (m_pMiniPlayerDlg != NULL)
	{
		//=== Save Settings
	}
	if (m_pQuickSearchDlg != NULL)
	{
		//=== Save Settings
	}
	if (m_pSearchDlg != NULL)
	{
		//=== Save Settings
	}

	if (m_pLastFMServices != NULL)
	{
		pAppSettings->Write(cLastFM, cOptionUser, m_pLastFMServices->GetUserName());
		pAppSettings->Write(cLastFM, cOptionPass, m_pLastFMServices->GetMD5Password());
	}
	if (m_pLocalPictureManager)
		m_pLocalPictureManager->SaveState(GetPath(PATH_UserRoot));
	WriteOptions();
	pAppSettings->Save();
}

#ifdef _UNITTESTING
BOOL PrgAPI::UnitTest()
{
	for (int i = 0; i < 250; i++)
	{
		FLOAT stars = Rating2Stars(i);
		UNITTEST(stars <= 6.0);
		if (i < 251)
			UNITTEST(Stars2Rating(stars) == i);
	}
	return TRUE;
}
#endif


PrgAPI::PrgAPI():
m_pNotificationTarget(NULL),
m_pAppSettings(NULL),
m_pLocalPictureManager(NULL),
m_pInfoDownloadManager(NULL),
m_pMediaPlayer(NULL),
m_pInfoProviderFactory(NULL),
m_hInternet(0),
m_pActionManager(NULL),
m_pServicesRegistrationSrv(NULL),
m_pFileTagger(NULL),
m_pSkinManager(NULL),
m_bDownloading(FALSE),
m_pMenuManager(NULL),
m_pTranslationManager(NULL),
m_pSQLManager(NULL),
//m_pTrackViewer(NULL),
//m_pSectionViewer(NULL),
m_pCollectionManager(NULL),
m_pHotKeyManager(NULL),
m_pTrayToolTipDlg(NULL),
m_pMiniPlayerDlg(NULL),
m_pQuickSearchDlg(NULL),
m_pSearchDlg(NULL),
m_pLastFMServices(NULL),
m_pTracker(NULL),
m_pTSOnlineServices(NULL),
m_pShortcutManager(NULL),
m_pConfigurationManager(NULL),
m_pStateManager(NULL),
m_pAppMonitor(NULL)
{
	memset(m_pAppFonts, 0, sizeof(m_pAppFonts));
	memset(m_AppIcons, 0, sizeof(m_AppIcons));
}

PrgAPI::~PrgAPI()
{
	//=== YOU SHOULD NOT DESTROY A WINDOW HERE. 
	//		AN ERROR MAY OCCUR AS THE MFC DOES NOT LIKE TO DESTROY WINDOWS AFTER IT HAS DESTROYED 
	//		HIS MANIN WND. For example... if you try to destroy TrayToolTipDlg here
	//		a premature exit may occur before reaching this point
	TRACEST(_T("PrgAPI::~PrgAPI"));
	GetAppMonitor()->StopMonitoring();
	SaveSettings();
	if (m_hInternet != 0)
	{
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
	delete m_pSkinManager;
	m_pSkinManager = NULL;
	delete m_pHotKeyManager;
	m_pHotKeyManager = NULL;
	DestroyTrayToolTipDlg();
	DestroyMiniPlayerDlg();
	DestroyQuickSearchDlg();
	DestroyAdvancedSearchDlg();

	//TRACE(_T("@2*delete* m_pShortcutManager\r\n"));
	delete m_pShortcutManager;
	m_pShortcutManager = NULL;
	delete m_pConfigurationManager;
	m_pConfigurationManager = NULL;
	delete m_pStateManager;
	m_pStateManager = NULL;
	//TRACE(_T("@2*delete* m_pLastFMServices\r\n"));
	delete m_pLastFMServices;
	m_pLastFMServices = NULL;
	//TRACE(_T("@2*delete* m_pTracker\r\n"));
	delete m_pTracker;
	m_pTracker = NULL;
	//TRACE(_T("@2*delete* m_pTSOnlineServices\r\n"));
	delete m_pTSOnlineServices;
	m_pTSOnlineServices = NULL;
	//TRACE(_T("@2*delete* m_pCollectionManager\r\n"));
	delete m_pCollectionManager;
	m_pCollectionManager = NULL;
	//TRACE(_T("@2*delete* m_pServicesRegistrationSrv\r\n"));
	delete m_pServicesRegistrationSrv;
	m_pServicesRegistrationSrv = NULL;
	//TRACE(_T("@2*delete* m_pMediaPlayer\r\n"));
	delete m_pMediaPlayer;
	m_pMediaPlayer = NULL;
	//TRACE(_T("@2*delete* m_pLocalPictureManager\r\n"));
	delete m_pLocalPictureManager;
	m_pLocalPictureManager = NULL;
	//TRACE(_T("@2*delete* m_pInfoDownloadManager\r\n"));
	delete m_pInfoDownloadManager;
	m_pInfoDownloadManager = NULL;
	//TRACE(_T("@2*delete* m_pActionManager\r\n"));
	delete m_pActionManager;
	m_pActionManager = NULL;
	//TRACE(_T("@2*delete* m_pInfoProviderFactory\r\n"));
	delete m_pInfoProviderFactory;
	m_pInfoProviderFactory = NULL;
	//TRACE(_T("@2*delete* m_pFileTagger\r\n"));
	delete m_pFileTagger;
	m_pFileTagger = NULL;
	//TRACE(_T("@2*delete* m_pMenuManager\r\n"));
	delete m_pMenuManager;
	m_pMenuManager = NULL;
	//TRACE(_T("@2*delete* m_pLanguageManager\r\n"));
	delete m_pTranslationManager;
	m_pTranslationManager = NULL;
	//TRACE(_T("@2*delete* ObjFactory\r\n"));
	delete ObjFactory::Instance();
	//TRACE(_T("@2*delete* m_pSQLManager\r\n"));
	delete m_pSQLManager;
	m_pSQLManager = NULL;
	//TRACE(_T("@2*delete* ..fonts...\r\n"));
	ClearFonts();
	//TRACE(_T("@2*delete* ..icons...\r\n"));
	for (int i=0 ; i<ICO_Last; i++)
	{
		if (m_AppIcons[i] != NULL)
		{
			DestroyIcon(m_AppIcons[i]);
			m_AppIcons[i] = NULL;
		}
	}
	//TRACE(_T("@2*delete* m_pAppSettings\r\n"));
	if (m_pAppSettings != NULL)
	{
		m_pAppSettings->Save();
		delete m_pAppSettings;
		m_pAppSettings = NULL;
	}
	delete m_pAppMonitor;
	m_pAppMonitor = NULL;
}


AppMonitor* PrgAPI::GetAppMonitor()					
{
	if (m_pAppMonitor == NULL)
	{
		m_pAppMonitor = new AppMonitor;
		TCHAR monitor[500];
		if (GetStringOption(SOPT_GEN_Monitoring, monitor, 500))
		{
			VERIFY(m_pAppMonitor->Load(monitor));
		}
		AppMonitor::State& st = m_pAppMonitor->GetState();
		if (st.TotalRuns == 0)
		{
			//=== This is the first time that this appmonitor runs [version >= 96b]
			//=== Check if ver 96 has ever been run
			ApplicationMonitor amJaangle(TRUE);
			amJaangle.InitByName(_T("Jaangle"));
			if (amJaangle.GetCount(ApplicationMonitor::CT_TotalRuns) > 0)
			{
				//=== Version 96 Found. Get the values
				st.TotalRuns = amJaangle.GetCount(ApplicationMonitor::CT_TotalRuns);
				st.TotalCrashes = amJaangle.GetCount(ApplicationMonitor::CT_TotalCrashes);
				amJaangle.GetInstallationDate(&st.InstallationDate);
				st.RunningTimeInMinutes = amJaangle.GetRunningTimeInMinutes();
				st.AppUniqueID = amJaangle.GetAppUniqueID();
			}
			//=== Check if ver < 96 (Teen Spirit) has ever been run
			ApplicationMonitor apTS(TRUE);
			apTS.InitByName(_T("Teen Spirit"));
			if (apTS.GetCount(ApplicationMonitor::CT_TotalRuns) > 0)
			{
				//=== Version Teen Spirit (<96) Found.
				if (st.TotalRuns == 0)
				{
					//=== Jaangle has never been run
					st.TotalRuns = apTS.GetCount(ApplicationMonitor::CT_TotalRuns);
					st.TotalCrashes = apTS.GetCount(ApplicationMonitor::CT_TotalCrashes);
				}
				//===The previous code has a bug and it couldn't take 
				//		the UniqueID & real installation Date. So we will re-grab it here
				apTS.GetInstallationDate(&st.InstallationDate);
				st.RunningTimeInMinutes = apTS.GetRunningTimeInMinutes();
				st.AppUniqueID = apTS.GetAppUniqueID();
			}

		}
		m_pAppMonitor->StartMonitoring();
		m_pAppMonitor->IncreaseAppCount();
		if (m_pAppMonitor->IsAfterCrash())
			m_pAppMonitor->IncreaseCrashCount();
		m_pAppMonitor->Save(monitor, 500);
		SetStringOption(SOPT_GEN_Monitoring, monitor);
	}
	return m_pAppMonitor;
}


BOOL PrgAPI::Init()
{
	TRACEST(_T("PrgAPI::Init"));


#ifdef _DEBUG
	for (INT i = 1; i < OPT_Last; i++)
	{
		if (Option2Info[i].optID != i)
			ASSERT(0);
	}
#endif

	ReadOptions();
	//===Check for Crash
	LPCTSTR files[] = CRASHREPORT_FILES;
	TCHAR crashLogPath[MAX_PATH];
	_sntprintf(crashLogPath, MAX_PATH, _T("%s%s"), GetPath(PATH_AppRoot), files[0]);
	BOOL bCrashLogExists = FALSE;
	if (_taccess(crashLogPath, 0) == 0)
	{
		bCrashLogExists = TRUE;
		TRACE(_T("@1 PrgAPI::Init Crash Log exists meaning that the app has crashed\r\n"));
		//=== Crash Log exists meaning that the app has crashed.
		//=== 1. Delete the file
		DeleteFile(crashLogPath);
		//=== 2. Increase the crash count
		//GetAppMonitor()->IncreaseCrashCount();
		//=== 3. Change the debugging option
		if (GetOption(OPT_GEN_Debugging) == 0)
		{
			if (MessageBox(NULL, _T("Jaangle seems to have crashed. Do you want to enable crash logging?"), NULL, MB_YESNO) == IDYES)
			{
				SetOption(OPT_GEN_Debugging, 1);
				WriteOptions();//=== Save this info (in a case of a second crash)
			}
		}
	}
	if (GetAppMonitor()->IsAfterCrash())
	{
		if (bCrashLogExists)
			TRACE(_T("@1 PrgAPI::Init Abnormal Ending Detected. CrashLog OK\r\n"));
		else
		{
			//ASSERT(0);
			TRACE(_T("@1 PrgAPI::Init Abnormal Ending Detected. CrashLog DOES NOT EXIST\r\n"));
		}
	}
	else
	{
		if (bCrashLogExists)
		{
			ASSERT(0);
			TRACE(_T("@1 PrgAPI::Init CrashLog Found but there was no crash detection\r\n"));
		}
	}
#ifdef ALOG_ACTIVE
	if (GetOption(OPT_GEN_Debugging) == 1)
	{
		TCHAR debugLogPath[MAX_PATH];
		_sntprintf(debugLogPath, MAX_PATH, _T("%sdebug.log"), GetPath(PATH_AppRoot));
		ALog* pLog = ALog::Inst();
		pLog->AddViewer(new PrunnedFileViewer(debugLogPath, 15));
		pLog->SetOption(ALog::LO_MaxAllowedDepth, 5);
		pLog->SetOption(ALog::LO_DefaultDepth, 3);
		pLog->SetOption(ALog::LO_ShowDepth, 1);
		pLog->SetOption(ALog::LO_ShowTimeFromStartup, 1);
		pLog->SetOption(ALog::LO_ShowThreadID, 1);
		pLog->SetOption(ALog::LO_AlwaysCheckForLastError, 1);
	}
#endif

	//=== Initial bass instance initialization
	BassInstance::bUseFloatDSP = GetOption(OPT_PL_UseFloatDSP) != 0;
	BassInstance::bUseSampleFloat = GetOption(OPT_PL_UseSampleFloat) != 0;

	std::tstring value;
	GetAppSettings()->Read(cGeneral, cOptionLanguage, value, _T(""));
	BOOL bLoaded = FALSE;
	TranslationManager* pLM = GetTranslationManager();
	if (value.empty())
	{
		//=== This value does not exist. Load lang ny langID
		pLM->LoadTranslationByLANGID(GetSystemDefaultLangID());
	}
	else if (value[0] == '@')
	{
		//=== Default Language. Do not load anything
	}
	else
	{
		//=== Load the selected translation
		pLM->LoadTranslation(value.c_str());
	}


	FileVersion fv;
	if (GetFileVersion(fv))
	{
		if (fv.Build != GetOption(OPT_GEN_LastVersion))
		{
			TRACE(_T("@2 New Version detected %d->%d\r\n"), GetOption(OPT_GEN_LastVersion), fv.Build);
			AppMonitor::State& state = GetAppMonitor()->GetState();
			state.Runs = 1;
			state.Crashes = 0;
			SetOption(OPT_GEN_LastVersion, fv.Build);
			SetOption(OPT_GEN_CheckLastCheck, 0);//Make it perform an update check instantly after first time
			//Write it here. Otherwise if the program crash It ll reset the creashes for the current version
			GetAppSettings()->Write(Option2Info[OPT_GEN_LastVersion].category, GetStringForOption(OPT_GEN_LastVersion), fv.Build);
		}
		else
			TRACE(_T("@2Version: %d\r\n"), fv.Build);

	}

	return TRUE;
}

//to BE removed
AppSettings* PrgAPI::GetAppSettings()
{
	if (m_pAppSettings == NULL)
	{
		TRACEST(_T("PrgAPI::GetAppSettings [CREATE]"));
		m_pAppSettings = ObjFactory::Instance()->CreateAppSettings();
	}
	return m_pAppSettings;
}

LPCTSTR PrgAPI::GetString(INT resID)
{
	ASSERT(resID != 0);
	LPCTSTR ret = NULL;
	if (resID > 0)
	{
		ret = GetTranslationManager()->GetString(resID);
		//if (m_pTranslationManager != NULL && m_pLanguageManager->IsLanguageLoaded())
		//	ret = m_pLanguageManager->GetLanguage().GetString(resID);
		//if (ret == NULL)
		//	ret = m_resourceBasedLanguage.GetString(resID);
	}
	else
	{
		TRACE(_T("@0 PrgAPI::GetString Unknown resID:%d\r\n"), resID);
	}
	if (ret == NULL)
		return _T("");
	return ret;
}

LPCTSTR PrgAPI::GetStringForItemType(ItemTypeEnum it)
{
	ASSERT(it >= IT_Unknown && it < IT_Last);
	INT resID = IDS_UNKNOWN;
	if (it >= IT_Unknown && it < IT_Last)
	{
		static const INT ItemTypeToStringResource[] =
		{
			IDS_UNKNOWN,
			IDS_VARIOUS,
			IDS_ARTIST,
			IDS_ALBUM,
			IDS_GENRE,
			IDS_YEAR,
			IDS_UNKNOWN,	//????
			IDS_COLLECTION,
			IDS_TITLE,
			IDS_UNKNOWN,	//???
			IDS_UNKNOWN
		};
		resID = ItemTypeToStringResource[it];
	}
	return GetString(resID);
}

LPCTSTR PrgAPI::GetStringForInfoItemType(InfoItemTypeEnum iit)
{
	ASSERT(iit >= IIT_Unknown && iit < IIT_Last);
	INT resID = IDS_UNKNOWN;
	if (iit >= IIT_Unknown && iit < IIT_Last)
	{
		static const INT InfoItemTypeToStringResource[] =
		{
			IDS_UNKNOWN,

			IDS_ARTISTBIO,
			IDS_ALBUMREVIEW,
			IDS_COMMENTS,
			IDS_LYRICS,
			IDS_PERSONAL,
			IDS_TABLATURES,
			
			IDS_ARTISTPICTURE,
			IDS_ALBUMPICTURE,
			IDS_UNKNOWN
		};
		resID = InfoItemTypeToStringResource[iit];
	}


	return GetString(resID);
}


LPCTSTR PrgAPI::GetStringForCollectionType(CollectionTypesEnum ct)
{
	ASSERT(ct >= CTYPE_Unknown && ct < CTYPE_Last);
	INT resID = IDS_UNKNOWN;
	if (ct >= CTYPE_Unknown && ct < CTYPE_Last)
	{
		static const INT CollectionTypeToStringResource[] =
		{
			IDS_UNKNOWN,				//CTYPE_Unknown
			IDS_LOCALFOLDER, 
			IDS_MEDIA, 
			IDS_NETFOLDER, 
			IDS_VIRTUALTRACKS,	//CTYPE_Virtual
			IDS_REMOVABLE, 
			IDS_AUDIOCD, 
			IDS_UNKNOWN
		};
		ASSERT(sizeof(CollectionTypeToStringResource) / sizeof(INT) == CTYPE_Last);
		resID = CollectionTypeToStringResource[ct];
	}
	return GetString(resID);
}
const INT RandomModeToStringResource[] =
{
	IDS_ALL,
	IDS_ARTIST, 
	IDS_ALBUM,
	IDS_GENRE,
	IDS_YEAR,
	IDS_COLLECTION,
	IDS_NEW
};

LPCTSTR PrgAPI::GetStringForRandomMode(RandomTrackModeEnum rtm)
{
	ASSERT(rtm >= RTM_All && rtm < RTM_Last);
	INT resID = IDS_UNKNOWN;
	if (rtm >= RTM_All && rtm < RTM_Last)
	{
		ASSERT(sizeof(RandomModeToStringResource) / sizeof(INT) == RTM_Last);
		resID = RandomModeToStringResource[rtm];
	}
	return GetString(resID);
}

const INT TrackTextTypeToStringResource[] =
{
	IDS_TITLE,
	IDS_ALBUM,
	IDS_ARTIST
};

LPCTSTR PrgAPI::GetStringForTrackTextType(TrackTextType ttt)
{
	ASSERT(ttt >= 0 && ttt < TTT_Last);
	INT resID = IDS_UNKNOWN;
	if (ttt >= 0 && ttt < TTT_Last)
	{
		ASSERT(sizeof(TrackTextTypeToStringResource) / sizeof(INT) == TTT_Last);
		resID = TrackTextTypeToStringResource[ttt];
	}
	return GetString(resID);
}



LPCTSTR PrgAPI::GetStringForOption(AppOptionsEnum option)
{
	ASSERT(option > OPT_Unknown && option < OPT_Last);
	ASSERT((INT) option < sizeof(Option2Info) / sizeof(Option2Info[0]));
	ASSERT(Option2Info[option].optID == option);
	if (Option2Info[option].optID != option)
		return NULL;
	return Option2Info[option].valueName;
	//INT resID = Option2Info[option].resID;
	//if (resID == 0)
	//{
	//	ASSERT(bDefLang == TRUE);
	//	ASSERT(Option2Info[option].valueName != NULL);
	//	return Option2Info[option].valueName;
	//}
	//else
	//{
	//	if (bDefLang)
	//		return GetTranslationManager()->GetDefaultTranslation().GetString(resID);
	//	else
	//		return GetString(resID);

	//}

	return NULL;
}

const INT PlayModeToStringResource[] =
{
	IDS_NORMAL,
	IDS_SHUFFLE, 
	IDS_LOOP,
	IDS_AUTOCONTINUE
};

LPCTSTR PrgAPI::GetStringForPlayListMode(MediaPlayerNextMode mpm)
{
	ASSERT(mpm >= MPNM_Normal && mpm < MPNM_Last);
	INT resID = IDS_NORMAL;
	if (mpm >= MPNM_Normal && mpm < MPNM_Last)
	{
		ASSERT(sizeof(PlayModeToStringResource) / sizeof(INT) == MPNM_Last);
		resID = PlayModeToStringResource[mpm];
	}
	return GetString(resID);

}
LPCTSTR PrgAPI::GetStringForTrackType(TrackTypesEnum tt)
{
	if (tt < TTYPE_FIRSTVIDEO)
		return GetString(IDS_AUDIO);
	else if (tt < TTYPE_FIRSTVIRTUAL)
		return GetString(IDS_VIDEO);
	return GetString(IDS_VIRTUALTRACKS);
}


LPCTSTR PrgAPI::GetStringForStars(UINT Stars)
{
	ASSERT(Stars >= 0 && Stars < 6); //should be 1 - 5
	if (!(Stars >= 0 && Stars < 6))
		return cErrStr;
	const INT stars2res[] = {IDS_CLEAR, IDS_POOR, IDS_OK, IDS_GOOD, IDS_VERYGOOD, IDS_OUTSTANDING};
	return GetString(stars2res[Stars]);
}

LPCTSTR PrgAPI::GetStringForCaseStyle(CaseStyleEnum cStyle)
{
	ASSERT(cStyle < CASE_Last); //should be 1 - 5
	if (!(cStyle < CASE_Last))
		return cErrStr;
	const INT cstyle2res[] = {IDS_ALLCAPS, IDS_NOCAPS, IDS_FIRSTCAP, IDS_PROPERCASE};
	return GetString(cstyle2res[cStyle]);

}

LPCTSTR PrgAPI::GetStringForRecordSortOption(RecordSortOptionsEnum rso)
{
	ASSERT(rso>RSO_None && rso<RSO_Last);
	if (!(rso>RSO_None && rso<RSO_Last))
		return cErrStr;
	INT RecordSortOptionsEnum2Res[] ={0, IDS_NAME, IDS_TRACKNO, IDS_ALBUM, IDS_ARTIST,
		IDS_RATING, IDS_LOCATION, IDS_SIZE, IDS_DURATION, IDS_BITRATE, IDS_YEAR, 
		IDS_GENRE, IDS_COLLECTION, IDS_DATEADDED, IDS_NUMBEROFTRACKS};
	return GetString(RecordSortOptionsEnum2Res[rso]);

}

LPCTSTR PrgAPI::GetStringForSectionFilter(SectionFiltersEnum sf)
{
	ASSERT(sf>SFILT_First && sf<SFILT_Last);
	if (!(sf>SFILT_First && sf<SFILT_Last))
		return cErrStr;
	const INT SectionFilterResource[] ={0,IDS_ALL,IDS_AUDIO,IDS_VIDEO, IDS_NEW};
	return GetString(SectionFilterResource[sf]);
}

LPCTSTR PrgAPI::GetStringForSectionType(SectionTypeEnum st)
{
	ASSERT(st > ST_Unknown && st <ST_Last);
	if (!(st > ST_Unknown && st < ST_Last))
		return cErrStr;
	const INT SectionTypeEnumResource[] ={0,0,IDS_ARTIST,IDS_ALBUM,IDS_GENRE,IDS_YEAR,IDS_COLLECTION,
		IDS_ALBUM, IDS_MONTH, 0};
	return GetString(SectionTypeEnumResource[st]);
}

void PrgAPI::GetStringForSectionMode(SectionModesEnum sm, LPTSTR bf, UINT bfLen)
{
	INT curLen = 0;
	SectionTypeEnum st = CMainTreeCtrl::GetChildForSectionType(sm, ST_Root);
	bf[0] = 0;
	while (st != ST_Unknown)
	{
		LPCTSTR prefix = _T("");
		if (bf[0] != 0)
			prefix = _T(" - ");
		//TRACE(_T("bf: %u(%s)\r\n"), (INT)&bf[0], bf);
		//TRACE(_T("bfLen:%u (%d)\r\n"), (INT)&bfLen, (INT) bfLen);
		//TRACE(_T("curLen:%u(%d)\r\n"), (INT)&curLen, (INT)curLen);
		INT len = _sntprintf(&bf[curLen], bfLen - curLen, _T("%s%s"), prefix, GetStringForSectionType(st));
		curLen += len;
		st = CMainTreeCtrl::GetChildForSectionType(sm, st);
	}
	bf[bfLen - 1] = 0;
}




void PrgAPI::NotifyUser(LPCTSTR msg, 
				LPCTSTR title/* = NULL*/, 
				SEVERITY sev /*= SEV_InfoL1*/)
{
	ASSERT(msg != NULL);
	PRGAPI()->GetActionManager()->LogUserMessage(msg, title, sev);
	if (m_pNotificationTarget != NULL)
		m_pNotificationTarget->Show(msg, title, sev);
}
void PrgAPI::NotifyUser(INT msgResID, 
						INT titleResID/* = NULL*/, 
						SEVERITY sev /*= SEV_InfoL1*/)
{
	NotifyUser(GetString(msgResID), (titleResID > 0) ? GetString(titleResID) : 0, sev);
}


int PrgAPI::MessageBox(HWND parentHWND, LPCTSTR message, LPCTSTR caption, DWORD style)
{
	CMessageDlg dlg;
	dlg.SetIcon(GetIcon(ICO_Main));
	dlg.SetText(CMessageDlg::TT_Title, caption);
	dlg.SetText(CMessageDlg::TT_SubTitle, message);
	DWORD type = style & 0xF;

	switch (type)
	{
	case MB_OK:
		dlg.SetText(CMessageDlg::TT_ButtonRight, GetString(IDS_OK));
		dlg.DoModal();
		return IDOK;
		break;
	case MB_YESNO:
		dlg.SetText(CMessageDlg::TT_ButtonRight, GetString(IDS_NO_S));
		dlg.SetText(CMessageDlg::TT_ButtonMid, GetString(IDS_YES_S));
		switch (dlg.DoModal())
		{
		case CMessageDlg::RES_Close:
		case CMessageDlg::RES_Right:
			return IDNO;
		case CMessageDlg::RES_Mid:
			return IDYES;
		}
		break;
	default:
		if (caption == NULL || caption[0] == 0)
			caption = CTeenSpiritApp::sAppTitle;
		return ::MessageBox(parentHWND, message, caption, style);
	}
	return IDCANCEL;
}

int PrgAPI::MessageBox(HWND parentHWND, INT messageID, INT captionID, DWORD style)
{
	return MessageBox(parentHWND, GetString(messageID), (captionID > 0) ? GetString(captionID) : 0, style);
}

INT PrgAPI::GetOption(AppOptionsEnum opt)
{
	ASSERT(opt>OPT_Unknown && opt<OPT_Last);
	return m_Options[opt];
}

void PrgAPI::SetOption(AppOptionsEnum opt, INT val)
{
	ASSERT(opt>OPT_Unknown && opt<OPT_Last);
	m_Options[opt] = val;
}

BOOL PrgAPI::GetStringOption(AppStringOptionsEnum opt, LPTSTR bf, UINT bfLen)
{	
	ASSERT(opt > SOPT_Unknown && opt < SOPT_Last);
	AppSettings* appSettings = GetAppSettings();
	LPCTSTR valueName = NULL;

	INT resID = StringOption2Info[opt].resID;
	if (resID == 0)
		valueName = StringOption2Info[opt].valueName;
	else
		valueName = GetTranslationManager()->GetDefaultTranslation().GetString(resID);
	if (valueName == NULL)
		return FALSE;

	std::tstring tmp;
	if (appSettings->Read(StringOption2Info[opt].category, valueName, tmp, StringOption2Info[opt].defVal) == FALSE)
		return FALSE;
	if (tmp.size() >= bfLen)
		return FALSE;
	_tcscpy(bf, tmp.c_str());
	return TRUE;
}
BOOL PrgAPI::SetStringOption(AppStringOptionsEnum opt, LPCTSTR value)
{
	ASSERT(opt > SOPT_Unknown && opt < SOPT_Last);
	LPCTSTR valueName = NULL;

	INT resID = StringOption2Info[opt].resID;
	if (resID == 0)
		valueName = StringOption2Info[opt].valueName;
	else
		valueName = GetTranslationManager()->GetDefaultTranslation().GetString(resID);
	if (valueName == NULL)
		return FALSE;
	return GetAppSettings()->Write(StringOption2Info[opt].category, valueName, value);
}



void PrgAPI::ReadOptions()
{
	TRACEST(_T("PrgAPI::ReadOptions"));
	AppSettings* appSettings = GetAppSettings();
	for (int i = OPT_Unknown + 1; i < OPT_Last; i++)
		appSettings->Read(Option2Info[i].category, GetStringForOption(AppOptionsEnum(i)), m_Options[i], Option2Info[i].defVal);
}

void PrgAPI::WriteOptions()
{
	TRACEST(_T("PrgAPI::WriteOptions."));
	AppSettings* pAppSettings = GetAppSettings();
	if (m_pMediaPlayer != NULL)//GetOption will fail to return Real Player Options
	{
		TCHAR playListFileName[MAX_PATH];
		GetPath(PATH_LastM3UFile, playListFileName);
		DeleteFile(playListFileName);
		MediaPlayerUtilities::SaveM3U(*m_pMediaPlayer, playListFileName);
		INT ACMode = 0;
		SetOption(OPT_PL_PlayMode, m_pMediaPlayer->GetNextMode(&ACMode));//OPT_PL_PlayMode,
		SetOption(OPT_PL_AutoContinueMode, ACMode);
		SetOption(OPT_PL_Volume, m_pMediaPlayer->GetVolume());
		SetOption(OPT_PL_Crossfade, m_pMediaPlayer->GetEngineConfig(MECO_Crossfade));
		SetOption(OPT_PL_CrossfadeAtStart, m_pMediaPlayer->GetEngineConfig(MECO_CrossfadeAtStart));
		SetOption(OPT_PL_CrossfadeAtEnd, m_pMediaPlayer->GetEngineConfig(MECO_CrossfadeAtEnd));
		SetOption(OPT_PL_EqualizerMode, m_pMediaPlayer->GetEngineConfig(MECO_EqualizerMode));
		SetOption(OPT_PL_Mute, m_pMediaPlayer->GetMute());

		if (m_pMediaPlayer->EngineSupports(MECO_EqualizerMode))
		{
			for (INT i = MECO_EqualizerBandStart; i < MECO_EqualizerBandEnd; i++)
			{
				if (m_pMediaPlayer->EngineSupports(MediaEngineConfigOption(i)))
				{
					TCHAR bf[20];
					_sntprintf(bf, 20, _T("EQ%d"), i - MECO_EqualizerBandStart);
					pAppSettings->Write(cPlayerSettings, bf, m_pMediaPlayer->GetEngineConfig(MediaEngineConfigOption(i)));
				}

			}
		}
		SetOption(OPT_PL_LastTrack, m_pMediaPlayer->GetPlayListPos());
		SetOption(OPT_PL_LastTime, (INT)m_pMediaPlayer->GetMediaPos());
	}
	for (int i = OPT_Unknown + 1; i < OPT_Last; i++)
		pAppSettings->Write(Option2Info[i].category, GetStringForOption(AppOptionsEnum(i)), GetOption(AppOptionsEnum(i)));
}

void PrgAPI::FormatDuration(LPTSTR bf, UINT bfLen, INT duration)
{
	ASSERT(bf != NULL && bfLen > 1);
	INT h = duration / 3600;
	duration = duration - h * 3600;
	INT m = duration / 60;
	if (h == 0)
		_sntprintf(bf, bfLen, _T("%d:%02d"), m, duration - m * 60);
	else
		_sntprintf(bf, bfLen, _T("%d:%02d:%02d"), h, m, duration - m * 60);
	bf[bfLen - 1] = 0;
}



void PrgAPI::FormatDate(TCHAR* bf, size_t bfLen, SYSTEMTIME& st, BOOL bSmart)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	DOUBLE dt, dst;
	SystemTimeToVariantTime(&t, &dt);
	SystemTimeToVariantTime(&st, &dst);
	if (dt - dst <= 4.0 && bSmart)
	{
		if (dt - dst > 1.0)
			GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &st, _T("dddd "), bf, bfLen);
		else
			bf[0] = 0;
		INT curLen = _tcslen(bf);
		GetTimeFormat(LOCALE_SYSTEM_DEFAULT, NULL, &st, _T("HH:mm:ss"), &bf[curLen], bfLen - curLen);
	}
	else
	{
		GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &st, _T("d MMM yyyy"), bf, bfLen);
	}
}

void PrgAPI::Monitor()
{
	if (m_pMediaPlayer && m_pMediaPlayer->GetPlayState() == PS_Playing)
		GetActionManager()->TrackHistory();
	if (InfoDownloadManagerExists())
		m_pInfoDownloadManager->ConsumeResult();
	if (CollectionManagerExists())
		m_pCollectionManager->ConsumeResult();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//------------Global Objects acquisition
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------



#include "TeenSpiritPlayer.h"
#include "DShowEngine.h"
#include "MultiEngine.h"
#include "HttpLinkEngine.h"
//#include "FullTracksPlaylist.h"
//#include "ImplPlaylist.h"
#include "WinampVisualization.h"
#include "NativeVisualization.h"
#include "FadeBassEngine.h"
#include "VectorPlaylist.h"
#include "YouTubePlayerEngine.h"



//---GetMediaPlayer()
//We Create on demand Using the objfactory and then we configure
MediaPlayer* PrgAPI::GetMediaPlayer()
{
	if (m_pMediaPlayer == NULL)
	{
		TRACEST(_T("PrgAPI::GetMediaPlayer [CREATION]"));
		HWND hwndPlayer = ::CreateWindow(_T("Static"), _T("PlayerWnd"), WS_CHILD, 0, 0, 0, 0, AfxGetMainWnd()->m_hWnd, 0, AfxGetInstanceHandle(), 0);
		
		m_pMediaPlayer = new TeenSpiritPlayer;
		MultiEngine* eng = new MultiEngine;
		eng->AddEngine(new FadeBassEngine);
		eng->AddEngine(new DShowEngine);
#ifdef USE_YOUTUBEENGINE
		YouTubePlayerEngine* pYTEng = new YouTubePlayerEngine;
		//=== Try to make it use directly the resource without writing a file
		//=== Otherwise write it in the "User" sub folder and then use it.
		TCHAR bf[MAX_PATH];
		GetPath(PATH_AppRoot, bf);
		_tcscat(bf, _T("youtubeapi.htm"));
		pYTEng->SetYouTubeAPIURL(bf);
		eng->AddEngine(pYTEng);
#else
		eng->AddEngine(new HttpLinkEngine);

#endif
		m_pMediaPlayer->SetMediaPlayerEngine(eng);
		MediaPlayList* pList = new VectorPlayList;
		m_pMediaPlayer->SetMediaPlayList(pList);
		WinampVisualization* pVis = new WinampVisualization;
		pVis->LoadPlugin(_T("vis_avs.dll"));
		//MediaVisualization* pVis = new NativeVisualization;
		m_pMediaPlayer->SetMediaVisualization(pVis);
		//m_pMediaPlayer->Init(eng, lst, pwVis, hwndPlayer);

		m_pMediaPlayer->SetNextMode(MediaPlayerNextMode(m_Options[OPT_PL_PlayMode]), m_Options[OPT_PL_AutoContinueMode]);
		eng->SetVolume(GetOption(OPT_PL_Volume));
		eng->SetMute(GetOption(OPT_PL_Mute));
		eng->SetConfig(MECO_Crossfade, GetOption(OPT_PL_Crossfade));
		eng->SetConfig(MECO_CrossfadeAtStart, GetOption(OPT_PL_CrossfadeAtStart));
		eng->SetConfig(MECO_CrossfadeAtEnd, GetOption(OPT_PL_CrossfadeAtEnd));
		eng->SetConfig(MECO_EqualizerMode, GetOption(OPT_PL_EqualizerMode));

		
		for (INT i = MECO_EqualizerBandStart; i < MECO_EqualizerBandEnd; i++)
		{
			TCHAR bf[20];
			_sntprintf(bf, 20, _T("EQ%d"), i - MECO_EqualizerBandStart);
			INT val = 50;
			GetAppSettings()->Read(cPlayerSettings, bf, val, 50);
			eng->SetConfig(MediaEngineConfigOption(i), val);
		}
	
		
		
		//SendMessage(SM_PlayerSettingsChanged);
		//SendMessage(SM_PlayerVolumeChanged);


		//Parse command line
		BOOL bEnqueue = FALSE;
		for (INT i=1; i<__argc;i++)
		{
			if (_tcscmp(_T("-q"), __targv[i]))
				bEnqueue = FALSE;
			if (__targv[i][0] != '-' && __targv[i][0] !='\\')
			{
				if (bEnqueue)
					m_pMediaPlayer->Enqueue(__targv[i]);
				else
					m_pMediaPlayer->Play(__targv[i]);
				bEnqueue = TRUE;
			}
		}
		BOOL bLoadOldPlaylist = FALSE;
		if (m_pMediaPlayer->GetPlayListCount() == 0)
		{
			bLoadOldPlaylist = TRUE;
			MediaPlayerUtilities::InsertM3U(*m_pMediaPlayer, GetPath(PATH_LastM3UFile));
		}
		if (bLoadOldPlaylist)
		{
			if (m_Options[OPT_PL_RememberLastTrack])
			{
				if ((INT)m_pMediaPlayer->GetPlayListCount() > m_Options[OPT_PL_LastTrack])
				{
					m_pMediaPlayer->SetPlayListPos(m_Options[OPT_PL_LastTrack]);
					if (m_Options[OPT_PL_RememberLastTime])
					{
						//m_pMediaPlayer->Start();
						//m_pMediaPlayer->Pause();
						m_pMediaPlayer->SetMediaPos(DOUBLE(m_Options[OPT_PL_LastTime]));
					}
				}
			}
		}
		if (m_Options[OPT_PL_AutoPlay] && m_pMediaPlayer->GetPlayListCount() > 0)
		{
			m_pMediaPlayer->Start();
		}
	}
	return m_pMediaPlayer;
}

//---GetInformationManager()
//We Create on demand HERE and then we configure
LocalPictureManager* PrgAPI::GetLocalPictureManager()
{
	if (m_pLocalPictureManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetLocalPictureManager [CREATION]"));
		m_pLocalPictureManager = new LocalPictureManager;
		m_pLocalPictureManager->Init(GetPath(PATH_StorageRoot));
		m_pLocalPictureManager->LoadState(GetPath(PATH_UserRoot));
		m_pLocalPictureManager->EnableFolderImages(GetOption(OPT_IM_UseFolderImages) != 0);
	}
	return m_pLocalPictureManager;
}



InfoDownloadManager* PrgAPI::GetInfoDownloadManager()
{
	if (m_pInfoDownloadManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetInfoDownloadManager [CREATION]"));
		m_pInfoDownloadManager = new InfoDownloadManager(*GetInfoProviderFactory());
	}
	return m_pInfoDownloadManager;

}

HINTERNET PrgAPI::GetInternetHandle()
{
	if (m_hInternet == NULL)
	{
		TRACEST(_T("PrgAPI::GetInternetHandle [CREATION]"));
		INT i = 0;
		GetAppSettings()->Read(_T("General"), _T("ImpersonateAgent"), (INT&) i,  0);
		std::tstring agent = _T("Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9) Gecko/2008052906 Firefox/3.0");
		if (i != 0)
			GetAppSettings()->Read(_T("General"), _T("FalseAgentName"), agent,  _T(""));
		TRACE(_T("@2{Internet Handle Created}\n"));
		m_hInternet = InternetOpen(agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	}
	return m_hInternet;
}

//---GetActionManager()
//We Create on demand HERE
ActionManager* PrgAPI::GetActionManager()
{
	if (m_pActionManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetActionManager"));
		m_pActionManager = new ActionManager;
	}
	return m_pActionManager;
}

//---GetCollectionManager()
//We Create on demand HERE
//CollectionManager* PrgAPI::GetCollectionManager(BOOL bForceCreate)
//{
//	if (m_pCollectionManager == NULL && bForceCreate)
//	{
//		m_pCollectionManager = new CollectionManager;
//		TRACE(_T("@2{Collection Manager Created}\n"));
//	}
//	return m_pCollectionManager;
//}

//---GetFileTagger()
//We Create on demand Using the objfactory
FileTagger* PrgAPI::GetFileTagger()
{
	if (m_pFileTagger == NULL)
	{
		TRACEST(_T("PrgAPI::GetFileTagger"));
		m_pFileTagger = ObjFactory::Instance()->CreateFileTagger();
	}
	m_pFileTagger->SetConfig(FileTagger::CONF_ReadDepth, GetOption(OPT_TAG_SafetyOptionManual));
	return m_pFileTagger;
}


//---GetServicesRegistrationSrv()
//We Create on demand HERE and we configure
ServicesRegistrationSrv* PrgAPI::GetServicesRegistrationSrv()
{
	if (m_pServicesRegistrationSrv == NULL)
	{
		TRACEST(_T("PrgAPI::GetServicesRegistrationSrv"));
		m_pServicesRegistrationSrv = new ServicesRegistrationSrv;
		std::tstring appPath;
		m_pServicesRegistrationSrv->LoadServices(GetPath(PATH_WebServicesRoot));
	}
	return m_pServicesRegistrationSrv;
}

CFont* PrgAPI::GetFont(APP_FONTS fon)
{
	if (m_pAppFonts[fon] == NULL)
	{
		LOGFONT lf;
		lf.lfHeight = 13;
		lf.lfWidth = 0;
		lf.lfEscapement = 0; 
		lf.lfOrientation = 0;
		lf.lfWeight = FW_NORMAL;
		lf.lfItalic = FALSE;
		lf.lfUnderline = FALSE;
		lf.lfStrikeOut = FALSE;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;//OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;//ANTIALIASED_QUALITY;
		lf.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
		_tcscpy(lf.lfFaceName, _T("Tahoma"));

		switch (fon)
		{
		case FONT_Dialogs:
			break;
		case FONT_DialogsUnderline:
			lf.lfUnderline = TRUE;
			break;
		case FONT_DialogsItalics:
			lf.lfItalic = TRUE;
			break;
		case FONT_DialogsBold:
			lf.lfWeight = FW_BOLD;
			break;
		case FONT_DialogsBig:
			lf.lfHeight = 18;
			break;
		case FONT_DialogsBigBold:
			lf.lfHeight = 18;
			lf.lfWeight = FW_BOLD;
			break;
		case FONT_Fixed:
			lf.lfHeight = 15;
			lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
			_tcscpy(lf.lfFaceName, _T("Courier New"));
			break;
		//case FONT_Marlett:
		//	lf.lfHeight = 11;
		//	_tcscpy(lf.lfFaceName, _T("Marlett"));
		//	break;
		default:
			ASSERT(0);
			break;
		}
		CFont* pFont = new CFont;
		pFont->CreateFontIndirect(&lf);
		m_pAppFonts[fon] = pFont;
	}
	return m_pAppFonts[fon];

}

const int AppIcon2Resource[][2] = 
{
	{IDR_MAINFRAME, 48},		//ICO_Info,

	{IDI_INFO,16},		//ICO_Info,
	{IDI_WARN,16},		//ICO_Warning,
	{IDI_TSERROR,16},	//ICO_Error,
	{IDI_PREVIOUS24,24},	//ICO_Prev24,
	{IDI_PLAY24,24},	//ICO_Play24,
	{IDI_PAUSE24,24},		//ICO_Pause24,
	{IDI_STOP24,24},		//ICO_Stop24,
	{IDI_NEXT24,24},		//ICO_Next24

	{IDI_CROSSFADE16,16},		//ICO_Crossfade16,
	{IDI_CROSSFADE_AC16,16},	//ICO_CrossfadeAc16,
	{IDI_EQUALIZER16,16},		//ICO_Equalizer16,
	{IDI_EQUALIZER_AC16,16},	//ICO_EqualizerAc16,
	
	{IDI_BACK16,16},	//ICO_Back16
	{IDI_FORWARD16,16},	//ICO_Forward16
	{IDI_HOME16,16},	//ICO_Home16
	{IDI_REFRESH16,16},	//ICO_Refresh16
	{IDI_INTERNET16,16},//ICO_Internet16
	{IDI_ADD16, 16},	//ICO_Add16

	{IDI_ARTISTS_ICON, 16},	//ICO_Artist16,
	{IDI_2STARS, 16},		//ICO_2Stars16,
	{IDI_3STARS, 16},		//ICO_3Stars16,

	{IDI_TOP1016, 16},		//ICO_Top1016,
	{IDI_TOP10016, 16},		//ICO_Top10016,
	{IDI_TOPREST16, 16},	//ICO_TopRest16,
	{IDI_PLAYED16, 16},		//ICO_Played16,
	{IDI_MOUSECLICK16, 16},	//ICO_MouseClick16,

	{IDI_AUDIO16, 16},		//ICO_Audio16,
	{IDI_VIDEO16, 16},		//ICO_Video16,
	{IDI_VIRTUAL16, 16},	//ICO_Virtual16,
	{IDI_COMMENTS16, 16},	//ICO_Comment16,
	{IDI_LYRICS16, 16},		//ICO_Lyrics16,
	{IDI_PERSONAL16, 16},	//ICO_Personal16,
	{IDI_TABLATURE16, 16},	//ICO_Tablature16,

	{IDI_STAREMPTY16, 16},	//ICO_StarEmpty16,
	{IDI_STARGOLD16, 16},	//ICO_StarGold16,
	{IDI_STARGOLDHALF16, 16},	//ICO_StarGoldHalf16,
	{IDI_STARBADEMPTY16, 16},	//ICO_StarEmptyBad16,
	{IDI_STARBAD16, 16},		//ICO_SrarBad16,
	{IDI_MINUS16, 16},			//ICO_Minus16,
	{IDI_NEW16, 16},			//ICO_New16

	{IDI_COLLAPSED16, 16},		//ICO_Collapsed16,
	{IDI_EXPANDED16, 16},		//ICO_Expanded16,
	{IDI_LOCALFOLDER16, 16},	//ICO_LocalFolder16,
	{IDI_NETFOLDER16, 16},		//ICO_NetFolder16,
	{IDI_MEDIA16, 16},			//ICO_Media16,
	{IDI_AUDIOCD16, 16},		//ICO_AudioCD16,
	{IDI_REMOVABLE16, 16},		//ICO_Removable16,

	{IDI_PLAY16, 16},			//ICO_Play16,
	{IDI_COPY16, 16},			//ICO_Copy16,
	{IDI_DELETE16, 16},			//ICO_Delete16,
	{IDI_FIND16, 16},			//ICO_Find16,
	{IDI_PROPERTIES16, 16},		//ICO_Properties16,
	{IDI_PROVIDERS16, 16},		//ICO_Providers16,
	{IDI_BUY16, 16},			//ICO_Buy16
	{IDI_OPTIONS16, 16},		//ICO_Options16
	{IDI_WINDOW16, 16},			//ICO_Window16
	{IDI_CLEAR16, 16},			//ICO_Clear16

	{IDI_TEXT16, 16},			//ICO_Text16,
	{IDI_PICTURE16, 16},		//ICO_Picture16,

	{IDI_UP16, 16},				//ICO_Up16,
	{IDI_DOWN16, 16},			//ICO_Down16,


	{IDI_COLLECTIONS16, 16},	//ICO_Collections16,
	{IDI_HISTORY16, 16},		//ICO_History16,
	{IDI_GAMES16, 16},			//ICO_Games16,
	{IDR_MAINFRAME, 16},		//ICO_Main16
	{IDI_SWITCHPANE16, 16},		//ICO_SwitchPane16

	{IDI_MINIMIZE16, 16},		//ICO_Minimize16,
	{IDI_RESTORE16, 16},		//ICO_Restore16,
	{IDI_MAXIMIZE16, 16},		//ICO_Maximize16,
	{IDI_CLOSE16, 16},			//ICO_Close16,

	{IDI_INCREASEFONT16, 16},	//ICO_IncreaseFont16,
	{IDI_DECREASEFONT16, 16},	//ICO_DecreaseFont16,
	{IDI_RESETFONT16, 16},		//ICO_ResetFont16,

	{IDI_ALBUM16, 16},		//ICO_DecreaseFont16,
	{IDI_INSERT16, 16},		//ICO_ResetFont16,
	{IDI_REMOVE16, 16},		//ICO_ResetFont16,

	{IDI_MUTE16, 16},		//ICO_Mute16,
	{IDI_NOMUTE16, 16}		//ICO_Unmute16,

};

HICON PrgAPI::GetIcon(APP_ICONS icon)
{
	ASSERT(sizeof(AppIcon2Resource)/(2*sizeof(INT)) == ICO_Last);
	ASSERT(icon < ICO_Last);
	if (m_AppIcons[icon] == NULL)
	{
		m_AppIcons[icon] = ( HICON ) ::LoadImage( AfxGetResourceHandle(),
			MAKEINTRESOURCE(AppIcon2Resource[icon][0]),
			IMAGE_ICON,
			AppIcon2Resource[icon][1],
			AppIcon2Resource[icon][1],
			LR_VGACOLOR /*| LR_LOADTRANSPARENT */);
		ASSERT(m_AppIcons[icon] != NULL);
	}
	return m_AppIcons[icon];
}

HICON PrgAPI::GetIconForTrackType(TrackTypesEnum tt)
{
	if (tt < TTYPE_FIRSTVIDEO)
		return GetIcon(ICO_Audio16);
	else if (tt < TTYPE_FIRSTVIRTUAL)
		return GetIcon(ICO_Video16);
	return GetIcon(ICO_Virtual16);

}

HICON PrgAPI::GetIconForRank(INT rank)
{
	return GetIcon(GetAppIconForRank(rank));
}

APP_ICONS PrgAPI::GetAppIconForRank(INT rank)
{
	if (rank >=1 && rank <=10)
		return ICO_Top1016;
	else if (rank >10 && rank <=100)
		return ICO_Top10016;
	return ICO_TopRest16;
}

HICON PrgAPI::GetIconForStars(INT stars)
{
	switch (stars)
	{
	case 0:
		return NULL;
	case 1:	
		return GetIcon(ICO_StarBad16);
	default:
		return GetIcon(ICO_StarGold16);
	//case 0:
	//case 2:	
	//	return GetIcon(ICO_Minus16);
	//case 3:	return GetIcon(ICO_StarGold16);
	//case 4:	return GetIcon(ICO_2Stars16);
	//case 5:	return GetIcon(ICO_3Stars16);
	}
	ASSERT(0);
	return AfxGetApp()->LoadStandardIcon(IDI_QUESTION);

}

HICON PrgAPI::GetIconForCollectionType(CollectionTypesEnum colType)
{
	APP_ICONS icon = GetAppIconForCollectionType(colType);
	if (icon != ICO_Last)
		return GetIcon(icon);
	ASSERT(0);
	return NULL;
}

HICON PrgAPI::GetIconForInfoItemType(InfoItemTypeEnum iit)
{
	switch (iit)
	{
	case IIT_AlbumPicture:
	case IIT_ArtistPicture:
		return GetIcon(ICO_Picture16);
	case IIT_AlbumReview:
	case IIT_ArtistBio:
		return GetIcon(ICO_Text16);
	case IIT_TrackLyrics:	
		return GetIcon(ICO_Lyrics16);
	case IIT_TrackPersonal:
		return GetIcon(ICO_Personal16);
	case IIT_TrackComment:
		return GetIcon(ICO_Comment16);
	case IIT_TrackTablatures:
		return GetIcon(ICO_Tablature16);
	}
	return 0;
}



CImageList& PrgAPI::GetImageList16()
{
	if (m_il16.m_hImageList == 0)
	{
		m_il16.Create(16,16,ILC_MASK|ILC_COLOR32,5,1);
		for (int i=0; i< ICO_Last; i++)
			m_il16.Add(GetIcon((APP_ICONS)i));
	}
	return m_il16;
}

void PrgAPI::SetGlobalButtonStyle(FlexButton& btn, APP_ICONS ico)
{
	btn.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	if (ico < ICO_Last)
		btn.SetIcon(GetIcon(ico), 16, 16);
	btn.SetTextStyle(FlexButton::TS_SingleLineCenter);
}


APP_ICONS PrgAPI::GetAppIconForCollectionType(CollectionTypesEnum colType)
{
	switch (colType)
	{
	case CTYPE_LocalFolder:		return ICO_LocalFolder16;
	case CTYPE_NetFolder:		return ICO_NetFolder16;
	case CTYPE_Media:			return ICO_Media16;
	case CTYPE_AudioCD:			return ICO_AudioCD16;
	case CTYPE_Virtual:			return ICO_Virtual16;
	case CTYPE_Removable:		return ICO_Removable16;
	}
	ASSERT(0);
	return ICO_Last;

}



SkinManager* PrgAPI::GetSkinManager()
{
	if (m_pSkinManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetSkinManager [CREATION]"));
		m_pSkinManager = new SkinManager;
		m_pSkinManager->SetSkinsFolder(GetPath(PATH_SkinRoot));
		std::tstring SkinName;
		GetAppSettings()->Read(cGeneral, cOptionSkin, SkinName, _T(""));
		if (!SkinName.empty())
		{
			if (m_pSkinManager->LoadSkin(SkinName.c_str()) == FALSE)
			{
				TRACE(_T("PrgAPI::GetSkinManager Skin not loaded\r\n"));
			}
		}
		if (m_pSkinManager->GetSkin().GetSkinPath()[0] == 0)
			m_pSkinManager->LoadSkin(_T("Default"));
		m_pSkinManager->GetSkin().SetFontQuality(GetOption(OPT_GEN_ForceClearTypeFonts) == 1 ? CLEARTYPE_QUALITY : DEFAULT_QUALITY);
		m_pSkinManager->GetSkin().SetRelativeFontSize(GetOption(OPT_GEN_RelativeFontSize));
	}
	return m_pSkinManager;
}

LPCTSTR PrgAPI::GetPath(PathsEnum pathType, LPTSTR buffer/* = NULL*/)
{
	if (buffer == NULL)
		buffer = m_tempPath;
	switch (pathType)
	{
	case PATH_AppRoot:
		{
			::GetModuleFileName(0, buffer, MAX_PATH);
			LPTSTR pszFileName = _tcsrchr(buffer, '\\');
			pszFileName[1] = 0;
		}
		break;
	case PATH_SkinRoot:
		{
			TCHAR appRoot[MAX_PATH];
			_sntprintf(buffer, MAX_PATH, _T("%sSkins\\"), GetPath(PATH_AppRoot, appRoot));
			if (!PathFileExists(buffer))
				CreateDirectory(buffer, NULL);
		}
		break;
	case PATH_TranslationsRoot:
		{
			TCHAR appRoot[MAX_PATH];
			_sntprintf(buffer, MAX_PATH, _T("%sLangs\\"), GetPath(PATH_AppRoot, appRoot));
			if (!PathFileExists(buffer))
				CreateDirectory(buffer, NULL);
		}
		break;
	case PATH_WebServicesRoot:
		TCHAR appRoot[MAX_PATH];
		_sntprintf(buffer, MAX_PATH, _T("%sWebServices\\"), GetPath(PATH_AppRoot, appRoot));
		if (!PathFileExists(buffer))
			CreateDirectory(buffer, NULL);
		break;
	case PATH_StorageRoot:
	case PATH_UserRoot:
	case PATH_DatabaseRoot:
		{
			LPCTSTR iniEntry = NULL;
			LPCTSTR defFolder = NULL;
			switch (pathType)
			{
			case PATH_StorageRoot:
				iniEntry = _T("StoragePath");
				defFolder = _T("Storage\\");
				break;
			case PATH_UserRoot:
				iniEntry = _T("UserPath");
				defFolder = _T("User\\");
				break;
			case PATH_DatabaseRoot:
				iniEntry = _T("DatabasePath");
				defFolder = _T("");
				break;
			default:
				ASSERT(0);
				return NULL;
			}
			BOOL bUseDefaultPath = TRUE;
			std::tstring path;
			if (GetAppSettings()->Read(cGeneral, iniEntry, path, _T("")) && path.size() > 0)
			{
				if (path[path.size() - 1] != '\\')
					path.push_back('\\');
				_tcscpy(buffer, path.c_str());
				if (CreateDirectory(path.c_str(), NULL))
					bUseDefaultPath = FALSE;
				else
				{
					INT le = GetLastError();
					if (le == ERROR_ALREADY_EXISTS)
						bUseDefaultPath = FALSE;
				}
			}
			if (bUseDefaultPath)
			{
				//=== Default Path
				TCHAR appRoot[MAX_PATH];
				_sntprintf(buffer, MAX_PATH, _T("%s%s"), GetPath(PATH_AppRoot, appRoot), defFolder);
				if (!PathFileExists(buffer))
					CreateDirectory(buffer, NULL);
			}
		}
		break;
	case PATH_LastM3UFile:
		{
			TCHAR userRoot[MAX_PATH];
			_sntprintf(buffer, MAX_PATH, _T("%slasttracks.m3u"), GetPath(PATH_UserRoot, userRoot));
		}
		break;
	case PATH_ServicesXMLFile:
		{
			TCHAR appRoot[MAX_PATH];
			_sntprintf(buffer, MAX_PATH, _T("%sservices.xml"), GetPath(PATH_AppRoot, appRoot));
		}
		break;
	case PATH_ServicesUserXMLFile:
		{
			TCHAR appRoot[MAX_PATH];
			_sntprintf(buffer, MAX_PATH, _T("%sservices.xml"), GetPath(PATH_UserRoot, appRoot));
		}
		break;
	default:
		ASSERT(0);
		return NULL;
	}
	return buffer;
}





#include "URLEncDecHelper.h"

BOOL PrgAPI::GetURLSystemID(std::string& systemID)
{
	TRACEST(_T("PrgAPI::GetURLSystemID"));
	CHAR sysID[500];
	DWORD volSerial = 0;
	GetWindowsDirectoryA(sysID, MAX_PATH);
	sysID[3] = 0;
	BOOL ret = GetVolumeInformationA(sysID, NULL, 0, &volSerial, 0,0,0,0);

	MEMORYSTATUSEX memStat;
	memStat.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memStat);

	DWORD cpuFreq = 0;
	DWORD dwBufLen = sizeof(DWORD);
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		_T("Hardware\\Description\\System\\CentralProcessor\\0"), 
		0, 
		KEY_QUERY_VALUE, 
		&hKey) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx( hKey, _T("~MHz"), NULL, NULL,
			(LPBYTE) &cpuFreq, &dwBufLen) != ERROR_SUCCESS)
			cpuFreq = 0;
		RegCloseKey(hKey);
	}

	SQLManager* pSM = GetSQLManager();
	SQLManager::HistoryLogStats logStats;
	pSM->GetHistoryLogStats(logStats);
	DOUBLE days = 0.0;
	if (logStats.firstTime.wYear > 0)
		SystemTimeToVariantTime(&logStats.firstTime, &days);
	UINT histFirstDateInDays = (UINT) days;

	AppMonitor::State& appMonitorState = GetAppMonitor()->GetState();
	SYSTEMTIME installDate = appMonitorState.InstallationDate;
	SystemTimeToVariantTime(&installDate, &days);
	UINT installedDay = (UINT) days;
#ifdef _DEBUG
	//Todays time
	GetLocalTime(&installDate);
	SystemTimeToVariantTime(&installDate, &days);
#endif

	INT len = _snprintf(sysID, 500, "%X:%X:%X:%X:%X:%X:%X:%X:%X:%X:%X:%X:%X:%X:%X:%X", 
		//===AppMonitor Stats (7)
		appMonitorState.AppUniqueID,
		appMonitorState.Runs,
		appMonitorState.TotalRuns,
		appMonitorState.Crashes,
		appMonitorState.TotalCrashes,
		installedDay,
		GetAppMonitor()->GetRunningTimeInMinutes(),
		//===System Stats (4)
		GetVersion(),
		GetSystemDefaultLangID(),
		(UINT)(memStat.ullTotalPhys/(1024.0*1024.0)),
		cpuFreq,
		//===DB Stats (3)
		pSM->GetTrackCount(TracksFilter()), 
		logStats.plays,
		histFirstDateInDays,
		//===Other (1)
		volSerial,
		BUILD_VERSION
		);

	CHAR* base64Data = URLEncDecHelper::EncodeData((LPBYTE)sysID, len, TRUE, FALSE);
	if (base64Data != NULL)
	{
		systemID = "8:";
		systemID.append(base64Data, len);
		delete[] base64Data;
		return TRUE;
	}
	return FALSE;
}



BOOL PrgAPI::IsTrackNew(SYSTEMTIME& dateAdded, UINT DaysMeansNew/* = 0*/)
{
	if (DaysMeansNew == 0)
		DaysMeansNew = m_Options[OPT_GEN_DaysMeansNew];
	if (DaysMeansNew == 0)
		return FALSE;
	SYSTEMTIME dateCur;
	GetLocalTime(&dateCur);
	DOUBLE dAdded, dCur;
	SystemTimeToVariantTime(&dateAdded, &dAdded);
	SystemTimeToVariantTime(&dateCur, &dCur);
	return (dCur - dAdded < m_Options[OPT_GEN_DaysMeansNew]);
}

BOOL PrgAPI::GetSiteURL(LPTSTR url, UINT urlLen, SiteActionEnum actID, LPCTSTR p1, LPCTSTR p2)
{
	TRACEST(_T("PrgAPI::GetSiteURL"));
	ASSERT(actID < SA_Last);
	if (p1 == NULL)
		p1 = _T("");
	if (p2 == NULL)
		p2 = _T("");
	LPCTSTR action = NULL;
	switch (actID)
	{
	case SA_Help:
		action=_T("hlp");
		break;
	case SA_Navigate:
		action=_T("nav");
		break;
	default:
		return FALSE;
	}
	FileVersion fv;
	GetFileVersion(fv);
	int len = _sntprintf(url, urlLen, 
		_T("http://services.artificialspirit.com/jaangle-fw.php?ver=%d&uid=%u&lng=%d&act=%s&p1=%s&p2=%s"), 
		fv.Build, 
		GetAppMonitor()->GetState().AppUniqueID,
		GetSystemDefaultLangID(), 
		action,
		p1, 
		p2
		);
	url[urlLen - 1] = 0; 
	return TRUE;
}

BOOL PrgAPI::GoToSiteURL(SiteActionEnum actID, LPCTSTR p1, LPCTSTR p2)
{
	TRACEST(_T("PrgAPI::GoToSiteURL"));
	TCHAR url[1000];
	if (!GetSiteURL(url, 1000, actID, p1, p2))
		return FALSE;
	ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
	return TRUE;
}

MenuManager* PrgAPI::GetMenuManager()
{
	if (m_pMenuManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetMenuManager [CREATION]"));
		m_pMenuManager = new MenuManager;
	}
	return m_pMenuManager;
}

TranslationManager* PrgAPI::GetTranslationManager()
{
	if (m_pTranslationManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetTranslationManager [CREATION]"));
		m_pTranslationManager = new TranslationManager();
		m_pTranslationManager->SetTranslationsFolder(GetPath(PATH_TranslationsRoot));
	}
	return m_pTranslationManager;
}

CollectionManager* PrgAPI::GetCollectionManager()
{
	if (m_pCollectionManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetCollectionManager [CREATION]"));
		m_pCollectionManager = new CollectionManager;
	}
	return m_pCollectionManager;
}

InfoProviderFactory* PrgAPI::GetInfoProviderFactory()
{
	if (m_pInfoProviderFactory == NULL)
	{
		TRACEST(_T("PrgAPI::GetInfoProviderFactory [CREATION]"));
		m_pInfoProviderFactory = new InfoProviderFactory;
		m_pInfoProviderFactory->Init(GetInternetHandle(), GetAppSettings());
		//m_pInfoProviderFactory->LoadPlugins(GetPath(PATH_InfoProvidersRoot));

	}
	return m_pInfoProviderFactory;
}
HotKeyManager* PrgAPI::GetHotKeyManager()
{
	if (m_pHotKeyManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetHotKeyManager [CREATION]"));
		m_pHotKeyManager = new HotKeyManager;
	}
	return m_pHotKeyManager;
}

TrayToolTipDlg* PrgAPI::GetTrayToolTipDlg()
{
	if (m_pTrayToolTipDlg == NULL)
	{
		TRACEST(_T("PrgAPI::TrayToolTipDlg [CREATION]"));
		m_pTrayToolTipDlg = new TrayToolTipDlg;
	}
	return m_pTrayToolTipDlg;
}

void PrgAPI::DestroyTrayToolTipDlg()
{
	if (m_pTrayToolTipDlg != NULL)
	{
		if (m_pTrayToolTipDlg->GetSafeHwnd())
			m_pTrayToolTipDlg->DestroyWindow();
		delete m_pTrayToolTipDlg;
		m_pTrayToolTipDlg = NULL;
	}
}

CMiniPlayerDlg* PrgAPI::GetMiniPlayerDlg(BOOL bForceCreate)
{
	if (m_pMiniPlayerDlg == NULL && bForceCreate)
	{
		TRACEST(_T("PrgAPI::GetMiniPlayerDlg [CREATION]"));
		m_pMiniPlayerDlg = new CMiniPlayerDlg;
		m_pMiniPlayerDlg->Create(CWnd::GetDesktopWindow());
		m_pMiniPlayerDlg->LoadOptions();
	}
	return m_pMiniPlayerDlg;
}

void PrgAPI::DestroyMiniPlayerDlg()
{
	if (m_pMiniPlayerDlg != NULL)
	{
		m_pMiniPlayerDlg->SaveOptions();
		if (m_pMiniPlayerDlg->GetSafeHwnd())
			m_pMiniPlayerDlg->DestroyWindow();
		delete m_pMiniPlayerDlg;
		m_pMiniPlayerDlg = NULL;
	}
}


QuickSearchDlg* PrgAPI::GetQuickSearchDlg()
{
	if (m_pQuickSearchDlg == NULL)
	{
		TRACEST(_T("PrgAPI::GetQuickSearchDlg [CREATION]"));
		m_pQuickSearchDlg = new QuickSearchDlg;
	}
	return m_pQuickSearchDlg;
}

void PrgAPI::DestroyQuickSearchDlg()
{
	if (m_pQuickSearchDlg != NULL)
	{
		if (m_pQuickSearchDlg->GetSafeHwnd())
			m_pQuickSearchDlg->DestroyWindow();
		delete m_pQuickSearchDlg;
		m_pQuickSearchDlg = NULL;
	}
}

CSearchDlg* PrgAPI::GetAdvancedSearchDlg()
{
	if (m_pSearchDlg == NULL)
	{
		TRACEST(_T("PrgAPI::GetAdvancedSearchDlg [CREATION]"));
		m_pSearchDlg = new CSearchDlg;
		m_pSearchDlg->Create(IDD_SEARCH, AfxGetMainWnd());
		m_pSearchDlg->SetWindowText(PRGAPI()->GetString(IDS_SEARCH));
		m_pSearchDlg->CenterWindow( AfxGetMainWnd());
	}				
	return m_pSearchDlg;

}
void PrgAPI::DestroyAdvancedSearchDlg()
{
	if (m_pSearchDlg != NULL)
	{
		if (m_pSearchDlg->GetSafeHwnd())
			m_pSearchDlg->DestroyWindow();
		delete m_pSearchDlg;
		m_pSearchDlg = NULL;
	}

}



ShortcutManager* PrgAPI::GetShortcutManager()
{
	if (m_pShortcutManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetShortcutManager [CREATION]"));
		m_pShortcutManager = new ShortcutManager;
	}
	return m_pShortcutManager;
}

ConfigurationManager* PrgAPI::GetConfigurationManager()
{
	if (m_pConfigurationManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetConfigurationManager [CREATION]"));
		m_pConfigurationManager = new ConfigurationManager;
	}
	return m_pConfigurationManager;
}

StateManager* PrgAPI::GetStateManager()
{
	if (m_pStateManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetStateManager [CREATION]"));
		m_pStateManager = new StateManager;
	}
	return m_pStateManager;
}



SQLManager* PrgAPI::GetSQLManager()
{
	if (m_pSQLManager == NULL)
	{
		TRACEST(_T("PrgAPI::GetSQLManager [CREATION]"));
		m_pSQLManager = new SQLManager;
		std::tstring path(GetPath(PATH_DatabaseRoot));
		path += _T("music.mdb");
		if (!m_pSQLManager->Init(path.c_str()))
		{
			delete m_pSQLManager;
			m_pSQLManager = 0;
			TRACE(_T("@0{Database Engine Failed to be created}\n"));
		}

	}

	return m_pSQLManager;
}

LastFMServices* PrgAPI::GetLastFMServices()
{
	if (m_pLastFMServices == NULL)
	{
		TRACEST(_T("PrgAPI::GetLastFMServices"));
		m_pLastFMServices = new LastFMServices();
		m_pLastFMServices->Init(GetInternetHandle());

		AppSettings* pSettings = GetAppSettings();
		std::tstring val;
		pSettings->Read(cLastFM, cOptionUser, val, _T(""));
		m_pLastFMServices->SetUserName(val.c_str());
		pSettings->Read(cLastFM, cOptionPass, val, _T(""));
		m_pLastFMServices->SetMD5Password(val.c_str());
	}
	return m_pLastFMServices;
}

ITracker* PrgAPI::GetTracker()
{
	if (m_pTracker == NULL)
	{
		TRACEST(_T("PrgAPI::GetTracker [CREATION]"));
		m_pTracker= new MultiTracker();
	}
	return m_pTracker;
}

TSOnlineServices* PrgAPI::GetTSOnlineServices()
{
	if (m_pTSOnlineServices == NULL)
	{
		TRACEST(_T("PrgAPI::GetTSOnlineServices"));
		std::string str;
		GetURLSystemID(str);
		m_pTSOnlineServices = new TSOnlineServices(str.c_str(), GetInternetHandle());
	}
	return m_pTSOnlineServices;

}

void PrgAPI::ClearFonts()
{
	for (int i=0 ; i<FONT_Last; i++)
	{
		delete m_pAppFonts[i];
		m_pAppFonts[i] = NULL;
	}
}

void PrgAPI::SendMessage(UINT stateMessage)
{
	GetStateManager()->SendMessage(stateMessage);
}
void PrgAPI::PostMessage(UINT stateMessage)
{
	GetStateManager()->PostMessage(stateMessage);
}

ITSMenu* PrgAPI::CreatePopupMenu()
{
	return new TSMenu;
}



UINT Stars2Rating(FLOAT Stars)
{
	ASSERT(Stars >= 0.0f && Stars <= 5.0f); //should be 1 - 5
	return UINT(INT(Stars*100+.5) * 50 / 100);

}

//Returns:
//0 for 0 (No Rating)
//1 for (1-50) Bad
//2 for (51-100) Neutral
//3 for (101-150) 1 Star ...
//4 for (151-200) 2 Star ...
//5 for (201-) 3 Star ...
FLOAT Rating2Stars(UINT Rating)
{	
	FLOAT stars = Rating / 50.0f;
	if (stars > 5.0f)
		stars = 5.0f;
	return stars;
}

BOOL CALLBACK SetFontToChilds(HWND hwnd, LPARAM lParam)
{
	ASSERT(hwnd != NULL);
	ASSERT(lParam != NULL);
	if (lParam == NULL)
		return FALSE;
	SendMessage(hwnd, WM_SETFONT, (WPARAM)lParam, FALSE);
	return TRUE;
}










