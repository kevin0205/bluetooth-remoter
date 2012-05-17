#pragma once

#include "ModuleManager.h"
#include "bt_serverDlg.h"

struct actionAndParams
{
	executableAction * action;
	CString params;

	actionAndParams(executableAction * action, CString params)
	{
		this->action = action;
		this->params = params;
	}
	bool execute(bool keyDown)
	{
		if (action == NULL) return false;
		action->plugin->callAction(action->actionId, params, keyDown);
		return true;
	}
};

struct infoCollectorAndParams
{
	executableAction * collector;
	CString params;

	infoCollectorAndParams(executableAction * collector, CString params)
	{
		this->collector = collector;
		this->params = params;
	}
	CString execute()
	{
		if (collector == NULL) return L"";
		return collector->plugin->callInfoCollector(collector->actionId, params);
	}
};


class CommandCore
{
protected:
	Cbt_serverDlg * mainDlg;
	CString currentProfile;
	std::map<int, actionAndParams> actions; // maps keycodes to actions
	HANDLE thread;
	bool executeThread;
	infoCollectorAndParams * infoCollectors[2];

public:
	CommandCore(Cbt_serverDlg * dlg);
	~CommandCore(void);
	bool receiveCommand(wchar_t * cmd);
	void onConnectionClosed();
	void updateInfoCollectors();

protected:
	bool loadProfile(CString name);
};
