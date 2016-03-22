#pragma once

#include "afxcmn.h"
#include <vector>
#include "MyTaskmgr.h"

using std::vector;

// C_PAGE1 �Ի���

//���б�����ӳ���;
typedef struct _MyProgram
{
	TCHAR lpWinTitle[MAX_PATH];
	TCHAR lpPaths[MAX_PATH];
	DWORD hPID;
	HICON hIcon;
	DWORD ListID;
}MyProgram, *pMyProgram;


class C_PAGE1 : public CDialogEx
{
	DECLARE_DYNAMIC(C_PAGE1)

public:
	C_PAGE1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~C_PAGE1();
public:
	void ListInitialize();	//1��ʼ���б���;
	void ShowList();		//2������ǰ�������е�ǰ̨����,�������б�;


// �Ի�������
	enum { IDD = IDD_PAGE1 };
	HANDLE m_Handle;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CImageList m_Ico_List;
	CListCtrl m_Ord_List;
	vector<MyProgram> m_Program;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnNMRClickOrdList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickOrdList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEndproc();
	virtual BOOL OnInitDialog();
};


