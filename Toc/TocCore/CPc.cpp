//---------------------------------
//	������	CPc
//	������	��ҿ��ƶ���
//	ģʽ��	����ģʽ
//---------------------------------


#include "CPc.h"
#include "..\DxWrapper\DInput.h"

using namespace TocCore;
using namespace DxWrapper;

//#define CPC_ASYNC_MOVE


CPc* CPc::m_pPcObj = 0;

bool CPc::InitInstance()
{
	CSetting* pSetting = CSetting::Instance();

	if(pSetting)
	{
		CPc::m_pPcObj = new CPc(pSetting);
		return true;
	}
	else
	{
		return false;
	}
}

void CPc::ExitInstance()
{
	if(m_pPcObj)
	{
		delete m_pPcObj;
		m_pPcObj = 0;
	}
}

CPc* CPc::Instance()
{
	return m_pPcObj;
}


#ifndef CPC_ASYNC_MOVE

//	�޶��ٶ�
void CPc::FreeDo()
{
	DWORD dwCurrTick = timeGetTime();
	DWORD dwTickDiff = dwCurrTick - m_dwLastTick;

	if(0 == dwTickDiff)
	{
		return;
	}

	m_dwLastTick = dwCurrTick;
	double fTickDiff = ((double)dwTickDiff) / 1000.0f;

	CGame* pGame = CGame::Instance();
	CDInput* pInput = CDInput::Instance();

	long x = 0;
	long y = 0;

	if(0 == m_pSetting || 0 == pGame || 0 == pInput)
	{
		//MessageBox(0, "CPc.cpp  Ln74", 0, MB_OK);
		pGame->Quit();
		return;
	}

	m_ptNewPos = m_ptPos;

	m_dwSpeed = pInput->IsKeyDown(DIK_LSHIFT) ? 320 : 180;

	if(pInput->UpdateKeyData() && pInput->UpdateJoyData())
	{
		if(pInput->GetJoyAxisY() < 0 || pInput->IsKeyDown(m_pSetting->GetKeyUp(TOC_DIKEY)))
		{
			y -= long(m_dwSpeed * fTickDiff);
		}
		if(pInput->GetJoyAxisY() > 0 || pInput->IsKeyDown(m_pSetting->GetKeyDown(TOC_DIKEY)))
		{
			y += long(m_dwSpeed * fTickDiff);
		}
		if(pInput->GetJoyAxisX() < 0 || pInput->IsKeyDown(m_pSetting->GetKeyLeft(TOC_DIKEY)))
		{
			x -= long(m_dwSpeed * fTickDiff);
		}
		if(pInput->GetJoyAxisX() > 0 || pInput->IsKeyDown(m_pSetting->GetKeyRight(TOC_DIKEY)))
		{
			x += long(m_dwSpeed * fTickDiff);
		}
	}
	else
	{
		//MessageBox(0, "CPc.cpp  Ln102", 0, MB_OK);
		CGame::Instance()->Quit();
	}

	m_ptNewPos.x += x;
	m_ptNewPos.y += y;


	if(y < 0)
	{
		m_nFace = 3;
	}
	else if(y > 0)
	{
		m_nFace = 0;
	}

	if(x < 0)
	{
		m_nFace = 1;
	}
	else if(x > 0)
	{
		m_nFace = 2;
	}
}

#else if

//	�����٣����⿿Ӳ������
void CPc::FreeDo()
{
	CGame* pGame = CGame::Instance();
	CDInput* pInput = CDInput::Instance();

	long x = 0;
	long y = 0;

	if(0 == m_pSetting || 0 == pGame || 0 == pInput)
	{
		//MessageBox(hWnd, "CPc.cpp  Ln142", 0, MB_OK);
		pGame->Quit();
		return;
	}

	m_ptNewPos = m_ptPos;
	m_nStep = 0;

	if(CDInput::Instance()->UpdateKeyData() && CDInput::Instance()->UpdateJoyData())
	{
		if(pInput->GetJoyAxisY() < 0 || pInput->IsKeyDown(m_pSetting->GetKeyUp(TOC_DIKEY)))
		{
			--y;
		}
		if(pInput->GetJoyAxisY() > 0 || pInput->IsKeyDown(m_pSetting->GetKeyDown(TOC_DIKEY)))
		{
			++y;
		}
		if(pInput->GetJoyAxisX() < 0 || pInput->IsKeyDown(m_pSetting->GetKeyLeft(TOC_DIKEY)))
		{
			--x;
		}
		if(pInput->GetJoyAxisX() > 0 || pInput->IsKeyDown(m_pSetting->GetKeyRight(TOC_DIKEY)))
		{
			++x;
		}
	}
	else
	{
		CGame::Instance()->Quit();
	}
//
//
//	//	API ����ð���״̬�ѷ���
//	if(pGame->IsKeyDown(m_pSetting->GetKeyUp()))
//	{
//		--y;
//	}
//
//	if(pGame->IsKeyDown(m_pSetting->GetKeyDown()))
//	{
//		++y;
//	}
//
//	if(pGame->IsKeyDown(m_pSetting->GetKeyLeft()))
//	{
//		--x;
//	}
//
//	if(pGame->IsKeyDown(m_pSetting->GetKeyRight()))
//	{
//		++x;
//	}
//
//
//
//	//	���ٹ��ܣ���ʱע��
//	if(pGame->IsKeyDown(VK_SHIFT))
//	{
//		x = x << 1;
//		y = y << 1;
//	}
//

	m_ptNewPos.x += x;
	m_ptNewPos.y += y;


	if(y < 0)
	{
		m_nFace = 3;
	}
	else if(y > 0)
	{
		m_nFace = 0;
	}

	if(x < 0)
	{
		m_nFace = 1;
	}
	else if(x > 0)
	{
		m_nFace = 2;
	}
}

#endif


void CPc::Move(bool bValid)
{
	if(false == bValid)
	{
		//	�������ڳ������Ϸ������ƶ�������Ϊվ����������
		m_nStep = 0;
		return;
	}
	else
	{
		if(m_ptNewPos.x != m_ptPos.x || m_ptNewPos.y != m_ptPos.y)
		{
			//	��������ԭ�����겻һ������ɫ���߶���

			//m_nStep = GetTickCount() / 120 % 4;
			m_nStep = (timeGetTime() >> 7) & 3;
			m_ptPos = m_ptNewPos;
		}
		else
		{
			//	��������ԭ��������ͬ����ɫû���߶���
			m_nStep = 0;
		}
	}
}

POINT CPc::GetCheckPoint()
{
	RECT r = GetCharRect();
	POINT ptPos;
	//ptPos.x = r.right - (r.right - r.left) / 2;
	//ptPos.y = r.bottom - (r.bottom - r.top) / 2;
	ptPos.x = r.right - ((r.right - r.left)  >> 1);
	ptPos.y = r.bottom - ((r.bottom - r.top) >> 1);

	switch(m_nFace)
	{
	case 0:
		ptPos.y += 16;
		break;
	case 1:
		ptPos.x -= 16;
		break;
	case 2:
		ptPos.x += 16;
		break;
	case 3:
		ptPos.y -= 16;
		break;
	default:
		break;
	}

	return ptPos;
}


void CPc::SetTileNo(const long& nTileNo)
{
	CDDraw::Instance()->InitSurface(51);
	m_nTileNo = nTileNo;
}

CPc::CPc(CSetting* pSetting) : CChar(51)
{
	m_ptPos.x = 0;
	m_ptPos.y = 0;
	m_nFace   = 0;
	m_nStep   = 0;

	m_dwLastTick = timeGetTime();
	m_dwSpeed    = 180;		//	��ɫ�ƶ��ٶ�   Pixel/Sec

	m_pSetting = pSetting;
}

CPc::~CPc()
{

}
