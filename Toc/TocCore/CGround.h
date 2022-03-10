//---------------------------------
//	类名：	CGround
//	描述：	场景地板类
//	模式：	单例
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
//	typedef vector<CTile> TileArray;			//	地砖序列（一维）
//	typedef vector<TileArray> TileMatrix;		//	地砖矩阵（二维）
	typedef vector<CChar> NpcArray;				//	非玩家控制角色序列

	class CPc;

	class CGround : public IDraw, 
					public IEvent
	{
	public:
		static bool InitInstance();
		static void ExitInstance();
		static CGround* Instance();

		bool ChangeByAnchor(const long& nAnchor);
		bool IsRectValid(const RECT& rect);		//	角色区域在场景中是否合法

		long GetAnchor();						//	获得角色的锚点
		bool SetGround(const long& nGroundID);	//	根据编号设置场景

		void OnIdle();				//	基类虚函数，
		void OnBlt();				//	基类虚函数，
		void RefreshTopLeft();		//	根据主角的位置确定绘图的左上角

		void OnEscape();			//	基类虚函数	按下ESC键处理函式
		void OnCheck();				//	基类虚函数	按下空格键处理函式

	private:
		CGround(CPc* pPc);
		~CGround();

		bool DrawBox();

		static CGround* m_pGround;

//		TileMatrix m_tmTile;	//	场景中的地砖
		NpcArray   m_naChar;	//	场景中的 NPC

		CPc* m_pPc;				//	玩家控制角色对象的引用

		long m_nLineCnt;		//	图素的行数  32 x 32
		long m_nColumnCnt;		//	图素的列数  32 x 32

		long m_nScreenWidth;	//	屏幕的宽度
		long m_nScreenHeight;	//	屏幕的高度

		long m_nLeft;			//	绘图用：场景左上顶点 x 坐标
		long m_nTop;			//	绘图用：场景左上顶点 y 坐标

		CTileFile m_TileFile;
		GROUND  m_stGround;

		long m_isLock;		//	是否将角色锁定
		//bool m_bText;
		//char m_pStr[64];
	};
};


#endif

