
// MyTaskmgr.cpp : ����Ӧ�ó��������Ϊ��
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


// CMyTaskmgrApp ����

CMyTaskmgrApp::CMyTaskmgrApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMyTaskmgrApp ����

CMyTaskmgrApp theApp;


// CMyTaskmgrApp ��ʼ��

BOOL CMyTaskmgrApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CMyTaskmgrDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


//************************************
// ������:    ErrorInfo
// FullName:  ErrorInfo
// Access:    public 
// ��������:   void
// ע��:	����ֵ����
//************************************
void ErrorInfo()
{
	LPVOID lpMsgBuf = nullptr;
	TCHAR szMessage[128];
	DWORD dw = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
	StringCchPrintfW(szMessage, 128, L"Error_0x%08x:%s", dw, lpMsgBuf);
	MessageBoxW(NULL, szMessage, _T("������!"), MB_OK);
	LocalFree(lpMsgBuf);
}

//************************************
// ������:    RaisePrivileges
// FullName:  RaisePrivileges
// Access:    public 
// ��������:   void
// ע��:	��ȡȨ��;
//************************************
void RaisePrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	//��һ�����̵ķ�������;getcurrentprocess��ñ����̾��;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		ErrorInfo();
	//�޸Ľ���Ȩ��;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;//������������Ȩ;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	//֪ͨNT�޸ı����̵�Ȩ��;
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
		if (!OpenProcessToken(Handle, TOKEN_QUERY, &hTokens))	//��ȡ���̷�������
		{
			QUIT = FALSE;
			ErrorInfo();
			__leave;
		}
		if (!GetTokenInformation(hTokens, TokenUser, pTokenUser, dwSize, &dwSize))	//��һ�����л�ȡ��С
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
		if (!GetTokenInformation(hTokens,TokenUser,pTokenUser,dwSize,&dwSize))		//�ڶ����ǻ�ȡsid;
		{
			QUIT = FALSE;
			__leave;
		}
		if (0!=LookupAccountSid(nullptr,pTokenUser->User.Sid,tcUserName,&szNamesize,dwDomain,&dwSize,&SidNameUse))	//��sid��ѯ�û���
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