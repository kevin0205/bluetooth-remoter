#pragma once
#include <map>
#include <atlstr.h>

class PluginBase;


struct executableAction
{
	bool hasParams;
	CString paramHelp;
	PluginBase * plugin;
	int actionId;

	executableAction(bool hasParams, wchar_t * paramHelp, PluginBase * plugin, int actionId)
	{
		this->hasParams = hasParams;
		this->paramHelp = paramHelp;
		this->plugin = plugin;
		this->actionId = actionId;
	}
};


class PluginBase
{
protected:
	std::map<CString, executableAction> actions;
	std::map<CString, executableAction> infoCollectors;

public:
	PluginBase() { }
	~PluginBase() { }

	virtual std::map<CString, executableAction> * getActionList()
	{
		return &actions;
	}

	virtual void callAction(int actionId, CString params, bool keyDown)
	{
	}

	virtual std::map<CString, executableAction> * getInfoCollectorList()
	{
		return &infoCollectors;
	}

	virtual CString callInfoCollector(int infoCollectorId, CString params)
	{
		return L"";
	}
	
	virtual void onProfileClosed()
	{
	}
};