// AssignKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bt_server.h"
#include "AssignKeyDlg.h"


// AssignKeyDlg dialog

IMPLEMENT_DYNAMIC(AssignKeyDlg, CDialog)

AssignKeyDlg::AssignKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AssignKeyDlg::IDD, pParent)
	, keyEditValue(_T(""))
	, actionComboValue(_T(""))
	, paramaterEditValue(_T(""))
	, appearanceComboValue(_T(""))
	, labelEditValue(_T(""))
{
	m_active = false;
}

AssignKeyDlg::~AssignKeyDlg()
{
}

void AssignKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, keyEditBox);
	DDX_Control(pDX, IDC_COMBO1, actionComboBox);
	DDX_Control(pDX, IDC_EDIT2, parameterEditBox);
	DDX_Control(pDX, IDC_COMBO2, appearanceComboBox);
	DDX_Control(pDX, IDC_HELPTXT, parameterHelpText);
	DDX_Text(pDX, IDC_EDIT1, keyEditValue);
	DDX_CBString(pDX, IDC_COMBO1, actionComboValue);
	DDX_Text(pDX, IDC_EDIT2, paramaterEditValue);
	DDX_CBString(pDX, IDC_COMBO2, appearanceComboValue);
	DDX_Control(pDX, IDC_EDIT3, labelEditBox);
	DDX_Text(pDX, IDC_EDIT3, labelEditValue);
}

int AssignKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModuleManager::get()->fillDropdownActions(actionComboBox);
	if (actionComboValue.IsEmpty())
	{
		actionComboBox.SetCurSel(-1);
		parameterEditBox.EnableWindow(false);
		parameterHelpText.SetWindowTextW(L"");
	}
	else
	{
		actionComboBox.SetCurSel(actionComboBox.FindString(-1, actionComboValue));
		executableAction * action = ModuleManager::get()->getAction(actionComboValue);
		if (action)
		{
			parameterEditBox.EnableWindow(action->hasParams);
			parameterHelpText.SetWindowTextW(action->paramHelp);
		}
		else
		{
			parameterEditBox.EnableWindow(false);
			parameterHelpText.SetWindowTextW(L"");
		}
	}
	
	ModuleManager::get()->fillDropdownAppearances(appearanceComboBox);
	if (appearanceComboValue.IsEmpty()) appearanceComboBox.SetCurSel(0);
	else
	{
		int n = appearanceComboBox.FindString(-1, appearanceComboValue);
		appearanceComboBox.SetCurSel(n);
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(AssignKeyDlg, CDialog)
//	ON_BN_CLICKED(IDOK, &AssignKeyDlg::OnBnClickedOk)
ON_CBN_SELCHANGE(IDC_COMBO1, &AssignKeyDlg::OnSelectAction)
END_MESSAGE_MAP()


INT_PTR AssignKeyDlg::DoModal()
{
	m_active = true;
	return CDialog::DoModal();
}

BOOL AssignKeyDlg::DestroyWindow()
{
	m_active = false;
	return CDialog::DestroyWindow();
}

bool AssignKeyDlg::isActive()
{
	return m_active;
}

void AssignKeyDlg::receiveKey(wchar_t *key)
{
	int keyCode;
	swscanf(key+3, L"%d", &keyCode); // the first 3 characters are either "UP:" or "DN:"
	keyEditBox.SetWindowTextW(ModuleManager::get()->getKeyName(keyCode));
	this->keyCode = keyCode;
}

void AssignKeyDlg::OnSelectAction()
{
	parameterEditBox.EnableWindow(false);
	parameterHelpText.SetWindowTextW(L"");

	CString actionName;
	actionComboBox.GetLBText(actionComboBox.GetCurSel(), actionName);

	executableAction * selectedAction = ModuleManager::get()->getAction(actionName);

	if (selectedAction)
	{
		if (selectedAction->hasParams) parameterEditBox.EnableWindow(true);
		parameterHelpText.SetWindowTextW(selectedAction->paramHelp);
	}
}
