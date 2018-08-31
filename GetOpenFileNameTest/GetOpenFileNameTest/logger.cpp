#include "stdafx.h"
#include "logger.h"
#include "stdio.h"
#include "global.h"
#include "CharacterHelper.h"
#include "timeUtil.h"
#include <atlstr.h>
#include "resource.h"
#include "fileUtil.h" 

extern SysConfig sys;

void selectStr(WCHAR *src, WCHAR *des, int start, int end);


Logger::Logger()
{
}

Logger::~Logger()
{
}


int Logger::displayLog(char* format, ...)
{
	int i = 0; 
	char str_tmp[1024];
	WCHAR text[1024*15] = {0};  
	 
	va_list vArgList; // ����һ��va_list�͵ı���, ���������ָ�������ָ��. 
	va_start(vArgList, format);                 //��va_start���ʼ������,�����ĵڶ��������ǵ�һ���ɱ������ǰһ������,��һ���̶��Ĳ���
	i = _vsnprintf_s(str_tmp, 1024, format, vArgList); //ע��,��Ҫ©��ǰ���_
	va_end(vArgList);                            //��va_end������ɱ�����Ļ�ȡ 
	 
	HWND textOut = GetDlgItem(hwndMainDlg, IDC_TEXTOUT); //��ȡ���������
	   
	GetWindowText(textOut, text, 1024 * 15);//��ȡ��ǰ�����������
	if (wcslen(text) >= 1024 * 14)//������ݳ�������������ݣ�
	{  
		SetDlgItemText(hwndMainDlg, IDC_TEXTOUT, L""); 
	}

	//��ȡ��ǰʱ��
	string now = getFormatedDate();
	//��ʱ�������ƴ��
	now.append(" ").append(str_tmp);

	SendMessage(textOut, EM_SETSEL, (WPARAM)-1, (LPARAM)-1); 
	str_utf16 * msg=CAPECharacterHelper::GetUTF16FromANSI(now.c_str());
	SendMessage(textOut, EM_REPLACESEL, FALSE, (LPARAM)msg);
	SendMessage(textOut, EM_SCROLLCARET, 0, 0); 
	delete[] msg; 
	DeleteObject(textOut);
	
	return i;                                    //���ز������ַ������м��ж��ż��
}

void selectStr(WCHAR *src, WCHAR *des, int start, int end)
{
	int index = 0;
	for (int i = start-1; i < end; i++)
	{
		des[index] = src[i];
		index++;
	}
}

void Logger::initGlog()
{ 
	google::InitGoogleLogging(sys.glogHead);
	google::SetLogDestination(google::GLOG_INFO, sys.glogDestination.c_str()); 
	//������־�ļ���󣬵�λM
	FLAGS_max_log_size = sys.glogMaxSize;
	FLAGS_minloglevel = sys.glogLevel;
}

bool Logger::setGlogLevel(int level)
{
	sys.glogLevel = level;
	FLAGS_minloglevel = level; 
	return true;
}

string Logger::getFormatedDate()
{ 
	return timeUtil::localTimeToString("%m-%d %H:%M:%S");
}