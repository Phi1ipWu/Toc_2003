//---------------------------------
//	类名：	IDraw
//	描述：	游戏绘图类接口
//	模式：	无
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

		virtual void OnIdle() = 0;	//	闲置时事件
		virtual void OnBlt() = 0;	//	绘图函式

/*
		virtual void OnUp();		//	上
		virtual void OnDown();		//	下
		virtual void OnLeft();		//	左
		virtual void OnRight();		//	右
		virtual void OnCheck();		//	调查
		virtual void OnEscape();	//	取消
*/
	};

};

#endif
