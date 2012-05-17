#pragma once
#include "stdafx.h"
#include "ModuleManager.h"
#include "afxwin.h"

class AssignKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(AssignKeyDlg)

public:
	AssignKeyDlg(CWnd* pParent = NULL);
	virtual ~AssignKeyDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual int OnInitDialog();
	bool m_active;

	CEdit labelEditBox;
	CEdit keyEditBox;
	CComboBox actionComboBox;
	CEdit parameterEditBox;
	CComboBox appearanceComboBox;
	CStatic parameterHelpText;

	DECLARE_MESSAGE_MAP()
public:
	int keyCode;

	virtual INT_PTR DoModal();
	virtual BOOL DestroyWindow();
	bool isActive();
	void receiveKey(wchar_t * keyCode);
	afx_msg void OnSelectAction();
	CString labelEditValue;
	CString keyEditValue;
	CString actionComboValue;
	CString paramaterEditValue;
	CString appearanceComboValue;
};

