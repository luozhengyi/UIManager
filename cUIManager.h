#pragma once
#include "list"
/*using namespace std;		//不然list无法识别*/

namespace myui	//建立自己的命名空间
{
	class cBaseUI	//控件的基类
	{//各种控件都有某些相同的属性
	public:
		cBaseUI();
		virtual ~cBaseUI();
		void SetPos(POINT pt);	//设置控件位置
		POINT GetPos();	//获得控件位置
	protected:
		POINT m_ptPos;	//控件的位置
	};

	class cPicture:public cBaseUI
	{
	public:
		cPicture();
		virtual ~cPicture();
		BOOL Create(LPCTSTR lpFilePath);	//创建图片
	private:
		HBITMAP m_hBitmap;	//保存位图
		
	};


	class cUIManager		//UI管理器
	{
	public:
		cUIManager(HWND hWnd);
		virtual ~cUIManager();
		//图片，文字、按钮、动画等控件
		//特效等控件
		cPicture* CreatePicture(LPCTSTR lpFilePath);		//通过图片路径名来创建图片

	private:
		HWND m_hWnd;		//绑定的窗口
		std::list<cBaseUI*> m_uiList;		//保存所有控件
	};
}

//为某个窗口创建UI管理器
myui::cUIManager* CreateUI(HWND hWnd);//UI是要在选定的窗口行