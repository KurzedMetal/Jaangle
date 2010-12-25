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
#include "BassInstance.h"
#include "cStringUtils.h"

#include <bass.h>
#include <bass_fx.h>
#ifndef DYN_LINK_BASS
#pragma comment(lib,"bass.lib")
#pragma comment(lib,"bass_fx.lib")
#else
#define LOADBASSFUNCTION(f) *((void**)&f)=GetProcAddress(hBass,#f)
DWORD BASSDEF(BASS_SetConfig)(DWORD option, DWORD value);
DWORD BASSDEF(BASS_GetConfig)(DWORD option);
DWORD BASSDEF(BASS_GetVersion)();
const char *BASSDEF(BASS_GetDeviceDescription)(DWORD device);
int BASSDEF(BASS_ErrorGetCode)();
BOOL BASSDEF(BASS_Init)(int device, DWORD freq, DWORD flags, HWND win, const GUID *dsguid);
BOOL BASSDEF(BASS_SetDevice)(DWORD device);
DWORD BASSDEF(BASS_GetDevice)();
BOOL BASSDEF(BASS_Free)();
void *BASSDEF(BASS_GetDSoundObject)(DWORD object);
BOOL BASSDEF(BASS_GetInfo)(BASS_INFO *info);
BOOL BASSDEF(BASS_Update)();
float BASSDEF(BASS_GetCPU)();
BOOL BASSDEF(BASS_Start)();
BOOL BASSDEF(BASS_Stop)();
BOOL BASSDEF(BASS_Pause)();
BOOL BASSDEF(BASS_SetVolume)(DWORD volume);
DWORD BASSDEF(BASS_GetVolume)();

HPLUGIN BASSDEF(BASS_PluginLoad)(const char *file, DWORD flags);
BOOL BASSDEF(BASS_PluginFree)(HPLUGIN handle);
const BASS_PLUGININFO *BASSDEF(BASS_PluginGetInfo)(HPLUGIN handle);

BOOL BASSDEF(BASS_Set3DFactors)(float distf, float rollf, float doppf);
BOOL BASSDEF(BASS_Get3DFactors)(float *distf, float *rollf, float *doppf);
BOOL BASSDEF(BASS_Set3DPosition)(const BASS_3DVECTOR *pos, const BASS_3DVECTOR *vel, const BASS_3DVECTOR *front, const BASS_3DVECTOR *top);
BOOL BASSDEF(BASS_Get3DPosition)(BASS_3DVECTOR *pos, BASS_3DVECTOR *vel, BASS_3DVECTOR *front, BASS_3DVECTOR *top);
void BASSDEF(BASS_Apply3D)();
BOOL BASSDEF(BASS_SetEAXParameters)(int env, float vol, float decay, float damp);
BOOL BASSDEF(BASS_GetEAXParameters)(DWORD *env, float *vol, float *decay, float *damp);

HMUSIC BASSDEF(BASS_MusicLoad)(BOOL mem, const void *file, DWORD offset, DWORD length, DWORD flags, DWORD freq);
BOOL BASSDEF(BASS_MusicFree)(HMUSIC handle);
DWORD BASSDEF(BASS_MusicSetAttribute)(HMUSIC handle, DWORD attrib, DWORD value);
DWORD BASSDEF(BASS_MusicGetAttribute)(HMUSIC handle, DWORD attrib);
DWORD BASSDEF(BASS_MusicGetOrders)(HMUSIC handle);
DWORD BASSDEF(BASS_MusicGetOrderPosition)(HMUSIC handle);

HSAMPLE BASSDEF(BASS_SampleLoad)(BOOL mem, const void *file, DWORD offset, DWORD length, DWORD max, DWORD flags);
void* BASSDEF(BASS_SampleCreate)(DWORD length, DWORD freq, DWORD chans, DWORD max, DWORD flags);
HSAMPLE BASSDEF(BASS_SampleCreateDone)();
BOOL BASSDEF(BASS_SampleFree)(HSAMPLE handle);
BOOL BASSDEF(BASS_SampleGetInfo)(HSAMPLE handle, BASS_SAMPLE *info);
BOOL BASSDEF(BASS_SampleSetInfo)(HSAMPLE handle, const BASS_SAMPLE *info);
HCHANNEL BASSDEF(BASS_SampleGetChannel)(HSAMPLE handle, BOOL onlynew);
BOOL BASSDEF(BASS_SampleStop)(HSAMPLE handle);

HSTREAM BASSDEF(BASS_StreamCreate)(DWORD freq, DWORD chans, DWORD flags, STREAMPROC *proc, DWORD user);
HSTREAM BASSDEF(BASS_StreamCreateFile)(BOOL mem, const void *file, DWORD offset, DWORD length, DWORD flags);
HSTREAM BASSDEF(BASS_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, DWORD user);
HSTREAM BASSDEF(BASS_StreamCreateFileUser)(BOOL buffered, DWORD flags, STREAMFILEPROC *proc, DWORD user);
BOOL BASSDEF(BASS_StreamFree)(HSTREAM handle);
DWORD BASSDEF(BASS_StreamGetFilePosition)(HSTREAM handle, DWORD mode);

const char *BASSDEF(BASS_RecordGetDeviceDescription)(DWORD device);
BOOL BASSDEF(BASS_RecordInit)(int device);
BOOL BASSDEF(BASS_RecordSetDevice)(DWORD device);
DWORD BASSDEF(BASS_RecordGetDevice)();
BOOL BASSDEF(BASS_RecordFree)();
BOOL BASSDEF(BASS_RecordGetInfo)(BASS_RECORDINFO *info);
const char *BASSDEF(BASS_RecordGetInputName)(int input);
BOOL BASSDEF(BASS_RecordSetInput)(int input, DWORD setting);
DWORD BASSDEF(BASS_RecordGetInput)(int input);
HRECORD BASSDEF(BASS_RecordStart)(DWORD freq, DWORD chans, DWORD flags, RECORDPROC *proc, DWORD user);

float BASSDEF(BASS_ChannelBytes2Seconds)(DWORD handle, QWORD pos);
QWORD BASSDEF(BASS_ChannelSeconds2Bytes)(DWORD handle, float pos);
DWORD BASSDEF(BASS_ChannelGetDevice)(DWORD handle);
BOOL BASSDEF(BASS_ChannelSetDevice)(DWORD handle, DWORD device);
DWORD BASSDEF(BASS_ChannelIsActive)(DWORD handle);
BOOL BASSDEF(BASS_ChannelGetInfo)(DWORD handle, BASS_CHANNELINFO *info);
const char *BASSDEF(BASS_ChannelGetTags)(DWORD handle, DWORD tags);
BOOL BASSDEF(BASS_ChannelSetFlags)(DWORD handle, DWORD flags);
BOOL BASSDEF(BASS_ChannelPreBuf)(DWORD handle, DWORD length);
BOOL BASSDEF(BASS_ChannelPlay)(DWORD handle, BOOL restart);
BOOL BASSDEF(BASS_ChannelStop)(DWORD handle);
BOOL BASSDEF(BASS_ChannelPause)(DWORD handle);
BOOL BASSDEF(BASS_ChannelSetAttributes)(DWORD handle, int freq, int volume, int pan);
BOOL BASSDEF(BASS_ChannelGetAttributes)(DWORD handle, DWORD *freq, DWORD *volume, int *pan);
BOOL BASSDEF(BASS_ChannelSlideAttributes)(DWORD handle, int freq, int volume, int pan, DWORD time);
DWORD BASSDEF(BASS_ChannelIsSliding)(DWORD handle);
BOOL BASSDEF(BASS_ChannelSet3DAttributes)(DWORD handle, int mode, float min, float max, int iangle, int oangle, int outvol);
BOOL BASSDEF(BASS_ChannelGet3DAttributes)(DWORD handle, DWORD *mode, float *min, float *max, DWORD *iangle, DWORD *oangle, DWORD *outvol);
BOOL BASSDEF(BASS_ChannelSet3DPosition)(DWORD handle, const BASS_3DVECTOR *pos, const BASS_3DVECTOR *orient, const BASS_3DVECTOR *vel);
BOOL BASSDEF(BASS_ChannelGet3DPosition)(DWORD handle, BASS_3DVECTOR *pos, BASS_3DVECTOR *orient, BASS_3DVECTOR *vel);
QWORD BASSDEF(BASS_ChannelGetLength)(DWORD handle);
BOOL BASSDEF(BASS_ChannelSetPosition)(DWORD handle, QWORD pos);
QWORD BASSDEF(BASS_ChannelGetPosition)(DWORD handle);
DWORD BASSDEF(BASS_ChannelGetLevel)(DWORD handle);
DWORD BASSDEF(BASS_ChannelGetData)(DWORD handle, void *buffer, DWORD length);
HSYNC BASSDEF(BASS_ChannelSetSync)(DWORD handle, DWORD type, QWORD param, SYNCPROC *proc, DWORD user);
BOOL BASSDEF(BASS_ChannelRemoveSync)(DWORD handle, HSYNC sync);
HDSP BASSDEF(BASS_ChannelSetDSP)(DWORD handle, DSPPROC *proc, DWORD user, int priority);
BOOL BASSDEF(BASS_ChannelRemoveDSP)(DWORD handle, HDSP dsp);
BOOL BASSDEF(BASS_ChannelSetLink)(DWORD handle, DWORD chan);
BOOL BASSDEF(BASS_ChannelRemoveLink)(DWORD handle, DWORD chan);
BOOL BASSDEF(BASS_ChannelSetEAXMix)(DWORD handle, float mix);
BOOL BASSDEF(BASS_ChannelGetEAXMix)(DWORD handle, float *mix);
HFX BASSDEF(BASS_ChannelSetFX)(DWORD handle, DWORD type, DWORD priority);
BOOL BASSDEF(BASS_ChannelRemoveFX)(DWORD handle, HFX fx);

BOOL BASSDEF(BASS_FXSetParameters)(HFX handle, const void *par);
BOOL BASSDEF(BASS_FXGetParameters)(HFX handle, void *par);
#endif
INT BassInstance::m_bassInstances = 0;
std::tstring BassInstance::m_sFormats(
		_T(".mp3.mp2.mp1.oggaiff.xm .it .s3m.mod.mtm.umx.mo3.wav")
		_T(".wma")
		_T(".mpc")
		_T(".cda")
		_T("flac")
		_T(".midmidi")
		_T(".ac3")
		_T(".mp4.aac.m4a")
		_T(".ape")
		_T(".wv ")
		);
std::vector<HPLUGIN> BassInstance::m_Plugins;


BOOL BassInstance::bUseFloatDSP = TRUE;
BOOL BassInstance::bUseSampleFloat = TRUE;

BassInstance::BassInstance()
#ifdef DYN_LINK_BASS
:hBass(0)
#endif
{
	if (m_bassInstances == 0)
	{
		if (InitBass())
			m_bassInstances++;
	}
	else
		m_bassInstances++;
	TRACE(_T("@3 BassInstance::BassInstance: Instances: %d\r\n"), m_bassInstances);

}



BassInstance::~BassInstance()
{
	if (m_bassInstances > 0)
	{
		m_bassInstances--;
		if (m_bassInstances == 0)
		{
			TRACE(_T("@2 BassInstance Freeing Bass\r\n"));
			BASS_Free();
		}
	}
	TRACE(_T("@3 BassInstance(s) (DESTR): %d\r\n"), m_bassInstances);

#ifdef DYN_LINK_BASS
	if (hBass)
		FreeLibrary(hBass);
#endif
}

BOOL BassInstance::InitBass()
{
	TRACE(_T("@2 BassInstance::InitBass."));
	ASSERT(m_bassInstances == 0);
	if (m_bassInstances == 0)
	{
#ifdef DYN_LINK_BASS
		TCHAR bf[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, bf);
		_tcscat(bf, _T("\\bass.dll"));
		hBass = LoadLibrary(bf);
		if (hBass == 0) 
		{
			return FALSE;
		}
		//*((void**)&BASS_SetConfig)=GetProcAddress(hBass,"BASS_SetConfig");
		LOADBASSFUNCTION(BASS_SetConfig);
		LOADBASSFUNCTION(BASS_GetConfig);
		LOADBASSFUNCTION(BASS_GetVersion);
		LOADBASSFUNCTION(BASS_GetDeviceDescription);
		LOADBASSFUNCTION(BASS_ErrorGetCode);
		LOADBASSFUNCTION(BASS_Init);
		LOADBASSFUNCTION(BASS_SetDevice);
		LOADBASSFUNCTION(BASS_GetDevice);
		LOADBASSFUNCTION(BASS_Free);
		LOADBASSFUNCTION(BASS_GetDSoundObject);
		LOADBASSFUNCTION(BASS_GetInfo);
		LOADBASSFUNCTION(BASS_Update);
		LOADBASSFUNCTION(BASS_GetCPU);
		LOADBASSFUNCTION(BASS_Start);
		LOADBASSFUNCTION(BASS_Stop);
		LOADBASSFUNCTION(BASS_Pause);
		LOADBASSFUNCTION(BASS_SetVolume);
		LOADBASSFUNCTION(BASS_GetVolume);
		LOADBASSFUNCTION(BASS_PluginLoad);
		LOADBASSFUNCTION(BASS_PluginFree);
		LOADBASSFUNCTION(BASS_PluginGetInfo);
		LOADBASSFUNCTION(BASS_Set3DFactors);
		LOADBASSFUNCTION(BASS_Get3DFactors);
		LOADBASSFUNCTION(BASS_Set3DPosition);
		LOADBASSFUNCTION(BASS_Get3DPosition);
		LOADBASSFUNCTION(BASS_Apply3D);
		LOADBASSFUNCTION(BASS_SetEAXParameters);
		LOADBASSFUNCTION(BASS_GetEAXParameters);
		LOADBASSFUNCTION(BASS_MusicLoad);
		LOADBASSFUNCTION(BASS_MusicFree);
		LOADBASSFUNCTION(BASS_MusicSetAttribute);
		LOADBASSFUNCTION(BASS_MusicGetAttribute);
		LOADBASSFUNCTION(BASS_MusicGetOrders);
		LOADBASSFUNCTION(BASS_MusicGetOrderPosition);
		LOADBASSFUNCTION(BASS_SampleLoad);
		LOADBASSFUNCTION(BASS_SampleCreate);
		LOADBASSFUNCTION(BASS_SampleCreateDone);
		LOADBASSFUNCTION(BASS_SampleFree);
		LOADBASSFUNCTION(BASS_SampleGetInfo);
		LOADBASSFUNCTION(BASS_SampleSetInfo);
		LOADBASSFUNCTION(BASS_SampleGetChannel);
		LOADBASSFUNCTION(BASS_SampleStop);
		LOADBASSFUNCTION(BASS_StreamCreate);
		LOADBASSFUNCTION(BASS_StreamCreateFile);
		LOADBASSFUNCTION(BASS_StreamCreateURL);
		LOADBASSFUNCTION(BASS_StreamCreateFileUser);
		LOADBASSFUNCTION(BASS_StreamFree);
		LOADBASSFUNCTION(BASS_StreamGetFilePosition);
		LOADBASSFUNCTION(BASS_RecordGetDeviceDescription);
		LOADBASSFUNCTION(BASS_RecordInit);
		LOADBASSFUNCTION(BASS_RecordSetDevice);
		LOADBASSFUNCTION(BASS_RecordGetDevice);
		LOADBASSFUNCTION(BASS_RecordFree);
		LOADBASSFUNCTION(BASS_RecordGetInfo);
		LOADBASSFUNCTION(BASS_RecordGetInputName);
		LOADBASSFUNCTION(BASS_RecordSetInput);
		LOADBASSFUNCTION(BASS_RecordGetInput);
		LOADBASSFUNCTION(BASS_RecordStart);
		LOADBASSFUNCTION(BASS_ChannelBytes2Seconds);
		LOADBASSFUNCTION(BASS_ChannelSeconds2Bytes);
		LOADBASSFUNCTION(BASS_ChannelGetDevice);
		LOADBASSFUNCTION(BASS_ChannelSetDevice);
		LOADBASSFUNCTION(BASS_ChannelIsActive);
		LOADBASSFUNCTION(BASS_ChannelGetInfo);
		LOADBASSFUNCTION(BASS_ChannelGetTags);
		LOADBASSFUNCTION(BASS_ChannelSetFlags);
		LOADBASSFUNCTION(BASS_ChannelPreBuf);
		LOADBASSFUNCTION(BASS_ChannelPlay);
		LOADBASSFUNCTION(BASS_ChannelStop);
		LOADBASSFUNCTION(BASS_ChannelPause);
		LOADBASSFUNCTION(BASS_ChannelSetAttributes);
		LOADBASSFUNCTION(BASS_ChannelGetAttributes);
		LOADBASSFUNCTION(BASS_ChannelSlideAttributes);
		LOADBASSFUNCTION(BASS_ChannelIsSliding);
		LOADBASSFUNCTION(BASS_ChannelSet3DAttributes);
		LOADBASSFUNCTION(BASS_ChannelGet3DAttributes);
		LOADBASSFUNCTION(BASS_ChannelSet3DPosition);
		LOADBASSFUNCTION(BASS_ChannelGet3DPosition);
		LOADBASSFUNCTION(BASS_ChannelGetLength);
		LOADBASSFUNCTION(BASS_ChannelSetPosition);
		LOADBASSFUNCTION(BASS_ChannelGetPosition);
		LOADBASSFUNCTION(BASS_ChannelGetLevel);
		LOADBASSFUNCTION(BASS_ChannelGetData);
		LOADBASSFUNCTION(BASS_ChannelSetSync);
		LOADBASSFUNCTION(BASS_ChannelRemoveSync);
		LOADBASSFUNCTION(BASS_ChannelSetDSP);
		LOADBASSFUNCTION(BASS_ChannelRemoveDSP);
		LOADBASSFUNCTION(BASS_ChannelSetLink);
		LOADBASSFUNCTION(BASS_ChannelRemoveLink);
		LOADBASSFUNCTION(BASS_ChannelSetEAXMix);
		LOADBASSFUNCTION(BASS_ChannelGetEAXMix);
		LOADBASSFUNCTION(BASS_ChannelSetFX);
		LOADBASSFUNCTION(BASS_ChannelRemoveFX);
		LOADBASSFUNCTION(BASS_FXSetParameters);
		LOADBASSFUNCTION(BASS_FXGetParameters);

#endif

		if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
			TRACE(_T("@0BassInstance::InitBass. An incorrect version of BASS.DLL was loaded"));
			return FALSE;
		}
		// check the correct BASS_FX was loaded
		if (HIWORD(BASS_FX_GetVersion())!=BASSVERSION) {
			TRACE(_T("@0BassInstance::InitBass. An incorrect version of BASS_FX.DLL was loaded (2.4 is required)"));
			return FALSE;
		}
		if (bUseFloatDSP)
			BASS_SetConfig(BASS_CONFIG_FLOATDSP, TRUE);
		if (!BASS_Init(-1,44100,0,0,NULL))
		{
			TRACE(_T("@0BassInstance::InitBass. Can't initialize device: %d"), BASS_ErrorGetCode());
			return FALSE;
		}
		if (bUseSampleFloat)
		{
			//=== check for floating-point capability
			HSTREAM floatable = BASS_StreamCreate(44100, 2, BASS_SAMPLE_FLOAT, 0, 0);
			if (floatable != 0)
				BASS_StreamFree(floatable);  //=== Successs
			else
			{
				TRACE(_T("@0BassInstance::InitBass. BASS_StreamCreate Failure\r\n"));
				bUseSampleFloat = FALSE;
			}
		}
		AddPlugin(_T("basswma.dll"), _T(""));//_T(".wma"));
		AddPlugin(_T("bass_mpc.dll"), _T(""));//_T(".mpc"));
		AddPlugin(_T("basscd.dll"), _T(""));//_T(".cda"));
		AddPlugin(_T("bassflac.dll"), _T(""));//_T("flac"));
		AddPlugin(_T("bassmidi.dll"), _T(""));//_T(".midmidi"));
		AddPlugin(_T("bass_ac3.dll"), _T(""));//_T(".ac3"));
		AddPlugin(_T("bass_aac.dll"), _T(""));//_T(".mp4.aac.m4a"));
		AddPlugin(_T("bass_ape.dll"), _T(""));//_T(".ape"));
		AddPlugin(_T("bass_wv.dll"), _T(""));//_T(".wv"));
	}
	return TRUE;

}

BOOL BassInstance::AddPlugin(LPCTSTR dllName, LPCTSTR supportedExtensions)
{
	HPLUGIN pl = BASS_PluginLoad((LPCSTR)CT2CA(dllName), 0);
	if (pl != NULL)
	{
		m_Plugins.push_back(pl);
		if (supportedExtensions != NULL)
			m_sFormats += supportedExtensions;
	}
	if (pl == NULL)
		TRACE(_T("BassInstance::AddPlugin: %s plugin not found.\r\n"), dllName);
	return pl != NULL;
}

BOOL BassInstance::IsFileSupported(LPCTSTR str)
{
	ASSERT(str != NULL);
	INT len = _tcslen(str);
	ASSERT(len >= 4);
	if (len >= 4)
	{
		LPCTSTR filePath = &str[len - 4];
		TCHAR ext[5];
		_tcsncpy(ext, filePath, 4);
		ext[4] = 0;
		_tcslwr(ext);
		return _tcsistr(m_sFormats.c_str(), ext) != NULL;
	}
	return FALSE;
}
