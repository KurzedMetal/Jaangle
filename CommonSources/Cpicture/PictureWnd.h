#ifndef _CPictureWnd_h_
#define _CPictureWnd_h_

#include "Cpicture/Picture.h"
// CPictureWnd

class CPictureWnd : public CWnd
{
	DECLARE_DYNAMIC(CPictureWnd)

public:
	CPictureWnd();
	virtual ~CPictureWnd();
	BOOL Load(LPCTSTR sFilePathName);
	BOOL Load(UINT ResourceName, LPCTSTR ResourceType);

	void StretchToContainer(BOOL bStretch, BOOL bKeepAspectRatio = FALSE)
	{
		m_bKeep = bKeepAspectRatio;
		m_bStretch = bStretch;
	}
	void SetBkColor(COLORREF bkColor)
	{
		m_bkColor = bkColor;
	}

	//void KeepAspectRatio(BOOL bKeep)	{m_bKeep = bKeep;}
	UINT GetPictureWidth()	{return m_Picture.m_Width;}
	UINT GetPictureHeight()	{return m_Picture.m_Height;}

protected:
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
private:
	CPicture m_Picture;
	COLORREF m_bkColor;
	BOOL m_bKeep;
	BOOL m_bStretch;
};


#endif