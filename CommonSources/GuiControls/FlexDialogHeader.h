#ifndef _FlexDialogHeader_h_
#define _FlexDialogHeader_h_


// DialogHeader.h : header file
//******************************************************************************
//Author : Jason Gurgel
//EMail  : jmgurgel@attbi.com
//(C)    : 7-14-2002
//
//Purpose: Provide a professional looking header area for dialog controls
//       : that will display an icon, a title phrase in bold, and a
//       : description phrase. And also provide a custom background color
//       : and custom header height.
//		  
//Note   : This code was insipired by Mustafa Demirhan's article about hacking
//		 : the CPropterySheet. See link below. 
//		 : http://www.codeproject.com/property/hacking_the_cpropertyshee.asp#xx162207xx
//
//Disclaimer: This code is free to use as long as this information
//			: is included. The author takes no responsibility for
//			: the corretness of the code or any damage incured from it's use. 
//			: It is free so use at your own risk, and enjoy!
//			: Any comments, suggestions, or additions would be appricated.
//******************************************************************************



// How to use this class
//==============================================================================
//1. Include the files "DialogHeader.h" & "DialogHeader.cpp" in your project.
//
//2. Create a member variable in your dialog class e.g.(m_HeaderCtrl) and don't
//   forget to include the DialogHeader.h file.
//
//3. In your dialogs OnInit function do the following.
//   You can set these options anywhere you have access to the header ctrl
//   but the OnInit function is probably the eaisest.
//	  a. Set the headers display data:
//              m_HeaderCtrl.SetHeaderData(hIcon,"Title","Description");
//		 Or you can set/change each memeber individually using:
//              m_HeaderCtrl.SetIconHandle(hIcon);
//              m_HeaderCtrl.SetTitleText("Title");
//              m_HeaderCtrl.SetDescText("Description.");
//      
//
// Note: The above functions will be the most commonly used.
//
//	  b. Set the font sizes:  m_HeaderCtrl.SetFontSizes(TitleSize,DescSize);
//
//	  c. Set the text alignment: m_HeaderCtrl.SetTextAlign(DT_LEFT);
//                  Options are: {DT_LEFT,DT_CENTER,DT_RIGHT}
//
//    d. Set the background color: 
//                            m_HeaderCtrl.SetBackgroundColor(RGB(255,255,255));
//
//    e. Set blank icon mode: m_HeaderCtrl.SetBlankIcon(TRUE);
//                            FALSE=If no icon then no space allocated for icon     
//                            TRUE=If no icon space is allocated for one
//
//    f. Set the drawing offsets:
//         m_HeaderCtrl.SetIconOffset(5); Defines top left corner of icon
//                                        and border around header ctrl
//         m_HeaderCtrl.SetTitleOffset(10); Defines space between title text
//                                          and icon
//         m_HeaderCtrl.SetDescOffset(10); Defines the amount of indent for
//                                         desc text from the title text
//
//    g. Set the header height: m_HeaderCtrl.SetHeaderHeight(x);
//           -Where x=Height of header, if you choose x to be too small
//            to display the icon, the title, and one line of desc text
//            then x will be automaticly set to this min height.
//
//           -Also if you choose x=0, then the height will also be
//            automaticly calculated to disp the icon, the title, and 
//            one line of the desc text.
//   
//           -This calculation is based on the font sizes and offsets
//            so you must set these options before calling SetHeaderHeight
//            and the Init function.
//
// Note: The above options all have default values. So change what you want.
// Note: All the above options can be changed at runtime and the ctrl will
//       repaint itself with the new options. However the header height 
//       may not be enough to accomodate the new changes and you may have
//       to adjust the header height and move dialog controls and resize
//       the parent window yourself!
//    
// 	  h. Create the header ctrl and resize the dialog window to accomodate
//       the header ctrl by calling:  m_HeaderCtrl.Init(this);
//
//	  i. Move all ctrls on the dialog down to accomodate the header ctrl
//       by calling: m_HeaderCtrl.MoveCtrls(this);

// *Note*: MoveCtls works for most dialogs, however if you use this with a 
//		   CPropertySheet or a complicated ctrl you may need to bypass
//         the MoveCtrls function and/or perform your own control
//         placement in the InitDialog function.
//
//
//Example:
/*
BOOL CDlgFileTypes::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	//Set inital header data
	m_HeaderCtrl.SetTitleText("Registered File Types on this computer");
	m_HeaderCtrl.SetDescText("Displaying all registered file types for this computer below.");
	m_HeaderCtrl.SetIconHandle(AfxGetApp()->LoadIcon(IDI_HARDDRIVE));

	//Create the header ctrl
	m_HeaderCtrl.Init(this);
	m_HeaderCtrl.MoveCtrls(this);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
*/
//==============================================================================


//Defines: All default values are defined here
//-------: If you wish to change any default values change them here
#define DEFAULTICONOFFSET		5
#define DEFAULTTITLEOFFSET		10
#define DEFAULTDESCOFFSET		10
#define DEFAULTTEXTALIGN		DT_LEFT
#define DEFAULTTITLEFONTSIZE	15
#define DEFAULTDESCFONTSIZE		13
#define DEFAULTBLANKICON		TRUE
#define DEFAULTHEADERHEIGHT		60


/////////////////////////////////////////////////////////////////////////////
// CDialogHeader window

#include "GdiPlusPicDrawer.h"

class FlexDialogHeader : public CWnd
{
// Construction
public:
	FlexDialogHeader();	//Default Constructor


// Attributes
public:


// Operations
public:
	void Init(void* ptrWnd);
	void MoveCtrls(void* ptrWnd);

// Access Functions
	COLORREF GetBkColor() const						{return m_bkColor;}
	void SetBkColor(const COLORREF color)			{m_bkColor = color;}
	void SetBkImage(LPCTSTR image, BOOL bAutoSize)	
	{
		m_bkImage.LoadFile(image); 
		m_bkImage.GetDrawParams().zoomLockMode = (bAutoSize ? GdiPlusPicDrawer::ZLM_FillArea : GdiPlusPicDrawer::ZLM_NoLock);
	}
	void SetIconImage(LPCTSTR image)				{m_iconImage.LoadFile(image);}
	LPCTSTR	GetTitleStr() const						{return m_sTitle;}
	void SetTitleText(LPCTSTR text)					{m_sTitle = text;}
	LPCTSTR	GetDescStr() const						{return m_sDesc;}
	void SetDescText(LPCTSTR text)					{m_sDesc = text;}
	CFont* GetTitleFont() const						{return m_pTitleFont;}
	void SetTitleFont(CFont* pFont)					{m_pTitleFont = pFont;}
	CFont* GetDescFont() const						{return m_pDescFont;}
	void SetDescFont(CFont* pFont)					{m_pDescFont = pFont;}
	INT GetHeaderHeight()							{return m_headerHeight;}
	void SetHeaderHeight(INT height)				{m_headerHeight = height;}
	void SetIconHandle(HICON hIcon)					{m_hIcon = hIcon;}
	HICON GetIconHandle()							{return m_hIcon;}

	void SetIconSize(INT cx, INT cy)				{m_cxIcon = cx; m_cyIcon = cy;}
	void SetDraggable(BOOL bDraggable)				{m_bDraggable = bDraggable;}

private:
	COLORREF m_bkColor;
	GdiPlusPicDrawer m_bkImage;
	GdiPlusPicDrawer m_iconImage;
	CFont* m_pTitleFont;
	CFont* m_pDescFont;
	CString	m_sTitle;
	CString	m_sDesc;
	INT m_headerHeight;
	INT m_cxIcon;
	INT m_cyIcon;
	HICON m_hIcon;
	BOOL		m_bDraggable;
	CPoint		m_dragPoint;



	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogHeader)
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~FlexDialogHeader();


	// Generated message map functions
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	// Member Variables
//protected:
//	int			m_nTitleFontSize;	//Size of font used to draw title text
//	int			m_nDescFontSize;	//Size of font used to draw desc text
//	int			m_nHeaderHeight;	//Height of dialog header
//	int			m_nIconWidth;		//System icon width
//	int			m_nIconHeight;		//System icon height
	int			m_nxEdge;			//System edge width
	int			m_nyEdge;			//System edge height
//	int			m_nIconOffset;		//Icon offset from top left client edge
//	int			m_nTitleOffset;		//Title offset from icon right
//	int			m_nDescOffset;		//Desc offset from left of title text
//	UINT		m_uAlign;			//Text alignment
//	BOOL		m_bFonts;			//Boolean to track if fonts have been created
//	BOOL		m_bBlankIcon;		//Always leave space for icon
//	HICON		m_hIcon;			//Handle to header icon (NULL if none)
//	CFont		m_BoldFont;			//Bold font for title
//	CFont		m_NormalFont;		//Normal font for body
//	CString		m_sTitle;			//Title text
//	CString		m_sDesc;			//Description text
//	COLORREF	m_cBackground;		//Background fill color
//	BOOL		m_bDraggable;
//	CPoint		m_dragPoint;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGHEADER_H__D47F665F_FEB3_4AA7_B746_D385FC7956ED__INCLUDED_)
