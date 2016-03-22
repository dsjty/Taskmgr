
// MyTaskmgrDlg.cpp : ʵ���ļ�

//�����������ĿҪ��
//�������ܣ�
//1. ����������Ϣ
//2. �����߳���Ϣ
//3. ����ģ����Ϣ
//4. CPU��Ϣ���ڴ���Ϣ
//5. ����������Ϣ
//6. �������̣��̣߳�ж��ģ�飬�ػ�������������
//7. �ϰ��
//8. ���Ӵ��ڣ�����ͼ
//
//��չ���ܣ�
//1. ���������������رյȲ���
//2. ע��DLL
//3. ע�� ֧�� X86��X64
//4. inline hook  ��������
//5. ���๦�������а���



#include "stdafx.h"
#include "MyTaskmgr.h"
#include "MyTaskmgrDlg.h"
#include "afxdialogex.h"
#include <windows.h>

#pragma comment(lib,"User32.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CMyTaskmgrDlg �Ի���



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


// CMyTaskmgrDlg ��Ϣ�������

BOOL CMyTaskmgrDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_ORDER.InsertItem(0, L"Ӧ�ó���");						//1,��������ҳ;���󶨱���;
	m_ORDER.InsertItem(1, L"����");
	m_ORDER.InsertItem(2, L"����");

	m_Page1.Create(MAKEINTRESOURCE(IDD_PAGE1), &m_ORDER);	//2,�����µĶԻ���,�½����Ӧ�����󶨵���ҳ��;
	m_Page2.Create(MAKEINTRESOURCE(IDD_PAGE2), &m_ORDER);
	m_Page3.Create(MAKEINTRESOURCE(IDD_PAGE3), &m_ORDER);

	CRect Crect;											//3  ����λ��;
	m_ORDER.GetClientRect(Crect);
	//Crect.DeflateRect(0, 20, 0, 0);
	Crect.top += 22;
	Crect.left += 1;
 	m_Page1.MoveWindow(Crect);
	m_Page1.ShowWindow(SW_SHOW);
	m_Page1.ListInitialize();								//4,��ʼ����һ��ҳ;
	m_Page1.ShowList();
	//m_Page1.SetTimer(0, 1000, NULL);

	m_Page2.ListInitialize();
	//m_Page2.SetTimer(0, 1000, NULL);

	m_Page3.ListInitialize();

	/*********״̬��*********/
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyTaskmgrDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyTaskmgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMyTaskmgrDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CRect Crect;		//3  ����λ��;
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
		m_Page2.ShowList();				//ʵ���Զ�ˢ�º�ɾ��;
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
// 	// TODO:  �ڴ˴������Ϣ����������
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
	// TODO:  �ڴ���������������
}


void CMyTaskmgrDlg::OnSleep()
{
	SystemShutDown();
	if (!ExitWindowsEx(EWX_SHUTDOWN, 0))
		ErrorInfo();
	// TODO:  �ڴ���������������
}

BOOL CMyTaskmgrDlg::SystemShutDown()
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

	if (GetLastError != ERROR_SUCCESS)
		return FALSE;
}


void CMyTaskmgrDlg::OnLogoff()
{
	SystemShutDown();
	if (!ExitWindowsEx(EWX_LOGOFF, 0))
		ErrorInfo();
	// TODO:  �ڴ���������������
}


void CMyTaskmgrDlg::OnPoweroff()
{
	SystemShutDown();
	if (!ExitWindowsEx(EWX_POWEROFF, 0))
		ErrorInfo();
	// TODO:  �ڴ���������������
}
