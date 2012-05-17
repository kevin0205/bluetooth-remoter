#pragma once
#include "afxwin.h"


// ProfileNameDlg dialog

class ProfileNameDlg : public CDialog
{
	DECLARE_DYNAMIC(ProfileNameDlg)

public:
	ProfileNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ProfileNameDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString profileName;
	virtual int OnInitDialog(void);
	CEdit profileNameField;
};
