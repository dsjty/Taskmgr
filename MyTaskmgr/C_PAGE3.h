#pragma once
#include "afxcmn.h"


// C_PAGE3 �Ի���

class C_PAGE3 : public CDialogEx
{
	DECLARE_DYNAMIC(C_PAGE3)

public:
	C_PAGE3(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~C_PAGE3();

public:
	void ListInitialize();
	void ShowList();

// �Ի�������
	enum { IDD = IDD_PAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_Sev_List;
};
