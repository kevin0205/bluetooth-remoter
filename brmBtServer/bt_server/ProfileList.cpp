#include "stdafx.h"
#include "ProfileList.h"
#include <vector>
#include <algorithm> // used for sorting std::vector
#include <io.h>
#include "ProfileNameDlg.h"

ProfileList::ProfileList(Cbt_serverDlg * mainDlg)
{
	this->mainDlg = mainDlg;
	loadList();
}

ProfileList::~ProfileList(void)
{
}

bool ProfileList::copyDefaultProfiles()
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	if (_wmkdir(appdata + L"\\Brm BT Remote") != 0 && errno != EEXIST) { AfxMessageBox(L"Unable to create a directory for the profiles", MB_ICONEXCLAMATION); return false; }
	if (_wmkdir(appdata + L"\\Brm BT Remote\\profiles") != 0 && errno != EEXIST) { AfxMessageBox(L"Unable to create a directory for the profiles", MB_ICONEXCLAMATION); return false; }

	WIN32_FIND_DATA f;
	HANDLE h;

	h = FindFirstFile(appdata + L"\\Brm BT Remote\\profiles\\*.profile", &f);
	
	if (h == INVALID_HANDLE_VALUE) // if the appdata directory is empty, copy the default profiles into it
	{
		h = FindFirstFile(L"default profiles\\*.profile", &f);
		if(h != INVALID_HANDLE_VALUE)
		{
			CString src, dst;
			do
			{
				src = L"default profiles\\";
				src += f.cFileName;
				dst = appdata;
				dst += L"\\Brm BT Remote\\profiles\\";
				dst += f.cFileName;
				
				CopyFile(src, dst, true);
			} while(FindNextFile(h, &f));

			//CloseHandle(h);
		}
		else { AfxMessageBox(L"Default profiles not found, please reinstall the application", MB_ICONEXCLAMATION); return false; }
	}
	//else CloseHandle(h);

	return true;
}

bool ProfileList::loadList()
{
	std::vector<CString> filenames;
	mainDlg->profileList.ResetContent();

	if (!copyDefaultProfiles())
	{
		PostQuitMessage(0);
		return false;
	}

	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(appdata + L"\\Brm BT Remote\\profiles\\*.profile", &f);
	if(h != INVALID_HANDLE_VALUE)
	{
		do
		{
			f.cFileName[wcslen(f.cFileName)-8] = 0;
			filenames.push_back(f.cFileName);
		} while(FindNextFile(h, &f));

		sort(filenames.begin(), filenames.end());
		for (std::vector<CString>::iterator it = filenames.begin(); it != filenames.end(); it++) mainDlg->profileList.InsertString(mainDlg->profileList.GetCurSel(),*it);

		//CloseHandle(h);

		return true;
	}
	else
	{
		return false;
	}
}

bool ProfileList::renameSelected(void)
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	CString filename;
	if (mainDlg->profileList.GetCurSel() == LB_ERR)
	{
		AfxMessageBox(L"Please select a profile!", MB_ICONEXCLAMATION);
		return false;
	}
	mainDlg->profileList.GetText(mainDlg->profileList.GetCurSel(), filename);
	ProfileNameDlg dlg;
	dlg.profileName = filename;
	if (dlg.DoModal() == IDOK)
	{
		if (_wrename(appdata + L"\\Brm BT Remote\\profiles\\"+filename+L".profile", appdata + L"\\Brm BT Remote\\profiles\\"+dlg.profileName+L".profile") != 0)
		{
			AfxMessageBox(L"Could not rename file!\nA file name cannot contain any of these characters:\n\\ / : ? * \" < > |", MB_ICONEXCLAMATION);
			return false;
		}
		loadList();
	}
	return true;
}

bool ProfileList::deleteSelected(void)
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	CString filename;
	if (mainDlg->profileList.GetCurSel() == LB_ERR)
	{
		AfxMessageBox(L"Please select a profile!", MB_ICONEXCLAMATION);
		return false;
	}
	mainDlg->profileList.GetText(mainDlg->profileList.GetCurSel(), filename);

	if (AfxMessageBox(L"Are you sure you want to delete the selected profile \""+filename+L"\"?", MB_ICONQUESTION | MB_YESNO) == IDYES)
	{
		if (_wremove(appdata + L"\\Brm BT Remote\\profiles\\"+filename+L".profile") != 0)
		{
			AfxMessageBox(L"Could not delete file!", MB_ICONEXCLAMATION);
			return false;
		}
		loadList();
	}

	mainDlg->m_profileEditor->disable();

	return true;
}

bool ProfileList::createNew(void)
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	ProfileNameDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		if (_waccess(appdata + L"\\Brm BT Remote\\profiles\\"+dlg.profileName+L".profile", 0) != -1)
		{
			AfxMessageBox(L"A profile with name \""+dlg.profileName+L"\" already exists!", MB_ICONEXCLAMATION);
			return false;
		}
		FILE * f = _wfopen(appdata + L"\\Brm BT Remote\\profiles\\"+dlg.profileName+L".profile", L"wt, ccs=UNICODE");
		fwprintf(f, L"Layout:\t1\n");
		if (f) fclose(f);
		else
		{
			AfxMessageBox(L"Could not create file!\nA file name cannot contain any of these characters:\n\\ / : ? * \" < > |", MB_ICONEXCLAMATION);
			return false;
		}

		loadList();
	}
	return true;
}
