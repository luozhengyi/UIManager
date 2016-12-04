#ifndef __CUIMANAGER_H__
#define __CUIMANAGER_H__

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
		virtual unsigned GetHeight(){return 0;};		//��ÿؼ��ĸ߶�
	protected:
		POINT m_ptPos;									//�ؼ���λ��
		ControlType m_type;								//�ؼ�����
	};

	struct stEvent				//�¼���Ϣ�ṹ��
	{
		EventType eventType;		//�¼�����
		cBaseUI* pUI;			//�¼������ؼ�
		EventFunc func;			//�¼��Ĵ�����
		int iParam;				//func�����Ĳ���
		stEvent()				//���캯������ʼ��
		{
			iParam=0;
		}
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

	enum TextType		//��������
	{
		text_normal,		//��������
		text_scoll,		//��������
		text_twinkle,	//��˸����
		testTypeNum		//����
	};
	
	class cText:public cBaseUI
	{//���ֵ����ԣ��������ݡ������С��������ɫ��
	public:
		cText();
		virtual ~cText();
		
		//�������ֿؼ���lpText���������ݣ�uiColor��������ɫ��textType����������
		bool Create(LPCTSTR lpText,UINT uiColor=RGB(0,0,0),TextType textType=text_normal);
		TextType GetTextType();					//��ȡ��������
		UINT GetTextColor();					//��ȡ���ֵ���ɫ
		void SetTextColor(UINT rgbColor);		//����������ɫ
		UINT GetTextSize();						//������ִ�С
		void SetTextSize(UINT textSize);		//�������ִ�С
		LPCSTR GetText();						//��ȡ��������
		unsigned GetWidth();					//��ÿؼ��Ŀ��
		void SetWidth(UINT width);				//���ÿؼ��Ŀ��
		unsigned GetHeight();					//��ÿؼ��ĸ߶�
		void SetHeight(UINT height);			//���ÿؼ��ĸ߶�
	public:
		POINT m_scollPos;						//���������λ��

	private:
		string m_text;			//��������
		UINT m_textColor;		//������ɫ
		TextType m_textType;	//��������
		UINT m_textSize;		//����Ĵ�С���ڹ��캯�����ʼ��
	private:
		UINT m_width;			//���ֿؼ��Ŀ��;�ڹ��캯�����ʼ��
		UINT m_height;			//���ֿؼ��ĸ߶�

	};

	enum ButtonStatus		//��ť��״̬
	{
		status_normal,		//����״̬
		status_mouseon,		//�������״̬
		status_pushdown,	//��갴��״̬
		status_unavailable	//������״̬
	};
	
	class cButton:public cBaseUI
	{
	public:
		cButton();
		virtual ~cButton();
		//������ť������״̬ͼƬ·����
		bool Create(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,LPCSTR lpPushDownImagePath);
	public:
		ButtonStatus GetStatus();				//���BUTTON״̬
		void SetStatus(ButtonStatus status);	//����BUTTON״̬
		HBITMAP GetCurStatusBitmap();			//����״̬��ȡ״̬ͼƬ	
		unsigned GetWidth();					//��ȡ��ť�ؼ��Ŀ��(��ȡͼƬ�Ŀ��)
		unsigned GetHeight();					//��ȡ��ť�ؼ��ĸ߶�
	private:
		ButtonStatus m_status;		//���水ť��״̬
		HBITMAP m_hBitmapNormal;	//��������״̬�İ�ťλͼ�ľ��
		HBITMAP m_hBitmapMouseOn;	//�����������״̬�İ�ťλͼ�ľ��
		HBITMAP m_hBitmapPushDown;	//������갴��״̬�İ�ťλͼ�ľ��


	};

	class cUIManager:public cMyTimer						//UI������
	{
	public:
		cUIManager(HWND hWnd);		//��ȡ���ھ������Ӷ�ʱ�������ù��ӣ�
		virtual ~cUIManager();
		virtual unsigned OnTimer(unsigned timerID,int iParam,string strParam);	//��ʱ���Ļص�����,��дcMyTimer�е��麯��
		void Draw();										//�ؼ�����,����ʱ���̵߳�OnTimer()���ã���ͣ�Ļ��ơ�
		HWND GetBindHwnd();									//��ȡ�󶨵Ĵ��ڵľ��
		void AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iParam=0 );
		//ͼƬ
		cPicture* CreatePicture(LPCTSTR lpFilePath);		//ͨ��ͼƬ·����������ͼƬ����Draw()��������
		void OnLButtonDown();							//������µ���Ϣ��Ӧ�����������ͼƬ�ؼ�����ť�ؼ�
		//����
		cText* CreateText(LPCTSTR lpText,UINT uiColor=RGB(0,0,0),TextType textType=text_normal);	//�������ֿؼ�
		void DrawText(cText* pText,HDC memDc);				//��������,��Draw()��������
		//��ť
		cButton* CreateButton(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,LPCSTR lpPushDownImagePath);	//������ʾBUTTON����Draw()��������
		void OnMouseMove();			//���������Ӧ�����������ӵĻص���������
		void OnLButtonUp();			//��굯����Ӧ�����������ӵĻص���������
		void CheckOnButton();		//����Ƿ��Ƶ�BUTTON�ϣ���OnMouseMove()����
		void CheckButtonUp();		//����Ƿ��BUTTON�ϵ��𣬱�OnLButtonUp()����
		void CheckClickButton();		//����Ƿ��ڰ�Ŧ�ϰ��£���OnLButtonDown()����
	private:
		HWND m_hWnd;										//�󶨵Ĵ���,��ͼ��ȡDC��ʱ��ʹ��
		std::list<cBaseUI*> m_uiList;						//�������пؼ�
		std::list<stEvent> m_eventList;						//�������е��¼�
	};
}


//Ϊĳ�����ڴ���UI������
myui::cUIManager* CreateUI(HWND hWnd);//UI��Ҫ��ѡ���Ĵ�����



#endif		//__CUIMANAGER_H__