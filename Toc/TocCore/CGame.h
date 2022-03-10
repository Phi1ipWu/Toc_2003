//---------------------------------
//	类名：	CGame
//	描述：	游戏中主要事件处理类
//	模式：	单例
//---------------------------------


#ifndef _CGAME_H
#define _CGAME_H


#include "..\DxWrapper\DDraw.h"

#include "CPc.h"
#include "CGame.h"
#include "CSetting.h"
#include "CGround.h"
#include <windows.h>


namespace TocCore
{

	class CGame
	{
	public:
		static CGame* Instance();
		static bool InitInstance();
		static void ExitInstance();

		bool CallDraw();
		void CallKeyDown(DWORD dwKey);
		void SetState(long nState);
		void SetEventProc(IEvent* pEvent);

		bool IsKeyDown(DWORD dwKey);
		bool IsPause();
		void SetError();
		void Quit(long nErrCode = 0);
		void SetDraw(IDraw*);
		void CallPause();

		inline bool IsQuit() { return m_bIsQuit; }
/*
		const static long Ground = 1;
		const static long Start = 2;
		const static long War = 3;
*/
	private:
		CGame();
		~CGame();

	private:
		bool m_bIsPause;
		bool m_bIsQuit;

		long m_nState;	//	1 场景	2 开始界面	3 战斗
		long m_nErrCode;

		IEvent* m_pEvent;	//	处理按键事件的接口
		IDraw*  m_pDraw;	//	处理绘图事件的接口

		static CGame* m_pGame;
	};

};

#endif
