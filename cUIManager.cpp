#include "stdafx.h"
#include "cUIManager.h"
using namespace myui;

/***********************全局函数*********************/
myui::cUIManager* CreateUI(HWND hWnd)
{
	cUIManager*uimanager=new cUIManager(hWnd);
	return uimanager;
}
/***************************************************/



/***********************UI管理器*********************/
cUIManager::cUIManager(HWND hWnd)
{
	m_hWnd=hWnd;
}
cUIManager::~cUIManager()
{

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
/***************************************************/


/***********************UI基类*********************/
cBaseUI::cBaseUI()
{
	m_ptPos.x=0;
	m_ptPos.y=0;
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
/***************************************************/



/***********************cPicture类*********************/
cPicture::cPicture()
{
	m_hBitmap=NULL;
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
/***************************************************/