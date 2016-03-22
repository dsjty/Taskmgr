#pragma once
#include "afxcmn.h"


// C_PAGE3 对话框

class C_PAGE3 : public CDialogEx
{
	DECLARE_DYNAMIC(C_PAGE3)

public:
	C_PAGE3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~C_PAGE3();

public:
	void ListInitialize();
	void ShowList();

// 对话框数据
	enum { IDD = IDD_PAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_Sev_List;
};
