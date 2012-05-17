#pragma once
#include "bt_serverDlg.h"

class Cbt_serverDlg;

class ProfileList
{
protected:
	Cbt_serverDlg * mainDlg;
public:
	ProfileList(Cbt_serverDlg * mainDlg);
	~ProfileList();
	bool copyDefaultProfiles();
	bool loadList();
	bool renameSelected();
	bool deleteSelected();
	bool createNew();
};
