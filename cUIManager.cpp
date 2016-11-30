#include "stdafx.h"
#include "cUIManager.h"
using namespace myui;
#pragma comment(lib,"msimg32.lib")	//TransparentBlt()��Ҫ

/***********************ȫ�ֱ���/����*********************/
HHOOK g_hhkmouse=0;						//ȫ�ֹ���
cUIManager* g_UIManager=NULL;			//ȫ�ֵ�UI������
myui::cUIManager* CreateUI(HWND hWnd)	//����UI������
{
	cUIManager*uimanager=new cUIManager(hWnd);
	g_UIManager=uimanager;
	return uimanager;
}
LRESULT CALLBACK MouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{//���ӵĻص�����
	if(g_UIManager==NULL)

	{
		return CallNextHookEx(g_hhkmouse,nCode,wParam,lParam);
	}
	HWND hWnd=GetActiveWindow();		//��ȡ�����
	//��Ϊ���Ƕ������ȫ�ֹ��ӣ���ػ����е�����¼�������Ҫ�ж��ǲ����ڱ������ϵ�
	if(hWnd==g_UIManager->GetBindHwnd())//������Ϊ�����ʱ����Ӧ����¼�
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
{//�жϵ��Ƿ��ھ���������
	if(pt.x>rect.left&&pt.x<rect.right&&pt.y>rect.top&&pt.y<rect.bottom)
		return TRUE;
	return FALSE;
}
/***************************************************/



/***********************UI������*********************/
cUIManager::cUIManager(HWND hWnd)
{
	m_hWnd=hWnd;
	AddTimer(1,50);						//��ʱ��IDΪ1,50ms����һ��
	g_hhkmouse=SetWindowsHookEx(WH_MOUSE_LL,MouseProc,NULL,0);	//��װ���ӣ��ǵ�Ҫж��
}
cUIManager::~cUIManager()
{
	UnhookWindowsHookEx(g_hhkmouse);				//ж�ع���
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
int cUIManager::OnTimer(int id,int iParam,string str)
{//��ʱ���Ļص�����
	switch(id)
	{
	case 1:
		Draw();
		break;
	default:
		break;
	}
	return 1;	//֮ǰ��������һ�� return NULL;���³������
}
void cUIManager::Draw()
{//�����ڰ󶨵Ĵ�����
	if(!m_hWnd)
	{
		return;
	}
	HDC hDc=GetDC(m_hWnd);				//��ȡ���ڵ�DC
	HDC memDc=CreateCompatibleDC(hDc);	//ʹ��˫�����ֹ��˸������hDc�ļ���DC
	RECT rect;
	GetClientRect(m_hWnd,&rect);			//��ȡ���ڵĿͻ�����С
	HBITMAP hBitmap=CreateCompatibleBitmap(hDc,rect.right-rect.left,
										rect.bottom-rect.top);	//��������λͼ
	SelectObject(memDc,hBitmap);		//������λͼѡ������DC��

	//�ı�memDc�Ļ�ˢ
	/*HBRUSH hBrush_bg=CreateSolidBrush(RGB(255,255,255));*/
	HBRUSH hBrush_bg=(HBRUSH)GetStockObject(WHITE_BRUSH);
	SelectObject(memDc,hBrush_bg);
	FillRect(memDc,&rect,hBrush_bg);
	//�ѿؼ����Ƶ�memDC�ϣ�
	list<cBaseUI*>::iterator it=m_uiList.begin();
	for( ;it!=m_uiList.end();it++)
	{
		if(!(*it))
		{
			continue;
		}
		if((*it)->GetControlType()==type_picture)
		{
			//����ͼƬ
			cPicture* pic=(cPicture*)(*it);
			if(pic==NULL)
				continue;
			POINT pt=pic->GetPos();				//ͼƬ��λ��
			HDC dcTemp=CreateCompatibleDC(NULL);//����һ��DC
			SelectObject(dcTemp,pic->GetHBitmap());
			TransparentBlt(memDc,pt.x,pt.y,pic->GetWidth(),pic->GetHeight(),
				dcTemp,0,0,pic->GetWidth(),pic->GetHeight(),RGB(0,0,0));//��Ҫmsimg32.lib
			DeleteObject(dcTemp);
		}
	}


	//�ٰ�memDC���Ƶ��豸hDc��
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
{//��Ӧ����¼�
	/*MessageBox(NULL,L"��������",L"a",MB_OK);*/
	POINT pt;
	GetCursorPos(&pt);	//�˴���ȡ������������������������Ͻǵ�
	ScreenToClient(m_hWnd,&pt);	//��ptת��Ϊ����ڿͻ������Ͻǵ�����
	list<stEvent>::iterator it=m_eventList.begin();		//�����б�ĵ�����
	for(;it!=m_eventList.end();it++)
	{
		//�ж��Ƿ�����Ӧ�Ŀؼ�
		if(IsInRect(it->pUI->GetRect(),pt))
		{
			//���ûص�����
			it->func(it->iUser);
		}
	}
};
void cUIManager::AddEnvent(cBaseUI *pUI,EventType eventType,EventFunc func,int iUser )
{//Ϊ�ĸ��ؼ�����¼�
	stEvent event;
	event.pUI=pUI;
	event.eventType=eventType;
	event.func=func;
	event.iUser=iUser;
	m_eventList.push_back(event);
}
/***************************************************/


/***********************UI����*********************/
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
{//��ÿؼ����������
	RECT rect;
	rect.left=m_ptPos.x;
	rect.right=m_ptPos.x+GetWidth();
	rect.top=m_ptPos.y;
	rect.bottom=m_ptPos.y+GetHeight();
	return rect;
}
/***************************************************/



/***********************cPicture��*********************/
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
	int error=GetLastError();	//�鿴����ʧ�ܴ�����룬error=2ʱ��ʾ�Ҳ����ļ�
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
{//���λͼ�Ŀ��
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmWidth;
}
unsigned cPicture::GetHeight()
{//���λͼ�ĸ߶�
	BITMAP bitinfo;
	GetObject(m_hBitmap,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}
/***************************************************/