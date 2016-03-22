// C_PAGE1.cpp : 实现文件
//

#include "stdafx.h"
#include "C_PAGE1.h"
#include "afxdialogex.h"
#include <Psapi.h>


// C_PAGE1 对话框

IMPLEMENT_DYNAMIC(C_PAGE1, CDialogEx)

C_PAGE1::C_PAGE1(CWnd* pParent /*=NULL*/)
	: CDialogEx(C_PAGE1::IDD, pParent)
{

}

C_PAGE1::~C_PAGE1()
{
}

void C_PAGE1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORD_LIST, m_Ord_List);
}


BEGIN_MESSAGE_MAP(C_PAGE1, CDialogEx)
	ON_WM_TIMER()
//	ON_WM_SIZE()
ON_WM_SIZE()
//ON_NOTIFY(NM_RCLICK, IDC_ORD_LIST, &C_PAGE1::OnNMRClickOrdList)
ON_NOTIFY(NM_RCLICK, IDC_ORD_LIST, &C_PAGE1::OnNMRClickOrdList)
ON_COMMAND(ID_EndProc, &C_PAGE1::OnEndproc)
END_MESSAGE_MAP()


BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM LParam)
{
	LRESULT WndClassex;
	TCHAR *Temp = (TCHAR*)malloc(MAX_PATH);
	C_PAGE1 *m_Program = (C_PAGE1*)LParam;
	MyProgram m_Temp_List;
	DWORD pPID;
	if (!::GetWindow(hWnd, GW_OWNER) && ::IsWindowVisible(hWnd))
	{
		::GetWindowText(hWnd, m_Temp_List.lpWinTitle, MAX_PATH - 1);
		if (0 == wcscmp(m_Temp_List.lpWinTitle, L"") || 0 == wcscmp(m_Temp_List.lpWinTitle, L"Program Manager"))
			return TRUE;
		else
		{
			GetWindowThreadProcessId(hWnd, &pPID);
			
			WndClassex = GetClassLong(hWnd, GCL_HICONSM);
			if (WndClassex == 0)
				WndClassex = SendMessage(hWnd, WM_GETICON, 0, 0);
			m_Temp_List.hIcon = (HICON)WndClassex;
			m_Temp_List.hPID = pPID;
			m_Program->m_Program.push_back(m_Temp_List);
		}
	}
	return TRUE;
}

// C_PAGE1 消息处理程序
void C_PAGE1::ListInitialize()
{
	//1添加列表项;
	m_Ord_List.DeleteAllItems();
	//m_Ord_List.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	m_Ord_List.SetExtendedStyle(m_Ord_List.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP);
	m_Ord_List.InsertColumn(0, _T("应用程序"),0,400);
	m_Ord_List.InsertColumn(1, _T("状态"),0,120);
}
void C_PAGE1::ShowList()
{	
	m_Ord_List.SetImageList(&m_Ico_List, LVSIL_SMALL);
	TCHAR *Temp = new TCHAR[MAX_PATH];
	m_Ord_List.DeleteAllItems();
	m_Program.clear();
	::EnumWindows(EnumWindowsProc, (LPARAM)this);
	for (UINT i = 0; i < m_Program.size(); i++)
	{
		m_Ord_List.InsertItem(i, m_Program[i].lpWinTitle,i);

		m_Ico_List.Add(m_Program[i].hIcon);

		wsprintf(Temp, L"%d", m_Program[i].hPID);
		m_Ord_List.SetItemText(i, 2, Temp);
		m_Program[i].ListID = i;
	}
	delete Temp;
}



void C_PAGE1::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	ShowList();

	CDialogEx::OnTimer(nIDEvent);
}


void C_PAGE1::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (0 != this->IsWindowVisible())
	{
		CRect Crect;
		GetWindowRect(Crect);
		Crect.top += 22;
		Crect.left += 1;
		m_Ord_List.MoveWindow(Crect);
	}
	// TODO:  在此处添加消息处理程序代码
}


void C_PAGE1::OnNMRClickOrdList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (-1 != pNMItemActivate->iItem)
	{
		CMenu cMenu,*pMenu;
		cMenu.LoadMenu(IDR_PAGE1_MENU);
		pMenu=cMenu.GetSubMenu(0);

		ClientToScreen(&pNMItemActivate->ptAction);

		UINT Selecte=0;
		Selecte= pMenu->TrackPopupMenuEx(TPM_RETURNCMD, pNMItemActivate->ptAction.x, pNMItemActivate->ptAction.y, this, NULL);
		if (Selecte == 32786)
		{
			for (int i = 0;i<m_Program.size(); i++)
			{
				if (pNMItemActivate->iItem == m_Program[i].ListID)
				{
					RaisePrivileges();
					m_Handle=OpenProcess(PROCESS_TERMINATE, FALSE, m_Program[i].hPID);					
					TerminateProcess(m_Handle,0);
				}
			}
		}
		pMenu->DestroyMenu();
	}
	// TODO:  在此添加控件通知处理程序代码
	ShowList();
	*pResult = 0;
}


void C_PAGE1::OnEndproc()
{
	
	// TODO:  在此添加命令处理程序代码
}


BOOL C_PAGE1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Ico_List.Create(16, 16, ILC_COLOR32,0, 0);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
