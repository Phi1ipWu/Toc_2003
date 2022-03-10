//---------------------------------
//	类名：	CChar
//	描述：	游戏角色接口
//	模式：	无
//---------------------------------

#include "CChar.h"
#include <windows.h>
using namespace TocCore;


//	获取当前角色在图素库中的区域
RECT CChar::GetTileRect()
{
	RECT r;

	r.top    = m_nFace * 48;
	r.left   = m_nStep * 32;
	r.bottom = r.top + 48;
	r.right  = r.left + 32;

	return r;
}

//	获取当前角色在地图中占据的区域（其它角色无法占用）
RECT CChar::GetCharRect()
{
	RECT r;
	
	r.top    = m_ptPos.y + 24;
	r.bottom = m_ptPos.y + 48;
	r.left   = m_ptPos.x + 4;
	r.right  = m_ptPos.x + 28;

	return r;
}

//	获得角色需要绘图的部分在场景中的位置
RECT CChar::GetDrawRect()
{
	RECT r;

	r.top    = m_ptPos.y;
	r.bottom = m_ptPos.y + 48;
	r.left   = m_ptPos.x;
	r.right  = m_ptPos.x + 32;

	return r;
}

//	获取当前角色新移动后未经判定合法的区域
RECT CChar::GetNewCharRect()
{
	RECT r;

	r.top    = m_ptNewPos.y + 24;
	r.bottom = m_ptNewPos.y + 48;
	r.left   = m_ptNewPos.x + 4;
	r.right  = m_ptNewPos.x + 28;

	return r;
}

long CChar::GetTileNo()
{
	return m_nTileNo;
}

void CChar::Move(bool bValid)
{
	if(bValid == true)
	{
		m_ptPos = m_ptNewPos;
	}
}

void CChar::FreeDo()
{
	m_nStep = GetTickCount() / 180 % 4;
	m_nFace = GetTickCount() / 2400 % 4;
}

POINT CChar::GetDrawPos()
{
	return m_ptPos;
}

//	测试时添加
void CChar::SetDrawPos(const long& x, const long& y)
{
	m_ptPos.x = x;
	m_ptPos.y = y;
}


//	设置面向
//	2005-09-24
void CChar::SetFace(const long& nFace)
{
	this->m_nFace = nFace & 3;
	this->m_nStep = 0;
}


//	判定矩形是否与角色占据范围有重叠
bool CChar::IsRectValid(const RECT& rc)
{
	//	角色所在位置可以被其它角色占用
	if(m_nAttrib != 0)
	{
		return true;
	}

	POINT pt;

	//	角色左上角是否合法
	pt.x = rc.left;
	pt.y = rc.top;
	if(false == IsPointValid(pt)) return false;

	//	角色右上角是否合法
	pt.x = rc.right;
	if(false == IsPointValid(pt)) return false;

	//	角色右下角是否合法
	pt.y = rc.bottom;
	if(false == IsPointValid(pt)) return false;

	//	角色左下角是否合法
	pt.x = rc.left;
	if(false == IsPointValid(pt)) return false;

	return true;
}

bool CChar::IsPointValid(const POINT& pt)
{
	//	获取自己自己的位置
	RECT r = GetCharRect();

	if(pt.x < r.right && pt.x >= r.left && pt.y < r.bottom && pt.y >= r.top)
	{
		return false;
	}

	return true;
}


CChar::CChar(long nTileNo)
{
	m_ptPos.x = 0;
	m_ptPos.y = 0;

	m_nTileNo = nTileNo;
	m_nFace = 0;
	m_nStep = 0;
	m_nAttrib = 0;
}

CChar::~CChar()
{}
