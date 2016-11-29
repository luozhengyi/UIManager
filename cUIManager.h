#pragma once
#include "list"
/*using namespace std;		//��Ȼlist�޷�ʶ��*/

namespace myui	//�����Լ��������ռ�
{
	class cBaseUI	//�ؼ��Ļ���
	{//���ֿؼ�����ĳЩ��ͬ������
	public:
		cBaseUI();
		virtual ~cBaseUI();
		void SetPos(POINT pt);	//���ÿؼ�λ��
		POINT GetPos();	//��ÿؼ�λ��
	protected:
		POINT m_ptPos;	//�ؼ���λ��
	};

	class cPicture:public cBaseUI
	{
	public:
		cPicture();
		virtual ~cPicture();
		BOOL Create(LPCTSTR lpFilePath);	//����ͼƬ
	private:
		HBITMAP m_hBitmap;	//����λͼ
		
	};


	class cUIManager		//UI������
	{
	public:
		cUIManager(HWND hWnd);
		virtual ~cUIManager();
		//ͼƬ�����֡���ť�������ȿؼ�
		//��Ч�ȿؼ�
		cPicture* CreatePicture(LPCTSTR lpFilePath);		//ͨ��ͼƬ·����������ͼƬ

	private:
		HWND m_hWnd;		//�󶨵Ĵ���
		std::list<cBaseUI*> m_uiList;		//�������пؼ�
	};
}

//Ϊĳ�����ڴ���UI������
myui::cUIManager* CreateUI(HWND hWnd);//UI��Ҫ��ѡ���Ĵ�����