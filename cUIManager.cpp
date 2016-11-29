#include "stdafx.h"
#include "cUIManager.h"
using namespace myui;

/***********************ȫ�ֺ���*********************/
myui::cUIManager* CreateUI(HWND hWnd)
{
	cUIManager*uimanager=new cUIManager(hWnd);
	return uimanager;
}
/***************************************************/



/***********************UI������*********************/
cUIManager::cUIManager(HWND hWnd)
{
	m_hWnd=hWnd;
}
cUIManager::~cUIManager()
{

}
cPicture* cUIManager::CreatePicture(LPCTSTR lpFilePath)
{//ͨ��·������ͼƬ���浽 m_hBitmap ��
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


/***********************UI����*********************/
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



/***********************cPicture��*********************/
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
	int error=GetLastError();	//�鿴����ʧ�ܴ�����룬error=2ʱ��ʾ�Ҳ����ļ�
	if(!hBitmap)
		return FALSE;
	m_hBitmap=hBitmap;
	return TRUE;
}
/***************************************************/