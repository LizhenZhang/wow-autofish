#ifndef __WOW__
#define __WOW__
#include <Windows.h> 
#include "sysconfig.h"
extern SysConfig sys;

class Wow
{
public:
	Wow();
	~Wow();
	  void startFish(bool displayLog);
	  void finishFish(POINT * fishFloatPoint, RECT *widowLocation); 
	//fish status  
	  bool find_cursor_thread_destroy = false;
	
	  bool start_find_cursor = false;
	  bool timer_destroy = false;//��ʱ���״̬�߳����ٱ�ʶ
	  bool  IS_FISHING = false; //�Ƿ����ڵ���
	  POINT fish_point = { 0,0 };//������껥��������  

	  int FIND_CUSOR = sys.not_started_searching_cursor_pos;//�Ƿ��Ѿ��ҵ�������껥���� 0δ�ҵ���1�ҵ���-1������
	  bool check_is_fishing = false;
	  bool RECOGNISE_SOUND = false;//�Ƿ��Ѿ�ʶ��������������
	  bool fish_status_matching = false;
	  bool start_rec_sys_sound = false;//�Ѿ���������ʶ���߳�
	  int FIND_CUSOR_FAILED_NUM = 0;
	   //״̬����
	  bool stopStatus = false;//�����
	  bool fishTimerPauseStatus = false;
	  bool pauseSoundRecognize = false;

	  time_t lastFinishFishTime=0;
private:

};

#endif