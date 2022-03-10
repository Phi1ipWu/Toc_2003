//---------------------------------
//	������	CChar
//	������	��Ϸ��ɫ�ӿ�
//	ģʽ��	��
//---------------------------------

#include "CChar.h"
#include <windows.h>
using namespace TocCore;


//	��ȡ��ǰ��ɫ��ͼ�ؿ��е�����
RECT CChar::GetTileRect()
{
	RECT r;

	r.top    = m_nFace * 48;
	r.left   = m_nStep * 32;
	r.bottom = r.top + 48;
	r.right  = r.left + 32;

	return r;
}

//	��ȡ��ǰ��ɫ�ڵ�ͼ��ռ�ݵ�����������ɫ�޷�ռ�ã�
RECT CChar::GetCharRect()
{
	RECT r;
	
	r.top    = m_ptPos.y + 24;
	r.bottom = m_ptPos.y + 48;
	r.left   = m_ptPos.x + 4;
	r.right  = m_ptPos.x + 28;

	return r;
}

//	��ý�ɫ��Ҫ��ͼ�Ĳ����ڳ����е�λ��
RECT CChar::GetDrawRect()
{
	RECT r;

	r.top    = m_ptPos.y;
	r.bottom = m_ptPos.y + 48;
	r.left   = m_ptPos.x;
	r.right  = m_ptPos.x + 32;

	return r;
}

//	��ȡ��ǰ��ɫ���ƶ���δ���ж��Ϸ�������
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

//	����ʱ���
void CChar::SetDrawPos(const long& x, const long& y)
{
	m_ptPos.x = x;
	m_ptPos.y = y;
}


//	��������
//	2005-09-24
void CChar::SetFace(const long& nFace)
{
	this->m_nFace = nFace & 3;
	this->m_nStep = 0;
}


//	�ж������Ƿ����ɫռ�ݷ�Χ���ص�
bool CChar::IsRectValid(const RECT& rc)
{
	//	��ɫ����λ�ÿ��Ա�������ɫռ��
	if(m_nAttrib != 0)
	{
		return true;
	}

	POINT pt;

	//	��ɫ���Ͻ��Ƿ�Ϸ�
	pt.x = rc.left;
	pt.y = rc.top;
	if(false == IsPointValid(pt)) return false;

	//	��ɫ���Ͻ��Ƿ�Ϸ�
	pt.x = rc.right;
	if(false == IsPointValid(pt)) return false;

	//	��ɫ���½��Ƿ�Ϸ�
	pt.y = rc.bottom;
	if(false == IsPointValid(pt)) return false;

	//	��ɫ���½��Ƿ�Ϸ�
	pt.x = rc.left;
	if(false == IsPointValid(pt)) return false;

	return true;
}

bool CChar::IsPointValid(const POINT& pt)
{
	//	��ȡ�Լ��Լ���λ��
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
