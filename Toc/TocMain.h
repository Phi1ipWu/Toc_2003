//---------------------------------
//	描述：	游戏的主程序
//	模式：	无
//---------------------------------


#ifndef _TOCMAIN_H
#define _TOCMAIN_H


#define  WS_MYSTYLE (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX)
#define  NAME "创造传说   By: KudoXinyiji"

#define  ERRMSG(x) MessageBox(NULL, x, "错误", MB_ICONERROR);


#include <windows.h>
#include <ddraw.h>
#include <mmsystem.h>
#include <dmusici.h>

#include "Res\resource.h"

#include "DxWrapper\DDraw.h"
#include "DxWrapper\DInput.h"
#include "DxWrapper\DMusic.h"

#include "TocCore\CChar.h"
#include "TocCore\CPc.h"
#include "TocCore\CSetting.h"
#include "TocCore\CGame.h"
#include "TocCore\CGround.h"



//	游戏对象的初始化
long GameInit(HWND hWnd);

//	游戏的清除
void GameClean();

//	窗体消息处理函式
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//	报错
void ReportErrorMsg(DWORD wErrCode);

//	全局变量
bool     g_bActive	= false;


#endif

