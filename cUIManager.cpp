#include "stdafx.h"
#include "cUIManager.h"
using namespace myui;
#pragma comment(lib,"msimg32.lib")	//TransparentBlt()需要

/***********************全局变量/函数*********************/
HHOOK g_hhkmouse=0;						//全局钩子
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
		case WM_RBUTTONDOWN:
			g_UIManager->OnLButtonDown();
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
	AddTimer(1,50);						//定时器ID为1,50ms触发一次
	g_hhkmouse=SetWindowsHookEx(WH_MOUSE_LL,MouseProc,NULL,0);	//安装钩子，记得要卸载
}
cUIManager::~cUIManager()
{
	UnhookWindowsHookEx(g_hhkmouse);				//卸载钩子
}
cPicture* cUIManager::CreatePicture(LPCTSTR lpFilePath)
{//通过路径，将图片保存到 m_hBitmap 中
	cPicture* pic=new cPicture();
	if(!pic->Create(lpFilePath))
	{
		delete pic;
		return NULL;
	}
	m_uiList.push_back(pic);
	return pic;
}
int cUIManager::OnTimer(int id,int iParam,string str)
{//定时器的回调函数
	switch(id)
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
	SelectObject(memDc,hBitmap);		//将兼容位图选到兼容DC上

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
		if((*it)->GetControlType()==type_picture)
		{
			//绘制图片
			cPicture* pic=(cPicture*)(*it);
			if(pic==NULL)
				continue;
			POINT pt=pic->GetPos();				//图片的位置
			HDC dcTemp=CreateCompatibleDC(NULL);//创建一个DC
			SelectObject(dcTemp,pic->GetHBitmap());
			TransparentBlt(memDc,pt.x,pt.y,pic->GetWidth(),pic->GetHeight(),
				dcTemp,0,0,pic->GetWidth(),pic->GetHeight(),RGB(0,0,0));//需要msimg32.lib
			DeleteObject(dcTemp);
		}
	}


	//再把memDC绘制到设备hDc上
	BitBlt(hDc,0,0,rect.right-rect.left,rect.bottom-rect.top,memDc,0,0,SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(memDc);
	ReleaseDC(m_hWnd,hDc);
}
HWND cUIManager::GetBindHwnd()
{
	return m_hWnd;
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
		//判断是否点击相应的控件
		if(IsInRect(it->pUI->GetRect(),pt))
		{
			//调用回调函数
			it->func(it->iUser);
		}
	}
};
void cUIManager::AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iUser )
{//为哪个控价添加事件
	stEvent event;
	event.pUI=pUI;
	event.eventType=eventType;
	event.func=func;
	event.iUser=iUser;
	m_eventList.push_back(event);
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

}
BOOL cPicture::Create(LPCTSTR lpFilePath)
{
	SetLastError(0xff);
	HBITMAP hBitmap=(HBITMAP)LoadImage(NULL,lpFilePath,IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	int error=GetLastError();	//查看加载失败错误代码，error=2时表示找不到文件
	if(!hBitmap)
		return FALSE;
	m_hBitmap=hBitmap;
	return TRUE;
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