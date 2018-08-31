#include "stdafx.h"
#include "timeUtil.h"
#include <stdio.h>    
#include <Windows.h>
#include <time.h>
#include "CInitSock.h"
#include "logger.h"
 

timeUtil::timeUtil()
{
}

timeUtil::~timeUtil()
{
}

long timeUtil::getCurrentTimeMilliseconds()
{
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	return lpsystime.wSecond * 1000 + lpsystime.wMilliseconds;
	//printf("%u:%u:%u:%u:%u:%u:%u:%u\n", lpsystime.wYear, lpsystime.wMonth, lpsystime.wDayOfWeek, lpsystime.wDay, lpsystime.wHour, lpsystime.wMinute, lpsystime.wSecond, lpsystime.wMilliseconds);
}

char * timeUtil::getCurrentTimeStr()
{
	char str_tmp[100];
	SYSTEMTIME lpsystime;
	GetLocalTime(&lpsystime);
	sprintf_s(str_tmp, "%d-%d-%d %d:%d:%d", lpsystime.wYear, lpsystime.wMonth, lpsystime.wDay, lpsystime.wHour, lpsystime.wMinute, lpsystime.wSecond);
	return str_tmp;
}

 

//2.ָ��YYYY-MM-DD�͵�ʱ�䣬��ʽ��Ϊtime_t�͵�ʱ��
time_t timeUtil::stringToTime_t(const std::string & time_string)
{
	struct tm tm1;
	ZeroMemory(&tm1, sizeof(tm1));
	time_t time1;
	int i = sscanf_s(time_string.c_str(), "%d/%d/%d %d:%d:%d",
		&(tm1.tm_year),
		&(tm1.tm_mon),
		&(tm1.tm_mday),
		&(tm1.tm_hour),
		&(tm1.tm_min),
		&(tm1.tm_sec),
		&(tm1.tm_wday),
		&(tm1.tm_yday));

	tm1.tm_year -= 1900;
	tm1.tm_mon--;
	tm1.tm_isdst = -1;
	time1 = mktime(&tm1);

	return time1;
}

bool timeUtil::compareTime(time_t t1, time_t t2)
{
	return t1 > t2;
}
string timeUtil::formatedLocalTimeToString()
{
	return localTimeToString("%Y-%m-%d %H:%M:%S");
}

string timeUtil::formatedGmtTimeToString()
{
	return gmtTimeToString("%Y-%m-%d %H:%M:%S");
}

string timeUtil::localTimeToString(char* format)
{
	time_t t;  //��ʱ��  
	tm local; //����ʱ��    
	char buf[128] = { 0 };
	ZeroMemory(buf,sizeof(buf));

	t = time(NULL); //��ȡĿǰ��ʱ��  
	errno_t error=localtime_s(&local,&t); //תΪ����ʱ��   
	//strftime(buf, 64, "%Y-%m-%d %H:%M:%S", local);  
	strftime(buf, 64, format,&local); ;  
	return buf;
}

string timeUtil::gmtTimeToString(char* format)
{ 
	time_t t;  //��ʱ��   
	tm gmt;   //��������ʱ��  
	char buf[128] = { 0 };
	ZeroMemory(buf, sizeof(buf));

	t = time(NULL); //��ȡĿǰ��ʱ��   
	errno_t error = gmtime_s(&gmt,&t);//תΪ��������ʱ��   
	//strftime(buf, 64, "%Y-%m-%d %H:%M:%S", gmt); 
	strftime(buf, 64, format, &gmt); 
	return buf;
}

time_t timeUtil::getCurrentTimeFromIP()
{
	ULONG ntpTime= CInitSock::getNtpTimeFromNetwork();  
	if (ntpTime == 0)
	{
		return -1;
	}
	SYSTEMTIME sysTime = getTimeFromULONG(ntpTime);
	time_t tt=SystemTimeToTimet(sysTime);   
	return tt;
}


SYSTEMTIME timeUtil::getTimeFromULONG(ULONG ulTime)	// ��ȡʱ��Э�鷵�ص�ʱ��
{
	// Windows�ļ�ʱ����һ��64λ��ֵ�����Ǵ�1601��1��1������12:00�����ڵ�ʱ������
	// ��λ��1/1000 0000�룬��1000���֮1�루100-nanosecond ��
	FILETIME ft;
	SYSTEMTIME st;

	// ���Ƚ���׼ʱ�䣨1900��1��1��0��0��0��0���룩ת��ΪWindows�ļ�ʱ��	
	st.wYear = 1900;
	st.wMonth = 1;
	st.wDay = 1;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &ft);

	// Ȼ��Time Protocolʹ�õĻ�׼ʱ������Լ���ȥ��ʱ�䣬��ulTime
	LONGLONG *pLLong = (LONGLONG *)&ft;
	// ע�⣬�ļ�ʱ�䵥λ��1/1000 0000�룬��1000���֮1�루100-nanosecond ��
	*pLLong += (LONGLONG)10000000 * ulTime;

	// �ٽ�ʱ��ת������������ϵͳʱ��
	FileTimeToSystemTime(&ft, &st); 

	return st;
}
/*
**SYSTEMTIMEתtime_t
*/
time_t timeUtil::SystemTimeToTimet(SYSTEMTIME st)
{
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	LONGLONG nLL;
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	nLL = (ft.dwHighDateTime << 32) + ft.dwLowDateTime;
	time_t pt = (long)((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
	return pt;
}

