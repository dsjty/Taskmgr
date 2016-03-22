// C_PAGE3.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTaskmgr.h"
#include "C_PAGE3.h"
#include "afxdialogex.h"
#include <winsvc.h>


// C_PAGE3 对话框

IMPLEMENT_DYNAMIC(C_PAGE3, CDialogEx)

C_PAGE3::C_PAGE3(CWnd* pParent /*=NULL*/)
	: CDialogEx(C_PAGE3::IDD, pParent)
{

}

C_PAGE3::~C_PAGE3()
{
}

void C_PAGE3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Sev_List);
}


BEGIN_MESSAGE_MAP(C_PAGE3, CDialogEx)
END_MESSAGE_MAP()


// C_PAGE3 消息处理程序
void C_PAGE3::ListInitialize()
{
	m_Sev_List.DeleteAllItems();
	m_Sev_List.InsertColumn(0, _T("名称"), 0, 100);
	m_Sev_List.InsertColumn(1, _T("PID"), 0, 50);
	m_Sev_List.InsertColumn(2, _T("描述"), 0, 200);
	m_Sev_List.InsertColumn(3, _T("状态"), 0, 70);
}

void C_PAGE3::ShowList()
{
	SC_HANDLE  sHandle = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
	if (NULL==sHandle)
	{
		ErrorInfo();
		return;
	}
	//SC_ENUM_TYPE lpSevervice = NULL;


	DWORD scSize = 0x00;
	ULONG lpSeverviceRet = scSize;
	ULONG dwBufNeed = 0x00;

	EnumServicesStatusEx(sHandle, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, 0, scSize, &dwBufNeed, &lpSeverviceRet, 0, 0);

	scSize = dwBufNeed + 0x10;

	PUCHAR pBuf = (PUCHAR)malloc(scSize);

	EnumServicesStatusEx(sHandle, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL, pBuf, scSize, &dwBufNeed, &lpSeverviceRet, 0, 0);

	LPENUM_SERVICE_STATUS_PROCESS pInfo = (LPENUM_SERVICE_STATUS_PROCESS)pBuf;

	for (UINT i = 0; i<lpSeverviceRet; i++)
	{
		m_Sev_List.InsertItem(i,pInfo[i].lpServiceName);

		TCHAR Temp[16];
		_itow_s(pInfo[i].ServiceStatusProcess.dwProcessId, Temp, 10);
		m_Sev_List.SetItemText(i, 1, Temp);
		
		m_Sev_List.SetItemText(i, 2, pInfo[i].lpDisplayName);

		switch (pInfo[i].ServiceStatusProcess.dwCurrentState)
		{
		case 1:
		case 3:
			wcsncpy_s(Temp, L"停止", 3);
			break;
		case 4:
			wcsncpy_s(Temp, L"运行中", 4);
			break;
		case 5:
			wcsncpy_s(Temp, L"等待", 3);
			break;
		case 6:
		case 7:
			wcsncpy_s(Temp, L"暂停", 3);
			break;
		default:
			wcsncpy_s(Temp, L"开始", 3);
			break;
		}
		m_Sev_List.SetItemText(i, 3, Temp);
		
	}
}