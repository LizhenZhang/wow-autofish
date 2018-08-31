#include "stdafx.h" 
#include "sysUtil.h"    

SysUtil::SysUtil()
{
}

SysUtil::~SysUtil()
{
}
BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam)//ö�����н���
{
	DWORD dwProcId;
	GetWindowThreadProcessId(hWnd, &dwProcId);
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	if (dwProcId == pInfo->dwProcessId)
	{
		// �ڴ���Ӹ�����������
		CString strTmp;

		GetClassName(hWnd, strTmp.GetBuffer(200), 200);
		strTmp.ReleaseBuffer();

		if (!strTmp.Compare(L"LButton"))	// ��ť, �༭��TEdit��, �����ʹ��Spy++�鿴
		{
			pInfo->hWnd = hWnd;
			return FALSE;
		}

		if (IsWindowVisible(hWnd)) // ��ǰ�����Ƿ�ɼ�
		{
			pInfo->hWnd = hWnd;		// ��ȡ����һ�����ھ��

			return FALSE;
		}
	}
	return TRUE;
}

bool SysUtil::GetProcessHwnd(DWORD proccessId, WNDINFO *wi)
{

	EnumWindows(MyEnumProc, (LPARAM)wi);
	// EnumChildWindows(hWnPar, MyEnumProc, (LPARAM)&wi); // ö�ٴ��ڵ��Ӵ��ھ��, MFC�еĿ�����
	return true;
}

bool SysUtil::GetProcessInfo(CString processName, WNDINFO *wi)
{
	wi->dwProcessId = 0;
	wi->hWnd = NULL;
	//�������̿���(TH32CS_SNAPPROCESS��ʾ�������н��̵Ŀ���)
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//PROCESSENTRY32���̿��յĽṹ��       
	PROCESSENTRY32 pe;
	//ʵ������ʹ��Process32First��ȡ��һ�����յĽ���ǰ�����ĳ�ʼ������
	pe.dwSize = sizeof(PROCESSENTRY32);
	//�����IFЧ��ͬ:       
	//if(hProcessSnap == INVALID_HANDLE_VALUE)   ��Ч�ľ��  
	if (!Process32First(hSnapShot, &pe))
	{
		::CloseHandle(hSnapShot);
		DeleteObject(hSnapShot);
		return false;
	}
	processName.MakeLower();
	BOOL clearprocess = FALSE;
	//��������Ч  ��һֱ��ȡ��һ�����ѭ����ȥ 
	while (Process32Next(hSnapShot, &pe))
	{
		//pe.szExeFile��ȡ��ǰ���̵Ŀ�ִ���ļ����� 
		CString scTmp = pe.szExeFile;
		scTmp.MakeLower();
		char modPath[MAX_PATH] = { 0 };
		if (!scTmp.Compare(processName))
		{
			//�ӿ��ս����л�ȡ�ý��̵�PID(������������е�PID)
			DWORD dwProcessID = pe.th32ProcessID;

			//��ȡ���̵ľ��
			//HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			//��ȡ��һ�����Ӵ��ڵľ��
			wi->dwProcessId = dwProcessID;
			DeleteObject(wi->hWnd);
			GetProcessHwnd(dwProcessID, wi);

			//RECT rect = { 0 };
			//GetWindowRect(hWnd, &rect);
			//drawFishHdc(hWnd); 

			break;
			//��ȡ�ļ�·��
			//MODULEENTRY32 me = { sizeof(me) };
			//HANDLE hpro;
			//hpro = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);
			//Module32First(hpro, &me);
			//TRACE(processName + " Path: " + me.szExePath);
			//::CloseHandle(hpro);

		}
	}
	::CloseHandle(hSnapShot);
	DeleteObject(hSnapShot);
	return true;
}