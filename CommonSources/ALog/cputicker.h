// TITLE:
// High-Resolution Counter Class.
//
// VERSION:
// 1.24
//
// AUTHORS:
// Created by J.M.McGuiness, http://www.hussar.demon.co.uk
//            PJ Naughter,   Email: pjn@indigo.ie Web: http://indigo.ie/~pjn
//
// DESCRIPTION:
// This file declares a class the wraps the Pentium-specific time stamp counter.
// This counter has a resolution in terms of PCLKS (processor clocks) so it can
// be used for direct instruction timings.
//
// LEGALITIES:
// Copyright © 1996-2001 by J.M.McGuiness and PJ Naughter, all rights reserved.
//
// This file must not be distributed without the authors prior
// consent.
//
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////// Macros ///////////////////////////////////

#ifndef __CPUTICKER_H__
#define __CPUTICKER_H__





////////////////////////// Classes //////////////////////////////////////////
class CCPUTicker : public CObject
{
public:
//constructors / Destructors
	CCPUTicker();
	CCPUTicker(const CCPUTicker& ticker);

//copy constructor
	inline CCPUTicker& operator=(const CCPUTicker& ticker);

//perform the actual measurement
  __int64 Measure();

//accessors to the actual measurement value
  double GetTickCountAsSeconds() const;  
  inline __int64 GetTickCount() const { return m_TickCount; };  

//static methods
	static BOOL GetCPUFrequency(double& frequency, double& target_ave_dev, 
                              unsigned long interval = 100,
                              unsigned int max_loops = 2);
  static BOOL GetCachedCPUFrequency(double& frequency, double& target_ave_dev);
  static double GetCachedCPUFrequency() { return m_freq; }

//Is high resolution available on this CPU
	BOOL IsAvailable() const;

//Standard MFC diagnostics
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  __int64 m_TickCount;

 	static BOOL m_bHasRDTSC;
  static BOOL m_bRunningOnNT;
  static BOOL m_bStaticsCalculated;

  static double m_deviation;
  static double m_freq;
  static BOOL m_bFrequencyCalculated;

  static BOOL CheckHasRDTSC();
  static BOOL RunningOnNT();

	DECLARE_DYNAMIC(CCPUTicker)
};


#endif //__CPUTICKER_H__