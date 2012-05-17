// ConsoleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bt_server.h"
#include "ConsoleDlg.h"


ConsoleDlg * ConsoleDlg::instance = NULL;

IMPLEMENT_DYNAMIC(ConsoleDlg, CDialog)

ConsoleDlg::ConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConsoleDlg::IDD, pParent)
{
	instance = this;
}

ConsoleDlg::~ConsoleDlg()
{
}

void ConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CONSOLE, richEdit);
}


BEGIN_MESSAGE_MAP(ConsoleDlg, CDialog)
END_MESSAGE_MAP()


// ConsoleDlg message handlers

ConsoleDlg * ConsoleDlg::get()
{
	return instance;
}

void ConsoleDlg::log(CString s)
{
	richEdit.SetSel(richEdit.GetTextLength(),richEdit.GetTextLength());
	richEdit.ReplaceSel(s);
	richEdit.SetSel(-1,0);
}

void ConsoleDlg::log(float f)
{
	wchar_t tmp[24];
	swprintf (tmp, 24, L"%f\n", f);
	richEdit.SetSel(richEdit.GetTextLength(),richEdit.GetTextLength());
	richEdit.ReplaceSel(tmp);
	richEdit.SetSel(-1,0);
}

void ConsoleDlg::log(int i)
{
	wchar_t tmp[24];
	swprintf (tmp, 24, L"%d\n", i);
	richEdit.SetSel(richEdit.GetTextLength(),richEdit.GetTextLength());
	richEdit.ReplaceSel(tmp);
	richEdit.SetSel(-1,0);
}

void _log(CString s) { ConsoleDlg::get()->log(s); }
void _log(float f) { ConsoleDlg::get()->log(f); }
void _log(int i) { ConsoleDlg::get()->log(i); }