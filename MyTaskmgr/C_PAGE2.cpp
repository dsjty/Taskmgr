// C_PAGE2.cpp : 实现文件
//

#include "stdafx.h"
#include "C_PAGE2.h"
#include "afxdialogex.h"
#include<NTSecAPI.h>
#include <Psapi.h>
#include "MyTaskmgrDlg.h"
#include <string>
#include "MyTaskmgr.h"

#pragma comment(lib,"Version.lib")


typedef LONG NTSTATUS;
typedef LONG KPRIORITY;

#define STATUS_SUCCESS                  ((NTSTATUS)0x00000000L)   
#define STATUS_UNSUCCESSFUL             ((NTSTATUS)0xC0000001L)   
#define STATUS_NOT_IMPLEMENTED          ((NTSTATUS)0xC0000002L)   
#define STATUS_INVALID_INFO_CLASS       ((NTSTATUS)0xC0000003L)   
#define STATUS_INFO_LENGTH_MISMATCH     ((NTSTATUS)0xC0000004L)  

typedef DWORD(WINAPI *ZWQUERTSYSTEMINFORMATION)(DWORD, PVOID, DWORD, PDWORD);


typedef enum _KWAIT_REASON
{
	Executive,
	FreePage,
	PageIn,
	PoolAllocation,
	DelayExecution,
	Suspended,
	UserRequest,
	WrExecutive,
	WrFreePage,
	WrPageIn,
	WrPoolAllocation,
	WrDelayExecution,
	WrSuspended,
	WrUserRequest,
	WrEventPair,
	WrQueue,
	WrLpcReceive,
	WrLpcReply,
	WrVirtualMemory,
	WrPageOut,
	WrRendezvous,
	Spare2,
	Spare3,
	Spare4,
	Spare5,
	Spare6,
	WrKernel
} KWAIT_REASON;

typedef enum _THREAD_STATE
{
	StateInitialized,
	StateReady,
	StateRunning,
	StateStandby,
	StateTerminated,
	StateWait,
	StateTransition,
	StateUnknown
} THREAD_STATE;

typedef struct _VM_COUNTERS
{
	ULONG PeakVirtualSize;              //虚拟存储峰值大小   
	ULONG VirtualSize;                  //虚拟存储大小   
	ULONG PageFaultCount;               //页故障数目   
	ULONG PeakWorkingSetSize;           //工作集峰值大小   
	ULONG WorkingSetSize;               //工作集大小   
	ULONG QuotaPeakPagedPoolUsage;      //分页池使用配额峰值   
	ULONG QuotaPagedPoolUsage;          //分页池使用配额   
	ULONG QuotaPeakNonPagedPoolUsage;   //非分页池使用配额峰值   
	ULONG QuotaNonPagedPoolUsage;       //非分页池使用配额   
	ULONG PagefileUsage;                //页文件使用情况   
	ULONG PeakPagefileUsage;            //页文件使用峰值   

} VM_COUNTERS, *PVM_COUNTERS;

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;

} CLIENT_ID;

typedef struct _SYSTEM_THREADS
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
	ULONG ContextSwitchCount;
	THREAD_STATE State;
	KWAIT_REASON WaitReason;

} SYSTEM_THREADS, *PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESS_INFOMATION
{
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	ULONG ProcessId;
	ULONG InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters;
	SYSTEM_THREADS Threads[1];
}SYSTEM_PROCESS_INFOMATION, *pSYSTEM_PROCESS_INFOMATION;


// C_PAGE2 对话框

IMPLEMENT_DYNAMIC(C_PAGE2, CDialogEx)

C_PAGE2::C_PAGE2(CWnd* pParent /*=NULL*/)
	: CDialogEx(C_PAGE2::IDD, pParent)
{

}

C_PAGE2::~C_PAGE2()
{
}

void C_PAGE2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROCESS_LIST, m_Proc_List);
	DDX_Control(pDX, IDC_BUTTON1, m_Button);
}


BEGIN_MESSAGE_MAP(C_PAGE2, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &C_PAGE2::OnBnClickedButton1)
	ON_NOTIFY(NM_DBLCLK, IDC_PROCESS_LIST, &C_PAGE2::OnNMDblclkProcessList)
	ON_NOTIFY(NM_RCLICK, IDC_PROCESS_LIST, &C_PAGE2::OnNMRClickProcessList)
END_MESSAGE_MAP()


// C_PAGE2 消息处理程序
void C_PAGE2::ListInitialize()
{
	m_Proc_List.DeleteAllItems();
	m_Proc_List.SetExtendedStyle(m_Proc_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	//m_Ord_List.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_Proc_List.InsertColumn(0, _T("进程名称"), 0, 100);
	m_Proc_List.InsertColumn(1, _T("PID"), 0, 50);
	m_Proc_List.InsertColumn(2, _T("用户名"),0,50);
	m_Proc_List.InsertColumn(3, _T("线程数"), 0, 50);
	m_Proc_List.InsertColumn(4, _T("占用内存"), 0, 60);
	m_Proc_List.InsertColumn(5, _T("命令行"), 0, 120);
	m_Proc_List.InsertColumn(6, _T("描述"), 0, 120);
	LVCOLUMN Col;
	Col.mask = LVCF_FMT;
	Col.fmt = LVCFMT_RIGHT;
	m_Proc_List.SetColumn(4, &Col);
}

void C_PAGE2::ShowList()
{
	m_Proc_List.DeleteAllItems();
	
	HMODULE hModule = GetModuleHandle(L"ntdll.dll");
	if (!hModule)
	{
		ErrorInfo();
		return;
	}
	ZWQUERTSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERTSYSTEMINFORMATION)GetProcAddress(hModule, "ZwQuerySystemInformation");
	
	ULONG cbBuffer = 0;
	NTSTATUS status = ZwQuerySystemInformation(5, NULL, 0, &cbBuffer);
	if (status != STATUS_INFO_LENGTH_MISMATCH)
	{
		ErrorInfo();
		return;
	}
	LPVOID pBuffer = malloc(cbBuffer);
	if (pBuffer==NULL)
	{
		ErrorInfo();
		return;
	}	
	status=ZwQuerySystemInformation(5, pBuffer, cbBuffer, NULL);
	if (status != STATUS_SUCCESS)
	{
		ErrorInfo();
		return;
	}

	pSYSTEM_PROCESS_INFOMATION ProcInfo = (pSYSTEM_PROCESS_INFOMATION)pBuffer;
	RaisePrivileges();
	for (int i = 0; ProcInfo->NextEntryDelta != 0; i++)
	{
		if (ProcInfo->ProcessName.Buffer == NULL)
		{
			i--;
			ProcInfo = (pSYSTEM_PROCESS_INFOMATION)((PUCHAR)ProcInfo + ProcInfo->NextEntryDelta);
			continue;
		}
		TCHAR temp[MAX_PATH] = { 0 };
		BOOL bIs64 = FALSE;
		HANDLE Handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcInfo->ProcessId);
		IsWow64Process(Handle, &bIs64);
	
		wcscpy_s(temp, ProcInfo->ProcessName.Length, ProcInfo->ProcessName.Buffer);
		if (TRUE == bIs64)
			wcscat_s(temp, MAX_PATH, L"  *32");
		m_Proc_List.InsertItem(i, temp);		//进程名称;

		_itow_s(ProcInfo->ProcessId, temp, 10);
		m_Proc_List.SetItemText(i, 1, temp);		//pid

		m_Proc_List.SetItemText(i, 2, GetProcessUserName(ProcInfo->ProcessId, Handle));	//用户名

		_itow_s(ProcInfo->ThreadCount, temp, 10);
		m_Proc_List.SetItemText(i, 3, temp);		//线程数量

		_itow_s(ProcInfo->VmCounters.WorkingSetSize / 1024, temp, 10); 3;
		wcscat_s(temp, sizeof(temp) / 2, L" K");
		m_Proc_List.SetItemText(i, 4, temp);	//拼接个K 单位上去;	//进程大小
		

		TCHAR dwDepict[MAX_PATH] = {0};
		GetModuleFileNameEx(Handle, 0, dwDepict, _countof(dwDepict));
		m_Proc_List.SetItemText(i, 5, dwDepict);

		m_Proc_List.SetItemText(i, 6, GetDescription(dwDepict));

		CloseHandle(Handle);
		ProcInfo = (pSYSTEM_PROCESS_INFOMATION)((PUCHAR)ProcInfo + ProcInfo->NextEntryDelta);
	}
	free (pBuffer);
}

void C_PAGE2::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	ShowList();
	CDialogEx::OnTimer(nIDEvent);
}

void C_PAGE2::OnBnClickedButton1()
{
	ShowWindow(SW_HIDE);
	TCHAR szApplication[MAX_PATH] = { 0 };
	DWORD cchLength = _countof(szApplication);
	QueryFullProcessImageName(GetCurrentProcess(), 0, szApplication, &cchLength);

	SHELLEXECUTEINFO SecInfo={sizeof(SHELLEXECUTEINFO)};
	SecInfo.lpVerb = _TEXT("runas");
	SecInfo.lpFile = szApplication;
	SecInfo.lpParameters = NULL;
	SecInfo.nShow = SW_SHOWNORMAL;
	if (ShellExecuteEx(&SecInfo))
	{
		PostQuitMessage(0);
	}
	else
	{
		ShowWindow(SW_SHOWNORMAL);
	}

	// TODO:  在此添加控件通知处理程序代码
}

LPWSTR C_PAGE2::GetDescription(TCHAR *strFilePath)
{
	DWORD dwSize;
	DWORD dwRtn;
	TCHAR szVersion = NULL; //获取版本信息大小 
	dwSize = GetFileVersionInfoSize(strFilePath, NULL);
	if (dwSize == 0)
	{
		int reeor = GetLastError();
		printf("%d\n", reeor);
		return L" ";
	}
	wchar_t *pBuf;
	pBuf = new wchar_t[dwSize + 1];
	if (pBuf == NULL)
		return L" ";
	memset(pBuf, 0, dwSize + 1);	//获取版本信息
	dwRtn = GetFileVersionInfo(strFilePath, NULL, dwSize, pBuf);
	if (dwRtn == 0)
	{
		return L" ";
	}
	UINT Chars = SearchChars(pBuf, dwSize);
	WCHAR *szVersions = &pBuf[Chars + 2];
	return szVersions;
}

UINT C_PAGE2::SearchChars(WCHAR *pBuffer, UINT Int)
{
	const WCHAR  Flag[] = L"FileDescription";
	for (UINT i = 0; i < Int; i++)
	{
		if (pBuffer[i] == 'F')
			if (pBuffer[i + 1] == 'i'&&pBuffer[i + 2] == 'l'&&pBuffer[i + 3] == 'e'&&pBuffer[i + 4] == 'D'&&pBuffer[i + 5] == 'e'&&pBuffer[i + 6] == 's'&&pBuffer[i + 14] == Flag[14])
			{
				return i + wcslen(Flag);
				break;
			}
	}
}

void C_PAGE2::OnNMDblclkProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// TODO:  在此添加控件通知处理程序代码

	//  EnumChildWindows(m_Proc_List,EnumWindowsPrco());

	std::wstring strPID;

	if (-1 != pNMItemActivate->iItem)
	{
		strPID=m_Proc_List.GetItemText(pNMItemActivate->iItem, 1);
	}
	DWORD dwPID=_wtoi(strPID.c_str());

	m_Module.Create(IDD_DIA_MODULE, &m_Proc_List);
	m_Module.ShowWindow(SW_SHOW);
	m_Module.ListInitialize();
	m_Module.ShowList(dwPID);

	*pResult = 0;
}


// BOOL CALLBACK EnumWindowsPrco(HWND hWnd, LPARAM lParam)
// {
// 	//LPCTSTR s_AppName = _T("ABCDEFG");//欲查找的线程ID.(SetProp函数指定的项)
// 	UINT s_AppMagic = 1024;
// 	//防止重复打开窗口
// 	HANDLE handSemp = CreateSemaphore(NULL, 0, 1, s_AppName);//创建基于指定名字的信号量
// 	if (GetLastError() == ERROR_ALREADY_EXISTS)
// 	{
// 		HWND hwndPre = ::GetWindow(::GetDesktopWindow(), 0);/*如果指定窗口是父窗口，则获得的是在Z序顶端的子窗口的句柄，否则为NULL。*/
// 		while (::IsWindow(hwndPre))
// 		{
// 			if (::GetProp(hwndPre, s_AppName) == (HANDLE)s_AppMagic)/*如果属性表中包含了给定的字符串，那么返回值为相关的数据句柄。否则，返回值为NULL。 */
// 			{
// 				//此处即已在系统中找到了指定线程
// 				if (::IsIconic(hwndPre)) //确定给定窗口是否是最小化(图标化)的窗口
// 				{
// 					::ShowWindow(hwndPre, SW_RESTORE);/*激活并显示窗口。SW_RESTORE:如果窗口最小化或最大化，则系统将窗口恢复到原来的尺寸和位置。在恢复最小化窗口时，应用程序应该指定这个标志。 */
// 				}
// 				::SetForegroundWindow(hwndPre);/*将创建指定窗口的线程设置到前台，并且激活该窗口。键盘输入转向该窗口，并为用户改各种可视的记号。*/
// 			}
// 			hwndPre = GetWindow(hwndPre, GW_HWNDNEXT);
// 		}
// 	}
// }



void C_PAGE2::OnNMRClickProcessList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO:  在此添加控件通知处理程序代码
	if (-1 == pNMItemActivate->iItem)
		return;
	CMenu cMenu, *pMenu;
	cMenu.LoadMenu(IDR_PAGE2_MENU);
	pMenu = cMenu.GetSubMenu(0);

	ClientToScreen(&pNMItemActivate->ptAction);

	UINT iSelecte = pMenu->TrackPopupMenu(TPM_RETURNCMD, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, this, NULL);
	switch (iSelecte)
	{
	case ID_32803:		//打开文件夹
	{
		WCHAR wchProcessName[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessName, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessName);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		GetModuleFileNameEx(hProcess, NULL, wchProcessName, MAX_PATH);
		PVOID pTemp=wcsrchr(wchProcessName, '\\');
		memset(pTemp,0,2);
		ShellExecute(NULL, L"explore", wchProcessName, NULL, NULL, SW_SHOWMAXIMIZED);
		CloseHandle(hProcess);
	}
	break;
	case ID_32800:		//结束进程
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		TerminateProcess(hProcess, 0);
	}
	break;
	case ID_32794:		//远程线程;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		Injiect_Remote(uiPID);
	}
	break;
	case ID_32795:		//消息钩子;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		Injiect_Message(uiPID);
	}
	break;	
	case ID_32796:		//apc
	{

	}
	break;
	case ID_32797:		//输入法
	{

	}
	break;
	case ID_32805:		//设置优先级;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS);
	}
	break;
	case ID_32806:		//设置优先级;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS);
	}
	break;
	case ID_32807:		//设置优先级;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		SetPriorityClass(hProcess, ABOVE_NORMAL_PRIORITY_CLASS);
	}
	break;
	case ID_32808:		//设置优先级;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		SetPriorityClass(hProcess, NORMAL_PRIORITY_CLASS);
	}
	break;
	case ID_32809:		//设置优先级;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		SetPriorityClass(hProcess, BELOW_NORMAL_PRIORITY_CLASS);
	}
	break;
	case ID_32810:		//设置优先级;
	{
		WCHAR wchProcessID[MAX_PATH] = { 0 };
		m_Proc_List.GetItemText(pNMItemActivate->iItem, 1, wchProcessID, MAX_PATH);
		DWORD uiPID = _wtoi(wchProcessID);
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, uiPID);
		SetPriorityClass(hProcess, IDLE_PRIORITY_CLASS);
	}
	break;
	default:
		break;
	}
	*pResult = 0;
}

BOOL C_PAGE2::Injiect_Remote(DWORD dPID)
{
	//获取要注入dll文件信息;
	WCHAR szFilePathe[MAX_PATH] = { 0 };
	WCHAR szFilter[] = L"dll文件(*.dll)\0*.dll\0\0;\0*.DLL\0\0";
	OPENFILENAME lpFiles = { 0 };
		
	lpFiles.lStructSize = sizeof(OPENFILENAME);
	lpFiles.lpstrTitle =NULL;
	lpFiles.lpstrFile = szFilePathe;
	lpFiles.nMaxFile = MAX_PATH;
	lpFiles.lpstrFileTitle = NULL;
	lpFiles.lpstrInitialDir = NULL;
	lpFiles.lpstrFilter = szFilter;
	lpFiles.Flags = OFN_EXPLORER;

	if (!GetOpenFileName(&lpFiles))
		return FALSE;

	//开始注入;
	HANDLE hProcess = NULL; 
	HANDLE hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	LPTHREAD_START_ROUTINE lpThreadProc = NULL;
	UINT uiFileSize = ((_tcslen(szFilePathe)+1)*sizeof(WCHAR));

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dPID);
	if (hProcess == NULL)
		return FALSE;
	
	pRemoteBuf = VirtualAllocEx(hProcess, NULL,uiFileSize ,MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pRemoteBuf)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	if (!WriteProcessMemory(hProcess, pRemoteBuf, szFilePathe, uiFileSize, NULL))
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	hMod=GetModuleHandle(L"kernel32.dll");
	if (NULL == hMod)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	lpThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");

	hThread = CreateRemoteThread(hProcess, NULL, 0, lpThreadProc, pRemoteBuf,NULL, NULL);
	if (NULL == hThread)
	{
		CloseHandle(hProcess);
		CloseHandle(hThread);
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);

	DWORD dwExitCode = 0;
	GetExitCodeThread(hThread, &dwExitCode);
	VirtualFreeEx(hProcess, szFilePathe, uiFileSize, MEM_COMMIT);
	CloseHandle(hProcess);
	CloseHandle(hThread);
	return TRUE;
}

BOOL C_PAGE2::Injiect_Message(DWORD dPID)
{
	//获取要注入dll文件信息;
	WCHAR szFilePathe[MAX_PATH] = { 0 };
	WCHAR szFilter[] = L"dll文件(*.dll)\0*.dll\0\0;\0*.DLL\0\0";
	OPENFILENAME lpFiles = { 0 };

	lpFiles.lStructSize = sizeof(OPENFILENAME);
	lpFiles.lpstrTitle = NULL;
	lpFiles.lpstrFile = szFilePathe;
	lpFiles.nMaxFile = MAX_PATH;
	lpFiles.lpstrFileTitle = NULL;
	lpFiles.lpstrInitialDir = NULL;
	lpFiles.Flags = OFN_EXPLORER;

	if (!GetOpenFileName(&lpFiles))
		return FALSE;

	//加载需要注入的dll到自身;
	HMODULE hMod = LoadLibrary(szFilePathe);
	if (NULL == hMod)
		return FALSE;

	
}