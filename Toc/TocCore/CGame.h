//---------------------------------
//	������	CGame
//	������	��Ϸ����Ҫ�¼�������
//	ģʽ��	����
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

		long m_nState;	//	1 ����	2 ��ʼ����	3 ս��
		long m_nErrCode;

		IEvent* m_pEvent;	//	�������¼��Ľӿ�
		IDraw*  m_pDraw;	//	�����ͼ�¼��Ľӿ�

		static CGame* m_pGame;
	};

};

#endif
