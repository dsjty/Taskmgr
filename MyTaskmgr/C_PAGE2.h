#pragma once
#include "afxcmn.h"
#include "MyTaskmgr.h"
#include "afxwin.h"
#include <Windows.h>
#include "My_Module.h"




// C_PAGE2 �Ի���
typedef struct _MyProcess
{
	TCHAR ProcName[MAX_PATH];
	UINT ProID;
	TCHAR ProcShell[MAX_PATH];
	UINT ProThreads;
	TCHAR UserName[MAX_PATH];
}MyProcess,*pMyprocess;

class C_PAGE2 : public CDialogEx
{
	DECLARE_DYNAMIC(C_PAGE2)

public:
	C_PAGE2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~C_PAGE2();

public:
	void ListInitialize();	//1��ʼ���б���;
	void ShowList();
	LPWSTR GetDescription(TCHAR *strFilePath);
	UINT SearchChars(WCHAR *pBuffer, UINT Int);


// �Ի�������
	enum { IDD = IDD_PAGE2 };
	My_Module m_Module;

	void SetModuleShow();
	BOOL Injiect_Remote(DWORD);
	BOOL Injiect_Message(DWORD);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_Proc_List;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton1();
	CButton m_Button;
	afx_msg void OnNMDblclkProcessList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickProcessList(NMHDR *pNMHDR, LRESULT *pResult);
};
