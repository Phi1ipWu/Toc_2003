//---------------------------------
//	������	IDraw
//	������	��Ϸ��ͼ��ӿ�
//	ģʽ��	��
//---------------------------------


#ifndef _IDRAW_H
#define _IDRAW_H


namespace TocCore
{

	class IDraw
	{
	public:

		virtual void OnSetFocus();
		virtual void OnKillFocus();

		virtual void OnIdle() = 0;	//	����ʱ�¼�
		virtual void OnBlt() = 0;	//	��ͼ��ʽ

/*
		virtual void OnUp();		//	��
		virtual void OnDown();		//	��
		virtual void OnLeft();		//	��
		virtual void OnRight();		//	��
		virtual void OnCheck();		//	����
		virtual void OnEscape();	//	ȡ��
*/
	};

};

#endif
