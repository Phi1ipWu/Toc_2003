////////////////////////////////////////////////////////////////////////////
//
//  声明：此文件属于游戏创造传说的一部分，全部版权归属 COKUCO 工作室所有
//        如有侵权，COKUCO 工作室保留法律起诉权利
//
//  类名：CDDraw （C - Class  DDraw - DirectDraw）
//  模式：Singleton 唯一实例
//  描述：DirectDraw 封装类，可以非常方便的进行 DirectDraw 的操作
//        支持窗体和全屏两种模式，以及各种常见分辨率，还能很方便的
//        初始化 DirectDraw，初始化离屏面板，位块传送，换帧，
//        并且全屏最小化后自动进行面板的恢复。
//
//  作者：Kudoxinyiji
//  日期：2005-09-28
//
////////////////////////////////////////////////////////////////////////////



#ifndef _CDDRAW_H
#define _CDDRAW_H


//#define DD_SURFACEFILENAME ".\\Graphics\\Bg%03d.kkc"
//#define DD_SURFACEFILENAME  "G:\\OwnSrc\\Toc\\Output\\Graphics\\Bg%03d.kkc"
#define DD_SURFACEFILENAME  ".\\Graphics\\Bg%03d.kkc"
#define DD_COLORKEY         0xFF00FF


#include <Windows.h>
#include <Wingdi.h>
#include <ddraw.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxerr8.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Gdi32.lib")



namespace DxWrapper
{

    class CDDraw
    {
    public:
        static bool InitInstance();
        static void ExitInstance();
        inline static CDDraw* Instance()
		{	return m_pDDraw;	}

        //  DirectDraw 初始化
        bool Init(HWND hWnd, long nWidth, long nHeight, long nBPP, long nIsWindow = 1);

        //  DirectDraw 终止，对象的释放
        void Term();

        //  初始化离屏面板
        bool InitSurface(long nID = 0);

        //  终止离屏面板
        void TermSurface(long nID = 0);

        //  更新窗体框架（用于窗体模式）
        void UpdateBounds();

        //  位块传送
        bool Blt(long nNum, LPRECT lpRect, long nX, long nY);

        //  位块传送（专用于绘制地砖）
        bool BltTile(long nTileNum, long nTileX, long nTileY, long nX, long nY);

        //  位块传送（专用于绘制角色）
        bool BltChar(long nCharNum, long nFace,  long nStep,  long nX, long nY);

        //  对整个客户区域进行清空
        bool Clear(DWORD = 0);

        //  换帧
        bool Flip();

		//	绘制矩形
		bool DrawRectangle(RECT* pRect, long nFillColor, long nBorderColor = 0xFFFFFF);

        //  获得后面板板
        LPDIRECTDRAWSURFACE7 GetBackSurface();

		inline long GetBPP() {	return this->m_nBPP;  }

    private:

        //  默认构造函数
        CDDraw();

        //  析构函数
        ~CDDraw();

        //  对指定的离屏面板重新显示位图
        bool DrawBitmap(long nID);

        //  设置关键色，用于透明
        void SetColorKey(LPDIRECTDRAWSURFACE7 lpTileDDS, unsigned long dwGDIColor);

        //  初始化全屏显示模式
        bool InitFullScreenMode(HWND hWnd, long nWidth, long nHeight, long nBPP);

        //  初始化窗口显示模式
        bool InitWindowMode(HWND hWnd, long nWidth, long nHeight);

    private:

        //  主 DDraw 对象
        LPDIRECTDRAW7 m_lpDD;

        //  主面板
        LPDIRECTDRAWSURFACE7 m_lpDDSPrimary;

        //  后台面板
        LPDIRECTDRAWSURFACE7 m_lpDDSBack;

        //  离屏地砖面板
        LPDIRECTDRAWSURFACE7 m_lpTileDDS[100];

        //  主窗体句柄
        HWND m_hWnd;

        //  主窗体所在位置的矩形
        RECT m_rcWindow;

        //  分辨率（宽度）
        long m_nWidth;

        //  分辨率（高度）
        long m_nHeight;

        //  发色数
        long m_nBPP;

        //  背景色
        long m_nBgColor;

        //  显示模式 0 - 全屏  1 - 窗体
        long m_nWindowMode;

        //  CDDraw 类对象唯一实例
        static CDDraw* m_pDDraw;

    };

};


#endif

