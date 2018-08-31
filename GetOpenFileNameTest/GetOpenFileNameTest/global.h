#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED 
/** @file
********************************************************************************
<PRE>
ģ����: ����ʶ��
�ļ���: global.h
����ļ�:
�ļ�ʵ�ֹ���: ȫ����Դ����������
����: Dake
�汾: V0.0.0.0
��̷�ʽ: ANSI C���Ա��
��Ȩ��ʽ: Copyright(C) Dake
��ϵ��ʽ: chen423@yeah.net
��������: 2010-11-16
--------------------------------------------------------------------------------
���̰߳�ȫ��: <��/��>[��˵��]
�쳣ʱ��ȫ��: <��/��>[��˵��]
--------------------------------------------------------------------------------
��ע: <����˵��>
--------------------------------------------------------------------------------
�޸ļ�¼:
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y      <���߻��޸�����>    <�޸�����>
</PRE>
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h> 
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
//#include "resource.h"
#pragma comment(lib,"winmm.lib")
//#define _DEBUG   //DEBUGѡ��

#define PI   3.1415926L
#define _2PI 6.2831853L
#define GET_ARRAY_LEN(array,len) {len = (sizeof(array) / sizeof(array[0]));}

#define BUFFLEN  480  //!<(512 * 2)
/* ¼���������� */
#define BUFF_NUM 10

#define PERCENT 19              /*< ʶ��Χ */
#define IDENTIFY_NUM 10          /*< ʶ����� */
#define IDENTIFY_FRAME_NUM 80  /*< ʶ��֡�� */
#define IDENTIFY_BUFF_NUM ((IDENTIFY_FRAME_NUM/2 +10)/BUFFLEN*FRAME_LEN) /*< ʶ�𻺳��� */

#ifdef _DEBUG
extern HWND hwndDebugDlg;   /*< ����ʹ�� */
extern HWND hwndEdit;
#define X_PRINT EditPrintf
#define X_HANDLE hwndEdit
#endif

extern int g_build_buff_num; /*< ��ģ������ */
extern BOOL bExit;           /*< �˳���¼���رճ��� */
extern BOOL g_bStopRecv;     /*< ����Ƿ�������նԷ���������Ƶ���� */
extern BOOL killThreading;   /*< ���ڹر����߳� */
extern BOOL g_buildGMM;      /*< �����ȡ���� */
extern TCHAR * szAppName;
extern TCHAR * szAppVersion ;
extern HINSTANCE hInst;      /*< �洢��ǰ������ */
extern HWND hwndMainWin;
extern HWND hwndMainDlg;
extern HWND hWndCursorSpeedSlider;
extern HWND hWndFishSoundMatchAccuracySlider ;
extern HWND hWndScreenFishStatusCheckAccuracySlider;

extern HWND qqContactWin ;
extern HWND helpWin ;
extern HWND setupWindow;
extern HWND superadminWindow;
extern HWND checkScreenFishStatusBoundaryValueSlider;


extern CRITICAL_SECTION cs;  /*< �ٽ��� */

void EditPrintf(HWND hwndEdit, char * szFormat, ...);
void QuitMyWindow();

bool  ConvertCharToLPWSTR(const char * szString, LPWSTR lpszPath);
//LPWSTR  ConvertCharToLPWSTR(const char * szString);
#endif // GLOBAL_H_INCLUDED
