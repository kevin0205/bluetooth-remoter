#pragma once
#include "afxcmn.h"


// ConsoleDlg dialog

class ConsoleDlg : public CDialog
{
protected:
	static ConsoleDlg * instance;
	CRichEditCtrl richEdit;

	DECLARE_DYNAMIC(ConsoleDlg)

public:
	ConsoleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ConsoleDlg();

	enum { IDD = IDD_CONSOLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	static ConsoleDlg * get();
	void log(CString s);
	void log(float f);
	void log(int i);
};

void _log(CString s);
void _log(float f);
void _log(int i);
