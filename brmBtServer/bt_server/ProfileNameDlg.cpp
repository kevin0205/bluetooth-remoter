// ProfileNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bt_server.h"
#include "ProfileNameDlg.h"


// ProfileNameDlg dialog

IMPLEMENT_DYNAMIC(ProfileNameDlg, CDialog)

ProfileNameDlg::ProfileNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ProfileNameDlg::IDD, pParent)
	, profileName(_T(""))
{

}

ProfileNameDlg::~ProfileNameDlg()
{
}

void ProfileNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, profileName);
	DDX_Control(pDX, IDC_EDIT1, profileNameField);
}


BEGIN_MESSAGE_MAP(ProfileNameDlg, CDialog)
END_MESSAGE_MAP()


// ProfileNameDlg message handlers

int ProfileNameDlg::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	profileNameField.SetFocus();
	return 0;
}
