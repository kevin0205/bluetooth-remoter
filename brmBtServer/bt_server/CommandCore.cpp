#include "StdAfx.h"
#include "CommandCore.h"
#include <process.h>

void infoCollectorThread (void * ccInstance)
{
	CommandCore * cc = (CommandCore *) ccInstance;
	Sleep(1000);
	cc->updateInfoCollectors();
}

CommandCore::CommandCore(Cbt_serverDlg * dlg)
{
	mainDlg = dlg;
	infoCollectors[0] = infoCollectors[1] = NULL;
	executeThread = false;
	thread = NULL;
}

CommandCore::~CommandCore(void)
{
}

void CommandCore::updateInfoCollectors()
{
	CString lastICValue[2], currICValue[2], tmp;
	_log(L"Info collector thread created\n");

	while (executeThread)
	{
		for (int ic = 0; ic < 2; ic++)
		{
			// update the field ONLY if the value's changed
			if (infoCollectors[ic])
				currICValue[ic] = infoCollectors[ic]->execute();
			else
				currICValue[ic] = L"";

			if (currICValue[ic] != lastICValue[ic])
			{
				lastICValue[ic] = currICValue[ic];
				tmp.Format(L"FIELD%d: %s", ic+1, currICValue[ic]); // we have FIELD1 and FIELD2, not 0 and 1, hence ic+1
				mainDlg->sendBTMessage(tmp.GetBuffer());
			}
		}

		Sleep(1000);
	}

	_log(L"Exiting info collector thread\n");
}

void CommandCore::onConnectionClosed() // unload the current profile and end the infoCollector thread
{
	currentProfile = L"";
	actions.clear();

	if (thread) SuspendThread(thread); // TODO: add proper synchronization

	if (infoCollectors[0]) { delete infoCollectors[0];  infoCollectors[0] = NULL; }
	if (infoCollectors[1]) { delete infoCollectors[1];  infoCollectors[1] = NULL; }

	executeThread = false;
	if (thread) ResumeThread(thread);

	WaitForSingleObject( thread, 2000 ); // note: it should take less than a second for the IC thread to finish

	thread = NULL;

	ModuleManager::get()->notifyPluginsOnProfileClosed();
}

bool CommandCore::loadProfile(CString name)
{
	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	FILE * f = _wfopen(appdata + L"\\Brm BT Remote\\profiles\\"+name+L".profile", L"rt, ccs=UNICODE");
	if (!f) return false;

	actions.clear();

	if (!thread) // a new connection requests to load a profile -> we need to create the info collector thread
	{
		executeThread = true;
		thread = (HANDLE) _beginthread(infoCollectorThread, 0, this);
	}

	SuspendThread(thread); // suspend thread to avoid possible crash between deleting infoCollector and setting its pointer to NULL

	if (infoCollectors[0]) { delete infoCollectors[0];  infoCollectors[0] = NULL; }
	if (infoCollectors[1]) { delete infoCollectors[1];  infoCollectors[1] = NULL; }

	currentProfile = name;
	wchar_t buf[2048], * substr;
	CString tmp, response; // this will be send to the client

	response = L"PROFILE: "+name+L"\n";

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

			tmp.Format(L"Key:\t%d\t%s\t%s\n", keyCode, label, appearance);
			response+= tmp;
			
			actions.insert(std::pair<int, actionAndParams>(keyCode, actionAndParams(ModuleManager::get()->getAction(action), params)));
		}
		else if (wcscmp(substr, L"Layout:") == 0)
		{
			int layoutNum;
			substr = wcstok_e(NULL, '\t');
			if (!swscanf(substr, L"%d", &layoutNum)) layoutNum = 1;

			tmp.Format(L"Layout:\t%d\n", layoutNum);
			response += tmp;
		}
		else if (wcscmp(substr, L"InfoCollector1:") == 0 || wcscmp(substr, L"InfoCollector2:") == 0)
		{
			wchar_t * num = substr+13;
			wchar_t * infoCollector, * params;
			if (!(infoCollector = wcstok_e(NULL, '\t'))) continue;
			if (!(params = wcstok_e(NULL, '\t'))) continue;

			if (num[0] == '1')
			{
				infoCollectors[0] = new infoCollectorAndParams(ModuleManager::get()->getInfoCollector(infoCollector), params);
			}
			else
			{
				infoCollectors[1] = new infoCollectorAndParams(ModuleManager::get()->getInfoCollector(infoCollector), params);
			}
		}
	}

	fclose(f);
	mainDlg->sendBTMessage(response.GetBuffer());

	ResumeThread(thread);

	return true;
}

bool CommandCore::receiveCommand(wchar_t * cmd)
{
	if (memcmp(cmd, L"UP:", 3*2) == 0 || memcmp(cmd, L"DN:", 3*2) == 0)
	{
		int keyCode;
		swscanf(cmd+3, L"%d", &keyCode);
		std::map<int, actionAndParams>::iterator it = actions.find(keyCode);
		if (it != actions.end()) (*it).second.execute(memcmp(cmd, L"DN:", 3*2) == 0);
		return true;
	}
	else if (memcmp(cmd, L"LIST PROFILES", 13*2) == 0)
	{
		CString msg(L"PROFILE LIST:\n"), tmp;
		int count = mainDlg->profileList.GetCount();
		
		for (int i = 0; i < count; i++)
		{
			mainDlg->profileList.GetText(i, tmp);
			msg += tmp+L"\n";
		}
		mainDlg->sendBTMessage(msg.GetBuffer());
	}
	else if (memcmp(cmd, L"LOAD PROFILE", 12*2) == 0)
	{
		if (!loadProfile(cmd+13)) mainDlg->sendBTMessage(L"PROFILE ERROR");
	}

	return false;
}
