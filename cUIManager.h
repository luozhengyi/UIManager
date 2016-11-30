#pragma once
#include "cMyTimer.h"
#include "list"
typedef void (*EventFunc)(int );		//���庯��ָ������
/*using namespace std;	//��Ȼlist�޷�ʶ��*/

namespace myui			//�����Լ��������ռ�
{
	enum ControlType		//����ؼ����͵�ö��
	{
		type_picture,	//ͼƬ����
		type_text,		//��������
		type_animation,	//��������
		type_button,		//��ť����
		type_num			//��������
		
	};

	enum EventType			//�¼�����
	{
		event_lbuttondown,	//�������	
		event_lbuttonup		//�������
	};

	class cBaseUI	//�ؼ��Ļ���
	{//���ֿؼ�����ĳЩ��ͬ������
	public:
		cBaseUI();
		virtual ~cBaseUI();
		void SetPos(POINT pt);							//���ÿؼ�λ��
		POINT GetPos();									//��ÿؼ�λ��
		ControlType GetControlType(){ return m_type; }	//��ȡ�ؼ�����
		RECT GetRect();									//��ÿؼ���������
		virtual unsigned GetWidth(){return 0;};			//��ÿؼ��Ŀ��
		virtual unsigned GetHeight(){return 0;};			//��ÿؼ��ĸ߶�
	protected:
		POINT m_ptPos;									//�ؼ���λ��
		ControlType m_type;								//�ؼ�����
	};

	struct stEvent				//�¼���Ϣ�ṹ��
	{
		EventType eventType;		//�¼�����
		cBaseUI* pUI;			//�¼������ؼ�
		EventFunc func;			//�¼��Ĵ�����
		int iUser;				//func�����Ĳ���
	};

	class cPicture:public cBaseUI
	{
	public:
		cPicture();
		virtual ~cPicture();
		BOOL Create(LPCTSTR lpFilePath);	//����ͼƬ
		HBITMAP GetHBitmap();			//���ͼƬλͼ
		unsigned GetWidth();				//���λͼ�Ŀ��
		unsigned GetHeight();			//���λͼ�ĸ߶�
	private:
		HBITMAP m_hBitmap;				//����λͼ

		
	};

	class cUIManager	:public cMyTimer							//UI������
	{
	public:
		cUIManager(HWND hWnd);
		virtual ~cUIManager();
		//ͼƬ�����֡���ť����������Ч�ȿؼ�
		cPicture* CreatePicture(LPCTSTR lpFilePath);			//ͨ��ͼƬ·����������ͼƬ
		void Draw();											//�ؼ�����
		virtual int OnTimer(int id,int iParam,string str);	//��ʱ���Ļص�����,��дcMyTimer�е��麯��
		HWND GetBindHwnd();									//��ȡ�󶨵Ĵ��ڵľ��
		void OnLButtonDown();								//������µ���Ϣ��Ӧ����
		void AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iUser );
	private:
		HWND m_hWnd;											//�󶨵Ĵ���
		std::list<cBaseUI*> m_uiList;						//�������пؼ�
		std::list<stEvent> m_eventList;						//�������е��¼�
	};
}


//Ϊĳ�����ڴ���UI������
myui::cUIManager* CreateUI(HWND hWnd);//UI��Ҫ��ѡ���Ĵ�����