#include "stdafx.h"
#include "Monitor.h" 
#include "sysUtil.h"
#include "tools.h"
#include "sysconfig.h"
#include "logger.h" 
#include "ThreadFunction.h"
#include "AudioManager.h" 
#include "global.h"
#include "AudioRecognize.h"
#include "resource.h"

extern Wow wow;
extern SysConfig sys;
using namespace  std;


    
Monitor::Monitor()
{
	
	
}



Monitor::~Monitor()
{ 
	DeleteObject(sys.wowWindowInfo.hWnd);
	if (checkLicExpiredThread != NULL)
	{
		CloseHandle(checkLicExpiredThread);
		checkLicExpiredThread = NULL;
	}
	if (hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
	}

	if (timerThread != NULL)
	{
		CloseHandle(timerThread);
		timerThread = NULL;
	}
	isMonitorRunning = false;
	//�ر�mq��ʱ��
	KillTimer(hwndMainDlg, sys.mqTimerId);
}


void  Monitor::initMqTimer(HWND hwnd)
{
	//����mq��ʱ��
	SetTimer(hwnd, sys.mqTimerId, 1000, MqTimerProc);
}

void Monitor::startLicExpirationCheckThread()
{
	checkLicExpiredThread = CreateThread(NULL, 0, licExpireCheck, NULL, 0, NULL);
	if (checkLicExpiredThread == NULL)
	{
		sys.isActive = false;
	}

}

void Monitor::getWowProcessID()
{
	SysUtil::GetProcessInfo(sys.WOW_EXE_64_NAME,&(sys.wowWindowInfo));
	if (sys.wowWindowInfo.dwProcessId == NULL || sys.wowWindowInfo.hWnd == NULL)
	{
		SysUtil::GetProcessInfo(sys.WOW_EXE_32_NAME,&(sys.wowWindowInfo));
	}
	if (sys.wowWindowInfo.dwProcessId == NULL || sys.wowWindowInfo.hWnd == NULL && !sys.otherWowName.empty())
	{
		SysUtil::GetProcessInfo(sys.otherWowName.c_str(), &(sys.wowWindowInfo));
	}
	
}

void Monitor::getFrontWindowID()
{
	HWND hWnd = ::GetForegroundWindow();  
	GetWindowThreadProcessId(hWnd, &frontWindowProcessID);
}
void Monitor::monitorInit()
{
	getWowProcessID();
}
bool Monitor::isWowWindowFocus()
{
	getFrontWindowID();
	return sys.wowWindowInfo.dwProcessId == frontWindowProcessID;
}
bool Monitor::checkWowWindowFocus()
{
	if (!isWowWindowFocus() && sys.wowWindowInfo.hWnd)
	{
		ShowWindow(sys.wowWindowInfo.hWnd, SW_SHOWNORMAL);
		Sleep(100);
	}
	return true;
}

void Monitor::getWowWindowRect(RECT *rect)
{
	if (sys.wowWindowInfo.hWnd)
	{
		GetWindowRect(sys.wowWindowInfo.hWnd, rect);
		return;
	} 

	rect->bottom = 0;
	rect->left = 0;
	rect->right = 0;
	rect->top = 0;
	return;
}



bool Monitor::checkWindowSize(RECT *rect)
{
	getWowWindowRect(rect); 
	if (calculateRectSize(rect) < 20000)
	{
		return false;
	}
	return true;
}

bool Monitor::refreshWowWindowInfo()
{
	sys.getSystemDisplayZoom();
	if (!checkWindowSize(&(sys.wowWindowRect)))
	{
		LOG(ERROR) << "�쳣��δ�ҵ���Ϸ����";
		MessageBox(NULL, TEXT("δ�ҵ���Ϸ����"), TEXT("����"), 0);
		return false;
	}
	sys.chooseImgID();
	//Logger::displayLog("ѡ��std imgIDΪ%s\r\n",sys.FISHING_STATUS_IMGID_FILE);
	sys.freshSearchField(&(sys.wowWindowRect));
	sys.freshFishStatusCheckFeild(sys.wowWindowRect, true);
	return true;
}

void  Monitor::startMonitor()
{
	if (isMonitorRunning)
	{
		return;
	}
	isMonitorRunning = true;
	 monitorInit(); 
	 if (!refreshWowWindowInfo())
	 {
		 stopMonitor();
		 return;
	 }
	Logger::displayLog("��Ϸ���ڴ�С%d*%d \r\n", sys.wowWindowRect.right- sys.wowWindowRect.left, sys.wowWindowRect.bottom- sys.wowWindowRect.top);
	LOG(INFO) << "����������";  
	wow.start_rec_sys_sound = true;  
	AudioRecognize::startRecord();
	PostMessage(hwndMainDlg, WM_BUILD_GMM, 0, 0);
	
	//�׸͵���
	//startFish();
	wow.timer_destroy = false;
	//����״̬����̣߳��Ƿ��ڵ���״̬��������껥��ʧ��2�Σ������׸�
	//�ر���껥�����
	if (hThread != NULL)
	{
		CloseHandle(hThread);
		hThread = NULL;
		wow.stopStatus = true;
	}
	wow.find_cursor_thread_destroy = false;
	//������껥������߳�
	if (hThread == NULL)
	{
		LOG(INFO) << "������껥������߳�";
		wow.stopStatus = false;
		hThread = CreateThread(NULL, 0,  Fun, NULL, 0, NULL);
	}
	//������Ļ����״̬����߳�
	if (timerThread == NULL)
	{
		LOG(INFO) << "��������״̬����߳�";
		timerThread = CreateThread(NULL, 0, fishTimer, NULL, 0, NULL);
	}
	startKeyPressTimer();
	 
}
void  Monitor::stopMonitor()
{
	isMonitorRunning = false;
	stopKeyPressTimer();
	//wow.start_rec_sys_sound = false;
	AudioRecognize::stopRecord();
	//������껥������߳�
	wow.find_cursor_thread_destroy = true;
	if (hThread != NULL)
	{
		LOG(INFO) << "�ر���껥������߳�";
		CloseHandle(hThread);
		hThread = NULL;
		wow.stopStatus = 1;
	}
	//������Ļ����״̬����߳�
	wow.timer_destroy = true;
	if (timerThread != NULL)
	{
		LOG(INFO) << "�رյ���״̬����߳�";
		CloseHandle(timerThread);
		timerThread = NULL;
	}
	wow.IS_FISHING = false;
	wow.FIND_CUSOR = sys.not_started_searching_cursor_pos;
	wow.RECOGNISE_SOUND = false;
	AudioRecognize::deleteVoiceFile();

}


bool Monitor::startKeyPressTimer()
{ 
	LOG(INFO) << "startKeyPressTimer initMqTimerId";
	SetTimer(hwndMainDlg, sys.initMqTimerId, sys.START_MONITOR_DELAY*1000, TimerProc); 
	if (sys.timerThread1_Enable)
	{
		SetTimer(hwndMainDlg, sys.timer1Id, sys.timerThread1_intervalSeconds * 1000, TimerProc);
		LOG(INFO) << "startKeyPressTimer timer1Id";
	}
	if (sys.timerThread2_Enable)
	{
		SetTimer(hwndMainDlg, sys.timer2Id, sys.timerThread2_intervalSeconds * 1000, TimerProc);
		LOG(INFO) << "startKeyPressTimer timer2Id";
	} 
	//���õ����Ϲ�ʱ������ⶨʱ��������25��û�Ϲ��������׸�
	wow.lastFinishFishTime = time(NULL)+3;
	SetTimer(hwndMainDlg, sys.fishIntervalCheckTimerId, 1000, TimerProc);
	
	return true;
}

bool Monitor::stopKeyPressTimer()
{
	
	KillTimer(hwndMainDlg,sys.timer1Id);
	KillTimer(hwndMainDlg, sys.timer2Id); 
	wow.lastFinishFishTime = 0;
	KillTimer(hwndMainDlg, sys.fishIntervalCheckTimerId);
	//�رյ����Ϲ�ʱ������ⶨʱ��
	LOG(INFO) << "stopKeyPressTimer timer stoped";
	return true;
}

 




bool Monitor::pauseMonitor()
{
	wow.stopStatus = true;
	wow.fishTimerPauseStatus = true;
	wow.pauseSoundRecognize = true;
	LOG(INFO) << "pauseMonitor";
	return true;
}
bool Monitor::continueMonitor()
{
	 
	wow.stopStatus = false; 
	wow.fishTimerPauseStatus = false;
	wow.pauseSoundRecognize = false;
		LOG(INFO) << "continueMonitor";
	return true;
}