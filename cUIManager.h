#ifndef __CUIMANAGER_H__
#define __CUIMANAGER_H__

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
		virtual unsigned GetHeight(){return 0;};		//获得控件的高度
	protected:
		POINT m_ptPos;									//控件的位置
		ControlType m_type;								//控件类型
	};

	struct stEvent				//事件信息结构体
	{
		EventType eventType;		//事件类型
		cBaseUI* pUI;			//事件所属控价
		EventFunc func;			//事件的处理函数
		int iParam;				//func函数的参数
		stEvent()				//构造函数，初始化
		{
			iParam=0;
		}
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

	enum TextType		//文字类型
	{
		text_normal,		//正常文字
		text_scoll,		//滚动文字
		text_twinkle,	//闪烁文字
		testTypeNum		//数字
	};
	
	class cText:public cBaseUI
	{//文字的属性：文字内容、字体大小、字体颜色，
	public:
		cText();
		virtual ~cText();
		
		//创建文字控件，lpText：文字内容；uiColor：文字颜色；textType：文字类型
		bool Create(LPCTSTR lpText,UINT uiColor=RGB(0,0,0),TextType textType=text_normal);
		TextType GetTextType();					//获取文字类型
		UINT GetTextColor();					//获取文字的颜色
		void SetTextColor(UINT rgbColor);		//设置文字颜色
		UINT GetTextSize();						//获得文字大小
		void SetTextSize(UINT textSize);		//设置文字大小
		LPCSTR GetText();						//获取文字内容
		unsigned GetWidth();					//获得控件的宽度
		void SetWidth(UINT width);				//设置控件的宽度
		unsigned GetHeight();					//获得控件的高度
		void SetHeight(UINT height);			//设置控件的高度
	public:
		POINT m_scollPos;						//滚动字体的位置

	private:
		string m_text;			//文字内容
		UINT m_textColor;		//文字颜色
		TextType m_textType;	//文字类型
		UINT m_textSize;		//字体的大小；在构造函数里初始化
	private:
		UINT m_width;			//文字控件的宽度;在构造函数里初始化
		UINT m_height;			//文字控件的高度

	};

	enum ButtonStatus		//按钮的状态
	{
		status_normal,		//正常状态
		status_mouseon,		//鼠标移入状态
		status_pushdown,	//鼠标按下状态
		status_unavailable	//不可用状态
	};
	
	class cButton:public cBaseUI
	{
	public:
		cButton();
		virtual ~cButton();
		//创建按钮：三种状态图片路径；
		bool Create(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,LPCSTR lpPushDownImagePath);
	public:
		ButtonStatus GetStatus();				//获得BUTTON状态
		void SetStatus(ButtonStatus status);	//设置BUTTON状态
		HBITMAP GetCurStatusBitmap();			//根据状态获取状态图片	
		unsigned GetWidth();					//获取按钮控件的宽度(就取图片的宽度)
		unsigned GetHeight();					//获取按钮控件的高度
	private:
		ButtonStatus m_status;		//保存按钮的状态
		HBITMAP m_hBitmapNormal;	//保存正常状态的按钮位图的句柄
		HBITMAP m_hBitmapMouseOn;	//保存鼠标移入状态的按钮位图的句柄
		HBITMAP m_hBitmapPushDown;	//保存鼠标按下状态的按钮位图的句柄


	};

	class cUIManager:public cMyTimer						//UI管理器
	{
	public:
		cUIManager(HWND hWnd);		//获取窗口句柄；添加定时器；设置钩子；
		virtual ~cUIManager();
		virtual unsigned OnTimer(unsigned timerID,int iParam,string strParam);	//定时器的回调函数,重写cMyTimer中的虚函数
		void Draw();										//控件绘制,被定时器线程的OnTimer()调用，不停的绘制。
		HWND GetBindHwnd();									//获取绑定的窗口的句柄
		void AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iParam=0 );
		//图片
		cPicture* CreatePicture(LPCTSTR lpFilePath);		//通过图片路径名来创建图片，被Draw()函数调用
		void OnLButtonDown();							//左键按下的消息响应函数，点击到图片控件、或按钮控件
		//文字
		cText* CreateText(LPCTSTR lpText,UINT uiColor=RGB(0,0,0),TextType textType=text_normal);	//创建文字控件
		void DrawText(cText* pText,HDC memDc);				//绘制文字,被Draw()函数调用
		//按钮
		cButton* CreateButton(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,LPCSTR lpPushDownImagePath);	//创建显示BUTTON，被Draw()函数调用
		void OnMouseMove();			//鼠标移入响应函数，被钩子的回调函数调用
		void OnLButtonUp();			//鼠标弹起响应函数，被钩子的回调函数调用
		void CheckOnButton();		//检测是否移到BUTTON上，被OnMouseMove()调用
		void CheckButtonUp();		//检测是否从BUTTON上弹起，被OnLButtonUp()调用
		void CheckClickButton();		//检测是否在按纽上按下，被OnLButtonDown()调用
	private:
		HWND m_hWnd;										//绑定的窗口,绘图获取DC的时候使用
		std::list<cBaseUI*> m_uiList;						//保存所有控件
		std::list<stEvent> m_eventList;						//保存所有的事件
	};
}


//为某个窗口创建UI管理器
myui::cUIManager* CreateUI(HWND hWnd);//UI是要在选定的窗口上



#endif		//__CUIMANAGER_H__