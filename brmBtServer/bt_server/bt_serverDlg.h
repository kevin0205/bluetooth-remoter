// bt_serverDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"

#include "ConsoleDlg.h"
#include "bt_comm.h"
#include "ModuleManager.h"
#include "ProfileList.h"
#include "ProfileEditor.h"
#include "CommandCore.h"

#define WM_TRAY_MESSAGE (WM_USER + 1) // used for initializing tray icon


// class prototypes, such as these, are mandatory when 2 classes mutually include each other
class BtComm;
class ProfileList;
class ProfileEditor;
class CommandCore;


class Cbt_serverDlg : public CDialog
{
public:
	enum { IDD = IDD_BT_SERVER_DIALOG };
	Cbt_serverDlg(CWnd* pParent = NULL);
	~Cbt_serverDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	BtComm * comm; // Bluetooth server
	HICON m_hIcon;
	static Cbt_serverDlg * m_instance; // this dialog needs to be accessed globally and acts kind of like a singleton
	bool m_minimized; // keeps the minimization state of a window
	bool m_serverStarted; // was the Bluetooth server started successfully?

	// stuff for tray icon
	CMenu m_menu;
	NOTIFYICONDATA m_TrayData;
	void OnTrayLButtonDown(CPoint pt);
	void OnTrayRButtonDown(CPoint pt);
	void initTray();
	afx_msg void OnTrayAbout();
	afx_msg void OnTrayOptions();
	afx_msg void OnTrayInstallmobileapp();
	afx_msg void OnTrayListofkeys();
	afx_msg void OnTrayExit();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	void minimize();
	void restore();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose(); // minimizes the dialog to tray instead of closing it
	virtual void OnCancel(); // prevents the "escape" key from closing the app
	afx_msg void OnDestroy(); // disposes of the tray icon + stops the Bluetooth server
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	// -------------------

	virtual int OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	ProfileList * m_profileList;
	CommandCore * m_commandCore;
public:
	ProfileEditor * m_profileEditor; // needs to be public, as ProfileList needs to enable and disable profile editor's controls

	// MFC controls
	CListCtrl keyList;
	CListBox profileList;
	ConsoleDlg console;
	CComboBox comboInfoCollector1;
	CEdit editInfoCollectorParam1;
	CComboBox comboInfoCollector2;
	CEdit editInfoCollectorParam2;

	// MFC event handlers
	afx_msg void OnBnClickedAssignKey();
	afx_msg void OnBnClickedNewprofilebtn();
	afx_msg void OnBnClickedRenameprofilebtn();
	afx_msg void OnBnClickedDeleteprofilebtn();
	afx_msg void OnProfileDblClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedEditKey();
	afx_msg void OnBnClickedDeleteKey();
	afx_msg void OnProfileSelChange();
	afx_msg void OnBnClickedparam1help();
	afx_msg void OnBnClickedparam2help();
	afx_msg void OnBnClickedRadio();
	afx_msg void OnKillfocusEditParams();
	afx_msg void OnCbnSelchangeInfoCollector1();
	afx_msg void OnCbnSelchangeInfoCollector2();
	afx_msg void OnBnClickedConsolebtn();

	static Cbt_serverDlg * instance() { return m_instance; }

	void receiveBTMessage(wchar_t * msg);
	void sendBTMessage(wchar_t * msg);
	void BTDisconnected();
	afx_msg void OnBnClickedButton1();
};