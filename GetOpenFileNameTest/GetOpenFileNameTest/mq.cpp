#include "stdafx.h"
#include "MQ.h"
#include "logger.h"

MQ::MQ()
{ 
	::InitializeCriticalSection(&mqCs);
}

MQ::~MQ()
{
	::DeleteCriticalSection(&mqCs);
}

bool MQ::isEmpty()
{
	return query.size() <= 0;
}

bool MQ::push(MqMsg msg)
{
	try
	{
		//�жϳ���
		if (query.size() >= mqMaxSize)
		{
			return false;
		}
		//����
		lock();
		query.push_back(msg);
		//����
		unLock();
	}
	catch (const std::exception&)
	{
		LOG(ERROR) << "PUSH ERROR";
	}
	 
	return true;
}
MqMsg MQ::pop()
{
	MqMsg msg ;
	//�жϳ��ȣ��Ƿ�Ϊ��
	if (query.size() <= 0)
	{
		return msg;
	}
	//����
	lock();
	msg = query.front();
	query.erase(query.begin());
	//����
	unLock();
	return msg;
}
bool MQ::clear()
{
	//�жϳ��ȣ��Ƿ�Ϊ��
	if (query.size() == 0)
	{
		return true;
	}
	//����
	lock();
	query.clear();
	//����
	unLock();
	return true;
}

bool MQ::lock()
{
	::EnterCriticalSection(&mqCs);
	return true; 
}
bool MQ::unLock()
{
	::LeaveCriticalSection(&mqCs);
	return true;
}

 

 