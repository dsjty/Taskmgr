
// MyTaskmgr.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once



#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include <Strsafe.h>


// CMyTaskmgrApp: 
// �йش����ʵ�֣������ MyTaskmgr.cpp
//

class CMyTaskmgrApp : public CWinApp
{
public:
	CMyTaskmgrApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyTaskmgrApp theApp;


void ErrorInfo();
void RaisePrivileges();
void RaisePrivileges2();
LPCTSTR GetProcessUserName(ULONG,HANDLE);