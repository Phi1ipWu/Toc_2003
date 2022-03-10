//---------------------------------
//	类名：	IChar
//	描述：	游戏角色接口
//	模式：	无
//---------------------------------

#ifndef _CCHAR_H
#define _CCHAR_H


#include "IDraw.h"
#include <windows.h>

#define FACE_UP     3
#define FACE_DOWN   0
#define FACE_LEFT   1
#define FACE_RIGHT  2



namespace TocCore
{

	class CChar
	{
	public:
		CChar(long nTileNo);				//	构造函数，参数1：素材库中的编号
		~CChar();

		long GetTileNo();					//	获得角色在图素库中的编号
		void SetTileNo(const long& nNo);	//	设置角色在图素库中的编号
		RECT GetTileRect();					//	角色在图素库中的位置（绘图用的）
		RECT GetDrawRect();					//	绘图的矩形，在场景类中的位置
		RECT GetCharRect();					//	获取角色所占据的区域，用于判定能否移动
		RECT GetNewCharRect();				//	获得角色变化后未经判定的区域
		POINT GetDrawPos();					//	获得角色当前左上角的位置

		bool IsRectValid(const RECT& rc);				//	判定矩形与当前角色是否有重叠
		bool IsPointValid(const POINT& pt);				//	判定点是否落在当前角色的范围内
		void SetDrawPos(const long& x, const long& y);	//	设置角色当前左上角的位置

		void SetFace(const long& nFace);	//	设置角色面向

		virtual void FreeDo();				//	基类虚函数：接受按键用
		virtual void Move(bool bValid);		//	是否可以移动到目标点

	protected:
		POINT	m_ptPos;			//	绘图时角色的左上角
		POINT	m_ptNewPos;			//	接受按键后的新位置（未经判定是否合法）

		long	m_nTileNo;			//	图素所在图素库的编号
		long	m_nTextNo;			//	对话内容的编号（分语种）
		long	m_nAttrib;			//	1 可以占位  0 不能占位
		long	m_nFace;			//	角色的方向（素材库中行编号）
		long	m_nStep;			//	角色的脚步（素材库中列编号）
		bool	m_bIsTalk;			//	是否在谈话
	};
};

#endif
