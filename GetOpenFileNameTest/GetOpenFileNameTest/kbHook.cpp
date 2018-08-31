#include "stdafx.h"
#include "kbHook.h" 
#include "logger.h"
#include "timeUtil.h"

//keypress time record
LONG VK_F9_AND_CTRL_LAST_PRESS_TIME = 0;
LONG VK_F10_AND_CTRL_LAST_PRESS_TIME = 0; 

KBHook::KBHook()
{
}

KBHook::~KBHook()
{
}
void KBHook::pressKeyboard(char key)
{
	keybd_event(key, 0, 0, 0);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);

}

void KBHook::pressKeyboardDown(char key)
{
	keybd_event(key, 0, 0, 0); 
}

void KBHook::keyboardUp(char key)
{
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

LRESULT KBHook::kbHookFun(int nCode, WPARAM wParam, LPARAM lParam, HHOOK g_HookHwnd, Monitor *monitor)
{
	// ���Structure�����˼��̵���Ϣ
	/*typedef struct {
	DWORD vkCode;
	DWORD scanCode;
	DWORD flags;
	DWORD time;
	ULONG_PTR dwExtraInfo;
	} KBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;*/
	// ����ֻ��Ҫ�Ǹ�vkCode
	PKBDLLHOOKSTRUCT pVirKey = (PKBDLLHOOKSTRUCT)lParam;
	int ctrlState = 0;
	// MSDN˵��,nCode < 0��ʱ�����
	if (nCode >= 0)
	{
		// ������Ϣ
		switch (wParam)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			switch (pVirKey->vkCode)
			{
			case VK_LWIN:
			case VK_RWIN:
				return 1;  // �Ե���Ϣ
				break;
			case VK_F9:
				//F9��Ctrlͬʱ���£����������ء��������

				if (ctrlState = GetKeyState(VK_LCONTROL) < 0)
				{
					//if (VK_F9_AND_CTRL_LAST_PRESS_TIME == 0 || (getCurrentTimeMilliseconds() - VK_F9_AND_CTRL_LAST_PRESS_TIME > 50))
					if(!monitor->isMonitorRunning)
					{  
						monitor->stopMonitor();
						monitor->startMonitor(); 
					}

				}
				break;
			case VK_F10:
				//F10��Ctrlͬʱ���£��رյ����ء��������
				if (ctrlState = GetKeyState(VK_LCONTROL) < 0)
				{
					//if (VK_F10_AND_CTRL_LAST_PRESS_TIME == 0 || (getCurrentTimeMilliseconds() - VK_F10_AND_CTRL_LAST_PRESS_TIME > 100))
					if(monitor->isMonitorRunning)
					{
						monitor->isMonitorRunning = false;
						Logger::displayLog("�����ѹر�\r\n");
						monitor->stopMonitor();
						//VK_F10_AND_CTRL_LAST_PRESS_TIME = getCurrentTimeMilliseconds();
					} 
				}

				break;
			}
			break;
		}
	}

	return CallNextHookEx(g_HookHwnd, nCode, wParam, lParam);


}