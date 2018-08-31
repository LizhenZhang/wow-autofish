/** @file
********************************************************************************
<PRE>
ģ����: ��˹���ģ�͹���
�ļ���: gmm.h
����ļ�: gmm.c
�ļ�ʵ�ֹ���: ��˹���ģ�͹�����غ���������������
����: Dake
�汾: V2010.09.01
��̷�ʽ: ANSI C���Ա��
��Ȩ��ʽ: Copyright(C) Dake
��ϵ��ʽ: chen423@yeah.net
��������: 2010-07-05
--------------------------------------------------------------------------------
���̰߳�ȫ��: <��/��>[��˵��]
�쳣ʱ��ȫ��: <��/��>[��˵��]
--------------------------------------------------------------------------------
��ע: <����˵��>
--------------------------------------------------------------------------------
�޸ļ�¼:
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y      <���߻��޸�����>    <�޸�����>
</PRE>
*******************************************************************************/

#ifndef _GMM_H_
#define _GMM_H_

#include "mfcc.h"

//#define DEBUG_GMM

#ifndef DEBUG_GMM
#define M 6  //!< GMM����(�����)8  3
#else
#define M 3
#endif // DEBUG_GMM

/**
* @brief ��˹���ģ�ͽṹ
*/
typedef struct GMM
{//�ṹ���ڸĳɶ�̬�ڴ�
	void(*f_Init)(struct GMM *, int);
	int m;             //!< ��˹�����
	double * p ;        //!< M���Ȩϵ��
	double ** u ;    	  //!< M��Dά��ֵ
	double ** cMatrix ; //!< M��Э�������(Dά�Խ���)
	void * (*f_CallocGMM)(struct GMM *);
	void(*f_FreeGMM)(struct GMM *);
} GMM;


BOOL GMMs(double ** X, GMM * pGMM, int train_frame_num, int m); // GMM��ģ����
BOOL GMM_identify(double ** X, double * value, GMM * pGMM, int frame_num, int m); // ʶ�����
void InitGMMClass(GMM * pGMM, int m);
void * CallocGMM(GMM * pGMM); // ����GMM�ڴ�
void FreeGMM(GMM * pGMM); // �ͷ�GMM�ڴ�

#endif /* _GMM_H_ */
