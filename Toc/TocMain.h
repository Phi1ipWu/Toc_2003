//---------------------------------
//	������	��Ϸ��������
//	ģʽ��	��
//---------------------------------


#ifndef _TOCMAIN_H
#define _TOCMAIN_H


#define  WS_MYSTYLE (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX)
#define  NAME "���촫˵   By: KudoXinyiji"

#define  ERRMSG(x) MessageBox(NULL, x, "����", MB_ICONERROR);


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



//	��Ϸ����ĳ�ʼ��
long GameInit(HWND hWnd);

//	��Ϸ�����
void GameClean();

//	������Ϣ����ʽ
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//	����
void ReportErrorMsg(DWORD wErrCode);

//	ȫ�ֱ���
bool     g_bActive	= false;


#endif

