/** @file
********************************************************************************
<PRE>
ģ����: ¼��������ģ��
�ļ���: myRecorder.h
����ļ�: myRecorder.c
�ļ�ʵ�ֹ���: ¼��ģ����غ��������������͵�����
����: Dake
�汾: V1.1
��̷�ʽ: win32 SDK C���Ա��
��Ȩ��ʽ: Copyright(C) Dake
��ϵ��ʽ: chen423@yeah.net
��������: 2010-05
--------------------------------------------------------------------------------
���̰߳�ȫ��: <��>[¼���������ֱ���һ���̣߳���������������߳�ͬ������]
�쳣ʱ��ȫ��: <��/��>[��˵��]
--------------------------------------------------------------------------------
��ע: <����˵��>
--------------------------------------------------------------------------------
�޸ļ�¼:
�� ��              �汾     �޸���              �޸�����
2011/03/29   1.1     <Dake>           <�޸���غ����������ļ���װ��>
</PRE>
*******************************************************************************/

#include "global.h"
#include <mmeapi.h>

#ifndef _MYRECORD_
#define _MYRECORD_

typedef void * HRECORD;

typedef struct MYRECORD
{
	LPHWAVEIN  lhWaveIn; //¼�����
	PBYTE pBuffer[BUFF_NUM];
	PWAVEHDR pWaveHdr[BUFF_NUM];
	PWAVEFORMATEX pWaveform;
} MYRECORD;

typedef struct RECPARAM
{
	HWND hwnd;
	HRECORD myRecord;
} RECPARAM;


//¼����غ�������
HRECORD CreateRecord(HWND hwnd);

#endif    //_MYRECORD_

