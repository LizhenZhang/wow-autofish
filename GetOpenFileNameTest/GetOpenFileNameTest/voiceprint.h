/*! @file
********************************************************************************
<PRE>
ģ����: ����ʶ�����
�ļ���: voiceprint.h
����ļ�: voiceprint.c
�ļ�ʵ�ֹ���: �û�������ز����Ķ���
����: Dake
�汾: V2010.09.01
��̷�ʽ:
��Ȩ��ʽ:
��ϵ��ʽ: chen423@yeah.net
��������: 2010-06-28
--------------------------------------------------------------------------------
���̰߳�ȫ��: <��>
�쳣ʱ��ȫ��: <��/��>[��˵��]
--------------------------------------------------------------------------------
��ע: <����˵��>
--------------------------------------------------------------------------------
�޸ļ�¼:
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y      <���߻��޸�����>    <�޸�����>
</PRE>
*******************************************************************************/
#ifndef VOICEPRINT_H_INCLUDED
#define VOICEPRINT_H_INCLUDED

#include "global.h" 
#include "FileDlg.h"
#include "gmm.h"
#include "myRecorder.h"

static int getTrainSample = 0;
static int getSample = 0;

typedef enum BUILD_MODE { E_BUILD = 0, E_IDENTIFY = 1 } BUILD_MODE; // GetUsrGMM�������÷�ʽ

																	/**
																	* @brief ��ģ���ݽṹ
																	*/
typedef struct VOICE_MODEL
{
	GMM gmm;                       /**< ��˹���ģ�� */
	double value[IDENTIFY_NUM][2]; /**< ֡ʶ����ֵ��[-][0]����ֵ��[-][1]��ʶ��ֵ */
	int percent;                   /**< ʶ��Χ */
	int frame_num;                 /**< ѵ��֡�� */
	int m;                         /**< ��˹����� */
	BOOL bValued;                  /**< ����Ƿ��Ѿ���ģ */
	FILE * file_stream = NULL;            /**< �ļ��� */
	TBYTE * pVoice = NULL;                /**< ������ȡ���� */
	FILE_CLASS voice_file;         /**< �ļ�·�� */
	void(*f_Init)(struct VOICE_MODEL *);
	BOOL(*f_GetUsrGMM)(struct VOICE_PRINT *, BUILD_MODE);
}VOICE_MODEL;

/**
* @brief ���ƽ��
*/
typedef struct VOICE_PRINT
{
	void(*f_Init)(struct VOICE_PRINT *);
	VOICE_MODEL voice_mdl;        /**< ����ģ�ͼ�ʶ����ֵ */
	HWND hwnd;                    /**< ���ھ�� */
	HRECORD myRecord;             /**< ¼��ʵ�� */
	HANDLE g_hRecThread;          /**< ¼���߳̾�� */
	DWORD g_RecThreadID;
	int rec_frame[IDENTIFY_NUM];  /**< ÿһ��ʶ��֡�� */
	BYTE * lRawVioce ;
	BYTE * RRawVioce ;
} VOICE_PRINT;


#endif // VOICEPRINT_H_INCLUDED
extern VOICE_PRINT my_voiceprint;

void VoiceModelInit(VOICE_MODEL * pvmodel);
void VoicePrintInit(VOICE_PRINT * pvprint);
BOOL GetUsrGMM(VOICE_PRINT * pUsr, BUILD_MODE mod);
void PrintThresholdValue(HWND hwnd); // ��ӡʶ����ֵ
