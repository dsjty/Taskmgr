
// MyTaskmgrDlg.cpp : 实现文件

//任务管理器项目要求：
//基础功能：
//1. 遍历进程信息
//2. 遍历线程信息
//3. 遍历模块信息
//4. CPU信息、内存信息
//5. 遍历窗口信息
//6. 结束进程，线程，卸载模块，关机、开机、重启
//7. 老板键
//8. 监视窗口，线形图
//
//扩展功能：
//1. 遍历服务，启动，关闭等操作
//2. 注入DLL
//3. 注入 支持 X86，X64
//4. inline hook  保护进程
//5. 更多功能请自行安排



#include "stdafx.h"
#include "MyTaskmgr.h"
#include "MyTaskmgrDlg.h"
#include "afxdialogex.h"
#include <windows.h>

#pragma comment(lib,"User32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyTaskmgrDlg 对话框



CMyTaskmgrDlg::CMyTaskmgrDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyTaskmgrDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyTaskmgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_ORDER);
}

BEGIN_MESSAGE_MAP(CMyTaskmgrDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMyTaskmgrDlg::OnTcnSelchangeTab1)
//	ON_WM_SIZE()
ON_WM_SIZE()
ON_COMMAND(ID_32772, &CMyTaskmgrDlg::On32772)
ON_COMMAND(ID_Sleep, &CMyTaskmgrDlg::OnSleep)
ON_COMMAND(ID_LogOff, &CMyTaskmgrDlg::OnLogoff)
ON_COMMAND(ID_POWEROFF, &CMyTaskmgrDlg::OnPoweroff)
END_MESSAGE_MAP()


// CMyTaskmgrDlg 消息处理程序

BOOL CMyTaskmgrDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_ORDER.InsertItem(0, L"应用程序");						//1,创建属性页;并绑定变量;
	m_ORDER.InsertItem(1, L"进程");
	m_ORDER.InsertItem(2, L"服务");

	m_Page1.Create(MAKEINTRESOURCE(IDD_PAGE1), &m_ORDER);	//2,创建新的对话框,新建相对应的类后绑定到主页面;
	m_Page2.Create(MAKEINTRESOURCE(IDD_PAGE2), &m_ORDER);
	m_Page3.Create(MAKEINTRESOURCE(IDD_PAGE3), &m_ORDER);

	CRect Crect;											//3  调整位置;
	m_ORDER.GetClientRect(Crect);
	//Crect.DeflateRect(0, 20, 0, 0);
	Crect.top += 22;
	Crect.left += 1;
 	m_Page1.MoveWindow(Crect);
	m_Page1.ShowWindow(SW_SHOW);
	m_Page1.ListInitialize();								//4,初始化第一个页;
	m_Page1.ShowList();
	//m_Page1.SetTimer(0, 1000, NULL);

	m_Page2.ListInitialize();
	//m_Page2.SetTimer(0, 1000, NULL);

	m_Page3.ListInitialize();

	/*********状态栏*********/
	RaisePrivileges2();


	return TRUE;
}

void CMyTaskmgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyTaskmgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyTaskmgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyTaskmgrDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此添加控件通知处理程序代码
	CRect Crect;		//3  调整位置;
	m_ORDER.GetClientRect(Crect);
	//Crect.DeflateRect(0, 20, 0, 0);
	Crect.top += 22;
	Crect.left += 1;

	*pResult = 0;
	switch (m_ORDER.GetCurSel())
	{
	case 0:
		m_Page1.ShowWindow(SW_SHOW);		
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page1.MoveWindow(Crect);
		m_Page1.ShowList();
		break;
	case 1:
		m_Page1.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page2.ShowWindow(SW_SHOW);
		m_Page2.MoveWindow(Crect);
		m_Page2.ShowList();				//实现自动刷新后删除;
		break;
	case 2:
		m_Page1.ShowWindow(SW_HIDE);
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_SHOW);
		m_Page3.MoveWindow(Crect);
		m_Page3.ShowList();
		break; 
	default:
		break;
	}
	
}

extern BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);

void CMyTaskmgrDlg::OnSize(UINT nType, int cx, int cy)
{
// 	CDialogEx::OnSize(nType, cx, cy);
// 	// TODO:  在此处添加消息处理程序代码
// 	if (0!=this->IsWindowVisible())
// 	{
// 		RECT rect;
// 		GetClientRect(&rect);
// 		EnumChildWindows(GetSafeHwnd(), EnumChildProc, (LPARAM)&rect);
// 	}
}

#define ID_FIRSTCHILD  100   
#define ID_SECONDCHILD 101   
#define ID_THIRDCHILD  102 

// BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
// {
// 	LPRECT lpRect;
// 	int i = 0, idChild;
// 	idChild = GetWindowLong(hWnd, GWL_ID);
// 	if (idChild == ID_FIRSTCHILD)
// 		i = 0;
// 	else if (idChild == ID_SECONDCHILD)
// 		i = 1;
// 	else
// 		i = 2;
// 	lpRect = (LPRECT)lParam;
// 	MoveWindow(hWnd, (lpRect->right / 3)*i, 0, lpRect->right / 3, lpRect->bottom, TRUE);
// 
// 	ShowWindow(hWnd, SW_SHOW);
// 	return TRUE;
// }


void CMyTaskmgrDlg::On32772()
{
	DestroyWindow();
	// TODO:  在此添加命令处理程序代码
}


void CMyTaskmgrDlg::OnSleep()
{
	SystemShutDown();
	if (!ExitWindowsEx(EWX_SHUTDOWN, 0))
		ErrorInfo();
	// TODO:  在此添加命令处理程序代码
}

BOOL CMyTaskmgrDlg::SystemShutDown()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	//打开一个进程的访问令牌;getcurrentprocess获得本进程句柄;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		ErrorInfo();
	//修改进程权限;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;//赋给本进程特权;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;	

	//通知NT修改本进程的权利;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0x10, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError != ERROR_SUCCESS)
		return FALSE;
}


void CMyTaskmgrDlg::OnLogoff()
{
	SystemShutDown();
	if (!ExitWindowsEx(EWX_LOGOFF, 0))
		ErrorInfo();
	// TODO:  在此添加命令处理程序代码
}


void CMyTaskmgrDlg::OnPoweroff()
{
	SystemShutDown();
	if (!ExitWindowsEx(EWX_POWEROFF, 0))
		ErrorInfo();
	// TODO:  在此添加命令处理程序代码
}
