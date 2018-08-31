#include "stdafx.h"
#include "wow.h" 
#include "global.h" 
#include "mouseHook.h" 
#include "resource.h"
#include "logger.h" 
Wow::Wow()
{  

}

Wow::~Wow()
{

}
void Wow::startFish(bool displayLog)
{
	LOG(INFO) << "�׸ͣ����ڰ�������";
	check_is_fishing = true;
	IS_FISHING = true;
	FIND_CUSOR = sys.not_started_searching_cursor_pos;
	RECOGNISE_SOUND = false;
	if (displayLog)
	{
		Logger::displayLog("�׸ͣ����ڰ�������\r\n");
	} 
	//�׸�
	PostMessage(hwndMainDlg, WM_START_FISH, 0, 0);
	//��ʼ��껥����� 
	stopStatus = false;
	lastFinishFishTime = time(NULL);
	 
}
void Wow::finishFish(POINT * fishFloatPoint,RECT *widowLocation)
{
	LOG(INFO) << "�ոͣ��ɹ�����һ����";
	Logger::displayLog("�ոͣ��ɹ�����һ����\r\n"); 
	IS_FISHING = false;
	RECOGNISE_SOUND = true;
	/*
	if (hThread != NULL)
	{
	CloseHandle(hThread);
	hThread = NULL;
	stopStatus = true;
	}
	*/
	if (FIND_CUSOR == sys.located_cursor_pos)
	{
		//point->x = fish_point.x;
		//point->y = fish_point.y;
		//MessageBox(NULL, TEXT("���ҵ�"), TEXT("��ӭ"), 0);
		//ClientToScreen(wowWindowInfo.hWnd, point);
		//MouseMove(point->x, point->y);
		MouseHook::mouseMoveClientToScreen(&fish_point, widowLocation);
		
	}
	//������ȡ�� shift+�������������Զ�ʰȡ
	//MouseHook::shiftAndMouseLClick();
	if (sys.isLeftMouseBtnPickup)
	{
		MouseHook::mouseLClick();
	}
	else
	{
		MouseHook::mouseRClick();
	}
	lastFinishFishTime = time(NULL);
	FIND_CUSOR = sys.not_started_searching_cursor_pos;
	//�ر���껥�����
	stopStatus = true;
	Sleep(sys.FINISH_FISH_DELAY);
}
