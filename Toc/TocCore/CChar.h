//---------------------------------
//	������	IChar
//	������	��Ϸ��ɫ�ӿ�
//	ģʽ��	��
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
		CChar(long nTileNo);				//	���캯��������1���زĿ��еı��
		~CChar();

		long GetTileNo();					//	��ý�ɫ��ͼ�ؿ��еı��
		void SetTileNo(const long& nNo);	//	���ý�ɫ��ͼ�ؿ��еı��
		RECT GetTileRect();					//	��ɫ��ͼ�ؿ��е�λ�ã���ͼ�õģ�
		RECT GetDrawRect();					//	��ͼ�ľ��Σ��ڳ������е�λ��
		RECT GetCharRect();					//	��ȡ��ɫ��ռ�ݵ����������ж��ܷ��ƶ�
		RECT GetNewCharRect();				//	��ý�ɫ�仯��δ���ж�������
		POINT GetDrawPos();					//	��ý�ɫ��ǰ���Ͻǵ�λ��

		bool IsRectValid(const RECT& rc);				//	�ж������뵱ǰ��ɫ�Ƿ����ص�
		bool IsPointValid(const POINT& pt);				//	�ж����Ƿ����ڵ�ǰ��ɫ�ķ�Χ��
		void SetDrawPos(const long& x, const long& y);	//	���ý�ɫ��ǰ���Ͻǵ�λ��

		void SetFace(const long& nFace);	//	���ý�ɫ����

		virtual void FreeDo();				//	�����麯�������ܰ�����
		virtual void Move(bool bValid);		//	�Ƿ�����ƶ���Ŀ���

	protected:
		POINT	m_ptPos;			//	��ͼʱ��ɫ�����Ͻ�
		POINT	m_ptNewPos;			//	���ܰ��������λ�ã�δ���ж��Ƿ�Ϸ���

		long	m_nTileNo;			//	ͼ������ͼ�ؿ�ı��
		long	m_nTextNo;			//	�Ի����ݵı�ţ������֣�
		long	m_nAttrib;			//	1 ����ռλ  0 ����ռλ
		long	m_nFace;			//	��ɫ�ķ����زĿ����б�ţ�
		long	m_nStep;			//	��ɫ�ĽŲ����زĿ����б�ţ�
		bool	m_bIsTalk;			//	�Ƿ���̸��
	};
};

#endif
