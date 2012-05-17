#pragma once
#include <map>
#include <vector>
#include "PluginBase.h"

wchar_t * wcstok_e (wchar_t * str, wchar_t delim); // like wcstok, but empty string aware

struct PluginDLL
{
	HINSTANCE dllInstance;
	PluginBase * plugin;
	PluginDLL (HINSTANCE dllInstance, PluginBase * plugin)
	{
		this->dllInstance = dllInstance;
		this->plugin = plugin;
	}
};

typedef PluginBase *(*PluginLoader)(); // create a function pointer type that points to CreatePlugin

class ModuleManager // singleton
{
private:
	static ModuleManager * instance;
	ModuleManager(void);
protected:
	std::map<CString, executableAction> actions;
	std::map<CString, executableAction> infoCollectors;
	std::map<int, CString> keys;
	std::vector<CString> appearances;
	std::vector<PluginDLL> plugins;
public:
	void loadPlugins();
	void unloadPlugins();
public:
	static ModuleManager * get();
	~ModuleManager(void);

	void init() {}
	void fillDropdownActions(CComboBox & comboBox);
	void fillDropdownInfoCollectors(CComboBox & comboBox);
	void fillDropdownAppearances(CComboBox & comboBox);
	void notifyPluginsOnProfileClosed();

	executableAction * getAction(CString name);
	executableAction * getInfoCollector(CString name);
	CString getKeyName(int keyCode);

};
