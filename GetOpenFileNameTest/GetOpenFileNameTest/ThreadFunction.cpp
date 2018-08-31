#include "stdafx.h"
#include "ThreadFunction.h"
#include "Security.h"
#include"dlgHelper.h"  
#include "kbHook.h"
#include "MQ.h" 

extern Monitor monitor;
extern Wow wow;
extern SysConfig sys;
extern MQ mq;

bool validateFishingFloatPoint(POINT *point);
bool  findFishingFloatWithinRect(int offset);
void monitorStartCountDown(bool display);

void processMqMsg(MqMsg msg);
void checkFishIntervalTime();

DWORD WINAPI  Fun(LPVOID lpParamter)
{
	
	monitorStartCountDown(true);
	if (monitor.isMonitorRunning)
	{
		wow.startFish(true);
	}
	
	while (!wow.find_cursor_thread_destroy && monitor.isMonitorRunning)
	{
		
		LOG(INFO) << "Fun�߳�ѭ��";
		int matchNum = 0; 
		if (!wow.stopStatus)
		{ 
			Sleep(sys.delayCursorFishFlotCheck);
			LOG(INFO) << "��ʼ�����껥��";
			wow.FIND_CUSOR_FAILED_NUM = 0;
			wow.FIND_CUSOR = sys.searching_cursor_pos;
			int matchTemplate = 0;
			matchNum = 0;
			while (matchTemplate > 0 && monitor.isMonitorRunning)
			{
				ImgEx::getFishingFloatFromCheckArea(&(wow.fish_point));
				//displayLog("ͼ��ģ��ʶ��� %d %d \r\n", fish_point.x, fish_point.y);
				MouseHook::mouseMoveClientToScreen(&(wow.fish_point), &(sys.wowWindowRect));
				if (validateFishingFloatPoint(&(wow.fish_point)))
				{
					matchNum++;
					if (matchNum >= 2)
					{
						//displayLog("ʹ��ͼ��ģ��ʶ��� %d %d \r\n", fish_point.x, fish_point.y);
						LOG(INFO) << "ʹ��ͼ��ģ��ʶ���";
						MouseHook::mouseMoveClientToScreen(&(wow.fish_point), &(sys.wowWindowRect));
						//CloseHandle(hThread);
						//hThread = NULL;
						wow.stopStatus = true;
						wow.FIND_CUSOR = sys.located_cursor_pos;
						break;
					}

				}
				Sleep(500);
				matchTemplate--;
			}

			while (wow.FIND_CUSOR == sys.searching_cursor_pos && !wow.stopStatus &&  monitor.isMonitorRunning)
			{
				if (findFishingFloatWithinRect(wow.FIND_CUSOR_FAILED_NUM == 0 ? 0 : sys.SEARCH_FISHING_CURSOR_Y_OFFSET / 2))
				{
					//displayLog("ʹ��ͼ�����پ���ʶ��� %d %d \r\n", fish_point.x, fish_point.y);
					LOG(INFO) << "ʹ��ͼ�����پ���ʶ���";
					MouseHook::mouseMoveClientToScreen(&(wow.fish_point), &(sys.wowWindowRect));
					wow.stopStatus = true;
					wow.FIND_CUSOR = sys.located_cursor_pos;
					break;
				}
				//������ѭ��һ�κ�δ�ҵ���������
				wow.FIND_CUSOR_FAILED_NUM++;
				//���һ�������ʧ��2�κ�ֹͣѰ��
				if (wow.FIND_CUSOR_FAILED_NUM >= 2)
				{
					LOG(INFO) << "δ�ҵ���껥����";
					wow.FIND_CUSOR = sys.not_find_cursor_pos;
					wow.stopStatus = true;
					break;
				}

			}

		}
		//if (FIND_CUSOR = wating_for_start_fish && check_is_fishing)
		//{
		//	displayLog("check_is_fishing δ���ڵ���״̬,���¼����껥��\r\n");
		//	FIND_CUSOR = not_started_searching_cursor_pos;
		//	stopStatus = false;
		//} 
		Sleep(1000);
	}
	return 1;
}

bool  findFishingFloatWithinRect(int offset)
{
	int x, y;
	for (y = sys.SEARCH_FISHING_CURSOR_Y_MIN + offset; y < sys.SEARCH_FISHING_CURSOR_Y_MAX && !wow.stopStatus; y += sys.SEARCH_FISHING_CURSOR_Y_OFFSET)
	{
		for (x = sys.SEARCH_FISHING_CURSOR_X_MIN; x < sys.SEARCH_FISHING_CURSOR_X_MAX && !wow.stopStatus; x += sys.SEARCH_FISHING_CURSOR_X_OFFSET)
		{
			if (wow.stopStatus || !monitor.isMonitorRunning)
			{
				//displayLog("check_is_fishing δ���ڵ���״̬\r\n"); 
				return false;
			}
			wow.fish_point.x = x;
			wow.fish_point.y = y;

			//��ǰ������껥����
			if (validateFishingFloatPoint(&(wow.fish_point)))
			{
				return true;
			}
			Sleep(20);
		}
	}
	return false;
}
bool validateFishingFloatPoint(POINT *point)
{
	MouseHook::mouseMoveClientToScreen(point, &(sys.wowWindowRect));
	Sleep(sys.cursorMoveSpeedsliderAccuracy);
	return  ImgEx::checkFishingFloatPoint();
}


DWORD WINAPI fishTimer(LPVOID lpParamter)
{
	monitorStartCountDown(false);
	while (!wow.timer_destroy &&  monitor.isMonitorRunning)
	{
		if (!wow.fishTimerPauseStatus)
		{
			LOG(INFO) << "fishTimer ѭ��";
			//checkWowWindowFocus(); 
			if (!monitor.refreshWowWindowInfo())
			{
				monitor.stopMonitor();
				return 0;
			}
			wow.check_is_fishing = ImgEx::checkScreenFishStatus();
			//double scale1 = (double)(sys.wowWindowRect.right - sys.wowWindowRect.left) / (double)(sys.wowWindowRect.bottom - sys.wowWindowRect.top);
			//double scale2 =((double)sys.maxWindowWidht / (double)sys.maxWindowHeight);
			//Logger::displayLog("����Ϊ%f  %f\r\n", scale1, scale2);
			//check_is_fishing = false;
			if (!wow.check_is_fishing && !wow.fishTimerPauseStatus)
			{
				//ʵ��δ���ڵ���״̬������ʶ��ʧ�ܣ�������껥������ʧ�����Σ������׸�
				if (!(wow.FIND_CUSOR == sys.searching_cursor_pos) && (wow.IS_FISHING && !wow.RECOGNISE_SOUND && (wow.FIND_CUSOR == sys.located_cursor_pos || wow.FIND_CUSOR == sys.wating_for_start_fish) || wow.FIND_CUSOR_FAILED_NUM >= 2) || wow.FIND_CUSOR == sys.located_cursor_pos)
				{
					//Logger::displayLog("fishTimer���δ���ڵ���״̬�������׸�  \r\n");
					LOG(INFO) << "fishTimer���δ���ڵ���״̬�������׸�";
					wow.startFish(false);
				}
			}
		}
		
		 
		Sleep(1500);

	}
	return 1;
}

void monitorStartCountDown(bool display)
{
	int i = 0;
	while (i < sys.START_MONITOR_DELAY && !wow.find_cursor_thread_destroy)
	{
		if (display)
		{
			Logger::displayLog("%d�������\r\n", sys.START_MONITOR_DELAY - i);
		} 
		Sleep(1000);
		i++;
	}
	if (display&& !wow.find_cursor_thread_destroy)
	{
		Logger::displayLog("�����ѿ���\r\n", sys.START_MONITOR_DELAY - i);
	} 
}


DWORD WINAPI licExpireCheck(LPVOID lpParamter)
{

	while (1)
	{
		 
		if (!sys.isActive)
		{
			MessageBox(NULL, L"��δ����", L"��������", 0); 
			LOG(ERROR) << "�����δ����";
			return 0;
		}

		if (Security::isLicExpiredThreadCheck())
		{
			DlgHelper::disaleAutoFish();
			monitor.stopMonitor();
			MessageBox(NULL, L"licence�ѹ���", L"��������", 0);
			Logger::displayLog("licence�ѹ���\r\n");
			LOG(ERROR) << "licence�ѹ���";
			return 0;
		} 

		Sleep(30000);
	}
	


}
/*

DWORD WINAPI keyPressTimer1(LPVOID lpParamter)
{ 
	    monitorStartCountDown(false);
		while (1)
		{
			if (monitor.keyPressTimer1PauseStatus)
			{
				return 1;
			}
			Logger::displayLog("keyPressTimer1ѭ��\r\n");
			if (sys.timerThread1_Enable)
			{
				
				monitor.pauseMonitor();
				//����
				KBHook::pressKeyboard(sys.timerThread1_keyName.c_str()[0]); 
				//ͣ�� 
				Sleep(sys.timerThread1_pauseTime * 1000);
				if (monitor.keyPressTimer1PauseStatus)
				{
					return 1;
				}
				monitor.continueMonitor();
				Sleep(sys.timerThread1_intervalSeconds * 1000);
				 
			}else
			{
				Sleep(3000);
			}
		}  
		return 1;
}

DWORD WINAPI keyPressTimer2(LPVOID lpParamter)
{ 
	monitorStartCountDown(false);
	while (1)
	{ 
		if (monitor.keyPressTimer2PauseStatus)
		{
			return 1;
		}
		Logger::displayLog("keyPressTimer2ѭ��\r\n");
		if (sys.timerThread2_Enable )
		{
			
			monitor.pauseMonitor();
			//����
			KBHook::pressKeyboard(sys.timerThread2_keyName.c_str()[0]);
			//ͣ�� 
			Sleep(sys.timerThread2_pauseTime * 1000);
			if (monitor.keyPressTimer2PauseStatus)
			{
				return 1;
			}
			monitor.continueMonitor();
			Sleep(sys.timerThread2_intervalSeconds * 1000);
			 
		}
		else
		{
			Sleep(3000);
		}
	}
	return 1;
}
*/

void pushDataBeforeKeyPressTimerStart()
{
	LOG(INFO) << "pushDataBeforeKeyPressTimerStart";
	if (sys.timerThread1_Enable)
	{ 
		mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY, sys.timerThread1_keyName, sys.timerThread1_pauseTime));
	}
	if (sys.timerThread2_Enable)
	{ 
		mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY, sys.timerThread2_keyName, sys.timerThread2_pauseTime));
	}

}
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{
	switch (nTimerid)
	{
	case sys.timer1Id:
		if (sys.timerThread1_Enable)
		{
			//Logger::displayLog("timer1\r\n");
			mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY,sys.timerThread1_keyName,sys.timerThread1_pauseTime));
		} 
		break;
	case sys.timer2Id:
		if (sys.timerThread2_Enable)
		{
			//Logger::displayLog("timer2\r\n");
			mq.push(MqMsg(operationEnum::PAUSE_AND_PRESS_KEY, sys.timerThread2_keyName, sys.timerThread2_pauseTime));
		} 
		break;
	case  sys.initMqTimerId:
	      { 
		       pushDataBeforeKeyPressTimerStart();
			   KillTimer(hWnd, sys.initMqTimerId);
			   LOG(INFO) << "pushDataBeforeKeyPressTimerStart and KillTimer";
	      }
		break;
	case sys.fishIntervalCheckTimerId:
		checkFishIntervalTime();
		break;
	default:
		break;
	} 
}
void CALLBACK MqTimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime)
{

	switch (nTimerid)
	{
	case sys.mqTimerId:
		if (!mq.isEmpty())
		{
			monitor.pauseMonitor();
			while (!mq.isEmpty())
			{
				wow.lastFinishFishTime = time(NULL);
				processMqMsg(mq.pop());
			}
			monitor.continueMonitor();
		} 
		break;
	default:
		break;
	}

}

void processMqMsg(MqMsg msg)
{
	switch (msg.operation)
	{
	case operationEnum::PAUSE_AND_PRESS_KEY:
	    { 
		  if (!msg.msg.empty() && msg.data>=0 )
		  {
			//����
			KBHook::pressKeyboard(msg.msg.c_str()[0]);
			//ͣ�� 
			Sleep(msg.data * 1000);
		  } 
	    }
		break;
	default:
		break;
	}

}

void checkFishIntervalTime()
{

	time_t now = time(NULL);
	if (now - wow.lastFinishFishTime >= (23+sys.FINISH_FISH_DELAY/1000)  )
	{
		wow.startFish(true);
		wow.lastFinishFishTime = time(NULL);
	}
	return;

}