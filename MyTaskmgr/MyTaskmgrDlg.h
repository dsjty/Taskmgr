
// MyTaskmgrDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "C_PAGE1.h"
#include "C_PAGE2.h"
#include "C_PAGE3.h"
#include "My_Module.h"


// CMyTaskmgrDlg �Ի���
class CMyTaskmgrDlg : public CDialogEx
{
// ����
public:
	CMyTaskmgrDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYTASKMGR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	BOOL SystemShutDown();


// ʵ��
protected:
	HICON m_hIcon;
	C_PAGE1 m_Page1;
	C_PAGE2 m_Page2;
	C_PAGE3 m_Page3;

	// ���ɵ���Ϣӳ�亯��
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
