#pragma once
#include "RectAlphaDrawer.h"
#include "FlickerFree.h"
#include "GdiPlusPicDrawer.h"

#include <string>

//Adds Features to CPicWnd
//1. Double Buffering via CFlickerFree
//2. Transition Effect. Use SetTransitionTime()
//3. Alpha Blended Title. Use SetTitle();

class CPicWndEx : public CWnd
{
	DECLARE_DYNAMIC(CPicWndEx)

public:
	CPicWndEx();
	virtual ~CPicWndEx();

	BOOL LoadFile(LPCTSTR imgFile);
	BOOL LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst = NULL);
	BOOL LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst = NULL);
	void ClearImage();

	void SetBkColor(COLORREF rgb, BYTE opacity = 255);
	GdiPlusPicDrawer::DrawParams& GetDrawParams()		{return m_picDrawer.GetDrawParams();}
	CSize GetPictureSize();

	void SetTransitionTime(UINT ms);
	void SetTitle(LPCTSTR title);
	void SetTitleFont(CFont* pTitleFont)	{m_pTitleFont = pTitleFont;}


protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	void Redraw();

	CFlickerFree m_ff;
	RectAlphaDrawer m_AlphaDrawer;
	INT m_curFadeStep;
	INT m_totalFadeSteps;
	BOOL m_bTransitioning;
	std::basic_string<TCHAR> m_title;
	BOOL m_bMemDCInvalidated;
	CFont* m_pTitleFont;
	GdiPlusPicDrawer m_picDrawer;



};


