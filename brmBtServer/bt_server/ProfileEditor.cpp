#include "StdAfx.h"
#include "ProfileEditor.h"
#include <algorithm>

ProfileEditor::ProfileEditor(Cbt_serverDlg * mainDlg)
{
	this->mainDlg = mainDlg;
}

ProfileEditor::~ProfileEditor(void)
{
}

void ProfileEditor::assignKey(void)
{
	assignKeyDlg.actionComboValue = L"";
	assignKeyDlg.appearanceComboValue = L"";
	assignKeyDlg.keyCode = 0;
	assignKeyDlg.labelEditValue = L"";
	assignKeyDlg.keyEditValue = L"";
	assignKeyDlg.paramaterEditValue = L"";

	if (assignKeyDlg.DoModal() != IDOK) return;

	if (!assignKeyDlg.keyCode)
		AfxMessageBox(L"You need to pick a key (use your cell phone)", MB_ICONEXCLAMATION);
	/*else if (assignKeyDlg.labelEditValue.IsEmpty())
		AfxMessageBox(L"You need to type a label", MB_ICONEXCLAMATION);*/
	else if (assignKeyDlg.actionComboValue.IsEmpty())
		AfxMessageBox(L"You need to specify an action", MB_ICONEXCLAMATION);
	else if (!addBinding(assignKeyDlg.keyCode, assignKeyDlg.labelEditValue, assignKeyDlg.actionComboValue, assignKeyDlg.paramaterEditValue, assignKeyDlg.appearanceComboValue))
		AfxMessageBox(L"Action already assigned to the key: " + assignKeyDlg.keyEditValue, MB_ICONEXCLAMATION);
	else saveProfile();
}

void ProfileEditor::editKey(void)
{
	int nItem = mainDlg->keyList.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1)
	{
		AfxMessageBox(L"Please select an item", MB_ICONEXCLAMATION);
		return;
	}

	KeyBinding * kb = (KeyBinding *) mainDlg->keyList.GetItemData(nItem);

	assignKeyDlg.actionComboValue = kb->action;
	assignKeyDlg.appearanceComboValue = kb->appearance;
	assignKeyDlg.keyCode = kb->keyCode; 
	assignKeyDlg.labelEditValue = kb->label;
	assignKeyDlg.keyEditValue = ModuleManager::get()->getKeyName(kb->keyCode);
	assignKeyDlg.paramaterEditValue = kb->parameter;

	if (assignKeyDlg.DoModal() != IDOK) return;

	deleteBinding(kb, nItem);

	if (!assignKeyDlg.keyCode)
		AfxMessageBox(L"You need to pick a key (use your cell phone)", MB_ICONEXCLAMATION);
	/*else if (assignKeyDlg.labelEditValue.IsEmpty())
		AfxMessageBox(L"You need to type a label", MB_ICONEXCLAMATION);*/
	else if (assignKeyDlg.actionComboValue.IsEmpty())
		AfxMessageBox(L"You need to specify an action", MB_ICONEXCLAMATION);
	else if (!addBinding(assignKeyDlg.keyCode, assignKeyDlg.labelEditValue, assignKeyDlg.actionComboValue, assignKeyDlg.paramaterEditValue, assignKeyDlg.appearanceComboValue))
		AfxMessageBox(L"Action already assigned to the key: " + assignKeyDlg.keyEditValue, MB_ICONEXCLAMATION);
	
	saveProfile();
}

void ProfileEditor::deleteKey(void)
{
	int nItem = mainDlg->keyList.GetNextItem(-1, LVNI_SELECTED);
	if (nItem == -1)
	{
		AfxMessageBox(L"Please select an item", MB_ICONEXCLAMATION);
		return;
	}
	KeyBinding * kb = (KeyBinding *) mainDlg->keyList.GetItemData(nItem);
	deleteBinding(kb, nItem);
	
	saveProfile();
}



bool ProfileEditor::addBinding(int keyCode, CString label, CString action, CString parameter, CString appearance)
{
	LVFINDINFO info;
	info.flags = LVFI_STRING;
	CString tmp = ModuleManager::get()->getKeyName(keyCode);
	info.psz = tmp;//ModuleManager::get()->getKeyName(keyCode);
	if (mainDlg->keyList.FindItem(&info) != -1) return false;

	bindings.push_back(KeyBinding(keyCode, label, action, parameter, appearance));
	KeyBinding * currentBinding = &bindings.back();

	int nItem;
	nItem = mainDlg->keyList.InsertItem(0, info.psz);
	mainDlg->keyList.SetItemText(nItem, 1, label);
	mainDlg->keyList.SetItemText(nItem, 2, action+L" ("+parameter+L")");
	mainDlg->keyList.SetItemText(nItem, 3, appearance);
	mainDlg->keyList.SetItemData(nItem, (DWORD_PTR) currentBinding);

	return true;
}

bool ProfileEditor::deleteBinding(KeyBinding * kb, int nItem /*= -1*/) // if nItem is specified, things are sped up a little bit
{
	if (nItem == -1)
	{
		CString tmp = ModuleManager::get()->getKeyName(kb->keyCode);

		LVFINDINFO info;
		info.flags = LVFI_STRING;
		info.psz = tmp;
		nItem = mainDlg->keyList.FindItem(&info);
	}

	std::list<KeyBinding>::iterator it = std::find(bindings.begin(), bindings.end(), *kb);
	if (it != bindings.end()) bindings.erase(it);

	return mainDlg->keyList.DeleteItem(nItem);
}

bool ProfileEditor::loadProfile(CString name)
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	currentProfile = name;
	wchar_t buf[2048], * substr;

	FILE * f = _wfopen(appdata + L"\\Brm BT Remote\\profiles\\"+name+L".profile", L"rt, ccs=UNICODE");
	if (!f) return false;

	while (!feof(f))
	{
		buf[0] = 0;
		fgetws(buf, 2048, f);
		if (buf[0] == 0) break;
		if (buf[wcslen(buf)-1] == 10 || buf[wcslen(buf)-1] == 13) buf[wcslen(buf)-1] = 0; // strip newline characters
		if (buf[wcslen(buf)-1] == 10 || buf[wcslen(buf)-1] == 13) buf[wcslen(buf)-1] = 0;

		substr = wcstok_e(buf, '\t');

		if (wcscmp(substr, L"Key:") == 0)
		{
			int keyCode;
			substr = wcstok_e(NULL, '\t');
			if (!swscanf(substr, L"%d", &keyCode)) continue;

			wchar_t * action, * params, * appearance, * label;

			if (!(label = wcstok_e(NULL, '\t'))) continue;
			if (!(action = wcstok_e(NULL, '\t'))) continue;
			if (!(params = wcstok_e(NULL, '\t'))) continue;
			if (!(appearance = wcstok_e(NULL, '\t'))) continue;
			
			addBinding(keyCode, label, action, params, appearance);
		}
		else if (wcscmp(substr, L"Layout:") == 0)
		{
			int layoutNum;
			substr = wcstok_e(NULL, '\t');
			if (!swscanf(substr, L"%d", &layoutNum)) { fclose(f); return false; }

			switch (layoutNum)
			{
			case 1: ((CButton *)mainDlg->GetDlgItem(IDC_RADIO1))->SetCheck(1); break;
			case 2: ((CButton *)mainDlg->GetDlgItem(IDC_RADIO2))->SetCheck(1); break;
			case 3: ((CButton *)mainDlg->GetDlgItem(IDC_RADIO3))->SetCheck(1); break;
			}
		}
		else if (wcscmp(substr, L"InfoCollector1:") == 0 || wcscmp(substr, L"InfoCollector2:") == 0)
		{
			wchar_t * num = substr+13;
			wchar_t * infoCollector, * params;
			if (!(infoCollector = wcstok_e(NULL, '\t'))) continue;
			if (!(params = wcstok_e(NULL, '\t'))) continue;
			if (num[0] == '1')
			{
				mainDlg->comboInfoCollector1.SetCurSel(mainDlg->comboInfoCollector1.FindString(-1, infoCollector));
				mainDlg->editInfoCollectorParam1.SetWindowTextW(params);
			}
			else
			{
				mainDlg->comboInfoCollector2.SetCurSel(mainDlg->comboInfoCollector2.FindString(-1, infoCollector));
				mainDlg->editInfoCollectorParam2.SetWindowTextW(params);
			}

		}
	}

	fclose(f);

	return true;
}

bool ProfileEditor::saveProfile()
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	FILE * f = _wfopen(appdata + L"\\Brm BT Remote\\profiles\\"+currentProfile+L".profile", L"wt, ccs=UNICODE");
	if (!f) return false;

	if (((CButton *)mainDlg->GetDlgItem(IDC_RADIO3))->GetCheck()) fwprintf(f, L"Layout:\t3\n");
	else if (((CButton *)mainDlg->GetDlgItem(IDC_RADIO2))->GetCheck()) fwprintf(f, L"Layout:\t2\n");
	else fwprintf(f, L"Layout:\t1\n"); // default to layout #1, even if nothing is selected

	if (mainDlg->comboInfoCollector1.GetCurSel() <= 0) fwprintf(f, L"InfoCollector1:\n");
	else
	{
		CString name, params;
		mainDlg->comboInfoCollector1.GetLBText(mainDlg->comboInfoCollector1.GetCurSel(), name);
		mainDlg->editInfoCollectorParam1.GetWindowTextW(params);
		fwprintf(f, L"InfoCollector1:\t%s\t%s\n", name, params);
	}

	if (mainDlg->comboInfoCollector2.GetCurSel() <= 0) fwprintf(f, L"InfoCollector2:\n");
	else
	{
		CString name, params;
		mainDlg->comboInfoCollector2.GetLBText(mainDlg->comboInfoCollector2.GetCurSel(), name);
		mainDlg->editInfoCollectorParam2.GetWindowTextW(params);
		fwprintf(f, L"InfoCollector2:\t%s\t%s\n", name, params);
	}

	for (std::list<KeyBinding>::iterator it = bindings.begin(); it != bindings.end(); it++)
	{
		KeyBinding * kb = &(*it);
		fwprintf(f, L"Key:\t%d\t%s\t%s\t%s\t%s\n", kb->keyCode, kb->label, kb->action, kb->parameter, kb->appearance);
	}

	fclose(f);
	_log(L"Saved: " + currentProfile + L"\n");
	return false;
}

void ProfileEditor::clear(void)
{
	((CComboBox *)mainDlg->GetDlgItem(IDC_COMBOVAL1))->SetCurSel(0); // ->ResetContent();
	((CComboBox *)mainDlg->GetDlgItem(IDC_COMBOVAL2))->SetCurSel(0); // ->ResetContent();
	mainDlg->GetDlgItem(IDC_EDITVAL1PARAMS)->SetWindowTextW(L"");
	mainDlg->GetDlgItem(IDC_EDITVAL2PARAMS)->SetWindowTextW(L"");
	((CButton *)mainDlg->GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *)mainDlg->GetDlgItem(IDC_RADIO2))->SetCheck(0);
	((CButton *)mainDlg->GetDlgItem(IDC_RADIO3))->SetCheck(0);

	bindings.clear();
	mainDlg->keyList.DeleteAllItems();
	currentProfile = L"";
}

void ProfileEditor::disable(void)
{
	clear();
	mainDlg->GetDlgItem(IDC_BUTTONASSIGNKEY)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_BUTTONEDITKEY)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_BUTTONDELETEKEY)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_COMBOVAL1)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_COMBOVAL2)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_EDITVAL1PARAMS)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_EDITVAL2PARAMS)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_BUTTONVAL1PARAMHELP)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_BUTTONVAL2PARAMHELP)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_RADIO1)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_RADIO2)->EnableWindow(false);
	mainDlg->GetDlgItem(IDC_RADIO3)->EnableWindow(false);
	mainDlg->keyList.EnableWindow(false);
}

void ProfileEditor::enable(void)
{
	mainDlg->GetDlgItem(IDC_BUTTONASSIGNKEY)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_BUTTONEDITKEY)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_BUTTONDELETEKEY)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_COMBOVAL1)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_COMBOVAL2)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_EDITVAL1PARAMS)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_EDITVAL2PARAMS)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_BUTTONVAL1PARAMHELP)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_BUTTONVAL2PARAMHELP)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_RADIO1)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_RADIO2)->EnableWindow(true);
	mainDlg->GetDlgItem(IDC_RADIO3)->EnableWindow(true);
	mainDlg->keyList.EnableWindow(true);
}
