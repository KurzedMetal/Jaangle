// PictureWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PictureWnd.h"
// CPictureWnd

IMPLEMENT_DYNAMIC(CPictureWnd, CWnd)

CPictureWnd::CPictureWnd():
m_bKeep(TRUE),
m_bStretch(TRUE),
m_bkColor(RGB(0,0,0))
{

}

CPictureWnd::~CPictureWnd()
{
}


BEGIN_MESSAGE_MAP(CPictureWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CPictureWnd message handlers



void CPictureWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CClientDC _dc(this);//CAUTION! The whole Area needs to be Invalidated!!!!
	CRect rc;
	GetClientRect(&rc);
	if (m_Picture.m_Height > 0)
	{
		if (m_bStretch)
		{
			if (m_bKeep)
			{
				CRect newRC;
				double ARBox = rc.Width() / (double)rc.Height();
				double ARPic = m_Picture.m_Width / (double)m_Picture.m_Height;
				if (ARBox >= ARPic)
				{
					INT newPicWidth = INT(rc.Height() * ARPic);
					newRC.left = (rc.Width() - newPicWidth) / 2;
					newRC.top = 0;
					newRC.right = newRC.left + newPicWidth;
					newRC.bottom = rc.Height();
					_dc.FillSolidRect(0, 0, newRC.left, newRC.bottom, m_bkColor);
					_dc.FillSolidRect(newRC.right, 0, newRC.left, newRC.bottom, m_bkColor);
				}
				else
				{
					INT newPicHeight = INT(rc.Width() / ARPic);
					newRC.left = 0;
					newRC.top = (rc.Height() - newPicHeight) / 2;
					newRC.right = rc.Width();
					newRC.bottom = newRC.top + newPicHeight;
					_dc.FillSolidRect(0, 0, newRC.right, newRC.top, m_bkColor);
					_dc.FillSolidRect(0, newRC.bottom, newRC.right, newRC.top, m_bkColor);
				}
				m_Picture.Show(&_dc, newRC);
			}
			else
			{
				m_Picture.Show(&_dc, rc);
			}
		}
		else
			m_Picture.Show(&_dc, CRect(0,0, m_Picture.m_Width, m_Picture.m_Height));

	}
}


BOOL CPictureWnd::Load(LPCTSTR sFilePathName)
{
	BOOL ret = FALSE;
	if(sFilePathName != NULL)
		ret = m_Picture.Load(sFilePathName);
	else
		m_Picture.FreePictureData();
	Invalidate();
	return ret;
}

BOOL CPictureWnd::Load(UINT ResourceName, LPCTSTR ResourceType)
{
	ASSERT(ResourceType != NULL);
	BOOL ret = m_Picture.Load(ResourceName, ResourceType);
	Invalidate();
	return ret;
}

BOOL CPictureWnd::OnEraseBkgnd(CDC* pDC)
{
	if (!m_bStretch)
	{
		CRect rc;
		GetClientRect(&rc);
		pDC->FillSolidRect(0,0, rc.Width(), rc.Height(), m_bkColor);
	}
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}
