/** 
********************************************************************************
<PRE>
ģ����: ��˹���ģ�͹���
�ļ���: gmm.c
����ļ�: gmm.h
�ļ�ʵ�ֹ���: ��˹���ģ�͹�����غ�����ʵ��
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
��ע: ��˹���ģ�ͣ�GMM����p[i], u[i][D], cMatrix[i][D]����i=0,...,M-1
--------------------------------------------------------------------------------
�޸ļ�¼:
�� ��        �汾     �޸���              �޸�����
YYYY/MM/DD   X.Y      <���߻��޸�����>    <�޸�����>
</PRE>
*******************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "gmm.h"
#include "cluster.h"


#define DIFF_GMM_VALUE     1              //!< GMM��һ��ֵ
#define IDENTIFY_VALUE    (-30000)        //!< ʶ����ֵ
//#define IDENTITY_FRAME_NUM 100           //!< ʶ��֡��

static double getDistance(double * p1, double * p2);
static BOOL InitGMM(double ** X, GMM * pGMM, int train_frame_num, int m); // ��ʼ��GMM����
static BOOL BuildGMM(GMM * pGMM, GMM * Out, double ** X, int train_frame_num, int m);
static double GMM_diff(GMM * gmm1, GMM * gmm2);
static double GMM_density(GMM * pGMM, double * X, int index); // ��˹�ܶȺ���


 /** @function
********************************************************************************
<PRE>
������: InitGMMClass()
����: ��ʼ��GMM�ṹ
�÷�: ��GMM�ṹ��f_init����
����:
[OUT] pGMM: GMM�ṹָ��
[IN] m: GMM�ĸ��ڻ����
����:
����:
</PRE>
*******************************************************************************/
void InitGMMClass(GMM * pGMM, int m)
{
	pGMM->m = m;
	pGMM->p = NULL;
	pGMM->u = NULL;
	pGMM->cMatrix = NULL;
	pGMM->f_CallocGMM = CallocGMM;
	pGMM->f_FreeGMM = FreeGMM;
}


/** @function
********************************************************************************
<PRE>
������: CallocGMM()
����: ΪGMM�ṹ�ڵ�ָ������ڴ�
�÷�: ��GMM�ṹ��f_CallocGMM����
����:
[OUT] pGMM: GMM�ṹָ��
����:
����:
</PRE>
*******************************************************************************/
void * CallocGMM(GMM * pGMM)
{
	int i;
	int m = pGMM->m;

	pGMM->p = (double *)calloc(m, sizeof(double));
	if (!pGMM->p)
	{
		return NULL;
	}
	pGMM->u = (double **)malloc(m * sizeof(double *));
	if (!pGMM->u)
	{
		free(pGMM->p);
		return NULL;
	}
	pGMM->cMatrix = (double **)malloc(m * sizeof(double *));
	if (!pGMM->cMatrix)
	{
		free(pGMM->p);
		free(pGMM->u);
		return NULL;
	}
	for (i = 0; i < m; ++i)
	{
		pGMM->u[i] = (double *)calloc(D, sizeof(double));
		if (!pGMM->u[i])
		{
			for (--i; i >= 0; --i)
			{
				free(pGMM->u[i]);
				free(pGMM->cMatrix[i]);
			}
			free(pGMM->p);
			free(pGMM->u);
			free(pGMM->cMatrix);
			return NULL;
		}
		pGMM->cMatrix[i] = (double *)calloc(D, sizeof(double));
		if (!pGMM->cMatrix[i])
		{
			free(pGMM->u[i]);
			for (--i; i >= 0; --i)
			{
				free(pGMM->u[i]);
				free(pGMM->cMatrix[i]);
			}
			free(pGMM->p);
			free(pGMM->u);
			free(pGMM->cMatrix);
			return NULL;
		}
	} // for (i = 0; i < m; ++i)
	return pGMM;
}


/** @function
********************************************************************************
<PRE>
������: FreeGMM()
����: �ͷ�ΪGMM�ṹ�ڵ�ָ������ڴ�
�÷�: ��GMM�ṹ��f_FreeGMM����
����:
[IN] pGMM: GMM�ṹָ��
����:
����:
</PRE>
*******************************************************************************/
void FreeGMM(GMM * pGMM)
{
	int i;
	int m = pGMM->m;

	if (!pGMM)
	{
		return;
	}
	if (pGMM->p)
	{
		free(pGMM->p);
	}
	if (!pGMM->u && !pGMM->cMatrix)
	{
		return;
	}
	for (i = 0; i < m; ++i)
	{
		if (pGMM->u && pGMM->u[i])
		{
			free(pGMM->u[i]);
		}
		if (pGMM->cMatrix && pGMM->cMatrix[i])
		{
			free(pGMM->cMatrix[i]);
		}
	}
	if (pGMM->u)
	{
		free(pGMM->u);
	}
	if (pGMM->cMatrix)
	{
		free(pGMM->cMatrix);
	}
}





/** @function
********************************************************************************
<PRE>
������: InitGMM()
����: ���GMM��ֵ
�÷�:
����:
[IN] X: ����������
[OUT] pGMM: GMM��ֵ
[IN] train_frame_num: ѵ��֡֡��
[IN] m: ��˹�����
����:
TRUE: �ɹ�
FALSE��ʧ��
����: k_means()
��������: GMMs()
</PRE>
*******************************************************************************/
static BOOL InitGMM(double ** X, GMM * pGMM, int train_frame_num, int m)
{
	int i;
	int j;
	int * clusterIndex = NULL;  // ����������������
	int * clusterSize = NULL;   // ��������������
	double ** sum = NULL;
	GMM gmm = *pGMM;


	clusterIndex = (int *)calloc(train_frame_num, sizeof(int));
	clusterSize = (int *)calloc(m, sizeof(int));
	sum = (double **)malloc(m * sizeof(double *));
	for (i = 0; i < m; ++i)
	{
		sum[i] = (double *)calloc(D, sizeof(double));
	}
	// ��ʼ����ֵu
	k_means(X, pGMM, clusterIndex, clusterSize, train_frame_num, m);

	// ��ʼ����Ȩϵ��p
	for (i = 0; i < m; ++i)
	{
		gmm.p[i] = 1.0 / m;
	}

	// ��ʼ��Э�������cMatrix
	for (i = 0; i < train_frame_num; ++i)
	{
		for (j = 0; j < D; ++j)
		{
			sum[clusterIndex[i]][j] += pow(X[i][j], 2);
		}
	}
	for (i = 0; i < m; ++i)
	{
		if (clusterSize[i] > 0) // ��ֹ��ĸΪ0
		{
			for (j = 0; j < D; ++j)
			{
				// TODO: �˴��и�ֵ��0ֵ
				gmm.cMatrix[i][j] = sum[i][j] / clusterSize[i] - pow(gmm.u[i][j], 2);
				if (gmm.cMatrix[i][j] < 0)
				{
					//xprint("Initial value of GMM`cMatrix < 0");
					gmm.f_FreeGMM(&gmm);
					free(clusterIndex);
					free(clusterSize);
					for (i = 0; i < m; ++i)
					{
						free(sum[i]);
					}
					free(sum);
					return FALSE;
				}
				else if (gmm.cMatrix[i][j] < 1e-10) // ������ͬdoubleֵ��������
				{
					gmm.cMatrix[i][j] = 0;
				}
			}
		} // end if (clusterSize[i] > 0)
		else
		{
			//xprint(TEXT("InitGMM: Decrease M and increase frame number"));
			//xprint(TEXT("clusterSize[%d]: %d\n"), i, clusterSize[i]);
			gmm.f_FreeGMM(&gmm);
			free(clusterIndex);
			free(clusterSize);
			for (i = 0; i < m; ++i)
			{
				free(sum[i]);
			}
			free(sum);
			return FALSE;
		}
	} // end for (i = 0; i < m; ++i)


	free(clusterIndex);
	free(clusterSize);
	for (i = 0; i < m; ++i)
	{
		free(sum[i]);
	}
	free(sum);
	return TRUE;
}


/** @function
********************************************************************************
<PRE>
������: GMM_diff()
����: �������GMM�Ĳ�ֵ
�÷�:
����:
[IN] gmm1: ��һ�ֵ����Ľ��
[IN] gmm2: ��һ�ֵ����Ľ��
����: ���ε���GMM�Ĳ�ֵ������Ӧ����ƽ���ͣ�
����:
��������: GMMs()
</PRE>
*******************************************************************************/
static double GMM_diff(GMM * gmm1, GMM * gmm2)
{
	int i;
	int j;
	double diff = 0;


	for (i = 0; i < M; ++i)
	{
		diff += pow(gmm1->p[i] - gmm2->p[i], 2);
		for (j = 0; j < D; ++j)
		{
			diff += pow(gmm1->u[i][j] - gmm2->u[i][j], 2);
			diff += pow(gmm1->cMatrix[i][j] - gmm2->cMatrix[i][j], 2);
		}
	}

	return diff;
}


/** @function
********************************************************************************
<PRE>
������: GMM_density()
����: �����˹�����ܶ�
�÷�:
����:
[IN] pGMM: ��һ�ֵ����Ľ��
[IN] X: ����������
[IN] index: ��˹���ģ�ͽ�������
����: ��˹�����ܶ�ֵ
����:
��������: BuildGMM()
</PRE>
*******************************************************************************/
static double GMM_density(GMM * pGMM, double * X, int index) //��˹�ܶȺ���
{
	int i;
	double temp = 0;
	double sqrt_Matrix_value = 1;
	double res = 0;


	for (i = 0; i < D; ++i)
	{
		//TODO �ԽǾ���ĶԽ����г���0�����������1#INF
		double x = X[i];
		temp += pow(X[i] - pGMM->u[index][i], 2) / pGMM->cMatrix[index][i];
		//TODO sqrt����-nan,��Ϊ����ֵΪ��
		sqrt_Matrix_value *= sqrt(pGMM->cMatrix[index][i]);
	}
	//TODO tempֵ̫�󣬵���exp(temp / -2)Ϊ0
	res = pow(_2PI, D / -2) / sqrt_Matrix_value * exp(temp / -2);
	return res;
}


/** @function
********************************************************************************
<PRE>
������: BuildGMM()
����: ������һ��GMM
�÷�:
����:
[IN] pGMM: ��һ�ε����Ľ��
[OUT] Out: �����õ���GMM
[IN] X: ����������
[IN] train_frame_num: ѵ��֡֡��
[IN] m: ��˹�����
����:
TRUE: �ɹ�
FALSE: ʧ��
����: GMM_density()
��������: GMMs()
</PRE>
*******************************************************************************/
static BOOL BuildGMM(GMM * pGMM, GMM * Out, double ** X, int train_frame_num, int m)
{
	int i;
	int j;
	int k;
	GMM temp_gmm = *Out;
	double ** p = NULL;      // train_frame_num��m����º����
	double * sum_p = NULL;
	double * pb = NULL;      // ����ÿ�������º���ʵ��м���
	double sum_pb = 0;


	pb = (double *)calloc(m, sizeof(double));
	sum_p = (double *)calloc(m, sizeof(double));
	p = (double **)malloc(train_frame_num * sizeof(double *));
	for (i = 0; i < train_frame_num; ++i)
	{
		p[i] = (double *)calloc(m, sizeof(double));
	}
	// �����º����
	for (i = 0; i < train_frame_num; ++i)
	{
		sum_pb = 0;
		for (j = 0; j < m; ++j)
		{
			pb[j] = pGMM->p[j] * GMM_density(pGMM, X[i], j);
			sum_pb += pb[j];
		}
		if (sum_pb == 0 || _isnan(sum_pb))
		{
			//xprint("sum_ph = %lf", sum_pb);
			goto ERROR_RETURN;
		}
		for (j = 0; j < m; ++j)
		{
			p[i][j] = pb[j] / sum_pb;
			sum_p[j] += p[i][j];
		}
	}

	// ������һ�ε�����GMM
	for (j = 0; j < m; ++j)
	{
		// �����Ȩϵ��P
		temp_gmm.p[j] = sum_p[j] / train_frame_num;

		// �����ֵ��Э�������
		for (i = 0; i < train_frame_num; ++i)
		{
			for (k = 0; k < D; ++k)
			{
				if (i == 0)
				{
					temp_gmm.u[j][k] = 0;
					temp_gmm.cMatrix[j][k] = 0;
				}
				temp_gmm.u[j][k] += p[i][j] * X[i][k];
				temp_gmm.cMatrix[j][k] += p[i][j] * pow(X[i][k], 2);
			}
		}
		for (k = 0; k < D; ++k)
		{
			// ��ֵu
			temp_gmm.u[j][k] /= sum_p[j];
			// Э�������cMatrix
			temp_gmm.cMatrix[j][k] /= sum_p[j];
			temp_gmm.cMatrix[j][k] -= pow(temp_gmm.u[j][k], 2);
			if (temp_gmm.cMatrix[j][k] <= 0)
			{
				//xprint("BuildGMM: cMatrix <= 0");
			ERROR_RETURN:
				for (i = 0; i < train_frame_num; ++i)
				{
					free(p[i]);
				}
				free(p);
				free(pb);
				free(sum_p);
				return FALSE;
			}
		} // for (k = 0; k < D; ++k)
	}

	for (i = 0; i < train_frame_num; ++i)
	{
		free(p[i]);
	}
	free(p);
	free(pb);
	free(sum_p);
	return TRUE;
}


/** @function
********************************************************************************
<PRE>
������: GMMs()
����: GMM��ģ����
�÷�:
����:
[IN] X: ����������
[OUT] pGMM: GMMģ��
[IN] train_frame_num: ѵ��֡֡��
[IN] m: ��˹�����
����:
TRUE: �ɹ�
FALSE: ʧ��
����: BuildGMM(), GMM_density()
��������:
</PRE>
*******************************************************************************/
BOOL GMMs(double ** X, GMM * pGMM, int train_frame_num, int m)
{
	GMM first_gmm = { InitGMMClass };
	GMM second_gmm = { InitGMMClass };
	GMM * first = &first_gmm;
	GMM * second = &second_gmm;
	GMM * temp = NULL;
	BOOL bRes;
	double diff = 0;


	first_gmm.f_Init(&first_gmm, m);
	first_gmm.f_CallocGMM(&first_gmm);
	second_gmm.f_Init(&second_gmm, m);
	second_gmm.f_CallocGMM(&second_gmm);
	// TODO ��ֵ�ĶԽ�����0ֵ�͸�ֵ
	if (!InitGMM(X, first, train_frame_num, m)) // ���GMM��ֵʧ��
	{
		return FALSE;
	}

	do // GMM��������
	{
		bRes = BuildGMM(first, second, X, train_frame_num, m);
		if (!bRes)
		{
			first->f_FreeGMM(first);
			second->f_FreeGMM(second);
			return FALSE;
		}
		temp = first;
		first = second;
		second = temp;
		diff = GMM_diff(first, second);

		//xprint("diff = %lf\n", diff);
	} while (diff >= DIFF_GMM_VALUE);

	first->f_FreeGMM(first);
	*pGMM = *second;
	return TRUE;
}


/** @function
********************************************************************************
<PRE>
������: GMM_identify()
����: ���������ʣ�����ʶ�����
�÷�:
����:
[IN] X: ����������
[OUT] value: ʶ��ֵ
[IN] pGMM: GMMģ��
[IN] frame_num: ʶ��֡��
[IN] m: GMMģ�͵ĸ�˹�����
����:
�ɹ�: TRUE
ʧ��: FALSE
����: GMM_density()
��������:
</PRE>
*******************************************************************************/
BOOL GMM_identify(double ** X, double * value, GMM * pGMM, int frame_num, int m)
{
	int j;
	int k;
	double temp = 0;
	double * pb = NULL;
	double sum_pb = 0;


	pb = (double *)calloc(m, sizeof(double));
	for (j = 0; j < frame_num; ++j)
	{
		sum_pb = 0;
		for (k = 0; k < m; ++k)
		{
			pb[k] = pGMM[0].p[k] * GMM_density(pGMM, X[j], k);
			sum_pb += pb[k];
		}
		// TODO sum_pb==0���¸�����-nan��temp_max
		if (sum_pb <= 0)
		{
			//xprint("�������sum_pb <= 0");
			free(pb);
			return FALSE;
		}
		else
		{
			temp += log(sum_pb);
		}
	}
	//xprint("max = %lf", temp);

	*value = temp;
	free(pb);
	return TRUE;
}

#ifdef DEBUG_GMM
int main(void)
{

	GMM gmm;
	double MFCC[9][2] = { { 1, 1 },{ 1, 1.5 },{ 1.5, 1 },
	{ 3, 3 },{ 3.5, 3 },{ 3, 3.5 },
	{ 5, 0 },{ 5, 0.5 },{ 5.5, 0 }
	};
	gmm = GMMs(MFCC);

	return 0;
}
#endif //DEBUG_GMM
