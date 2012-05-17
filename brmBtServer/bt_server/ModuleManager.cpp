#include "StdAfx.h"
#include "ModuleManager.h"

/**
 * like wcstok, but empty string aware
 */
wchar_t * wcstok_e (wchar_t * str, wchar_t delim)
{
	static wchar_t * tmp;
	static int i, start;
	static bool reachedEnd;

	if (str)
	{
		start = i = 0;
		tmp = str;
		reachedEnd = false;
	}
	else
	{
		if (reachedEnd) return NULL;
		start = ++i;
	}

	while (tmp[i] != 0 && tmp[i] != delim) i++;
	
	if (tmp[i] == 0) reachedEnd = true;
	tmp[i] = 0;

	return tmp + start;
}



ModuleManager * ModuleManager::instance = NULL;

void ModuleManager::loadPlugins()
{
	WIN32_FIND_DATA f;
	HANDLE h = FindFirstFile(L"plugins\\*.dll", &f);
	if(h != INVALID_HANDLE_VALUE)
	{
		#ifdef DEBUG
			CString pluginDir(L"plugins_debug\\");
		#else
			CString pluginDir(L"plugins\\");
		#endif

		do
		{
			HINSTANCE hinst = LoadLibrary(pluginDir+f.cFileName);
			if (hinst && GetProcAddress(hinst, "loadPlugin"))
			{
				PluginLoader loadPlugin = (PluginLoader)GetProcAddress(hinst, "loadPlugin");
				PluginBase * plugin = loadPlugin();

				if (!plugin->getActionList()->empty())
					actions.insert(plugin->getActionList()->begin(), plugin->getActionList()->end());
				if (!plugin->getInfoCollectorList()->empty())
					infoCollectors.insert(plugin->getInfoCollectorList()->begin(), plugin->getInfoCollectorList()->end());

				plugins.push_back(PluginDLL(hinst, plugin));
			}
			else if (hinst)
			{
				FreeLibrary(hinst);
			}
		} while(FindNextFile(h, &f));
	}
}

void ModuleManager::unloadPlugins()
{
	actions.clear();
	infoCollectors.clear();

	for (std::vector<PluginDLL>::iterator it = plugins.begin(); it != plugins.end(); it++)
	{
		delete ((*it).plugin);
		FreeLibrary((*it).dllInstance);
	}

	plugins.clear();
}

ModuleManager::ModuleManager(void)
{
	instance = this;

	infoCollectors.insert(std::pair<CString, executableAction>(L"(none)", executableAction(false, L"", NULL, 0)) );

	loadPlugins();

	keys.insert(std::pair<int, CString>(-1, L"Up"));    keys.insert(std::pair<int, CString>(-2, L"Down"));
	keys.insert(std::pair<int, CString>(-3, L"Left"));  keys.insert(std::pair<int, CString>(-4, L"Right"));
	keys.insert(std::pair<int, CString>(-5, L"OK"));    keys.insert(std::pair<int, CString>(-8, L"C"));
	keys.insert(std::pair<int, CString>(-10, L"Accept call"));
	// we'll deal with numbers and other characters in getKeyName
	/*wchar_t tmp[] = L"x";
	for (int i = 48; i<=57; i++) // keys 0-9
	{
		tmp[0] = i;
		keys.insert(std::pair<int, CString>(i, tmp));
	}
	keys.insert(std::pair<int, CString>(42, L"*"));  keys.insert(std::pair<int, CString>(35, L"#"));*/

	appearances.push_back(L"Normal");
	appearances.push_back(L"Green");
	appearances.push_back(L"Teal");
	appearances.push_back(L"Orange");
	appearances.push_back(L"Yellow");
}

ModuleManager::~ModuleManager(void)
{
	unloadPlugins();
}

ModuleManager * ModuleManager::get()
{
	return instance ? instance : new ModuleManager();
}

void ModuleManager::fillDropdownActions(CComboBox & comboBox)
{
	for (std::map<CString, executableAction>::iterator it = actions.begin(); it != actions.end(); it++)
	{
		comboBox.AddString((*it).first);
	}
}

void ModuleManager::fillDropdownInfoCollectors(CComboBox & comboBox)
{
	for (std::map<CString, executableAction>::iterator it = infoCollectors.begin(); it != infoCollectors.end(); it++)
	{
		comboBox.AddString((*it).first);
	}
}

void ModuleManager::fillDropdownAppearances(CComboBox & comboBox)
{
	for (std::vector<CString>::iterator it = appearances.begin(); it != appearances.end(); it++)
	{
		comboBox.AddString(*it);
	}
}

executableAction * ModuleManager::getAction(CString name)
{
	if (actions.find(name) == actions.end()) return NULL;
	return &((*(actions.find(name))).second);
}

executableAction * ModuleManager::getInfoCollector(CString name)
{
	if (infoCollectors.find(name) == infoCollectors.end()) return NULL;
	return &((*(infoCollectors.find(name))).second);
}

CString ModuleManager::getKeyName(int keyCode)
{
	if (keyCode > 0) // this is for all the characters including numbers..
	{
		/*wchar_t tmp [2];
		tmp[0] = keyCode;
		tmp[1] = 0;*/
		CString tmp;
		tmp.Format(L"%c", keyCode);
		return tmp;//CString(tmp);
	}

	if (keys.find(keyCode) == keys.end())
	{
		CString str;
		str.Format(L"Key #%d", keyCode);
		return str;
	}
	return (*(keys.find(keyCode))).second;
}

void ModuleManager::notifyPluginsOnProfileClosed()
{
	for (std::vector<PluginDLL>::iterator it = plugins.begin(); it != plugins.end(); it++)
	{
		(*it).plugin->onProfileClosed();
	}
}