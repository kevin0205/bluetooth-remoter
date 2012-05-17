// bt_serverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bt_server.h"
#include "bt_serverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//
// About Dialog
//
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedHomepage();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_HOMEPAGE, &CAboutDlg::OnBnClickedHomepage)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedHomepage()
{
	ShellExecute(NULL, NULL, L"http://brmbt.sourceforge.net", L"", NULL, NULL);
}




//
// Main dialog
//

//
// MFC stuff + initialization
//
Cbt_serverDlg * Cbt_serverDlg::m_instance = NULL;

Cbt_serverDlg::Cbt_serverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cbt_serverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_instance = this;
}

Cbt_serverDlg::~Cbt_serverDlg()
{
	delete m_profileList;
	delete m_profileEditor;
	delete m_commandCore;
}

void Cbt_serverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST4, keyList);
	DDX_Control(pDX, IDC_LIST1, profileList);
	DDX_Control(pDX, IDC_COMBOVAL1, comboInfoCollector1);
	DDX_Control(pDX, IDC_EDITVAL1PARAMS, editInfoCollectorParam1);
	DDX_Control(pDX, IDC_COMBOVAL2, comboInfoCollector2);
	DDX_Control(pDX, IDC_EDITVAL2PARAMS, editInfoCollectorParam2);
}

BEGIN_MESSAGE_MAP(Cbt_serverDlg, CDialog)
	//{{AFX_MSG_MAP(Cbt_serverDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TRAY_MESSAGE,OnTrayNotify)
	ON_COMMAND(ID_BRM_EXIT, &Cbt_serverDlg::OnTrayExit)
	ON_COMMAND(ID_BRM_OPTIONS, &Cbt_serverDlg::OnTrayOptions)
	ON_COMMAND(ID_BRM_ABOUT, &Cbt_serverDlg::OnTrayAbout)
	ON_COMMAND(ID_TRAY_INSTALLMOBILEAPP, &Cbt_serverDlg::OnTrayInstallmobileapp)
	ON_COMMAND(ID_TRAY_LISTOFKEYS, &Cbt_serverDlg::OnTrayListofkeys)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTONASSIGNKEY, &Cbt_serverDlg::OnBnClickedAssignKey)
	ON_BN_CLICKED(IDC_NEWPROFILEBTN, &Cbt_serverDlg::OnBnClickedNewprofilebtn)
	ON_BN_CLICKED(IDC_RENAMEPROFILEBTN, &Cbt_serverDlg::OnBnClickedRenameprofilebtn)
	ON_BN_CLICKED(IDC_DELETEPROFILEBTN, &Cbt_serverDlg::OnBnClickedDeleteprofilebtn)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST4, &Cbt_serverDlg::OnProfileDblClick)
	ON_BN_CLICKED(IDC_BUTTONEDITKEY, &Cbt_serverDlg::OnBnClickedEditKey)
	ON_BN_CLICKED(IDC_BUTTONDELETEKEY, &Cbt_serverDlg::OnBnClickedDeleteKey)
	ON_LBN_SELCHANGE(IDC_LIST1, &Cbt_serverDlg::OnProfileSelChange)
	ON_BN_CLICKED(IDC_BUTTONVAL1PARAMHELP, &Cbt_serverDlg::OnBnClickedparam1help)
	ON_BN_CLICKED(IDC_BUTTONVAL2PARAMHELP, &Cbt_serverDlg::OnBnClickedparam2help)
	ON_BN_CLICKED(IDC_RADIO1, &Cbt_serverDlg::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, &Cbt_serverDlg::OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO3, &Cbt_serverDlg::OnBnClickedRadio)
	ON_EN_KILLFOCUS(IDC_EDITVAL1PARAMS, &Cbt_serverDlg::OnKillfocusEditParams)
	ON_EN_KILLFOCUS(IDC_EDITVAL2PARAMS, &Cbt_serverDlg::OnKillfocusEditParams)
	ON_CBN_SELCHANGE(IDC_COMBOVAL1, &Cbt_serverDlg::OnCbnSelchangeInfoCollector1)
	ON_CBN_SELCHANGE(IDC_COMBOVAL2, &Cbt_serverDlg::OnCbnSelchangeInfoCollector2)
	ON_BN_CLICKED(IDC_CONSOLEBTN, &Cbt_serverDlg::OnBnClickedConsolebtn)
	ON_WM_WINDOWPOSCHANGING()
	ON_BN_CLICKED(IDC_BUTTON1, &Cbt_serverDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// Cbt_serverDlg message handlers
void Cbt_serverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void Cbt_serverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cbt_serverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// initialization
BOOL Cbt_serverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	console.Create(ConsoleDlg::IDD, GetDesktopWindow());

	comm = new BtComm();
	m_serverStarted = comm->startServer();

	if (!m_serverStarted) ((CStatic *) GetDlgItem(IDC_SERVER_ERROR))->ShowWindow(true);

	initTray(); // we need to initialize the tray icon before ProfileList - ProfileList automatically creates Brm BT folder in AppData and the 1st run tray balloon appears only if it doesn't exist

	ModuleManager::get()->init();

	keyList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	keyList.InsertColumn(0, L"Key", LVCFMT_LEFT, 50);
	keyList.InsertColumn(1, L"Label", LVCFMT_LEFT, 72);
	keyList.InsertColumn(2, L"Action", LVCFMT_LEFT, 100);
	keyList.InsertColumn(3, L"Appearance", LVCFMT_LEFT, 70);

	ModuleManager::get()->fillDropdownInfoCollectors(comboInfoCollector1);
	comboInfoCollector1.SetCurSel(0);
	ModuleManager::get()->fillDropdownInfoCollectors(comboInfoCollector2);
	comboInfoCollector2.SetCurSel(0);

	m_profileList = new ProfileList(this);
	m_profileEditor = new ProfileEditor(this);
	m_commandCore = new CommandCore(this);

	m_profileEditor->disable();

	return TRUE;  // return TRUE  unless you set the focus to a control
}




//
// tray icon stuff begins here ...
//
afx_msg LRESULT Cbt_serverDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID = (UINT) wParam; 
	UINT uMsg = (UINT) lParam; 

	if (uID != 1) return 0;

	CPoint pt;

	switch (uMsg)
	{
	case WM_LBUTTONUP:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDown(pt);
		break;

	case WM_RBUTTONUP:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		OnTrayRButtonDown(pt);
		break;
	}

	return 0;
}

void Cbt_serverDlg::OnTrayLButtonDown(CPoint pt)
{
	if (m_minimized) restore(); else minimize();
}

void Cbt_serverDlg::OnTrayRButtonDown(CPoint pt)
{
	m_menu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
}

void Cbt_serverDlg::OnTrayExit()
{
	restore(); // window order gets messed up if there's no window open and the app closes
	PostQuitMessage(0);//AfxGetMainWnd()->SendMessage(WM_CLOSE);
}

void Cbt_serverDlg::OnTrayOptions()
{
	restore();
}

void Cbt_serverDlg::OnTrayAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void Cbt_serverDlg::OnTrayInstallmobileapp() // this just opens the directory which contains the mobile app .jar
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	wchar_t tmp[512];
	GetFullPathName(L"mobile app", 511, tmp,NULL);
	ShellExecute(NULL, NULL, tmp, L"", NULL, SW_SHOWNORMAL);
}

void Cbt_serverDlg::OnTrayListofkeys()
{
	ShellExecute(NULL, L"open", L"Brm BT Key Reference.png", L"", NULL, NULL);
}

void Cbt_serverDlg::initTray()
{
    m_TrayData.cbSize = sizeof(NOTIFYICONDATA);
    m_TrayData.hWnd  = this->m_hWnd; 
    m_TrayData.uID = 1;
    m_TrayData.uCallbackMessage  = WM_TRAY_MESSAGE;
    m_TrayData.hIcon = this->m_hIcon;
    
    wcscpy_s(m_TrayData.szTip, 128, L"Brm Bluetooth Server");
    m_TrayData.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;

	wchar_t adPath[256];
	GetEnvironmentVariable(L"appdata", adPath, 200);
	CString appdata(adPath);

	if (GetFileAttributes(appdata+L"\\Brm BT Remote") == INVALID_FILE_ATTRIBUTES)
	{
		m_TrayData.uFlags |= NIF_INFO;
		m_TrayData.dwInfoFlags = NIIF_INFO;
		m_TrayData.uTimeout = 20;
		
		wcscpy( m_TrayData.szInfoTitle, L"Hello");
		if (m_serverStarted) wcscpy( m_TrayData.szInfo, L"You can configure Brm BT with this icon.\nTry connecting with your cell phone now.");
		else wcscpy( m_TrayData.szInfo, L"You can configure Brm BT with this icon.\nPlease make sure that you have the correct drivers installed and Bluetooth enabled.");
	}

    if (! m_menu.LoadMenu(IDR_MENU1) ) MessageBox(L"Unabled to load menu", L"Error");
    if (! Shell_NotifyIcon(NIM_ADD, &m_TrayData) ) MessageBox(L"Unable to set tray icon", L"Error");
	m_minimized = true;
}

void Cbt_serverDlg::minimize()
{
	m_minimized = true;
    //this->ShowWindow(SW_MINIMIZE);
    ShowWindow(SW_HIDE);
}

void Cbt_serverDlg::restore()
{
	m_minimized = false;
    ShowWindow(SW_SHOW);
    //ShowWindow(SW_RESTORE);
	SetForegroundWindow();
	//BringWindowToTop();
}

// minimizes to tray when clicked on the standard minimize butotn
// (not used anymore)
void Cbt_serverDlg::OnSize(UINT nType, int cx, int cy) 
{
    if (nType == SIZE_MINIMIZED) minimize();
    else CDialog::OnSize(nType, cx, cy);
}

void Cbt_serverDlg::OnDestroy() 
{
	comm->stopServer();
    Shell_NotifyIcon(NIM_DELETE, &m_TrayData);
    m_menu.DestroyMenu();
	CDialog::OnDestroy();
}

void Cbt_serverDlg::OnClose()
{
	minimize();
	//CDialog::OnClose();
}

void Cbt_serverDlg::OnCancel()
{
	minimize();
}

// hack to hide the main window before it's shown.. unfortunately, this is the cleanest workaround for DoModal()
void Cbt_serverDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	if (m_minimized) lpwndpos->flags &= ~SWP_SHOWWINDOW;

	CDialog::OnWindowPosChanging(lpwndpos);
}

//
// ... end of tray icon stuff
//



//
// event handlers for buttons, lists, edit fields and whatnot
// ...

// *profile list*

void Cbt_serverDlg::OnBnClickedNewprofilebtn()
{
	m_profileList->createNew();
}

void Cbt_serverDlg::OnBnClickedRenameprofilebtn()
{
	m_profileList->renameSelected();
}

void Cbt_serverDlg::OnBnClickedDeleteprofilebtn()
{
	m_profileList->deleteSelected();
}

void Cbt_serverDlg::OnProfileSelChange()
{
	m_profileEditor->clear();
	wchar_t buf[256];
	if (profileList.GetCurSel() == -1) return; // blank space was "selected" -> ignore it

	profileList.GetText(profileList.GetCurSel(), buf);
	if (m_profileEditor->loadProfile(buf)) m_profileEditor->enable();
	else
	{
		AfxMessageBox(L"Error loading selected profile", MB_ICONEXCLAMATION);
		m_profileEditor->disable();
	}
}


// *profile editor*

void Cbt_serverDlg::OnBnClickedAssignKey()
{
	m_profileEditor->assignKey();
}

void Cbt_serverDlg::OnBnClickedEditKey()
{
	m_profileEditor->editKey();
}

void Cbt_serverDlg::OnProfileDblClick(NMHDR *pNMHDR, LRESULT *pResult) // double click on a key binding
{
	m_profileEditor->editKey();
	*pResult = 0;
}

void Cbt_serverDlg::OnBnClickedDeleteKey()
{
	m_profileEditor->deleteKey();
}


// *profile auto saving on parameter change*

void Cbt_serverDlg::OnBnClickedRadio()
{
	m_profileEditor->saveProfile();
}

void Cbt_serverDlg::OnKillfocusEditParams()
{
	m_profileEditor->saveProfile();
}

void Cbt_serverDlg::OnCbnSelchangeInfoCollector1()
{
	editInfoCollectorParam1.SetWindowTextW(L"");
	m_profileEditor->saveProfile();
}

void Cbt_serverDlg::OnCbnSelchangeInfoCollector2()
{
	editInfoCollectorParam2.SetWindowTextW(L"");
	m_profileEditor->saveProfile();
}


// *info collector help buttons*

void Cbt_serverDlg::OnBnClickedparam1help()
{
	if (comboInfoCollector1.GetCurSel() < 1) return;
	wchar_t buf [256];
	comboInfoCollector1.GetWindowTextW(buf, 256);
	executableAction * collector = ModuleManager::get()->getInfoCollector(buf);
	if (collector) AfxMessageBox(collector->paramHelp, MB_ICONINFORMATION);
	else AfxMessageBox(L"No such info collector");
}

void Cbt_serverDlg::OnBnClickedparam2help()
{
	if (comboInfoCollector2.GetCurSel() < 1) return;
	wchar_t buf [256];
	comboInfoCollector2.GetWindowTextW(buf, 256);
	executableAction * collector = ModuleManager::get()->getInfoCollector(buf);
	if (collector) AfxMessageBox(collector->paramHelp, MB_ICONINFORMATION);
	else AfxMessageBox(L"No such info collector");
}

// opens the debugging console
void Cbt_serverDlg::OnBnClickedConsolebtn()
{
	console.ShowWindow(SW_SHOW);
	CRect rect;

	GetClientRect(&rect);  ClientToScreen(rect);

	rect.SetRect(rect.TopLeft().x, rect.TopLeft().y,
		rect.TopLeft().x + 358, rect.TopLeft().y + 320 );

	rect.OffsetRect(-400, 0);
	console.MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());
	console.BringWindowToTop();
}


//
// and finally, a few Bluetooth functions
//

// the socket sends all the received bluetooth messages to this method
// if the "assign key" dialog is open, all UP and DN messages are dispatched to it, otherwise they are sent to the command core
void Cbt_serverDlg::receiveBTMessage(wchar_t * msg)
{
	if (m_profileEditor->assignKeyDlg.isActive() && (memcmp(msg, L"UP:", 3*2) == 0 || memcmp(msg, L"DN:", 3*2) == 0)) m_profileEditor->assignKeyDlg.receiveKey(msg);
	else
	{
		_log(msg);
		m_commandCore->receiveCommand(msg);
	}
}

// send a message via bluetooth
void Cbt_serverDlg::sendBTMessage(wchar_t * msg)
{
	comm->sendMessage(msg);
}

// called when the bluetooth connection is disconnected
void Cbt_serverDlg::BTDisconnected()
{
	m_commandCore->onConnectionClosed(); // command core unloads the current profile and stops the info collectors thread
}
void Cbt_serverDlg::OnBnClickedButton1()
{
	// CString s = ModuleManager::get()->getInfoCollector(L"PowerPoint: List of slides")->plugin->callInfoCollector(1, L"");
	// _log(s);
	ModuleManager::get()->unloadPlugins();
	AfxMessageBox(L"Plugins unloaded!\nClick OK when you've replaced the plugins");
	ModuleManager::get()->loadPlugins();
}
