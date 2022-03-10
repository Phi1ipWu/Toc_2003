//---------------------------------
//	类名：	CGame
//	描述：	游戏中主要事件处理类
//	模式：	单例
//---------------------------------

#include "CGame.h"
using namespace TocCore;
using namespace DxWrapper;


CGame* CGame::m_pGame = 0;

void CGame::Quit(long nErrCode)
{
	m_nErrCode = nErrCode;
	m_bIsQuit  = true;
}

bool CGame::IsPause()
{	return m_bIsPause;	}

//inline bool CGame::IsQuit()
//{	return m_bIsQuit;	}

inline CGame* CGame::Instance()
{
	return m_pGame;
}

bool CGame::InitInstance()
{
	if(m_pGame == 0)
	{
		m_pGame = new CGame();
	}

	return true;
}

/*
void CGame::SetState(long nState)
{
	m_nState = CGame::Ground;
	m_bIsQuit = ! CGround::Instance()->SetGround(1);
	CDDraw::Instance()->SetDraw(CGround::Instance());
}
*/

bool CGame::IsKeyDown(DWORD dwKey)
{
	return ((GetAsyncKeyState(dwKey) & 0x8000) == 0x8000);
}


void CGame::CallKeyDown(DWORD dwKey)
{
	CSetting* pSetting = CSetting::Instance();
	
	if(0 == pSetting || 0 == m_pEvent) return;


	if(dwKey == pSetting->GetKeyLeft())
	{
		m_pEvent->OnLeft();
		return;
	}

	if(dwKey == pSetting->GetKeyRight())
	{
		m_pEvent->OnRight();
		return;
	}

	if(dwKey == pSetting->GetKeyUp())
	{
		m_pEvent->OnUp();
		return;
	}

	if(dwKey == pSetting->GetKeyDown())
	{
		m_pEvent->OnDown();
		return;
	}

	if(dwKey == pSetting->GetKeyCheck())
	{
/*		POINT pt = CPc::Instance()->GetDrawPos();
		char buffer[255] = {0};
		sprintf(buffer, "x = %d\ny = %d", pt.x, pt.y);
		MessageBox(0, buffer, "Msg", MB_ICONINFORMATION);
*/
		m_pEvent->OnCheck();
		return;
	}

	if(dwKey == pSetting->GetKeyEscape())
	{
		//this->m_bIsQuit = true;
		

		m_pEvent->OnEscape();
		return;
	}
}

void CGame::SetEventProc(IEvent* pEvent)
{
	if(pEvent)
	{
		m_pEvent = pEvent;
	}
	else
	{
		//MessageBox(0, "CGame.cpp  Ln116", 0, MB_OK);
		CGame::Instance()->Quit();
	}
}


CGame::CGame()
{
	m_pDraw    = 0;
	m_pEvent   = 0;

	m_bIsQuit  = false;
	m_bIsPause = false;
}

CGame::~CGame()
{}

void CGame::ExitInstance()
{
	if(0 != m_pGame)
	{
		delete m_pGame;
		m_pGame = 0;
	}
}


bool CGame::CallDraw()
{
	CDDraw::Instance()->Clear();

	if(m_pDraw)
	{
		m_pDraw->OnIdle();
		m_pDraw->OnBlt();
	}

	return CDDraw::Instance()->Flip();
}


void CGame::SetDraw(IDraw* pDraw)
{
	//	2005-09-30 隐藏 N 久的低级失误被修正
	if(pDraw)
	{
		m_pDraw = pDraw;
	}
	else
	{
		//MessageBox(0, "CGame.cpp  Ln165", 0, MB_OK);
		CGame::Instance()->Quit();
	}
}
