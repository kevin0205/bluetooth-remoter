#pragma once
#include "bt_serverDlg.h"
#include "AssignKeyDlg.h"
#include "ModuleManager.h"
#include <list>

class Cbt_serverDlg;

struct KeyBinding
{
	KeyBinding (int keyCode, CString label, CString action, CString parameter, CString appearance)
	{
		this->keyCode = keyCode;  this->label = label;  this->action = action;  this->parameter = parameter; this->appearance = appearance;
	}

	bool operator==(const KeyBinding & kb) const
	{
		return memcmp(&kb, this, sizeof(KeyBinding)) == 0;
	}

	int keyCode;
	CString label;
	CString action;
	CString parameter;
	CString appearance;
};

class ProfileEditor
{
protected:
	Cbt_serverDlg * mainDlg;
	CString currentProfile;
	std::list<KeyBinding> bindings; // list does not invalidate pointers on modification as opposed to vector
public:
	AssignKeyDlg assignKeyDlg;

	ProfileEditor(Cbt_serverDlg * mainDlg);
	~ProfileEditor(void);
	void assignKey(void);
	void editKey(void);
	void deleteKey(void);
	bool addBinding(int keyCode, CString label, CString action, CString parameter, CString appearance);
	bool deleteBinding(KeyBinding * kb, int nItem = -1);

	bool loadProfile(CString name);
	bool saveProfile();

	void clear(void);
	void disable(void);
	void enable(void);
};
