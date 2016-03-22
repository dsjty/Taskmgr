
// MyTaskmgrDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "C_PAGE1.h"
#include "C_PAGE2.h"
#include "C_PAGE3.h"
#include "My_Module.h"


// CMyTaskmgrDlg 对话框
class CMyTaskmgrDlg : public CDialogEx
{
// 构造
public:
	CMyTaskmgrDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYTASKMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	BOOL SystemShutDown();


// 实现
protected:
	HICON m_hIcon;
	C_PAGE1 m_Page1;
	C_PAGE2 m_Page2;
	C_PAGE3 m_Page3;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_ORDER;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void On32772();
	afx_msg void OnSleep();
	afx_msg void OnLogoff();
	afx_msg void OnPoweroff();
};
