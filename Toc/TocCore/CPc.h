//---------------------------------
//	类名：	CPc
//	描述：	玩家控制对象
//	模式：　单例
//---------------------------------


#ifndef _CPC_H
#define _CPC_H


#include "CGame.h"
#include "CChar.h"
#include "CSetting.h"
#include <windows.h>


namespace TocCore
{

	class CPc : public CChar
	{
	public:
		static bool InitInstance();
		static void ExitInstance();
		static CPc* Instance();

		void FreeDo();
		void Move(bool bValid);
		void SetTileNo(const long& nTileNo);
		POINT GetCheckPoint();

	private:
		CPc(CSetting* pSetting);
		~CPc();

		CSetting* m_pSetting;
		static CPc* m_pPcObj;

		DWORD m_dwSpeed;		//	角色行走的速度
		DWORD m_dwLastTick;		//	前一帧的计数
	};

};

#endif
