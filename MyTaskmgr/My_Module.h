#pragma once
#include "afxcmn.h"
#include <vector>
#include <Tlhelp32.h>

using std::vector;


// My_Module 对话框

class My_Module : public CDialogEx
{
	DECLARE_DYNAMIC(My_Module)

public:
	My_Module(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~My_Module();

// 对话框数据
	enum { IDD = IDD_DIA_MODULE };

	void ListInitialize();
	void ShowList(DWORD);
	void GetModuleInfo(DWORD);
	vector<MODULEENTRY32> m_ModeleVc;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
protected:
//	afx_msg LRESULT OnSubMsg(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_Module_List;
};
