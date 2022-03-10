
//  2005-09-09 �޸ĳɶ��������ļ�


#ifndef _MAPCREATOR_H
#define _MAPCREATOR_H


#include "..\Common\TileFile.h"

#include <windows.h>
#include "resource.h"

#include <string>
#include <fstream>
using namespace std;

#pragma comment(lib, "winmm.lib")


#define IDG_TILE    1
#define IDG_MAP     2
#define WM_GETSELTILE   WM_USER + 100   //  ���ѡ�е�ͼ���кź��к�
#define WM_GETSELSIZE   WM_USER + 101   //  ���ѡ�е�ͼ�صĳ��ȺͿ��
#define WM_CHANGETILE   WM_USER + 102   //  �ı�ͼ�ؿ�ı��
#define WM_SETMAPSIZE   WM_USER + 103   //  ���õ�ͼ�Ĵ�С
#define WM_SETLAYER     WM_USER + 104   //  ���õ�ͼ�Ĳ㣨ȡֵ��Χ 0 ~ 3��



LRESULT CALLBACK MainWndProc   (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK TileWndProc   (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK MapWndProc    (HWND, UINT, WPARAM, LPARAM) ;
LRESULT CALLBACK MapSizeDlgProc(HWND, UINT, WPARAM, LPARAM);

VOID DrawRect(HWND, long, long, long = 1, long = 1);
VOID DrawInfo(HWND, long, long);
BOOL DropMapFile(HDROP, HWND, HWND);
BOOL LoadMapFile(HWND, HWND);
BOOL SaveMapFile();
VOID MapInfo(HWND);
VOID GetCurrPath();
BOOL LoadTileInfo();
BOOL SaveTileInfo();
//VOID SaveCsvFile();


TCHAR g_szAppName[]   = TEXT("MapCreator");
TCHAR g_szMapClass[]  = TEXT("MapWindow");
TCHAR g_szTileClass[] = TEXT("TileWindow");


HBITMAP g_hBmpImage = 0;
HBITMAP g_hBmpMask  = 0;

long    g_nSelTile  = 0;        //  ��ǰ�򿪵��زĿ���
long    g_nParam1   = 0;		//	����1

BOOL    g_bSound    = FALSE;
BOOL    g_bDrawInfo = FALSE;	//	�����ǵ�ש���ԣ�ԲȦ���


GROUND  g_stGround;     //  �µĵ�ͼ����
TCHAR   g_szFileName[MAX_PATH] = {0};   //  �򿪵��ļ���
TCHAR   g_szCurrPath[MAX_PATH] = {0};   //	��ǰĿ¼

#define TILEINFO_SIZE 400				//	��ש��Ϣ���ݴ�С
#define ALLTILEINFO_SIZE 400 * 50		//	ȫ����Ϣ���ݴ�С

BYTE* g_pTileInfo = 0;					//	��ש��Ϣ���ݣ�ӳ�䵽ȫ�������ϣ�
BYTE  g_pAllTileInfo[ALLTILEINFO_SIZE] = {0};	//	ȫ����ש��Ϣ����

#endif

