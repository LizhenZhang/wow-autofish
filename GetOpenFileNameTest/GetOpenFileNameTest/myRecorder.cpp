/** @file
********************************************************************************
<PRE>
ģ����: ¼��������ģ��
�ļ���: myRecorder.c
����ļ�: myRecorder.h
�ļ�ʵ�ֹ���: ¼��ģ����غ��������������͵Ķ���
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
#include "stdafx.h"
#include "myRecorder.h"
#include "voiceprint.h"
#include "CharacterHelper.h"
#include "AudioConfig.h"
#include "resource.h"

static CAPECharacterHelper charHelper;

extern SysConfig sys;

// һЩ������ʾ�ִ�
static TCHAR * szOpenError = TEXT("�� waveform audio����!");
static TCHAR * szMemError = TEXT("��Ƶ���仺�����!");
static WAVEFORMATEX waveform;

static void isWaveSuccess(MMRESULT waveresult);
static void SetWaveform(PWAVEFORMATEX pWaveform, WORD nChannels, DWORD nSamplesPerSec, WORD wBitsPerSample);
static void SetPWaveHDR(PWAVEHDR pWaveHdr, PBYTE pBuffer, DWORD bufferLen, DWORD dwFlg, DWORD dwLoops);
static void SetRecord(HRECORD hRecord);
static void CloseRecord(HRECORD hRecord);
static DWORD WINAPI myRecThread(void * pRecParam);

/** @function
********************************************************************************
<PRE>
������: isWaveSuccess()
����: ���wave�������õķ���ֵ���д��򵯳��Ի���
�÷�:
����:
[IN] waveresult: wave�������õķ���ֵ
����:
����:
��������: myRecThread()
</PRE>
*******************************************************************************/
static void isWaveSuccess(MMRESULT waveresult)
{
	switch (waveresult)
	{
	case MMSYSERR_INVALHANDLE:    //�豸�����Ч
		MessageBox(NULL, TEXT("MMSYSERR_INVALHANDLE"), szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		break;

	case WAVERR_UNPREPARED:     //waveͷδ׼��
		MessageBox(NULL, TEXT("WAVERR_UNPREPARED"), szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		break;
	default:	 break;
	}
}


/** @function
********************************************************************************
<PRE>
������: SetWaveform()
����: ���ã���䣩WAVEFORMATEX�ṹ
�÷�:
����:
[IN] pWaveform: WAVEFORMATEX�ṹָ��
[IN] nChannels: ���� [1����������or 2����������]
[IN] nSamplesPerSec: ����Ƶ��
[IN] wBitsPerSample: ������������
����:
����:
��������: CreateRecord()
</PRE>
*******************************************************************************/
static void SetWaveform(PWAVEFORMATEX pWaveform, WORD nChannels,
	DWORD nSamplesPerSec, WORD wBitsPerSample)
{
	pWaveform->wFormatTag = WAVE_FORMAT_PCM;
	pWaveform->nChannels = nChannels;    //1
	pWaveform->nSamplesPerSec = nSamplesPerSec;    //SAMPLE_FREQUENCY
	pWaveform->nAvgBytesPerSec = nChannels * nSamplesPerSec;    //SAMPLE_FREQUENCY
	pWaveform->nBlockAlign = nChannels * wBitsPerSample / 8;    //1
	pWaveform->wBitsPerSample = wBitsPerSample;    //8 
	pWaveform->cbSize = 0;
}

/** @function
********************************************************************************
<PRE>
������: SetPWaveHDR()
����: ���ã���䣩PWAVEHDR�ṹ
�÷�:
����:
[IN] pWaveHdr: WAVEͷ���ָ��
[IN] pBuffer: ¼��/��������
[IN] bufferLen: ���峤��
[IN] dwFlg: ������״̬��־
[IN] dwLoops: �����ظ����Ŵ���
����:
����:
��������: SetRecord()
</PRE>
*******************************************************************************/
static void SetPWaveHDR(PWAVEHDR pWaveHdr, PBYTE pBuffer,
	DWORD bufferLen, DWORD dwFlg, DWORD dwLoops)
{
	//pWaveHdr->lpData = pBuffer;
	pWaveHdr->dwBufferLength = bufferLen;   //BUFFLEN;
	pWaveHdr->dwBytesRecorded = 0;
	pWaveHdr->dwUser = 0;
	pWaveHdr->dwFlags = dwFlg;   //0��Ĭ�ϣ�
	pWaveHdr->dwLoops = dwLoops;   //¼��ʱȡ1
	pWaveHdr->lpNext = NULL;
	pWaveHdr->reserved = 0;
}


/** @function
********************************************************************************
<PRE>
������: CreateRecord()
����: ��¼����������ʼ��WAVEFORMATEX�ṹ����ʼ��"¼��д��"���ָ��
lhWaveIn��������¼���̡߳�
�÷�:
����:
����:
�ǿ�: ��ʼ���ɹ�
NULL: ʧ��
����: SetWaveform()
��������: MainDlg()
</PRE>
*******************************************************************************/
HRECORD CreateRecord(HWND hwnd)
{
	MYRECORD * myRecord = NULL;
	RECPARAM * pRecParam = NULL;    //¼���߳��������  

	// �����ٽ�������֤���߳���¼���߳��ڴ���¼���߳�ʱ���� 
	 EnterCriticalSection(&cs); 
	myRecord = (MYRECORD *)malloc(sizeof(MYRECORD));
	pRecParam = (RECPARAM *)malloc(sizeof(RECPARAM));
	if (!myRecord || !pRecParam)
	{
		MessageBox(NULL, TEXT("��ʼ��myrecordʧ�ܣ�"), szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	//����¼�����
	myRecord->lhWaveIn = (LPHWAVEIN)malloc(sizeof(HWAVEIN));
	if (!myRecord->lhWaveIn)
	{
		MessageBox(NULL, TEXT("����¼�����ʧ�ܣ�"), szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	//����waveform�ṹ
	myRecord->pWaveform = (PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));
	if (!myRecord->pWaveform)
	{
		MessageBox(NULL, TEXT("��ʼ��WAVEFORMATEX�ṹʧ�ܣ�"), szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	// Open waveform audio for input
	SetWaveform(myRecord->pWaveform, 1,sys.soundDevcieFrequency , 8);

	pRecParam->hwnd = hwnd;
	pRecParam->myRecord = myRecord;
	my_voiceprint.g_hRecThread = CreateThread(NULL, 0, myRecThread, pRecParam, 0, &my_voiceprint.g_RecThreadID);

	//����MM_WIM_OPEN��Ϣ
	if (waveInOpen(myRecord->lhWaveIn, WAVE_MAPPER, myRecord->pWaveform,
		my_voiceprint.g_RecThreadID, 0, CALLBACK_THREAD))
	{
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(hwnd, szOpenError, szAppVersion,
			MB_ICONEXCLAMATION | MB_OK);
		return NULL;
	}

	LeaveCriticalSection(&cs);    // �뿪�ٽ���
	return((void *)myRecord);
}


/** @function
********************************************************************************
<PRE>
������: SetRecord()
����: ����¼����������ʼ¼�������䲢����PWAVEHDR �ṹ������¼��������
������¼�����У�
�÷�: ��waveInOpen()������MM_WIM_OPEN��Ϣ��ʹ��
����:
[IN] hRecord: ¼��ʵ��
����:
����: SetPWaveHDR()
��������: myRecThread()
</PRE>
*******************************************************************************/
static void SetRecord(HRECORD hRecord)
{
	int  i;
	MYRECORD * myRecord = (MYRECORD *)hRecord;
	HWAVEIN hWaveIn = *(myRecord->lhWaveIn);

	// ����¼�������PWAVEHDR�ṹ
	for (i = 0; i < BUFF_NUM; ++i)
	{
		myRecord->pBuffer[i] = (PBYTE)malloc(BUFFLEN);
		myRecord->pWaveHdr[i] = (PWAVEHDR)malloc(sizeof(WAVEHDR));
		if (!myRecord->pBuffer[i] || !myRecord->pWaveHdr[i])
		{
			break;
		}

		// ����WAVEHDR��׼����¼��
		SetPWaveHDR(myRecord->pWaveHdr[i], myRecord->pBuffer[i], BUFFLEN, 0, 1);
		waveInPrepareHeader(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));
		waveInAddBuffer(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));

	} //������for (i=0; i<BUFF_NUM; ++i)

	  // �ڴ�������
	if (i < BUFF_NUM)
	{
		for (--i; i >= 0; --i)
		{
			waveInUnprepareHeader(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));
			free(myRecord->pBuffer[i]);
			free(myRecord->pWaveHdr[i]);
		}
		return;
	} // ������if (i<BUFF_NUM)

	  //��ʼ¼����������¼������MM_WIM_DATA��Ϣ
	waveInStart(hWaveIn);
}


/** @function
********************************************************************************
<PRE>
������: CloseRecord()
����: ���¼������
�÷�: ��waveInClose()������MM_WIM_CLOSE��Ϣ��ʹ��
����:
[IN] hRecord: ¼��ʵ��
����:
����:
��������: myRecThread()
</PRE>
*******************************************************************************/
static void CloseRecord(HRECORD hRecord)
{
	int i;
	MYRECORD * myRecord = (MYRECORD *)hRecord;
	HWAVEIN hWaveIn = *(myRecord->lhWaveIn);

	for (i = 0; i<BUFF_NUM; ++i)
	{
		waveInUnprepareHeader(hWaveIn, myRecord->pWaveHdr[i], sizeof(WAVEHDR));
		free(myRecord->pBuffer[i]);
		free(myRecord->pWaveHdr[i]);
	}
	free(myRecord->pWaveform);
	free(myRecord->lhWaveIn);
	free(myRecord);
}


/** @function
********************************************************************************
<PRE>
������: myRecThread()
����: ¼���̺߳���
�÷�:
����:
[IN] pRecParam: ָ��RECPARAM�ṹ��ָ��
����:
����: SetRecord(), CloseRecord()
��������: CreateRecord()
</PRE>
*******************************************************************************/
static DWORD WINAPI myRecThread(void * pRecParam)
{
	MSG msg;
	HWND hwnd = ((RECPARAM *)pRecParam)->hwnd;
	MYRECORD * myRecord =(MYRECORD*)((RECPARAM *)pRecParam)->myRecord;
	BOOL  bEnding = FALSE;
	BYTE * pVoiceBuffer = NULL;

	int i;
	int frame_num = 0;
	int buff_num = g_build_buff_num;
	double temp = 0;


	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		case  MM_WIM_OPEN:
			EnterCriticalSection(&cs);     //�����ٽ���
			SetRecord(myRecord);
			//bEnding =  FALSE; 
			LeaveCriticalSection(&cs);    //�뿪�ٽ���
			break;

		case  MM_WIM_DATA:
			if (bEnding && myRecord)    //�ж�myRecord��Ϊ�˱����ظ�waveInClose()
			{
				//����MM_WIM_CLOSE��Ϣ
				EnterCriticalSection(&cs);     //�����ٽ���
				isWaveSuccess(waveInClose(*(myRecord->lhWaveIn)));
				CloseRecord(myRecord);
				myRecord = NULL;
				LeaveCriticalSection(&cs);    //�뿪�ٽ���
				break;
			}

			if (!myRecord)
			{
				break;
			}
			
			pVoiceBuffer = charHelper.GetUTF8FromANSI(((PWAVEHDR)(msg.lParam))->lpData);
			//--------------------------------------------------------------------------------------------------
			// ��ȡ����������ʶ������
			//
			if (((PWAVEHDR)(msg.lParam))->dwBytesRecorded > 0 && g_buildGMM)
			{
				if (!my_voiceprint.voice_mdl.bValued && frame_num >= buff_num)
				{
					break;
				}
				else if (my_voiceprint.voice_mdl.bValued && frame_num >= IDENTIFY_BUFF_NUM)
				{
					break;
				}
				for (i = 0, temp = 0; i < BUFFLEN; ++i)
				{
					temp += abs((pVoiceBuffer[i] - 128));
				}
				if (temp > 1050)  // �����ж� 1050
				{
					memcpy(my_voiceprint.voice_mdl.pVoice + (frame_num % 5)*BUFFLEN, pVoiceBuffer, BUFFLEN);
					++frame_num;
				}
				if (frame_num > 0 && frame_num % 5 == 0)
				{
					fwrite(my_voiceprint.voice_mdl.pVoice, sizeof(TBYTE), BUFFLEN * 5, my_voiceprint.voice_mdl.file_stream);
				}
				if ((!my_voiceprint.voice_mdl.bValued && frame_num == buff_num) ||
					(my_voiceprint.voice_mdl.bValued && frame_num == IDENTIFY_BUFF_NUM))
				{
					if (frame_num > 0 && frame_num % 5 != 0)
					{
						fwrite(my_voiceprint.voice_mdl.pVoice, sizeof(TBYTE), (frame_num % 5)* BUFFLEN, my_voiceprint.voice_mdl.file_stream);
					}
					free(my_voiceprint.voice_mdl.pVoice);
					my_voiceprint.voice_mdl.pVoice = NULL;
					fclose(my_voiceprint.voice_mdl.file_stream);
					my_voiceprint.voice_mdl.file_stream = NULL;
					PostMessage(hwndMainDlg, WM_BUILD_GMM, 0, 0);

					//xprint(TEXT("�����ռ����"));
				}
			} // end: if (g_buildGMM)
			waveInAddBuffer(*(myRecord->lhWaveIn), (PWAVEHDR)(msg.lParam), sizeof(WAVEHDR));
			break;

		case MM_WIM_CLOSE:
			CloseHandle(my_voiceprint.g_hRecThread);
			my_voiceprint.g_RecThreadID = 0;
			free(pRecParam);
			/*SetWindowText(GetDlgItem(hwnd, IDC_BUILD_GMM), TEXT("��ȡ����"));*/
			if (bExit)
			{
				bExit = FALSE;
				killThreading = FALSE;
				QuitMyWindow();
			}
			if (my_voiceprint.voice_mdl.file_stream)
			{
				fclose(my_voiceprint.voice_mdl.file_stream);
				my_voiceprint.voice_mdl.file_stream = NULL;
			}
			return msg.wParam;

		case WM_RECORD_END:
			if (bExit)
			{
				killThreading = TRUE;
			}
			bEnding = TRUE;
			// ʹ¼��ֹͣ��������MM_WIM_DATA��Ϣ
			isWaveSuccess(waveInReset(*(myRecord->lhWaveIn)));
			break;
		}
	}

	free(pRecParam);
	return msg.wParam;
}