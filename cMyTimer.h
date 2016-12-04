#ifndef __CMYTIMER_H__
#define __CMYTIMER_H__

#pragma once
#include <string>
#include "list"
#include "iostream"
#include "windows.h"
using namespace std;

/******************************* ��ʱ���ṹ�� ****************************/
struct stTimer				//���涨ʱ����Ϣ�Ķ�ʱ��
{
	unsigned timerID	;		//��ʱ��ID
	unsigned timeElapse;		//��ʱ������ʱ����
	unsigned timeLastRun;	//��ʱ���ϴ�����ʱ��
	int  iParam;				//Ԥ������
	string strParam;			//Ԥ������
	bool bDel;				//�Ƿ�Ҫ��ɾ��
	stTimer()				//���캯��,��ʼ��
	{
		timerID=0;
		timeElapse=0;
		timeLastRun=0;
		iParam=0;
		strParam="";
		bDel=false;
	}
};
/************************************************************************/

/**************************** �����ض��� *********************************/
typedef std::list<stTimer>				ListTimer;	
typedef std::list<stTimer>::iterator	  itListTimer;	
/************************************************************************/

/************************* ��ʱ���ࣺcMyTimer ************************/
class cMyTimer
{
public:
	cMyTimer();
	virtual ~cMyTimer();
public:
	//��Ӷ�ʱ����������ʱ���߳�:timerID(��ʱ��ID)��timeElapse(��ʱ��ʱ����)��3��4(Ԥ������)
	void AddTimer(unsigned timerID,unsigned timeElapse,int iParam=0,string strParam="");	
	//��ʱ�����ƺ������жϡ����ƶ�ʱ�������У�����ʱ���̻߳ص�����CallBack_TimerProc()����
	bool TimerCtrlRun();
public:
	//��ʱ����Ӧ����������ʱ�����ƺ���TimerCtrlRun()���ã�
	//����Ҫ�̳С����أ��������0����ִ������κ����٣��������;Ҳ���Ը��ݷ���ֵ�޸ļ��ʱ��
	//Ontimer()�ڲ�Ӧ���ж�timerID����ͬ�Ķ�ʱ��IDִ�в�ͬ����Ӧ��
	virtual unsigned OnTimer(unsigned timerID,int iParam,string strParam)=0;
public:
	//��⡢ɾ��bDel״̬ΪTRUE�Ķ�ʱ��
	void CheckDelTimer();
	//����timerIDɾ����ʱ��
	void DeletTimer(unsigned timerID);


private:
	ListTimer m_listTimer;	//�б������������еĶ�ʱ��

};
/*********************************************************************/

#endif  //__CMYTIMER_H__