#include "stdafx.h"
#include "AudioRecognize.h"
#include "voiceprint.h"
#include "logger.h" 
#include "wow.h" 

#include "global.h"
#include "AudioConfig.h"  
#include "AudioManager.h" 


extern VOICE_PRINT my_voiceprint; 
extern Wow wow;
extern SysConfig sys;

//¼��
class CPlaybackEventHandler : public IPlaybackEvent
{
public:
	virtual VOID OnPlaybackEnd()
	{

	}
};



CPlaybackEventHandler g_playbackEventHandler;
CAudioManager g_audioMgr(&g_playbackEventHandler);




AudioRecognize::AudioRecognize()
{
}

AudioRecognize::~AudioRecognize()
{
}

 void AudioRecognize::voicePrintIdentify(HWND hwnd)
{

	LOG(INFO) << "VoicePrintIdentify";
	int i;
	int temp_num = 0;
	double temp = 0;
	double percent = my_voiceprint.voice_mdl.percent / 100.0; 
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		temp = (1 + percent) * my_voiceprint.voice_mdl.value[i][0]; 
		temp >= my_voiceprint.voice_mdl.value[i][1] ? ++temp_num : temp_num;
	} 

	//Logger::displayLog("����ƥ�������%d\r\n", temp_num);
	if (temp_num >= sys.fishSoundMatchAccuracy)
	{
		LOG(INFO) << "VoicePrintIdentify ʶ��ɹ�,ʶ�������="<< temp_num;
		
		if (wow.start_rec_sys_sound )
		{
			if (!wow.pauseSoundRecognize)
			{
				//�ո�
				wow.finishFish(&(wow.fish_point), &(sys.wowWindowRect));
				//�׸� 
				wow.startFish(true);
			}
			
		}
		else
		{
			MessageBox(NULL, L"ʶ��ɹ�", szAppVersion, 0);
		} 
	}
	else
	{
		LOG(INFO) << "VoicePrintIdentify ʶ��ʧ��,ʶ�����="<< temp_num;
	}
	 
}
BOOL AudioRecognize::buildGMM(HWND hwnd)
{
	LOG(INFO) << "OnBuildGMM";
	int buff_num = g_build_buff_num;
	//MessageBox(hwnd, TEXT("�����Ѿ��ɼ���"), szAppVersion, 0);
	// �ļ�̽�� 
	CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	//my_voiceprint.voice_mdl.voice_file.f_CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	if (my_voiceprint.voice_mdl.voice_file.file_size == -1)
	{
		LOG(ERROR) << "�쳣��������ȡ�ļ������ڣ�";
		return FALSE;
	}
	else if ((!my_voiceprint.voice_mdl.bValued && my_voiceprint.voice_mdl.voice_file.file_size<BUFFLEN*buff_num)
		|| (my_voiceprint.voice_mdl.bValued && my_voiceprint.voice_mdl.voice_file.file_size<BUFFLEN*IDENTIFY_BUFF_NUM))
	{
		LOG(ERROR) << "�쳣���������ݹ��٣���";
		return FALSE;
	}
	/**/
	BOOL getUsrGMM = false;

	try
	{
		getUsrGMM = GetUsrGMM(&my_voiceprint, my_voiceprint.voice_mdl.bValued ? E_IDENTIFY : E_BUILD);
		LOG(ERROR) << "getUsrGMM";
	}
	catch (const std::exception&)
	{
		LOG(ERROR) << "�쳣��getUsrGMM�쳣";
		getUsrGMM = false;
	}
	catch (...)
	{
		LOG(ERROR) << "�쳣��getUsrGMM�쳣";
		getUsrGMM = false;
	}


	if (getUsrGMM)
	{
		if (!my_voiceprint.voice_mdl.bValued)
		{
			my_voiceprint.voice_mdl.bValued = TRUE;
			LOG(INFO) << "��ģ�ɹ���";
			MessageBox(hwnd, TEXT("��ģ�ɹ���"), szAppVersion, 0);

		}
		else
		{
			voicePrintIdentify(hwnd);
		}
	}
	else
	{
		LOG(ERROR) << "��ģʧ�ܣ�";
	}

	return TRUE;
}

 
 


 

void AudioRecognize::stopVoice(HWND hwnd)
{
	LOG(INFO) << "stopVoice";
	g_buildGMM = FALSE;
	LOG(INFO) << "stopVoice g_buildGMM = FALSE"; 
	stopRecord();
	LOG(INFO) << "stopRecord";
}


 
 

void AudioRecognize::startBuild(HWND hwnd)
{ 
	//if (!g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	if (!g_buildGMM )
	{
		my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);
		VoicePrintInit(&my_voiceprint);
		/* ����¼�� */
		MessageBox(hwnd, TEXT("��ģ��ʼ\n��ʼ¼��..."), szAppVersion, 0);
		g_buildGMM = TRUE;
		startRecord();
	}
	else if (my_voiceprint.voice_mdl.bValued) // �Ѿ���ģ��	
	{
		MessageBox(hwnd, TEXT("��������ģ�ͣ��뵼�������ã�"), szAppVersion, 0);
	}
}

 void AudioRecognize::identify(HWND hwnd)
{
	LOG(INFO) << "OnIdentifyBtn";
	char cont[100];
	sprintf_s(cont, "g_buildGMM= %d \r\n my_voiceprint.voice_mdl.bValued= %d\r\n  ", (int)g_buildGMM, (int)my_voiceprint.voice_mdl.bValued);
	LOG(INFO) << cont;
	if (!g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("������ȡ�������ƣ�"), szAppVersion, 0);
	}
	else if (g_buildGMM && !my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("������ȡ���ƣ�"), szAppVersion, 0);
	}
	else  if (!g_buildGMM && my_voiceprint.voice_mdl.bValued)
	{

		g_buildGMM = TRUE;
		startRecord();
		LOG(INFO) << "OnIdentifyBtn StartRecordmgt"; 
	}
}

 void AudioRecognize::exportGMM(HWND hwnd ,string exportFilePath)
{
	int i;
	FILE * fsave = NULL;
	VOICE_MODEL * p_data = NULL;  
	if (!my_voiceprint.voice_mdl.bValued)
	{
		MessageBox(hwnd, TEXT("��������ģ�ͣ�"), szAppVersion, 0);
		return;
	}  
	fopen_s(&fsave, exportFilePath.c_str(), "wb");
	if (!fsave)
	{
		MessageBox(hwnd, TEXT("�����ļ�ʧ�ܣ�"), szAppVersion, 0);
		return;
	}
	p_data = &my_voiceprint.voice_mdl;

	// д��GMMģ������
	fwrite(&p_data->frame_num, sizeof(p_data->frame_num), 1, fsave);
	fwrite(&p_data->m, sizeof(p_data->m), 1, fsave);
	fwrite(&p_data->percent, sizeof(p_data->percent), 1, fsave);
	fwrite(p_data->gmm.p, sizeof(p_data->gmm.p[0]), p_data->m, fsave);
	for (i = 0; i < p_data->m; ++i)
	{
		fwrite(p_data->gmm.u[i], sizeof(p_data->gmm.u[0][0]), D, fsave);
	}
	for (i = 0; i < p_data->m; ++i)
	{
		fwrite(p_data->gmm.cMatrix[i], sizeof(p_data->gmm.cMatrix[0][0]), D, fsave);
	}

	// д��ʶ����ֵ
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		fwrite(&p_data->value[i][0], sizeof(p_data->value[0][0]), 1, fsave);
	}
	fclose(fsave); 
}


  

void AudioRecognize::importGMM(HWND hwnd,DWORD sampleFrequency)
{ 
	int i;
	FILE * f_open = NULL;
	VOICE_MODEL * p_data = NULL;
	/*
	FILE_CLASS open_file = { FileClassInit };
	FileClassInit(&open_file);
	FileInitialize(hwnd);
	FileOpenDlg(hwnd, &open_file);
	*/
	string gmmFile=sys.getStdGMMFilePath(sampleFrequency);

	fopen_s(&f_open, gmmFile.c_str(), "rb");
	if (!f_open)
	{
		MessageBox(hwnd, TEXT("���ļ�ʧ�ܣ�"), szAppVersion, 0);
		return;
	}
	p_data = &my_voiceprint.voice_mdl;
	p_data->gmm.f_FreeGMM(&p_data->gmm);

	// ����GMMģ������
	fread(&p_data->frame_num, sizeof(p_data->frame_num), 1, f_open);
	fread(&p_data->m, sizeof(p_data->m), 1, f_open);
	fread(&p_data->percent, sizeof(p_data->percent), 1, f_open);
	p_data->gmm.f_Init(&p_data->gmm, p_data->m);
	p_data->gmm.f_CallocGMM(&p_data->gmm);
	fread(p_data->gmm.p, sizeof(p_data->gmm.p[0]), p_data->m, f_open);
	for (i = 0; i < p_data->m; ++i)
	{
		fread(p_data->gmm.u[i], sizeof(p_data->gmm.u[0][0]), D, f_open);
	}
	for (i = 0; i < p_data->m; ++i)
	{
		fread(p_data->gmm.cMatrix[i], sizeof(p_data->gmm.cMatrix[0][0]), D, f_open);
	}

	// ����ʶ����ֵ
	for (i = 0; i < IDENTIFY_NUM; ++i)
	{
		fread(&p_data->value[i][0], sizeof(p_data->value[0][0]), 1, f_open);
	}
	fclose(f_open);
	p_data->bValued = TRUE;
	p_data = NULL;
	//-------------------------------------------------------------------------------------------------
	// ˢ�½�����ʾ
	//  
	g_build_buff_num = my_voiceprint.voice_mdl.frame_num * FRAME_LEN / BUFFLEN; 
}


/* ɾ����ʱ����ת���ļ� */
void AudioRecognize::deleteVoiceFile()
{
	// �ļ�̽�� 
	CheckFileSize(&my_voiceprint.voice_mdl.voice_file);
	if (my_voiceprint.voice_mdl.voice_file.file_size == -1)
	{
		return;
	}
	if (my_voiceprint.voice_mdl.file_stream)
	{
		fclose(my_voiceprint.voice_mdl.file_stream);
	}
	if (remove(my_voiceprint.voice_mdl.voice_file.szFileName))
	{
		LOG(ERROR) << "ɾ���ļ�"<< my_voiceprint.voice_mdl.voice_file.szFileName<<"ʧ��";
	}
}


void AudioRecognize::reset(HWND hwnd)
{
	my_voiceprint.g_hRecThread = NULL;
	my_voiceprint.g_RecThreadID = 0;
	g_buildGMM = FALSE;
	my_voiceprint.voice_mdl.bValued = FALSE;
	my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);
	my_voiceprint.f_Init(&my_voiceprint);
	my_voiceprint.hwnd = hwnd; 
}

void AudioRecognize::freeVoicePrint()
{
	g_buildGMM = FALSE;
	my_voiceprint.voice_mdl.gmm.f_FreeGMM(&my_voiceprint.voice_mdl.gmm);

}

void AudioRecognize::initVoicePrint()
{
	VoicePrintInit(&my_voiceprint);

}

void AudioRecognize::updateVoicePrint(int percent,int m,int frame_num,bool identify)
{
	my_voiceprint.voice_mdl.percent = percent;
	if (identify)
	{
		return;
	}
	my_voiceprint.voice_mdl.m = m;
	my_voiceprint.voice_mdl.frame_num = frame_num;
	g_build_buff_num = frame_num* FRAME_LEN / BUFFLEN;

}

BOOL AudioRecognize::hasVoiceId()
{ 
	return my_voiceprint.voice_mdl.bValued; 
}

bool AudioRecognize::beginRecord(HWND hwnd)
{
	if (!my_voiceprint.myRecord && !my_voiceprint.g_RecThreadID)
	{
		my_voiceprint.myRecord = CreateRecord(hwnd); 
		return true;
	}
	return false;
}
bool AudioRecognize::endRecord(int msg)
{
	if (my_voiceprint.myRecord && my_voiceprint.g_RecThreadID)
	{ 
		// ��¼���̷߳��ͽ�����Ϣ
		PostThreadMessage(my_voiceprint.g_RecThreadID, msg, 0, 0);
		my_voiceprint.myRecord = NULL;
		return true;
	}
	return false;
}

//¼��

void AudioRecognize::startRecord()
{
	if (g_audioMgr.StartCapture())
	{
		LOG(INFO) << "g_audioMgr.StartCapture()";
	}
}

void AudioRecognize::stopRecord()
{
	g_audioMgr.StopCapture();
	LOG(INFO) << "ֹͣ¼��";

}

void AudioRecognize::startPlay()
{
	if (g_audioMgr.CanPlay())
	{
		if (g_audioMgr.StartPlayback())
		{

		}
	}
}

void AudioRecognize::stopPlay()
{
	g_audioMgr.StopPlayback();
}

bool  AudioRecognize::refreshSoundDeviceFrequency()
{

	DWORD deviceFrequency=g_audioMgr.getDeviceFrequency();
	if (deviceFrequency == 0)
	{
		return false;
	}
	
	sys.soundDevcieFrequency = deviceFrequency;
	return true;


}