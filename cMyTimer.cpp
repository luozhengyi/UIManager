#include "stdafx.h"
#include "cMyTimer.h"
#include "process.h"		//���߳���Ҫ��ͷ�ļ�

/*******************************ȫ�ֺ���*********************************/
void CallBack_TimerProc(void* p)	//��ʱ���̵߳Ļص�����
{//��ͣ�����ж�ʱ�����ƺ������ƶ�ʱ������
	cMyTimer* pTimer=(cMyTimer*)p;	//��ȡ��ǰcMyTimer�������ָ��
	if(pTimer==NULL)
	{
		return;
	}
	while(1)		//��ͣ�����ж�ʱ�����ƺ���
	{
		//��⡢ɾ��bDel״̬ΪTRUE�Ķ�ʱ��
		pTimer->CheckDelTimer();
		if(!pTimer->TimerCtrlRun())	//�����ʱ������ʧ�ܾ��˳��߳�
		{
			_endthread();//ɾ���߳�	
			return;
		}
		Sleep(20);		//˯��20ms
	}

}
/************************************************************************/


/****************************cMyTimer��Ա����******************************/
cMyTimer::cMyTimer()
{

}

cMyTimer::~cMyTimer()
{

}

void cMyTimer::AddTimer(unsigned timerID,unsigned timeElapse,int iParam,string strParam)
{//��Ӷ�ʱ����������ʱ���߳�

	if(timeElapse==0)
	{//�����ʱ��ʱ����Ϊ0��ֱ�ӷ���
		return;
	}

	//ͨ���������������һ����ʱ������
	stTimer stTimerTemp;		//��ʱ���ṹ��
	stTimerTemp.timerID=timerID;
	stTimerTemp.timeElapse=timeElapse;
	stTimerTemp.iParam=iParam;
	stTimerTemp.strParam=strParam;
	stTimerTemp.timeLastRun=::GetTickCount();	//��ǰʱ��(��Ե�,���Ǿ��Ե�Ŷ)

	//������Ķ�ʱ�����뵽��ʱ���б���
	m_listTimer.push_back(stTimerTemp);
	
	//����ǵ�һ����ʱ���ʹ���һ���µ��̣߳������Ķ�����һ����һ���̣߳������½��̡߳�
	if(m_listTimer.size()==1)
	{
		//��һ���������̵߳Ļص��������������������ǻص������Ĳ���
		_beginthread(CallBack_TimerProc,0,this); //������ʱ���߳����������ж�ʱ��
	}
}

bool cMyTimer::TimerCtrlRun()
{//�жϡ����ƶ�ʱ��������
	if(m_listTimer.size()==0)
	{//��ʱ���б�Ϊ�գ���û�ж�ʱ��ʱ
		return false;
	}

	//�жϡ����ж�ʱ����
	unsigned timeNow=GetTickCount();		//��ȡ��ǰʱ��
	itListTimer it=m_listTimer.begin();	//���嶨ʱ���б������
	for( ; it!=m_listTimer.end();it++)	//����ÿ����ʱ����
	{
		//�Ƚ϶�ʱ���ϴ����е�ʱ��͵�ǰʱ����Ƿ���ڵ���timeElapse
		if((timeNow-it->timeLastRun) >= it->timeElapse)
		{//������ڣ������ж�ʱ�������С�ھͲ����С�
			it->timeLastRun = timeNow;

			unsigned ret=OnTimer(it->timerID,it->iParam,it->strParam);
			//���OnTimer()����0��ִ������κ����ٸö�ʱ����
			if(ret==0)
			{
				//�ı䶨ʱ��������״̬Ϊtrue
				it->bDel=true;
			}
			/*else if(ret>1000)
			{//���ݶ�ʱ������ֵ�޸Ķ�ʱ�����
				it->timeElapse=ret;
			}*/
		}
	}
	return true;
}

void cMyTimer::CheckDelTimer()
{//��⡢ɾ��bDel״̬ΪTRUE�Ķ�ʱ��
	itListTimer it=m_listTimer.begin();
	for( ;it!=m_listTimer.end(); )
	{
		if(it->bDel)
		{
			it=m_listTimer.erase(it);//ɾ����ʱ��
			continue;
		}
		it++;
	}
}

void cMyTimer::DeletTimer(unsigned timerID)
{
	itListTimer it=m_listTimer.begin();
	for( ;it!=m_listTimer.end(); it++)
	{
		if(it->timerID==timerID)
		{
			it->bDel=true;
			return;
		}
	}
}
/************************************************************************/