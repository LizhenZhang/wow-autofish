//////////////////////////////////////////////////////////////////////////
//Copyright(C) Dake��2010-05
//��̷�ʽ��win32 SDK C���Ա��
//�ļ�����global.c
//������������ص�ȫ�ֱ����������Ķ���
//��Ҫ������EditPrintf()������ָ���ı����������ʽ�����ִ�
//�汾��V1.1
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "global.h" 
#include "voiceprint.h"
#include "resource.h"
#ifdef _DEBUG
HWND hwndDebugDlg = NULL;      //����ʹ��
HWND hwndEdit = NULL;
#endif

int g_build_buff_num = 0;       // ��ģ������
BOOL bExit = FALSE;           // �˳���¼���رճ���
BOOL killThreading = FALSE;           // �ر��߳�
BOOL g_buildGMM = FALSE;          //�����ȡ����
TCHAR * szAppName = TEXT("MAINDLG");
TCHAR * szAppVersion = TEXT("���θ���");
HWND hwndMainWin = NULL;
HWND hwndMainDlg = NULL;
HWND hWndCursorSpeedSlider = NULL;
HWND hWndFishSoundMatchAccuracySlider = NULL;
HWND hWndScreenFishStatusCheckAccuracySlider = NULL;

HWND qqContactWin = NULL;
HWND helpWin = NULL;

HWND setupWindow=NULL;
HWND superadminWindow = NULL;
HWND checkScreenFishStatusBoundaryValueSlider = NULL;

CRITICAL_SECTION cs;


//////////////////////////////////////////////////////////////////////////
//��������EditPrintf(��
//���ܣ���ָ���ı����������ʽ�����ִ�
//���룺hwndEdit���ı�������szFormat����ʽ�ִ��� ...�������ɱ�Ĳ���
//���ã�wvsprintf()
//����������
//////////////////////////////////////////////////////////////////////////
void EditPrintf(HWND hwndEdit, char * szFormat, ...)
{
	char szBuffer[2200];
	va_list pArgList;
	va_start(pArgList, szFormat);
	vsprintf_s(szBuffer,sizeof(szBuffer), szFormat, pArgList);
	va_end(pArgList);
	SendMessage(hwndEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);
	SendMessage(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)szBuffer);
	SendMessage(hwndEdit, EM_SCROLLCARET, 0, 0);
}


//////////////////////////////////////////////////////////////////////////
//��������QuitMyWindow(��
//���ܣ��ر��������̺߳����˳�����
//���룺
//���ã�
//����������UsrDlg( )��voiceSockThread( )��myPlayThread( )��myRecThread( )
//////////////////////////////////////////////////////////////////////////
void QuitMyWindow()
{
	if (bExit || killThreading)    //�ж�bExit��ֹ�ظ��ر������Ϣ�ظ�����
	{//��Ϊ���̻߳��bExit��ΪFALSE
		return;
	}
	if (my_voiceprint.g_RecThreadID)
	{
		bExit = TRUE;
		PostThreadMessage(my_voiceprint.g_RecThreadID, WM_RECORD_END, 0, 0);    //�ر�¼���߳�
		return;
	}
	PostMessage(GetParent(hwndMainDlg), WM_DESTROY, 0, 0);
}


bool  ConvertCharToLPWSTR(const char * szString, LPWSTR lpszPath)
{
	int dwLen = strlen(szString) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//������ʵĳ���
	//LPWSTR lpszPath = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
	return true;
}