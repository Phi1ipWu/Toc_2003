//---------------------------------
//	������	CGround
//	������	�����ذ���
//	ģʽ��	����
//---------------------------------


#ifndef _CGROUND_H
#define _CGROUND_H


#include "IDraw.h"
#include "IEvent.h"

#include "CPc.h"
#include "CChar.h"
#include "..\DxWrapper\DDraw.h"
#include "..\..\Common\CReader.h"
#include "..\..\Common\TileFile.h"
#include <vector>
using namespace std;


namespace TocCore
{
//	typedef vector<CTile> TileArray;			//	��ש���У�һά��
//	typedef vector<TileArray> TileMatrix;		//	��ש���󣨶�ά��
	typedef vector<CChar> NpcArray;				//	����ҿ��ƽ�ɫ����

	class CPc;

	class CGround : public IDraw, 
					public IEvent
	{
	public:
		static bool InitInstance();
		static void ExitInstance();
		static CGround* Instance();

		bool ChangeByAnchor(const long& nAnchor);
		bool IsRectValid(const RECT& rect);		//	��ɫ�����ڳ������Ƿ�Ϸ�

		long GetAnchor();						//	��ý�ɫ��ê��
		bool SetGround(const long& nGroundID);	//	���ݱ�����ó���

		void OnIdle();				//	�����麯����
		void OnBlt();				//	�����麯����
		void RefreshTopLeft();		//	�������ǵ�λ��ȷ����ͼ�����Ͻ�

		void OnEscape();			//	�����麯��	����ESC������ʽ
		void OnCheck();				//	�����麯��	���¿ո������ʽ

	private:
		CGround(CPc* pPc);
		~CGround();

		bool DrawBox();

		static CGround* m_pGround;

//		TileMatrix m_tmTile;	//	�����еĵ�ש
		NpcArray   m_naChar;	//	�����е� NPC

		CPc* m_pPc;				//	��ҿ��ƽ�ɫ���������

		long m_nLineCnt;		//	ͼ�ص�����  32 x 32
		long m_nColumnCnt;		//	ͼ�ص�����  32 x 32

		long m_nScreenWidth;	//	��Ļ�Ŀ��
		long m_nScreenHeight;	//	��Ļ�ĸ߶�

		long m_nLeft;			//	��ͼ�ã��������϶��� x ����
		long m_nTop;			//	��ͼ�ã��������϶��� y ����

		CTileFile m_TileFile;
		GROUND  m_stGround;

		long m_isLock;		//	�Ƿ񽫽�ɫ����
		//bool m_bText;
		//char m_pStr[64];
	};
};


#endif

