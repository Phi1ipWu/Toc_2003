//---------------------------------
//	������	IEvent
//	������	��Ϸ��������ӿ�
//	ģʽ��	��
//---------------------------------


#ifndef _IEVENT_H
#define _IEVENT_H


namespace TocCore
{

	class IEvent
	{
	public:
		virtual void OnUp();		//	��
		virtual void OnDown();		//	��
		virtual void OnLeft();		//	��
		virtual void OnRight();		//	��
		virtual void OnCheck();		//	����
		virtual void OnEscape();	//	ȡ��
	};

};

#endif
