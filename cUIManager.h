#pragma once
#include "cMyTimer.h"
#include "list"
typedef void (*EventFunc)(int );		//定义函数指针类型
/*using namespace std;	//不然list无法识别*/

namespace myui			//建立自己的命名空间
{
	enum ControlType		//定义控件类型的枚举
	{
		type_picture,	//图片类型
		type_text,		//文字类型
		type_animation,	//动画类型
		type_button,		//按钮类型
		type_num			//数字类型
		
	};

	enum EventType			//事件类型
	{
		event_lbuttondown,	//左键按下	
		event_lbuttonup		//左键弹起
	};

	class cBaseUI	//控件的基类
	{//各种控件都有某些相同的属性
	public:
		cBaseUI();
		virtual ~cBaseUI();
		void SetPos(POINT pt);							//设置控件位置
		POINT GetPos();									//获得控件位置
		ControlType GetControlType(){ return m_type; }	//获取控件类型
		RECT GetRect();									//获得控件矩形区域
		virtual unsigned GetWidth(){return 0;};			//获得控件的宽度
		virtual unsigned GetHeight(){return 0;};			//获得控件的高度
	protected:
		POINT m_ptPos;									//控件的位置
		ControlType m_type;								//控件类型
	};

	struct stEvent				//事件信息结构体
	{
		EventType eventType;		//事件类型
		cBaseUI* pUI;			//事件所属控价
		EventFunc func;			//事件的处理函数
		int iUser;				//func函数的参数
	};

	class cPicture:public cBaseUI
	{
	public:
		cPicture();
		virtual ~cPicture();
		BOOL Create(LPCTSTR lpFilePath);	//创建图片
		HBITMAP GetHBitmap();			//获得图片位图
		unsigned GetWidth();				//获得位图的宽度
		unsigned GetHeight();			//获得位图的高度
	private:
		HBITMAP m_hBitmap;				//保存位图

		
	};

	class cUIManager	:public cMyTimer							//UI管理器
	{
	public:
		cUIManager(HWND hWnd);
		virtual ~cUIManager();
		//图片，文字、按钮、动画、特效等控件
		cPicture* CreatePicture(LPCTSTR lpFilePath);			//通过图片路径名来创建图片
		void Draw();											//控件绘制
		virtual int OnTimer(int id,int iParam,string str);	//定时器的回调函数,重写cMyTimer中的虚函数
		HWND GetBindHwnd();									//获取绑定的窗口的句柄
		void OnLButtonDown();								//左键按下的消息响应函数
		void AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iUser );
	private:
		HWND m_hWnd;											//绑定的窗口
		std::list<cBaseUI*> m_uiList;						//保存所有控件
		std::list<stEvent> m_eventList;						//保存所有的事件
	};
}


//为某个窗口创建UI管理器
myui::cUIManager* CreateUI(HWND hWnd);//UI是要在选定的窗口上