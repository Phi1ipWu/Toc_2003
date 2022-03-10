//---------------------------------
//	类名：	IEvent
//	描述：	游戏按键处理接口
//	模式：	无
//---------------------------------


#ifndef _IEVENT_H
#define _IEVENT_H


namespace TocCore
{

	class IEvent
	{
	public:
		virtual void OnUp();		//	上
		virtual void OnDown();		//	下
		virtual void OnLeft();		//	左
		virtual void OnRight();		//	右
		virtual void OnCheck();		//	调查
		virtual void OnEscape();	//	取消
	};

};

#endif
