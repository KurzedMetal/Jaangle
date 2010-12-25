
#ifndef _CDetailsDlg_h_
#define _CDetailsDlg_h_

class CDetailsDlg : public CDialog
{
public:
	CDetailsDlg(CWnd* pParent = NULL);
	virtual ~CDetailsDlg();

	enum { IDD = IDD_DETAILS };
	CString	m_FileContents;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
private:
	CFont m_FileContentsFont;

};


#endif 
