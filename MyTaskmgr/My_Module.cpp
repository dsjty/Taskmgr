// My_Module.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTaskmgr.h"
#include "My_Module.h"
#include "afxdialogex.h"
#include <Psapi.h>


#define  SystemModuleInformation 11
//
//typedef LONG NTSTATUS;
//typedef LONG KPRIORITY;
//
//typedef DWORD(WINAPI *ZWQUERTSYSTEMINFORMATION)(DWORD, PVOID, DWORD, PDWORD);
//
//typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY {
//	HANDLE Handle;
//	PVOID MappedBase;
//	PVOID Base;
//	ULONG Size;
//	ULONG Flags;
//	USHORT LoadOrderIndex;
//	USHORT InitOrderIndex;
//	USHORT LoadCount;
//	USHORT PathLength;
//	CHAR ImageName[256];
//}SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;
//
//typedef struct _SYSTEM_MODULE_INFORMATION
//{
//	ULONG Count;
//	SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
//} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;
//



// My_Module 对话框

IMPLEMENT_DYNAMIC(My_Module, CDialogEx)

My_Module::My_Module(CWnd* pParent /*=NULL*/)
	: CDialogEx(My_Module::IDD, pParent)
{

}

My_Module::~My_Module()
{
}

void My_Module::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODULE, m_Module_List);
}


BEGIN_MESSAGE_MAP(My_Module, CDialogEx)
	ON_WM_CLOSE()
//	ON_MESSAGE(WM_SUB_MSG, &My_Module::OnSubMsg)
END_MESSAGE_MAP()


// My_Module 消息处理程序


void My_Module::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	DestroyWindow();
	CDialogEx::OnClose();
}

void My_Module::ListInitialize()
{
	m_Module_List.InsertColumn(0, _T("模块名称"), 0, 200);
//	m_Module_List.InsertColumn(1, _T("安全状态"), 0, 60);
	m_Module_List.InsertColumn(1, _T("基址"), 0, 70);
	m_Module_List.InsertColumn(2, _T("大小"), 0, 50);
	m_Module_List.InsertColumn(3, _T("路径"), 0, 300);
	//m_Module_List.InsertColumn(5, _T("描述"), 0, 100);
	LVCOLUMN Col;
	Col.mask = LVCF_FMT;
	Col.fmt = LVCFMT_RIGHT;
	m_Module_List.SetColumn(2, &Col);
}


//void My_Module::ShowList()
//{
//	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
//	ZWQUERTSYSTEMINFORMATION ZwQuerySystemInformation = (ZWQUERTSYSTEMINFORMATION)GetProcAddress(hNtdll, "ZwQuerySystemInformation");
//
//	PDWORD pBuffer;
//	DWORD szBufSize=0;
//
//	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &szBufSize);
//	
//	LPVOID lpBuffer = malloc(szBufSize);
//
//	status = ZwQuerySystemInformation(SystemModuleInformation, lpBuffer, szBufSize, NULL);
//
//	PSYSTEM_MODULE_INFORMATION ModuleInfo = (PSYSTEM_MODULE_INFORMATION)lpBuffer;
//
//	for (UINT i=0;i<ModuleInfo->Count;i++)
//	{
//		TCHAR Temp[MAX_PATH];
//	}
//}

void My_Module::ShowList(DWORD dwPID)
{
	GetModuleInfo(dwPID);

	for (UINT i = 0; i < m_ModeleVc.size(); i++)
	{
		m_Module_List.InsertItem(i, m_ModeleVc[i].szExePath);
		size_t converted = 0;
		TCHAR *Temp=new TCHAR[MAX_PATH];

		//		if(!mbstowcs_s(&converted, Temp, strlen((char*)m_ModeleVc[i].modBaseAddr), (char*)m_ModeleVc[i].modBaseAddr,_TRUNCATE))
		wsprintf(Temp, L"%08X", m_ModeleVc[i].modBaseAddr);
		m_Module_List.SetItemText(i, 1, Temp);

		wsprintf(Temp, L"%dK", m_ModeleVc[i].modBaseSize/1000);
		m_Module_List.SetItemText(i, 2, Temp);

		m_Module_List.SetItemText(i, 3, m_ModeleVc[i].szExePath);
	}
}

void My_Module::GetModuleInfo(DWORD dwPID)
{
	m_Module_List.DeleteAllItems();
	m_ModeleVc.clear();

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if (hProcess == NULL)
	{
		ErrorInfo();
		return;
	}
	HMODULE hModule[0x100] = { 0 };
	DWORD szNeeds = 0;

	if (!EnumProcessModulesEx(hProcess, hModule, sizeof(hModule), &szNeeds, LIST_MODULES_ALL))
	{
		ErrorInfo();
		return;
	}
	DWORD dwModuleCount = szNeeds / sizeof(HMODULE);

	HMODULE *pModBuf = new HMODULE[dwModuleCount];

	if (!EnumProcessModulesEx(hProcess, pModBuf, dwModuleCount*sizeof(HMODULE), &szNeeds, LIST_MODULES_ALL))
	{
		ErrorInfo();
		return;
	}

	TCHAR szModuleName[0x200] = { 0 };

	MODULEENTRY32 stcMod32 = { sizeof(MODULEENTRY32) };
	MODULEINFO stcModInfo;
	for (UINT i = 0; i < dwModuleCount; i++)
	{
		if (!GetModuleFileNameEx(hProcess, pModBuf[i], stcMod32.szExePath, MAX_PATH))
		{
			ErrorInfo();
			break;
		}
		if (!GetModuleInformation(hProcess, pModBuf[i], &stcModInfo, sizeof(MODULEINFO)))
		{
			ErrorInfo();
			break;
		}
		stcMod32.modBaseAddr = (PBYTE)stcModInfo.lpBaseOfDll;
		stcMod32.modBaseSize = stcModInfo.SizeOfImage;
		stcMod32.th32ProcessID = dwPID;
		m_ModeleVc.push_back(stcMod32);
	}

	CloseHandle(hProcess);
}

