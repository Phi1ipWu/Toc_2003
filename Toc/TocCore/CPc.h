//---------------------------------
//	������	CPc
//	������	��ҿ��ƶ���
//	ģʽ��������
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

		DWORD m_dwSpeed;		//	��ɫ���ߵ��ٶ�
		DWORD m_dwLastTick;		//	ǰһ֡�ļ���
	};

};

#endif
