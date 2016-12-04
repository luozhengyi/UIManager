#include "stdafx.h"
#include "cUIManager.h"
using namespace myui;
#pragma comment(lib,"msimg32.lib")	//TransparentBlt()需要

/***********************全局变量/函数*********************/
HHOOK g_hhkmouse=0;						//全局钩子,在cUIManager()里初始化；
cUIManager* g_UIManager=NULL;			//全局的UI管理器
myui::cUIManager* CreateUI(HWND hWnd)	//创建UI管理器
{
	cUIManager*uimanager=new cUIManager(hWnd);
	g_UIManager=uimanager;
	return uimanager;
}
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{//钩子的回调函数
	if(g_UIManager==NULL)

	{
		return CallNextHookEx(g_hhkmouse,nCode,wParam,lParam);
	}
	HWND hWnd=GetActiveWindow();		//获取活动窗口
	//因为我们定义的是全局钩子，会截获所有的鼠标事件，所以要判断是不是在本窗口上的
	if(hWnd==g_UIManager->GetBindHwnd())//本窗口为活动窗口时才响应鼠标事件
	{
		switch(wParam)
		{
		case WM_LBUTTONDOWN:		//鼠标点击事件
			g_UIManager->OnLButtonDown();
			break;
		case WM_MOUSEMOVE:		//鼠标移入按钮事件(但是没有往UIManager里面添加事件)
			g_UIManager->OnMouseMove();
			break;
		case WM_LBUTTONUP:		//鼠标点击按钮弹起事件(但是没有往UIManager里面添加事件)
			g_UIManager->OnLButtonUp();
			break;

		}

	}

	return CallNextHookEx(g_hhkmouse,nCode,wParam,lParam);
}
BOOL IsInRect(RECT rect,POINT pt)
{//判断点是否在矩形区域内
	if(pt.x>rect.left&&pt.x<rect.right&&pt.y>rect.top&&pt.y<rect.bottom)
		return TRUE;
	return FALSE;
}
/***************************************************/



/***********************UI管理器*********************/
cUIManager::cUIManager(HWND hWnd)
{
	m_hWnd=hWnd;
	AddTimer(1,50);												//定时器ID为1,50ms触发一次
	g_hhkmouse=SetWindowsHookEx(WH_MOUSE_LL,MouseProc,NULL,0);	//安装钩子，记得要卸载
}
cUIManager::~cUIManager()
{
	UnhookWindowsHookEx(g_hhkmouse);				//卸载钩子
}
unsigned cUIManager::OnTimer(unsigned timerID,int iParam,string strParam)
{//定时器的回调函数
	switch(timerID)
	{
	case 1:
		Draw();
		break;
	default:
		break;
	}
	return 1;	//之前先乱填了一个 return NULL;导致程序出错
}
void cUIManager::Draw()
{//绘制在绑定的窗口上
	if(!m_hWnd)
	{
		return;
	}
	HDC hDc=GetDC(m_hWnd);				//获取窗口的DC
	HDC memDc=CreateCompatibleDC(hDc);	//使用双缓存防止闪烁，创建hDc的兼容DC
	RECT rect;
	GetClientRect(m_hWnd,&rect);			//获取窗口的客户区大小
	HBITMAP hBitmap=CreateCompatibleBitmap(hDc,rect.right-rect.left,
				rect.bottom-rect.top);	//创建兼容位图
	SelectObject(memDc,hBitmap);			//将兼容位图选到兼容DC上

	//改变memDc的画刷
	/*HBRUSH hBrush_bg=CreateSolidBrush(RGB(255,255,255));*/
	HBRUSH hBrush_bg=(HBRUSH)GetStockObject(WHITE_BRUSH);
	SelectObject(memDc,hBrush_bg);
	FillRect(memDc,&rect,hBrush_bg);
	//把控件绘制到memDC上，
	list<cBaseUI*>::iterator it=m_uiList.begin();
	for( ;it!=m_uiList.end();it++)
	{
		if(!(*it))
		{
			continue;
		}
		if((*it)->GetControlType()==type_picture)	//绘制图片控件
		{
			
			cPicture* pic=(cPicture*)(*it);		//将it的内容(cBase*)转换成cPicture*
			if(pic==NULL)
				continue;
			POINT pt=pic->GetPos();							//图片的位置
			HDC dcTemp=CreateCompatibleDC(NULL);				//创建一个临时兼容DC
			SelectObject(dcTemp,pic->GetHBitmap());			//将实际位图选入临时兼容DC
			TransparentBlt(memDc,pt.x,pt.y,pic->GetWidth(),pic->GetHeight(),
				dcTemp,0,0,pic->GetWidth(),pic->GetHeight(),RGB(0,0,0));//需要msimg32.lib
			DeleteObject(dcTemp);
		}
		else if((*it)->GetControlType()==type_text)	//绘制文字控件
		{
			cText* pText=(cText*)(*it);				//将it的内容(cBase*)转换成cText*
			if(pText==NULL)
			{
				continue;
			}
			DrawText(pText,memDc);
		}
		else if((*it)->GetControlType()==type_button)	//绘制按钮控件
		{
			cButton* pButton=(cButton*)(*it);				//将it的内容(cBase*)转换成cButton*
			if(pButton==NULL)
			{
				continue;
			}
			HBITMAP hBitmap=pButton->GetCurStatusBitmap();
			if(hBitmap==NULL)
			{
				continue;
			}
			HDC dcTemp=CreateCompatibleDC(NULL);
			SelectObject(dcTemp,hBitmap);
			POINT pt=pButton->GetPos();
			TransparentBlt(memDc,pt.x,pt.y,pButton->GetWidth(),pButton->GetHeight(),
							dcTemp,0,0,pButton->GetWidth(),pButton->GetHeight(),RGB(0,0,0));
			DeleteObject(dcTemp);
		}
	}


	//再把memDC绘制到设备hDc上；参数二、三是兼容DC的初始位置；四、五兼容DC的大小；
	BitBlt(hDc,0,0,rect.right-rect.left,rect.bottom-rect.top,memDc,0,0,SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(memDc);
	ReleaseDC(m_hWnd,hDc);
}
void cUIManager::DrawText(cText* pText,HDC memDc)
{//绘制文字，被Draw()调用
	
	switch(pText->GetTextType())		//根据文字类型分别绘制
	{
	case text_normal:	//正常普通字体
		{
			SetBkMode(memDc,0);								//设置文字背景透明；
			UINT colorOld=SetTextColor(memDc,pText->GetTextColor());	//colorOld是原始memDc上的文字颜色
			LOGFONT logfont;								//字体结构体，改变输出字体
			ZeroMemory(&logfont,sizeof(LOGFONT));		//ZeroMemory宏用0来填充一块内存区域。
			logfont.lfCharSet=GB2312_CHARSET;			//设置字体类型
			logfont.lfHeight=pText->GetTextSize();		//设置字体大小
			logfont.lfWeight=700;						//字体加粗
			HFONT hFont=CreateFontIndirect(&logfont);	//创建一种在结构LOGFONT中定义特性的逻辑字体
			HGDIOBJ hOldFont=SelectObject(memDc,hFont);	//为设备环境选择新字体，返回原始字体。
			TextOutA(memDc,pText->GetPos().x,pText->GetPos().y,pText->GetText(),strlen(pText->GetText()));
			SetTextColor(memDc,colorOld);				//绘制完后选回DC上原来的字体颜色
			SelectObject(memDc,hOldFont);				//绘制完后选回DC上原来的字体
			DeleteObject(hFont);							//GDI对象使用完后一定要记得删除
			break;
		}
		
	case text_twinkle:		//闪烁字体
		{
			//改变字体颜色
			UINT uiCurColor=pText->GetTextColor();	//当前字体颜色
			UINT uiNextColor;
			/*
			UINT rValue=0xff & uiCurColor;			//获取颜色重的R分量值
			UINT gValue=0xff & (uiCurColor>>8);		//获取颜色重的G分量值
			UINT bValue=0xff & (uiCurColor>>16);		//获取颜色重的B分量值
			rValue=(rValue+50)%256;
			gValue=(gValue+50)%256;
			bValue=(bValue+50)%256;
			*/
			srand(GetTickCount());
			UINT rValue=rand()/256;
			UINT gValue=rand()/256;
			UINT bValue=rand()/256;
			uiNextColor=RGB(rValue,gValue,bValue);
			pText->SetTextColor(uiNextColor);

			//绘制文字部分同 text_normal
			SetBkMode(memDc,0);										//设置文字背景透明；
			UINT colorOld=SetTextColor(memDc,pText->GetTextColor());	//colorOld是原始memDc上的文字颜色
			LOGFONT logfont;											//字体结构体，改变输出字体
			ZeroMemory(&logfont,sizeof(LOGFONT));					//ZeroMemory宏用0来填充一块内存区域。
			logfont.lfCharSet=GB2312_CHARSET;						//设置字体类型
			logfont.lfHeight=pText->GetTextSize();					//设置字体大小
			logfont.lfWeight=700;									//字体加粗
			HFONT hFont=CreateFontIndirect(&logfont);				//创建一种在结构LOGFONT中定义特性的逻辑字体
			HGDIOBJ hOldFont=SelectObject(memDc,hFont);				//为设备环境选择新字体，返回原始字体。
			TextOutA(memDc,pText->GetPos().x,pText->GetPos().y,pText->GetText(),strlen(pText->GetText()));
			SetTextColor(memDc,colorOld);							//绘制完后选回DC上原来的字体颜色
			SelectObject(memDc,hOldFont);							//绘制完后选回DC上原来的字体
			DeleteObject(hFont);										//GDI对象使用完后一定要记得删除
			break;
		}
	case text_scoll:			//滚动字体
		{
			HDC dcTemp = CreateCompatibleDC(memDc);			//创建memDc的临时兼容DC
			//创建兼容位图；
			HBITMAP hBitmap=CreateCompatibleBitmap(memDc,pText->GetWidth(),pText->GetHeight());	
			SelectObject(dcTemp,hBitmap);					//将兼容位图选到兼容DC上
			SetBkMode(dcTemp,0);								//设置文字背景透明；

			//将memDc上被dcTemp遮住的部分绘制到dcTemp上
			BitBlt(dcTemp,0,0,pText->GetWidth(),pText->GetHeight(),memDc,pText->GetPos().x,pText->GetPos().y,SRCCOPY);

			UINT colorOld=SetTextColor(dcTemp,pText->GetTextColor());	//colorOld是原始memDc上的文字颜色
			LOGFONT logfont;												//字体结构体，改变输出字体
			ZeroMemory(&logfont,sizeof(LOGFONT));						//ZeroMemory宏用0来填充一块内存区域。
			logfont.lfCharSet=GB2312_CHARSET;							//设置字体类型
			logfont.lfHeight=pText->GetTextSize();						//设置字体大小
			logfont.lfWeight=700;										//字体加粗
			HFONT hFont=CreateFontIndirect(&logfont);					//创建一种在结构LOGFONT中定义特性的逻辑字体
			HGDIOBJ hOldFont=SelectObject(dcTemp,hFont);					//为设备环境选择新字体，返回原始字体。

			//计算字符串占的像素。
			SIZE szText;
			GetTextExtentPoint32(dcTemp,pText->GetText(),strlen(pText->GetText()),&szText);

			//参数2、3是文字相对于dc的位置（文字是从左向右排的，所以是最左边文字的坐标）
			TextOutA(dcTemp,pText->m_scollPos.x,pText->m_scollPos.y,pText->GetText(),strlen(pText->GetText()));
			
			//让文字动起来：绘制完后就让文字右移
			if(pText->m_scollPos.x>=(int)pText->GetWidth())
			{//如果文字从右边移出了,重新让文字的最右边从文本控件的最左边进入
				pText->m_scollPos.x=-szText.cx;
			}
			pText->m_scollPos.x+=5;	//每次都往右移5个像素
	
			
			SetTextColor(dcTemp,colorOld);				//绘制完后选回DC上原来的字体颜色
			SelectObject(dcTemp,hOldFont);				//绘制完后选回DC上原来的字体
			DeleteObject(hFont);							//GDI对象使用完后一定要记得删除
			//再把dcTemp绘制到memDc上；
			BitBlt(memDc,pText->GetPos().x,pText->GetPos().y,pText->GetWidth(),pText->GetHeight(),dcTemp,0,0,SRCCOPY);
			DeleteObject(hBitmap);
			DeleteObject(dcTemp);
		}
		break;
	default:
		break;

	}
}
HWND cUIManager::GetBindHwnd()
{
	return m_hWnd;
}
void cUIManager::AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iParam)
{//为哪个控价添加事件
	stEvent event;
	event.pUI=pUI;
	event.eventType=eventType;
	event.func=func;
	event.iParam=iParam;
	m_eventList.push_back(event);
}
cPicture* cUIManager::CreatePicture(LPCTSTR lpFilePath)
{//创建图片控件
	//通过路径，将图片保存到 m_hBitmap 中
	cPicture* pic=new cPicture();
	if(!pic->Create(lpFilePath))
	{
		delete pic;
		return NULL;
	}
	m_uiList.push_back(pic);		//将图片控件加入控件列表
	return pic;
}
void cUIManager::OnLButtonDown()
{//响应点击事件
	/*MessageBox(NULL,L"左键点击了",L"a",MB_OK);*/
	POINT pt;
	GetCursorPos(&pt);	//此处获取的坐标是相对于整个窗口左上角的
	ScreenToClient(m_hWnd,&pt);	//将pt转化为相对于客户区左上角的坐标
	list<stEvent>::iterator it=m_eventList.begin();		//定义列表的迭代器
	for(;it!=m_eventList.end();it++)
	{
		if (it->eventType != event_lbuttondown)
		{
			continue;
		}
		//判断是否点击相应的控件
		if(IsInRect(it->pUI->GetRect(),pt))
		{
			//调用回调函数
			it->func(it->iParam);
		}
		//检测是否在按纽上扫下左键
		CheckClickButton();
	}
};
cText* cUIManager::CreateText(LPCTSTR lpText,UINT uiColor,TextType textType)
{//创建文字控件
	cText* pText=new cText();
	if(!pText->Create(lpText,uiColor,textType))
	{
		delete pText;
		return NULL;
	}
	m_uiList.push_back(pText);		//将文字控件加入控件列表
	return pText;

}
cButton* cUIManager::CreateButton(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,LPCSTR lpPushDownImagePath)
{
	cButton *pButton=new cButton;
	if(!pButton->Create(lpNormalImagePath,lpMouseOnImagePath,lpPushDownImagePath))
		return  NULL;
	m_uiList.push_back(pButton);
	return pButton;
}
void cUIManager::CheckClickButton()
{//检测是否在按纽上按下
	POINT pt ;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if ((*it)->GetControlType() != type_button)
		{
			continue;
		}
		cButton* pButton = (cButton*)(*it);
		if (pButton == NULL)
		{
			continue;
		}
		if (IsInRect(pButton->GetRect(),pt))
		{
			pButton->SetStatus(status_pushdown);
		}
		else
		{
			pButton->SetStatus(status_normal);
		}
	}
}
void cUIManager::CheckOnButton()
{//检测是否移到BUTTON上
	POINT pt ;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if ((*it)->GetControlType() != type_button)
		{
			continue;
		}
		cButton* pButton = (cButton*)(*it);
		if (pButton == NULL)
		{
			continue;
		}
		if (IsInRect(pButton->GetRect(),pt))
		{
			if (pButton->GetStatus() == status_pushdown)
			{
				continue;
			}
			pButton->SetStatus(status_mouseon);
		}
		else
		{
			pButton->SetStatus(status_normal);
		}
	}
	
}
void cUIManager::CheckButtonUp()
{//检测是否从BUTTON上弹起
	POINT pt ;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd,&pt);
	for (list<cBaseUI*>::iterator it = m_uiList.begin();
		it != m_uiList.end();it++)
	{
		if ((*it)->GetControlType() != type_button)
		{
			continue;
		}
		cButton* pButton = (cButton*)(*it);
		if (pButton == NULL)
		{
			continue;
		}
		if (IsInRect(pButton->GetRect(),pt))
		{
			pButton->SetStatus(status_mouseon);
		}
	}
}
void cUIManager::OnMouseMove()
{//鼠标移入事件响应函数
	CheckOnButton();		//检测是否移到BUTTON上
}
void cUIManager::OnLButtonUp()
{
	CheckButtonUp();		//检测是否从BUTTON上弹起
}
/***************************************************/



/***********************UI基类*********************/
cBaseUI::cBaseUI()
{
	m_ptPos.x=0;
	m_ptPos.y=0;
	m_type=type_num;
}
cBaseUI::~cBaseUI()
{

}
tagPOINT cBaseUI::GetPos()
{
	return m_ptPos;
}
void cBaseUI::SetPos(POINT pt)
{
	m_ptPos=pt;
}
RECT cBaseUI::GetRect()
{//获得控件的区域矩形
	RECT rect;
	rect.left=m_ptPos.x;
	rect.right=m_ptPos.x+GetWidth();
	rect.top=m_ptPos.y;
	rect.bottom=m_ptPos.y+GetHeight();
	return rect;
}
/***************************************************/



/***********************cPicture类*********************/
cPicture::cPicture()
{
	m_hBitmap=NULL;
	m_type=type_picture;
}
cPicture::~cPicture()
{
	if(m_hBitmap)
		DeleteObject(m_hBitmap);
}
BOOL cPicture::Create(LPCTSTR lpFilePath)
{
	SetLastError(0xff);
	HBITMAP hBitmap=(HBITMAP)LoadImage(NULL,lpFilePath,IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	int error=GetLastError();	//查看加载失败错误代码，error=2时表示找不到文件
	if(!hBitmap)
	{
		DeleteObject(hBitmap);
		return false;
	}
	m_hBitmap=hBitmap;
	return true;
}
HBITMAP cPicture::GetHBitmap()
{
	return m_hBitmap;
}
unsigned cPicture::GetWidth()
{//获得位图的宽度
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmWidth;
}
unsigned cPicture::GetHeight()
{//获得位图的高度
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}
/***************************************************/



/***********************cText类*********************/
cText::cText()
{	
	m_type=type_text;		//控件类型

	m_textSize=18;			//字体的大小；18号字体12个像素
	m_width=500;				//文字控件的宽度
	m_height=50;				//文字控件的高度
	//滚动字体的位置
	m_scollPos.x=0;			
	m_scollPos.y=0;	
}
cText::~cText()
{

}
bool cText::Create(LPCTSTR lpText,UINT uiColor,TextType textType)
{//创建文字控件
	m_text=lpText;			//文字内容
	m_textColor=uiColor;		//文字颜色
	m_textType=textType;		//文字类型
	return true;
}
TextType cText::GetTextType()
{
	return m_textType;
}
UINT cText::GetTextColor()
{
	return m_textColor;
}
void cText::SetTextColor(UINT rgbColor)
{
	m_textColor=rgbColor;
}
UINT cText::GetTextSize()
{
	return m_textSize;
}
void cText::SetTextSize(UINT textSize)
{
	m_textSize=textSize;
}
LPCSTR cText::GetText()
{
	return m_text.c_str();
}
unsigned cText::GetWidth()
{//获得控件的宽度
	return m_width;
}
void cText::SetWidth(UINT width)
{//设置控件的宽度
	m_width=width;
}
unsigned cText::GetHeight()
{//获得控件的高度
	return m_height;
}
void cText::SetHeight(UINT height)	
{//设置控件的高度
	m_height=height;
}
/***************************************************/



/***********************cButton类*********************/
cButton::cButton()
{
	m_hBitmapNormal=NULL;
	m_hBitmapMouseOn=NULL;
	m_hBitmapPushDown=NULL;
	m_status=status_normal;
	m_type=type_button;			//控件类型
}
cButton::~cButton()
{
	if(m_hBitmapNormal)
		DeleteObject(m_hBitmapNormal);
	if(m_hBitmapMouseOn)
		DeleteObject(m_hBitmapMouseOn);
	if(m_hBitmapPushDown)
		DeleteObject(m_hBitmapPushDown);
}
bool cButton::Create(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,LPCSTR lpPushDownImagePath)
{//创建一个BUTTON
	//加载三种状态对应的三幅位图
	HBITMAP hBitmap=(HBITMAP)LoadImage(NULL,lpNormalImagePath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(hBitmap==NULL)
		return false;
	m_hBitmapNormal=hBitmap;

	hBitmap=(HBITMAP)LoadImage(NULL,lpMouseOnImagePath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(hBitmap==NULL)
		return false;
	m_hBitmapMouseOn=hBitmap;

	hBitmap=(HBITMAP)LoadImage(NULL,lpPushDownImagePath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(hBitmap==NULL)
		return false;
	m_hBitmapPushDown=hBitmap;

	return true;
}
ButtonStatus cButton::GetStatus()
{
	return m_status;
}
void cButton::SetStatus(ButtonStatus status)
{
	m_status=status;
}
HBITMAP cButton::GetCurStatusBitmap()
{//根据BUTTON的状态获取对应的贴图
	if(m_status==status_normal)
		return m_hBitmapNormal;
	if(m_status==status_mouseon)
		return m_hBitmapMouseOn;
	if(m_status==status_pushdown)
		return m_hBitmapPushDown;
	return NULL;
}
unsigned cButton::GetWidth()
{
	//获得位图的宽度
	BITMAP bitinfo;
	GetObject(m_hBitmapNormal,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmWidth; 
}
unsigned cButton::GetHeight()
{
	//获得位图的高度
	BITMAP bitinfo;
	GetObject(m_hBitmapNormal,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}
/*****************************************************/