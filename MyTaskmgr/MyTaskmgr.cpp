
// MyTaskmgr.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "MyTaskmgr.h"
#include "MyTaskmgrDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyTaskmgrApp

BEGIN_MESSAGE_MAP(CMyTaskmgrApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyTaskmgrApp 构造

CMyTaskmgrApp::CMyTaskmgrApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMyTaskmgrApp 对象

CMyTaskmgrApp theApp;


// CMyTaskmgrApp 初始化

BOOL CMyTaskmgrApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CMyTaskmgrDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


//************************************
// 函数名:    ErrorInfo
// FullName:  ErrorInfo
// Access:    public 
// 返回类型:   void
// 注释:	错误值反馈
//************************************
void ErrorInfo()
{
	LPVOID lpMsgBuf = nullptr;
	TCHAR szMessage[128];
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	StringCchPrintfW(szMessage, 128, L"Error_0x%08x:%s", dw, lpMsgBuf);
	MessageBoxW(NULL, szMessage, _T("出错啦!"), MB_OK);
	LocalFree(lpMsgBuf);
}

//************************************
// 函数名:    RaisePrivileges
// FullName:  RaisePrivileges
// Access:    public 
// 返回类型:   void
// 注释:	获取权限;
//************************************
void RaisePrivileges()
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
	if (hToken)
		CloseHandle(hToken);
}

void RaisePrivileges2()
{
	HANDLE hToken=NULL;
	TOKEN_PRIVILEGES tkp;
	LUID sedebugnameValue;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return ;
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{	
		CloseHandle(hToken);
		return ;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
		CloseHandle(hToken);
	return;
}


LPCTSTR GetProcessUserName(ULONG ProcID,HANDLE Handle)
{	
	if (Handle == NULL)
			return NULL;	

	static TCHAR tcUserName[MAX_PATH] = {0};
	DWORD szNamesize = MAX_PATH;
	HANDLE hTokens = nullptr;
	TCHAR dwDomain[MAX_PATH] = { 0 };
	DWORD dwDomSize = MAX_PATH;
	DWORD dwSize = 0;
	BOOL QUIT = FALSE;

	SID_NAME_USE SidNameUse;
	PTOKEN_USER pTokenUser=NULL;
	__try
	{
		if (!OpenProcessToken(Handle, TOKEN_QUERY, &hTokens))	//获取进程访问令牌
		{
			QUIT = FALSE;
			ErrorInfo();
			__leave;
		}
		if (!GetTokenInformation(hTokens, TokenUser, pTokenUser, dwSize, &dwSize))	//第一次运行获取大小
		{
			if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			{
				QUIT = FALSE;
				ErrorInfo();
				__leave;
			}
		}
		pTokenUser = (PTOKEN_USER)malloc(dwSize);
		if (pTokenUser == NULL)
		{
			ErrorInfo();
			QUIT = FALSE;
			__leave;
		}
		if (!GetTokenInformation(hTokens,TokenUser,pTokenUser,dwSize,&dwSize))		//第二次是获取sid;
		{
			QUIT = FALSE;
			__leave;
		}
		if (0!=LookupAccountSid(nullptr,pTokenUser->User.Sid,tcUserName,&szNamesize,dwDomain,&dwSize,&SidNameUse))	//用sid查询用户名
		{
			return tcUserName;
		}

	}
	__finally
	{
		if (pTokenUser != NULL)
			free(pTokenUser);
	}
	return NULL;
}