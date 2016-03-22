#pragma once
#include "afxcmn.h"
#include <vector>
#include <Tlhelp32.h>

using std::vector;


// My_Module �Ի���

class My_Module : public CDialogEx
{
	DECLARE_DYNAMIC(My_Module)

public:
	My_Module(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~My_Module();

// �Ի�������
	enum { IDD = IDD_DIA_MODULE };

	void ListInitialize();
	void ShowList(DWORD);
	void GetModuleInfo(DWORD);
	vector<MODULEENTRY32> m_ModeleVc;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
protected:
//	afx_msg LRESULT OnSubMsg(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_Module_List;
};
