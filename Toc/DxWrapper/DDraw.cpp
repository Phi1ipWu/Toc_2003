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



#include <stdio.h>
#include <windowsx.h>
#include <winerror.h>

#include "DDraw.h"
using namespace DxWrapper;


//  CDDraw 唯一实例初始化
CDDraw* CDDraw::m_pDDraw = 0;



//-----------------------------------------------------
//  描述： CDDraw 唯一实例的内存分配（静态）
//  日期： 2005/05/28
//-----------------------------------------------------
bool CDDraw::InitInstance()
{
    if(NULL == m_pDDraw)
    {
        m_pDDraw = new CDDraw();
    }
    
    return true;
}




//-----------------------------------------------------
//  描述： CDDraw 唯一实例的内存回收（静态）
//  日期： 2005/05/28
//-----------------------------------------------------
void CDDraw::ExitInstance()
{
    if(m_pDDraw)
    {
        delete m_pDDraw;
        m_pDDraw = 0;
    }
}




//-----------------------------------------------------
//  描述： 获得 CDDraw 唯一实例（静态）
//  日期： 2005/05/28
//  返回值： CDDraw 唯一实例的指针
//-----------------------------------------------------
//inline CDDraw* CDDraw::Instance()
//{
//    return m_pDDraw;
//}




//-----------------------------------------------------
//  描述： CDInput 类的构造函数
//  日期： 2005/05/28
//-----------------------------------------------------
CDDraw::CDDraw()
{
    //  设置默认 640 x 480 @ 32 位色
    m_nWidth   = 0;
    m_nHeight  = 0;
    m_nBPP     = 0;
    m_hWnd     = 0;

    m_lpDD         = 0;
    m_lpDDSPrimary = 0;
    m_lpDDSBack    = 0;

    memset(m_lpTileDDS, 0, sizeof(m_lpTileDDS));

/*    for(long i = 0; i <= 50; ++i)
    {
        m_lpTileDDS[i] = 0;
    }*/
}




//-----------------------------------------------------
//  描述： CDInput 类的析构函数
//  日期： 2005/05/28
//-----------------------------------------------------
CDDraw::~CDDraw()
{
    TermSurface();
    Term();
}




//-----------------------------------------------------
//  描述： 设置分辨率
//  日期： 2005/09/27
//  参数： hWnd 主窗体句柄
//-----------------------------------------------------
bool CDDraw::Init(HWND hWnd, long nWidth, long nHeight, long nBPP, long nIsWindow)
{
    //  如果 DirectDraw 对象已经初始化
    //  则先将已经初始化的对象销毁
    Term();

    if(nIsWindow)
    {
        //  窗体模式
        return InitWindowMode(hWnd, nWidth, nHeight);
    }
    else
    {
        //  全屏模式
        return InitFullScreenMode(hWnd, nWidth, nHeight, nBPP);
    }

	return true;
}




//-----------------------------------------------------
//  描述： 释放 DirectDraw 对象
//  日期： 2005/05/28
//-----------------------------------------------------
void CDDraw::Term()
{
    //  释放后台面板对象
    if(0 != m_lpDDSBack)
    {
        m_lpDDSBack->Release();
        m_lpDDSBack = 0;
    }

    //  释放前台面板对象
    if(0 != m_lpDDSPrimary)
    {
        m_lpDDSPrimary->Release();
        m_lpDDSPrimary = 0;
    }

    //  释放主 DirectDraw 对象
    //  在释放前先将协作模式设为普通
    if(0 != m_lpDD)
    {
        m_lpDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
        m_lpDD->Release();
        m_lpDD = 0;
    }

	m_nWidth  = 0;
	m_nHeight = 0;
	m_nBPP    = 0;
    m_hWnd    = 0;
}




//-----------------------------------------------------
//  描述： 初始化离屏面板
//  日期： 2005/09/27
//  参数： long nID  [in]  需要初始化的离屏面板编号
//  返回值： true - 成功   false - 失败
//
//  注：必须之前已经调用了 Init() 创建 DDRAW 对象
//-----------------------------------------------------
bool CDDraw::InitSurface(long nID)
{
    //  先判定 DirectDraw 主对象是否已经初始化
    if(0 == m_lpDD)
    {
        //  DirectDraw 主对象未初始化
        return false;
    }
    else if(0 != nID && m_lpTileDDS[nID])
    {
        //  当时前的面板已经初始化过
        //  无需再次初始化
        return true;
    }


    //  如果已经初始化离屏面板则先销毁
    //TermSurface(nID);

    //  初始化全部离屏地砖面板
    if(0 == nID)
    {
        for(long i = 1; i <= 51; ++ i)
        {
            if(false == DrawBitmap(i))
            {
                TermSurface();
                return false;
            }
        }

        return true;
    }

    //  初始化指定编号的单个面板
    return DrawBitmap(nID);
}




//---------------------------------------------------------
//  描述： 对指定的离屏面板显示位图
//  日期： 2005/09/27
//  参数： long nID  [in]  需要重新显示位图的离屏面板编号
//  返回值： true - 成功   false - 失败
//---------------------------------------------------------
bool CDDraw::DrawBitmap(long nID)
{
    HBITMAP hBitmap = 0;
    BITMAP  bm;

    HDC hdc = 0;
    HDC hdcImage = 0;
    DDSURFACEDESC2 ddsd;
    //char szBufferA[MAX_PATH] = {0};
    const long i = nID;

    TCHAR szBuffer[MAX_PATH] = {0};
    wsprintf(szBuffer, DD_SURFACEFILENAME, i);

    //  文件 BMP 文件中读取
    //sprintf(szBufferA, DD_SURFACEFILENAME, i);
    hBitmap = (HBITMAP)LoadImage(NULL, szBuffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    //  如果读文件失败
    if(0 == hBitmap)
    {
        //TermSurface();
        return false;
    }

    //  获得 BMP 文件的一些信息
    GetObject(hBitmap, sizeof(bm), &bm);

    //  如果未创建离屏面板，则创建之
    if(0 == m_lpTileDDS[i])
    {
        //  创建离屏面板
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        if(this->m_nWindowMode) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        ddsd.dwWidth = bm.bmWidth;      //  离屏面板和 BMP 的宽度一样
        ddsd.dwHeight = bm.bmHeight;    //  离屏面板和 BMP 的高度一样

        m_lpDD->CreateSurface(&ddsd, &m_lpTileDDS[i], NULL);
    }

    //  如果创建离屏面板失败或者读文件失败
    if(0 == m_lpTileDDS[i] || 0 == hBitmap)
    {
        return false;
    }

    m_lpTileDDS[i]->Restore();

    hdcImage = CreateCompatibleDC(NULL);
    SelectObject(hdcImage, hBitmap);

    //  将位图位块传送进离屏面板
    m_lpTileDDS[i]->GetDC(&hdc);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, SRCCOPY);
    m_lpTileDDS[i]->ReleaseDC(hdc);
    DeleteObject(hBitmap);
    DeleteDC(hdcImage);

    //  设置关键色透明
    SetColorKey(m_lpTileDDS[i], DD_COLORKEY);

    return true;
}




//-----------------------------------------------------
//  描述： 终止离屏面板
//  日期： 2005/09/27
//  注：   必须之前已经调用了 Init()创建 DDRAW 对象
//-----------------------------------------------------
void CDDraw::TermSurface(long nID)
{
    if(0 != nID)
    {
        //  终止特定编号(nID)的离屏面板

        if(m_lpTileDDS[nID])
        {
            m_lpTileDDS[nID]->Release();
            m_lpTileDDS[nID] = 0;
        }
    }
    else
    {
        //  终止全部的离屏面板

        for(long i = 1; i < 51; ++ i)
        {
            if(0 != m_lpTileDDS[i])
            {
                m_lpTileDDS[i]->Release();
                m_lpTileDDS[i] = 0;
            }
        }
    }
}




//-----------------------------------------------------
//  描述： 变更窗体大小或位置时调用更新
//  日期： 2005/06/07
//  返回值： 
//-----------------------------------------------------
void CDDraw::UpdateBounds()
{
    if(m_nWindowMode)
    {
        GetClientRect(m_hWnd, &m_rcWindow);
        ClientToScreen(m_hWnd, (LPPOINT)&m_rcWindow);
        ClientToScreen(m_hWnd, (LPPOINT)&m_rcWindow + 1);
    }
    else
    {
        SetRect(&m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                 GetSystemMetrics(SM_CYSCREEN));
    }
}





//-----------------------------------------------------
//  描述： 后台面板清空
//  日期： 2005/06/06
//  参数： dwColor 　清空后的颜色
//  返回值： true - 成功   false - 失败
//-----------------------------------------------------
bool CDDraw::Clear(DWORD dwColor)
{
    if(0 != m_lpDDSBack)
    {
        DDBLTFX ddbltfx;
        ZeroMemory(&ddbltfx,  sizeof(ddbltfx));
        ddbltfx.dwSize      = sizeof(ddbltfx);
        ddbltfx.dwFillColor = dwColor;

        //  将指定的颜色充满后台面板
        return SUCCEEDED(m_lpDDSBack->Blt(0, 0, 0, DDBLT_COLORFILL, &ddbltfx));
    }

    return false;
}




//-----------------------------------------------------
//  描述： 换帧
//  日期： 2005/09/27
//  返回值： true - 成功   false - 失败
//-----------------------------------------------------
bool CDDraw::Flip()
{
    HRESULT hr;

    if(0 != m_lpDDSPrimary && 0 != m_lpDDSBack)
    {
        for(;;)
        {
            if(m_nWindowMode)
            {
                //  窗体模式
                hr = m_lpDDSPrimary->Blt(&m_rcWindow, m_lpDDSBack,
                                            NULL, DDBLT_WAIT, NULL);
            }
            else
            {
                //  全屏模式
                hr = m_lpDDSPrimary->Flip(0, 0);
            }

            //  如果丢帧，则尝试恢复全部的面板
            if(hr == DDERR_SURFACELOST)
            {
                if(SUCCEEDED(m_lpDD->RestoreAllSurfaces()))
                {
                    for(long i = 0; i <= sizeof(m_lpTileDDS); ++i)
                    {
                        if(m_lpTileDDS[i])
                        {
                            //  重新绘制
                            DrawBitmap(i);
                        }
                    }

                    return true;
                }
                else
                {
                    return false;
                }
            }

            //  正在绘制
            if(hr != DDERR_WASSTILLDRAWING)
            {
                return SUCCEEDED(hr);
            }
        }
    }

    return false;

}




//---------------------------------------------------------
//  描述： 将离屏表面以位块拷贝到后台表面（只限地砖）
//  日期： 2005/06/06
//  参数： long nNum     [in]  图素素材编号
//         LPRECT lpRect [in]  要复制的离屏表面上的区域
//         long nX       [in]  屏幕上 X 轴位置（左上角）
//         long nY       [in]  屏幕上 Y 轴位置（左上角）
//  返回值： true - 成功   false - 失败
//---------------------------------------------------------
bool CDDraw::Blt(long nNum, LPRECT lpRect, long nX, long nY)
{
    long nRcWidth  = lpRect->right  - lpRect->left;
    long nRcHeight = lpRect->bottom - lpRect->top;

    if(0 != m_lpDDSBack && 0 != m_lpTileDDS[nNum])
    {
        if(nX + nRcWidth <= 0 || nY + nRcHeight <= 0 || 
           nX >= m_nWidth || nY >= m_nHeight)
        {
            //  所需绘制的素材完全超出屏幕范围
            //  直接返回，无需绘制，可以提高效率咯　：）

            return true;
        }

        if(0 != lpRect)
        {
            if(nX < 0 && nRcWidth > -nX)
            {
                //  超过左边界
                lpRect->left -= nX;
                nX = 0;
            }

            if(nX + nRcWidth > m_nWidth)
            {
                //  超出右边界
                lpRect->right -= nRcWidth + nX - m_nWidth;
            }

            if(nY < 0 && nRcHeight > -nY)
            {
                //  超出上边界
                lpRect->top -= nY;
                nY = 0;
            }

            if(nY + nRcHeight >= m_nHeight)
            {
                //  超出下边界
                lpRect->bottom -= nRcHeight + nY - m_nHeight;
            }
        }

        //  位块传送指定矩形到后台面板上
        return SUCCEEDED(m_lpDDSBack->BltFast(nX, nY, m_lpTileDDS[nNum], lpRect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT));
    }

    return false;
}




//---------------------------------------------------------
//  描述： 将离屏表面以位块拷贝到后台表面（只限地砖）
//  日期： 2005/06/06
//  参数： long nTileNum [in]  图素素材编号
//         long nTileX   [in]  图素素材的列号
//         long nTileY   [in]  图素素材的行号
//         long nX       [in]  屏幕上 X 轴位置（左上角）
//         long nY       [in]  屏幕上 Y 轴位置（左上角）
//  返回值： true - 成功   false - 失败
//
//  注： 地砖素材是 32 * 32 大小
//---------------------------------------------------------
bool CDDraw::BltTile(long nTileNum, long nTileX, long nTileY, long nX, long nY)
{
/*
    HDC hdc;
    if(DD_OK == m_lpDDSBack->GetDC(&hdc))
    {
        char szBufferA[255];
        sprintf(szBufferA, "Pos: [%d, %d]", nX, nY);

        TextOut(hdc, 500, 500, szBufferA, (int)strlen(szBufferA));
        m_lpDDSBack->ReleaseDC(hdc);
    }
*/
    //  先判定后台面板和指定的离屏面板是否已经初始化
    if(0 != m_lpDDSBack && 0 != m_lpTileDDS[nTileNum])
    {
        RECT rc;
        SetRect(&rc, nTileX << 5, nTileY << 5, (nTileX + 1)<< 5, (nTileY + 1)<< 5);

        if(nX + 32 <= 0 || nY + 32 <= 0 || 
           nX >= m_nWidth || nY >= m_nHeight)
        {
            //  所需绘制的素材完全超出屏幕范围
            //  直接返回，无需绘制，可以提高效率咯　：）

            return true;
        }

        if(nX < 0)
        {
            //  超过左边界
            rc.left -= nX;
            nX = 0;
        }
        else if(nX + 32 >= m_nWidth)
        {
            //  超过右边界
            rc.right -= 32 + nX - m_nWidth;
        }

        if(nY < 0)
        {
            //  超过上边界
            rc.top -= nY;
            nY = 0;
        }
        else if(nY + 32 >= m_nHeight)
        {
            //  超过下边界
            rc.bottom -= 32 + nY - m_nHeight;
        }

        return SUCCEEDED(m_lpDDSBack->BltFast(nX, nY, m_lpTileDDS[nTileNum], &rc, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT));
    }

    return false;
}




//-----------------------------------------------------
//  描述： 获得后台面板的引用
//  日期： 2005/06/07
//  返回值： 后台面板的引用
//-----------------------------------------------------
LPDIRECTDRAWSURFACE7 CDDraw::GetBackSurface()
{
    return m_lpDDSBack;
}




//-----------------------------------------------------
//  描述： 设置离屏面板的关键色
//  日期： 2005-09-27
//  参数： LPDIRECTDRAWSURFACE7 lpTileDDS  [in]
//         需要设置关键色的离屏面板
//         unsigned long dwGDIColor        [in]
//         需要透明的关键色
//
//  返回值： true - 成功   false - 失败
//-----------------------------------------------------
void CDDraw::SetColorKey(LPDIRECTDRAWSURFACE7 lpTileDDS, unsigned long dwGDIColor)
{
    if(lpTileDDS)
    {
        COLORREF       rgbT = DD_COLORKEY;
        HDC            hdc;
        DWORD          dw = CLR_INVALID;
        DDSURFACEDESC2 ddsd;
        HRESULT        hr;

        //  Use GDI SetPixel to color match for us
        if(dwGDIColor != CLR_INVALID && lpTileDDS->GetDC(&hdc)== DD_OK)
        {
            rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
            SetPixel(hdc, 0, 0, dwGDIColor);       // Set our value
            lpTileDDS->ReleaseDC(hdc);
        }

        // Now lock the surface so we can read back the converted color
        ddsd.dwSize = sizeof(ddsd);
        hr = lpTileDDS->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
        if(hr == DD_OK)
        {
            dw = *(DWORD *)ddsd.lpSurface; 
            if(ddsd.ddpfPixelFormat.dwRGBBitCount < 32)// Mask it to bpp
                dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)- 1;  
            lpTileDDS->Unlock(NULL);
        }

        //  Now put the color that was there back.
        if(dwGDIColor != CLR_INVALID && lpTileDDS->GetDC(&hdc)== DD_OK)
        {
            SetPixel(hdc, 0, 0, rgbT);
            lpTileDDS->ReleaseDC(hdc);
        }

        //return dw;
        DDCOLORKEY ddck;
        ddck.dwColorSpaceLowValue  = dw;
        ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
        lpTileDDS->SetColorKey(DDCKEY_SRCBLT, &ddck);
    }
}




//-----------------------------------------------------
//  描述： 创建全屏模式
//  日期： 2005/09/28
//  参数： HWND hWnd    [in]  主窗体句柄
//         long nWidth  [in]  要创建全屏的宽度
//         long nHeight [in]  要创建全屏的高度
//         long nBPP    [in]  颜色深度
//  返回值： true - 成功   false - 失败
//-----------------------------------------------------
bool CDDraw::InitFullScreenMode(HWND hWnd, long nWidth, long nHeight, long nBPP)
{
    HRESULT hr;

    //  对已初始化的对象销毁
    Term();

    //
    //  1.  创建主 DirectDraw7 对象
    //
    if(FAILED(hr = DirectDrawCreateEx(NULL, (void**)&m_lpDD,
                                         IID_IDirectDraw7, NULL)))
        return false;

    //
    //  2.  设置协作等级：独占，全屏
    //
    hr = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
    if(FAILED(hr))
        return false;

    //
    //  3.  设置显示模式：分辨率，发色数
    //
    if(FAILED(m_lpDD->SetDisplayMode(nWidth, nHeight, nBPP, 0, 0)))
        return false;

    //
    //  4.  创建前台面板，包含一个后台缓冲面板
    //
    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize            = sizeof(ddsd);
    ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP |
                             DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
    ddsd.dwBackBufferCount = 1;

    if(FAILED(hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSPrimary,
                                           NULL)))
        return false;

    //
    //  5.  获得指向后台面板的指针
    //
    DDSCAPS2 ddscaps;
    ZeroMemory(&ddscaps, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

    if(FAILED(hr = m_lpDDSPrimary->GetAttachedSurface(&ddscaps, &m_lpDDSBack)))
        return false;

    m_lpDDSBack->AddRef();

    //
    //  6.  初始化成功，保存信息，刷新
    //
    m_hWnd         = hWnd;
    m_nWidth       = nWidth;
    m_nHeight      = nHeight;
    m_nBPP         = nBPP;
    m_nWindowMode  = 0;
    UpdateBounds();

    return true;
}




//-------------------------------------------------------
//  描述： 创建窗体模式
//  日期： 2005/09/28
//  参数： HWND hWnd    [in]  主窗体句柄
//         long nWidth  [in]  要创建窗体客户区域的宽度
//         long nHeight [in]  要创建窗体客户区域的高度
//  返回值： true - 成功   false - 失败
//-------------------------------------------------------
bool CDDraw::InitWindowMode(HWND hWnd, long nWidth, long nHeight)
{
    HRESULT hr;

    //  对已初始化的对象销毁
    Term();

    //
    //  1.  创建主 DirectDraw7 对象
    //
    if(FAILED(hr = DirectDrawCreateEx(NULL, (void**)&m_lpDD,
                                         IID_IDirectDraw7, NULL)))
        return false;

    //
    //  2.  设置协作等级：普通
    //
    hr = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if(FAILED(hr))
        return false;

    RECT  rcWork;
    RECT  rc;
    DWORD dwStyle;

    //
    //  3.  窗口模式下的窗体风格转换，禁用 WS_POP 风格
    //
    dwStyle  = GetWindowStyle(hWnd);
    dwStyle &= ~WS_POPUP;
    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
    SetWindowLong(hWnd, GWL_STYLE, dwStyle);

    //
    //  4.  设置窗体大小，将客户区设置成指定的分辨率
    //
    SetRect(&rc, 0, 0, nWidth, nHeight);

    AdjustWindowRectEx(&rc, GetWindowStyle(hWnd), GetMenu(hWnd)!= NULL,
                        GetWindowExStyle(hWnd));

    SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
                  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                  SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

    //
    //  5.  设置窗体位置，如果窗体在看不见的区域，
    //      则设置窗体位置于左上原点
    //
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
    GetWindowRect(hWnd, &rc);
    if(rc.left < rcWork.left)rc.left = rcWork.left;
    if(rc.top  < rcWork.top) rc.top  = rcWork.top;
    SetWindowPos(hWnd, NULL, rc.left, rc.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    //
    //  6.  创建主面板
    //
    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if(FAILED(m_lpDD->CreateSurface(&ddsd, &m_lpDDSPrimary, NULL)))
        return false;

    //
    //  7.  创建后台面板，窗体模式下是创建离屏面板作为后台面板
    //
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    ddsd.dwWidth        = nWidth;
    ddsd.dwHeight       = nHeight;
    
    if(FAILED(hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSBack, NULL)))
        return false;

    //
    //  8.  创建窗体模式下必须的裁减器对象
    //
    LPDIRECTDRAWCLIPPER lpClipper;
    if(FAILED(hr = m_lpDD->CreateClipper(0, &lpClipper, NULL)))
        return false;

    //
    //  9.  设置将主窗体句柄关联到裁减器对象
    //
    if(FAILED(hr = lpClipper->SetHWnd(0, hWnd)))
    {
        lpClipper->Release();
        return false;
    }

    //
    //  10. 将裁减器对像关联到主面板对象
    //      防止在窗体客户区外面绘制图形
    //
    if(FAILED(hr = m_lpDDSPrimary->SetClipper(lpClipper)))
    {
        lpClipper->Release();
        return false;
    }

    //
    //  11. 裁减器对象的清除，由于引用计数器现在是 2
    //      调用此方法后会降为 1，不会真正销毁截减器对象
    //
    lpClipper->Release();

	//
	//	12. 获得桌面颜色深度
	//
	HDC hdc;
	if(FAILED(m_lpDDSPrimary->GetDC(&hdc)))
	{
		return false;
	}
	m_nBPP = GetDeviceCaps(hdc, BITSPIXEL);
	m_lpDDSPrimary->ReleaseDC(hdc);

    //
    //  13. 初始化成功，保存信息，刷新
    //
    m_hWnd         = hWnd;
    m_nWidth       = nWidth;
    m_nHeight      = nHeight;
    m_nWindowMode  = 1;
    UpdateBounds();

    return true;
}



bool CDDraw::DrawRectangle(RECT* pRect, long nFillColor, long nBorderColor)
{
	DDSURFACEDESC2 ddsd;
	memset(&ddsd, 0, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);

	HDC hdc;
	if(FAILED(m_lpDDSBack->GetDC(&hdc)))
	{
		return false;
	}

//	SetDCBrushColor(hdc, nFillColor);
//	SetDCPenColor(hdc, nBorderColor);

	//	绘制矩形
	Rectangle(hdc, pRect->left, pRect->top, pRect->right, pRect->bottom);

	if(nFillColor != nBorderColor)
	{
		MoveToEx(hdc, pRect->left, pRect->top, 0);
		LineTo(hdc, pRect->right - 1,  pRect->top);
		LineTo(hdc, pRect->right - 1,  pRect->bottom - 1);
		LineTo(hdc, pRect->left,  pRect->bottom - 1);
		LineTo(hdc, pRect->left,  pRect->top);
	}

	m_lpDDSBack->ReleaseDC(hdc);

	return true;
}


