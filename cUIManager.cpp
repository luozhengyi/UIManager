#include "stdafx.h"
#include "cUIManager.h"
using namespace myui;
#pragma comment(lib,"msimg32.lib")	//TransparentBlt()��Ҫ

/***********************ȫ�ֱ���/����*********************/
HHOOK g_hhkmouse=0;						//ȫ�ֹ���,��cUIManager()���ʼ����
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
		case WM_LBUTTONDOWN:		//������¼�
			g_UIManager->OnLButtonDown();
			break;
		case WM_MOUSEMOVE:		//������밴ť�¼�(����û����UIManager��������¼�)
			g_UIManager->OnMouseMove();
			break;
		case WM_LBUTTONUP:		//�������ť�����¼�(����û����UIManager��������¼�)
			g_UIManager->OnLButtonUp();
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
	AddTimer(1,50);												//��ʱ��IDΪ1,50ms����һ��
	g_hhkmouse=SetWindowsHookEx(WH_MOUSE_LL,MouseProc,NULL,0);	//��װ���ӣ��ǵ�Ҫж��
}
cUIManager::~cUIManager()
{
	UnhookWindowsHookEx(g_hhkmouse);				//ж�ع���
}
unsigned cUIManager::OnTimer(unsigned timerID,int iParam,string strParam)
{//��ʱ���Ļص�����
	switch(timerID)
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
	SelectObject(memDc,hBitmap);			//������λͼѡ������DC��

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
		if((*it)->GetControlType()==type_picture)	//����ͼƬ�ؼ�
		{
			
			cPicture* pic=(cPicture*)(*it);		//��it������(cBase*)ת����cPicture*
			if(pic==NULL)
				continue;
			POINT pt=pic->GetPos();							//ͼƬ��λ��
			HDC dcTemp=CreateCompatibleDC(NULL);				//����һ����ʱ����DC
			SelectObject(dcTemp,pic->GetHBitmap());			//��ʵ��λͼѡ����ʱ����DC
			TransparentBlt(memDc,pt.x,pt.y,pic->GetWidth(),pic->GetHeight(),
				dcTemp,0,0,pic->GetWidth(),pic->GetHeight(),RGB(0,0,0));//��Ҫmsimg32.lib
			DeleteObject(dcTemp);
		}
		else if((*it)->GetControlType()==type_text)	//�������ֿؼ�
		{
			cText* pText=(cText*)(*it);				//��it������(cBase*)ת����cText*
			if(pText==NULL)
			{
				continue;
			}
			DrawText(pText,memDc);
		}
		else if((*it)->GetControlType()==type_button)	//���ư�ť�ؼ�
		{
			cButton* pButton=(cButton*)(*it);				//��it������(cBase*)ת����cButton*
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


	//�ٰ�memDC���Ƶ��豸hDc�ϣ������������Ǽ���DC�ĳ�ʼλ�ã��ġ������DC�Ĵ�С��
	BitBlt(hDc,0,0,rect.right-rect.left,rect.bottom-rect.top,memDc,0,0,SRCCOPY);
	DeleteObject(hBitmap);
	DeleteObject(memDc);
	ReleaseDC(m_hWnd,hDc);
}
void cUIManager::DrawText(cText* pText,HDC memDc)
{//�������֣���Draw()����
	
	switch(pText->GetTextType())		//�����������ͷֱ����
	{
	case text_normal:	//������ͨ����
		{
			SetBkMode(memDc,0);								//�������ֱ���͸����
			UINT colorOld=SetTextColor(memDc,pText->GetTextColor());	//colorOld��ԭʼmemDc�ϵ�������ɫ
			LOGFONT logfont;								//����ṹ�壬�ı��������
			ZeroMemory(&logfont,sizeof(LOGFONT));		//ZeroMemory����0�����һ���ڴ�����
			logfont.lfCharSet=GB2312_CHARSET;			//������������
			logfont.lfHeight=pText->GetTextSize();		//���������С
			logfont.lfWeight=700;						//����Ӵ�
			HFONT hFont=CreateFontIndirect(&logfont);	//����һ���ڽṹLOGFONT�ж������Ե��߼�����
			HGDIOBJ hOldFont=SelectObject(memDc,hFont);	//Ϊ�豸����ѡ�������壬����ԭʼ���塣
			TextOutA(memDc,pText->GetPos().x,pText->GetPos().y,pText->GetText(),strlen(pText->GetText()));
			SetTextColor(memDc,colorOld);				//�������ѡ��DC��ԭ����������ɫ
			SelectObject(memDc,hOldFont);				//�������ѡ��DC��ԭ��������
			DeleteObject(hFont);							//GDI����ʹ�����һ��Ҫ�ǵ�ɾ��
			break;
		}
		
	case text_twinkle:		//��˸����
		{
			//�ı�������ɫ
			UINT uiCurColor=pText->GetTextColor();	//��ǰ������ɫ
			UINT uiNextColor;
			/*
			UINT rValue=0xff & uiCurColor;			//��ȡ��ɫ�ص�R����ֵ
			UINT gValue=0xff & (uiCurColor>>8);		//��ȡ��ɫ�ص�G����ֵ
			UINT bValue=0xff & (uiCurColor>>16);		//��ȡ��ɫ�ص�B����ֵ
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

			//�������ֲ���ͬ text_normal
			SetBkMode(memDc,0);										//�������ֱ���͸����
			UINT colorOld=SetTextColor(memDc,pText->GetTextColor());	//colorOld��ԭʼmemDc�ϵ�������ɫ
			LOGFONT logfont;											//����ṹ�壬�ı��������
			ZeroMemory(&logfont,sizeof(LOGFONT));					//ZeroMemory����0�����һ���ڴ�����
			logfont.lfCharSet=GB2312_CHARSET;						//������������
			logfont.lfHeight=pText->GetTextSize();					//���������С
			logfont.lfWeight=700;									//����Ӵ�
			HFONT hFont=CreateFontIndirect(&logfont);				//����һ���ڽṹLOGFONT�ж������Ե��߼�����
			HGDIOBJ hOldFont=SelectObject(memDc,hFont);				//Ϊ�豸����ѡ�������壬����ԭʼ���塣
			TextOutA(memDc,pText->GetPos().x,pText->GetPos().y,pText->GetText(),strlen(pText->GetText()));
			SetTextColor(memDc,colorOld);							//�������ѡ��DC��ԭ����������ɫ
			SelectObject(memDc,hOldFont);							//�������ѡ��DC��ԭ��������
			DeleteObject(hFont);										//GDI����ʹ�����һ��Ҫ�ǵ�ɾ��
			break;
		}
	case text_scoll:			//��������
		{
			HDC dcTemp = CreateCompatibleDC(memDc);			//����memDc����ʱ����DC
			//��������λͼ��
			HBITMAP hBitmap=CreateCompatibleBitmap(memDc,pText->GetWidth(),pText->GetHeight());	
			SelectObject(dcTemp,hBitmap);					//������λͼѡ������DC��
			SetBkMode(dcTemp,0);								//�������ֱ���͸����

			//��memDc�ϱ�dcTemp��ס�Ĳ��ֻ��Ƶ�dcTemp��
			BitBlt(dcTemp,0,0,pText->GetWidth(),pText->GetHeight(),memDc,pText->GetPos().x,pText->GetPos().y,SRCCOPY);

			UINT colorOld=SetTextColor(dcTemp,pText->GetTextColor());	//colorOld��ԭʼmemDc�ϵ�������ɫ
			LOGFONT logfont;												//����ṹ�壬�ı��������
			ZeroMemory(&logfont,sizeof(LOGFONT));						//ZeroMemory����0�����һ���ڴ�����
			logfont.lfCharSet=GB2312_CHARSET;							//������������
			logfont.lfHeight=pText->GetTextSize();						//���������С
			logfont.lfWeight=700;										//����Ӵ�
			HFONT hFont=CreateFontIndirect(&logfont);					//����һ���ڽṹLOGFONT�ж������Ե��߼�����
			HGDIOBJ hOldFont=SelectObject(dcTemp,hFont);					//Ϊ�豸����ѡ�������壬����ԭʼ���塣

			//�����ַ���ռ�����ء�
			SIZE szText;
			GetTextExtentPoint32(dcTemp,pText->GetText(),strlen(pText->GetText()),&szText);

			//����2��3�����������dc��λ�ã������Ǵ��������ŵģ���������������ֵ����꣩
			TextOutA(dcTemp,pText->m_scollPos.x,pText->m_scollPos.y,pText->GetText(),strlen(pText->GetText()));
			
			//�����ֶ�������������������������
			if(pText->m_scollPos.x>=(int)pText->GetWidth())
			{//������ִ��ұ��Ƴ���,���������ֵ����ұߴ��ı��ؼ�������߽���
				pText->m_scollPos.x=-szText.cx;
			}
			pText->m_scollPos.x+=5;	//ÿ�ζ�������5������
	
			
			SetTextColor(dcTemp,colorOld);				//�������ѡ��DC��ԭ����������ɫ
			SelectObject(dcTemp,hOldFont);				//�������ѡ��DC��ԭ��������
			DeleteObject(hFont);							//GDI����ʹ�����һ��Ҫ�ǵ�ɾ��
			//�ٰ�dcTemp���Ƶ�memDc�ϣ�
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
{//Ϊ�ĸ��ؼ�����¼�
	stEvent event;
	event.pUI=pUI;
	event.eventType=eventType;
	event.func=func;
	event.iParam=iParam;
	m_eventList.push_back(event);
}
cPicture* cUIManager::CreatePicture(LPCTSTR lpFilePath)
{//����ͼƬ�ؼ�
	//ͨ��·������ͼƬ���浽 m_hBitmap ��
	cPicture* pic=new cPicture();
	if(!pic->Create(lpFilePath))
	{
		delete pic;
		return NULL;
	}
	m_uiList.push_back(pic);		//��ͼƬ�ؼ�����ؼ��б�
	return pic;
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
		if (it->eventType != event_lbuttondown)
		{
			continue;
		}
		//�ж��Ƿ�����Ӧ�Ŀؼ�
		if(IsInRect(it->pUI->GetRect(),pt))
		{
			//���ûص�����
			it->func(it->iParam);
		}
		//����Ƿ��ڰ�Ŧ��ɨ�����
		CheckClickButton();
	}
};
cText* cUIManager::CreateText(LPCTSTR lpText,UINT uiColor,TextType textType)
{//�������ֿؼ�
	cText* pText=new cText();
	if(!pText->Create(lpText,uiColor,textType))
	{
		delete pText;
		return NULL;
	}
	m_uiList.push_back(pText);		//�����ֿؼ�����ؼ��б�
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
{//����Ƿ��ڰ�Ŧ�ϰ���
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
{//����Ƿ��Ƶ�BUTTON��
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
{//����Ƿ��BUTTON�ϵ���
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
{//��������¼���Ӧ����
	CheckOnButton();		//����Ƿ��Ƶ�BUTTON��
}
void cUIManager::OnLButtonUp()
{
	CheckButtonUp();		//����Ƿ��BUTTON�ϵ���
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
	if(m_hBitmap)
		DeleteObject(m_hBitmap);
}
BOOL cPicture::Create(LPCTSTR lpFilePath)
{
	SetLastError(0xff);
	HBITMAP hBitmap=(HBITMAP)LoadImage(NULL,lpFilePath,IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	int error=GetLastError();	//�鿴����ʧ�ܴ�����룬error=2ʱ��ʾ�Ҳ����ļ�
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



/***********************cText��*********************/
cText::cText()
{	
	m_type=type_text;		//�ؼ�����

	m_textSize=18;			//����Ĵ�С��18������12������
	m_width=500;				//���ֿؼ��Ŀ��
	m_height=50;				//���ֿؼ��ĸ߶�
	//���������λ��
	m_scollPos.x=0;			
	m_scollPos.y=0;	
}
cText::~cText()
{

}
bool cText::Create(LPCTSTR lpText,UINT uiColor,TextType textType)
{//�������ֿؼ�
	m_text=lpText;			//��������
	m_textColor=uiColor;		//������ɫ
	m_textType=textType;		//��������
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
{//��ÿؼ��Ŀ��
	return m_width;
}
void cText::SetWidth(UINT width)
{//���ÿؼ��Ŀ��
	m_width=width;
}
unsigned cText::GetHeight()
{//��ÿؼ��ĸ߶�
	return m_height;
}
void cText::SetHeight(UINT height)	
{//���ÿؼ��ĸ߶�
	m_height=height;
}
/***************************************************/



/***********************cButton��*********************/
cButton::cButton()
{
	m_hBitmapNormal=NULL;
	m_hBitmapMouseOn=NULL;
	m_hBitmapPushDown=NULL;
	m_status=status_normal;
	m_type=type_button;			//�ؼ�����
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
{//����һ��BUTTON
	//��������״̬��Ӧ������λͼ
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
{//����BUTTON��״̬��ȡ��Ӧ����ͼ
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
	//���λͼ�Ŀ��
	BITMAP bitinfo;
	GetObject(m_hBitmapNormal,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmWidth; 
}
unsigned cButton::GetHeight()
{
	//���λͼ�ĸ߶�
	BITMAP bitinfo;
	GetObject(m_hBitmapNormal,sizeof(BITMAP),&bitinfo);
	return bitinfo.bmHeight;
}
/*****************************************************/