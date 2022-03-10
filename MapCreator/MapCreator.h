
//  2005-09-09 修改成读二进制文件


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
#define WM_GETSELTILE   WM_USER + 100   //  获得选中的图素行号和列号
#define WM_GETSELSIZE   WM_USER + 101   //  获得选中的图素的长度和宽度
#define WM_CHANGETILE   WM_USER + 102   //  改变图素库的编号
#define WM_SETMAPSIZE   WM_USER + 103   //  设置地图的大小
#define WM_SETLAYER     WM_USER + 104   //  设置地图的层（取值范围 0 ~ 3）



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

long    g_nSelTile  = 0;        //  当前打开的素材库编号
long    g_nParam1   = 0;		//	参数1

BOOL    g_bSound    = FALSE;
BOOL    g_bDrawInfo = FALSE;	//	绘制是地砖属性，圆圈大叉


GROUND  g_stGround;     //  新的地图数据
TCHAR   g_szFileName[MAX_PATH] = {0};   //  打开的文件名
TCHAR   g_szCurrPath[MAX_PATH] = {0};   //	当前目录

#define TILEINFO_SIZE 400				//	地砖信息数据大小
#define ALLTILEINFO_SIZE 400 * 50		//	全部信息数据大小

BYTE* g_pTileInfo = 0;					//	地砖信息数据（映射到全部数据上）
BYTE  g_pAllTileInfo[ALLTILEINFO_SIZE] = {0};	//	全部地砖信息数据

#endif

