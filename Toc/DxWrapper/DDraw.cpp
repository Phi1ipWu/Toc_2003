////////////////////////////////////////////////////////////////////////////
//
//  ���������ļ�������Ϸ���촫˵��һ���֣�ȫ����Ȩ���� COKUCO ����������
//        ������Ȩ��COKUCO �����ұ�����������Ȩ��
//
//  ������CDDraw ��C - Class  DDraw - DirectDraw��
//  ģʽ��Singleton Ψһʵ��
//  ������DirectDraw ��װ�࣬���Էǳ�����Ľ��� DirectDraw �Ĳ���
//        ֧�ִ����ȫ������ģʽ���Լ����ֳ����ֱ��ʣ����ܷܺ����
//        ��ʼ�� DirectDraw����ʼ��������壬λ�鴫�ͣ���֡��
//        ����ȫ����С�����Զ��������Ļָ���
//
//  ���ߣ�Kudoxinyiji
//  ���ڣ�2005-09-28
//
////////////////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <windowsx.h>
#include <winerror.h>

#include "DDraw.h"
using namespace DxWrapper;


//  CDDraw Ψһʵ����ʼ��
CDDraw* CDDraw::m_pDDraw = 0;



//-----------------------------------------------------
//  ������ CDDraw Ψһʵ�����ڴ���䣨��̬��
//  ���ڣ� 2005/05/28
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
//  ������ CDDraw Ψһʵ�����ڴ���գ���̬��
//  ���ڣ� 2005/05/28
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
//  ������ ��� CDDraw Ψһʵ������̬��
//  ���ڣ� 2005/05/28
//  ����ֵ�� CDDraw Ψһʵ����ָ��
//-----------------------------------------------------
//inline CDDraw* CDDraw::Instance()
//{
//    return m_pDDraw;
//}




//-----------------------------------------------------
//  ������ CDInput ��Ĺ��캯��
//  ���ڣ� 2005/05/28
//-----------------------------------------------------
CDDraw::CDDraw()
{
    //  ����Ĭ�� 640 x 480 @ 32 λɫ
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
//  ������ CDInput �����������
//  ���ڣ� 2005/05/28
//-----------------------------------------------------
CDDraw::~CDDraw()
{
    TermSurface();
    Term();
}




//-----------------------------------------------------
//  ������ ���÷ֱ���
//  ���ڣ� 2005/09/27
//  ������ hWnd ��������
//-----------------------------------------------------
bool CDDraw::Init(HWND hWnd, long nWidth, long nHeight, long nBPP, long nIsWindow)
{
    //  ��� DirectDraw �����Ѿ���ʼ��
    //  ���Ƚ��Ѿ���ʼ���Ķ�������
    Term();

    if(nIsWindow)
    {
        //  ����ģʽ
        return InitWindowMode(hWnd, nWidth, nHeight);
    }
    else
    {
        //  ȫ��ģʽ
        return InitFullScreenMode(hWnd, nWidth, nHeight, nBPP);
    }

	return true;
}




//-----------------------------------------------------
//  ������ �ͷ� DirectDraw ����
//  ���ڣ� 2005/05/28
//-----------------------------------------------------
void CDDraw::Term()
{
    //  �ͷź�̨������
    if(0 != m_lpDDSBack)
    {
        m_lpDDSBack->Release();
        m_lpDDSBack = 0;
    }

    //  �ͷ�ǰ̨������
    if(0 != m_lpDDSPrimary)
    {
        m_lpDDSPrimary->Release();
        m_lpDDSPrimary = 0;
    }

    //  �ͷ��� DirectDraw ����
    //  ���ͷ�ǰ�Ƚ�Э��ģʽ��Ϊ��ͨ
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
//  ������ ��ʼ���������
//  ���ڣ� 2005/09/27
//  ������ long nID  [in]  ��Ҫ��ʼ�������������
//  ����ֵ�� true - �ɹ�   false - ʧ��
//
//  ע������֮ǰ�Ѿ������� Init() ���� DDRAW ����
//-----------------------------------------------------
bool CDDraw::InitSurface(long nID)
{
    //  ���ж� DirectDraw �������Ƿ��Ѿ���ʼ��
    if(0 == m_lpDD)
    {
        //  DirectDraw ������δ��ʼ��
        return false;
    }
    else if(0 != nID && m_lpTileDDS[nID])
    {
        //  ��ʱǰ������Ѿ���ʼ����
        //  �����ٴγ�ʼ��
        return true;
    }


    //  ����Ѿ���ʼ�����������������
    //TermSurface(nID);

    //  ��ʼ��ȫ��������ש���
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

    //  ��ʼ��ָ����ŵĵ������
    return DrawBitmap(nID);
}




//---------------------------------------------------------
//  ������ ��ָ�������������ʾλͼ
//  ���ڣ� 2005/09/27
//  ������ long nID  [in]  ��Ҫ������ʾλͼ�����������
//  ����ֵ�� true - �ɹ�   false - ʧ��
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

    //  �ļ� BMP �ļ��ж�ȡ
    //sprintf(szBufferA, DD_SURFACEFILENAME, i);
    hBitmap = (HBITMAP)LoadImage(NULL, szBuffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    //  ������ļ�ʧ��
    if(0 == hBitmap)
    {
        //TermSurface();
        return false;
    }

    //  ��� BMP �ļ���һЩ��Ϣ
    GetObject(hBitmap, sizeof(bm), &bm);

    //  ���δ����������壬�򴴽�֮
    if(0 == m_lpTileDDS[i])
    {
        //  �����������
        ZeroMemory(&ddsd, sizeof(ddsd));
        ddsd.dwSize = sizeof(ddsd);
        ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
        if(this->m_nWindowMode) ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
        ddsd.dwWidth = bm.bmWidth;      //  �������� BMP �Ŀ��һ��
        ddsd.dwHeight = bm.bmHeight;    //  �������� BMP �ĸ߶�һ��

        m_lpDD->CreateSurface(&ddsd, &m_lpTileDDS[i], NULL);
    }

    //  ��������������ʧ�ܻ��߶��ļ�ʧ��
    if(0 == m_lpTileDDS[i] || 0 == hBitmap)
    {
        return false;
    }

    m_lpTileDDS[i]->Restore();

    hdcImage = CreateCompatibleDC(NULL);
    SelectObject(hdcImage, hBitmap);

    //  ��λͼλ�鴫�ͽ��������
    m_lpTileDDS[i]->GetDC(&hdc);
    BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, SRCCOPY);
    m_lpTileDDS[i]->ReleaseDC(hdc);
    DeleteObject(hBitmap);
    DeleteDC(hdcImage);

    //  ���ùؼ�ɫ͸��
    SetColorKey(m_lpTileDDS[i], DD_COLORKEY);

    return true;
}




//-----------------------------------------------------
//  ������ ��ֹ�������
//  ���ڣ� 2005/09/27
//  ע��   ����֮ǰ�Ѿ������� Init()���� DDRAW ����
//-----------------------------------------------------
void CDDraw::TermSurface(long nID)
{
    if(0 != nID)
    {
        //  ��ֹ�ض����(nID)���������

        if(m_lpTileDDS[nID])
        {
            m_lpTileDDS[nID]->Release();
            m_lpTileDDS[nID] = 0;
        }
    }
    else
    {
        //  ��ֹȫ�����������

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
//  ������ ��������С��λ��ʱ���ø���
//  ���ڣ� 2005/06/07
//  ����ֵ�� 
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
//  ������ ��̨������
//  ���ڣ� 2005/06/06
//  ������ dwColor ����պ����ɫ
//  ����ֵ�� true - �ɹ�   false - ʧ��
//-----------------------------------------------------
bool CDDraw::Clear(DWORD dwColor)
{
    if(0 != m_lpDDSBack)
    {
        DDBLTFX ddbltfx;
        ZeroMemory(&ddbltfx,  sizeof(ddbltfx));
        ddbltfx.dwSize      = sizeof(ddbltfx);
        ddbltfx.dwFillColor = dwColor;

        //  ��ָ������ɫ������̨���
        return SUCCEEDED(m_lpDDSBack->Blt(0, 0, 0, DDBLT_COLORFILL, &ddbltfx));
    }

    return false;
}




//-----------------------------------------------------
//  ������ ��֡
//  ���ڣ� 2005/09/27
//  ����ֵ�� true - �ɹ�   false - ʧ��
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
                //  ����ģʽ
                hr = m_lpDDSPrimary->Blt(&m_rcWindow, m_lpDDSBack,
                                            NULL, DDBLT_WAIT, NULL);
            }
            else
            {
                //  ȫ��ģʽ
                hr = m_lpDDSPrimary->Flip(0, 0);
            }

            //  �����֡�����Իָ�ȫ�������
            if(hr == DDERR_SURFACELOST)
            {
                if(SUCCEEDED(m_lpDD->RestoreAllSurfaces()))
                {
                    for(long i = 0; i <= sizeof(m_lpTileDDS); ++i)
                    {
                        if(m_lpTileDDS[i])
                        {
                            //  ���»���
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

            //  ���ڻ���
            if(hr != DDERR_WASSTILLDRAWING)
            {
                return SUCCEEDED(hr);
            }
        }
    }

    return false;

}




//---------------------------------------------------------
//  ������ ������������λ�鿽������̨���棨ֻ�޵�ש��
//  ���ڣ� 2005/06/06
//  ������ long nNum     [in]  ͼ���زı��
//         LPRECT lpRect [in]  Ҫ���Ƶ����������ϵ�����
//         long nX       [in]  ��Ļ�� X ��λ�ã����Ͻǣ�
//         long nY       [in]  ��Ļ�� Y ��λ�ã����Ͻǣ�
//  ����ֵ�� true - �ɹ�   false - ʧ��
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
            //  ������Ƶ��ز���ȫ������Ļ��Χ
            //  ֱ�ӷ��أ�������ƣ��������Ч�ʿ�������

            return true;
        }

        if(0 != lpRect)
        {
            if(nX < 0 && nRcWidth > -nX)
            {
                //  ������߽�
                lpRect->left -= nX;
                nX = 0;
            }

            if(nX + nRcWidth > m_nWidth)
            {
                //  �����ұ߽�
                lpRect->right -= nRcWidth + nX - m_nWidth;
            }

            if(nY < 0 && nRcHeight > -nY)
            {
                //  �����ϱ߽�
                lpRect->top -= nY;
                nY = 0;
            }

            if(nY + nRcHeight >= m_nHeight)
            {
                //  �����±߽�
                lpRect->bottom -= nRcHeight + nY - m_nHeight;
            }
        }

        //  λ�鴫��ָ�����ε���̨�����
        return SUCCEEDED(m_lpDDSBack->BltFast(nX, nY, m_lpTileDDS[nNum], lpRect, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT));
    }

    return false;
}




//---------------------------------------------------------
//  ������ ������������λ�鿽������̨���棨ֻ�޵�ש��
//  ���ڣ� 2005/06/06
//  ������ long nTileNum [in]  ͼ���زı��
//         long nTileX   [in]  ͼ���زĵ��к�
//         long nTileY   [in]  ͼ���زĵ��к�
//         long nX       [in]  ��Ļ�� X ��λ�ã����Ͻǣ�
//         long nY       [in]  ��Ļ�� Y ��λ�ã����Ͻǣ�
//  ����ֵ�� true - �ɹ�   false - ʧ��
//
//  ע�� ��ש�ز��� 32 * 32 ��С
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
    //  ���ж���̨����ָ������������Ƿ��Ѿ���ʼ��
    if(0 != m_lpDDSBack && 0 != m_lpTileDDS[nTileNum])
    {
        RECT rc;
        SetRect(&rc, nTileX << 5, nTileY << 5, (nTileX + 1)<< 5, (nTileY + 1)<< 5);

        if(nX + 32 <= 0 || nY + 32 <= 0 || 
           nX >= m_nWidth || nY >= m_nHeight)
        {
            //  ������Ƶ��ز���ȫ������Ļ��Χ
            //  ֱ�ӷ��أ�������ƣ��������Ч�ʿ�������

            return true;
        }

        if(nX < 0)
        {
            //  ������߽�
            rc.left -= nX;
            nX = 0;
        }
        else if(nX + 32 >= m_nWidth)
        {
            //  �����ұ߽�
            rc.right -= 32 + nX - m_nWidth;
        }

        if(nY < 0)
        {
            //  �����ϱ߽�
            rc.top -= nY;
            nY = 0;
        }
        else if(nY + 32 >= m_nHeight)
        {
            //  �����±߽�
            rc.bottom -= 32 + nY - m_nHeight;
        }

        return SUCCEEDED(m_lpDDSBack->BltFast(nX, nY, m_lpTileDDS[nTileNum], &rc, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT));
    }

    return false;
}




//-----------------------------------------------------
//  ������ ��ú�̨��������
//  ���ڣ� 2005/06/07
//  ����ֵ�� ��̨��������
//-----------------------------------------------------
LPDIRECTDRAWSURFACE7 CDDraw::GetBackSurface()
{
    return m_lpDDSBack;
}




//-----------------------------------------------------
//  ������ �����������Ĺؼ�ɫ
//  ���ڣ� 2005-09-27
//  ������ LPDIRECTDRAWSURFACE7 lpTileDDS  [in]
//         ��Ҫ���ùؼ�ɫ���������
//         unsigned long dwGDIColor        [in]
//         ��Ҫ͸���Ĺؼ�ɫ
//
//  ����ֵ�� true - �ɹ�   false - ʧ��
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
//  ������ ����ȫ��ģʽ
//  ���ڣ� 2005/09/28
//  ������ HWND hWnd    [in]  ��������
//         long nWidth  [in]  Ҫ����ȫ���Ŀ��
//         long nHeight [in]  Ҫ����ȫ���ĸ߶�
//         long nBPP    [in]  ��ɫ���
//  ����ֵ�� true - �ɹ�   false - ʧ��
//-----------------------------------------------------
bool CDDraw::InitFullScreenMode(HWND hWnd, long nWidth, long nHeight, long nBPP)
{
    HRESULT hr;

    //  ���ѳ�ʼ���Ķ�������
    Term();

    //
    //  1.  ������ DirectDraw7 ����
    //
    if(FAILED(hr = DirectDrawCreateEx(NULL, (void**)&m_lpDD,
                                         IID_IDirectDraw7, NULL)))
        return false;

    //
    //  2.  ����Э���ȼ�����ռ��ȫ��
    //
    hr = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
    if(FAILED(hr))
        return false;

    //
    //  3.  ������ʾģʽ���ֱ��ʣ���ɫ��
    //
    if(FAILED(m_lpDD->SetDisplayMode(nWidth, nHeight, nBPP, 0, 0)))
        return false;

    //
    //  4.  ����ǰ̨��壬����һ����̨�������
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
    //  5.  ���ָ���̨����ָ��
    //
    DDSCAPS2 ddscaps;
    ZeroMemory(&ddscaps, sizeof(ddscaps));
    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

    if(FAILED(hr = m_lpDDSPrimary->GetAttachedSurface(&ddscaps, &m_lpDDSBack)))
        return false;

    m_lpDDSBack->AddRef();

    //
    //  6.  ��ʼ���ɹ���������Ϣ��ˢ��
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
//  ������ ��������ģʽ
//  ���ڣ� 2005/09/28
//  ������ HWND hWnd    [in]  ��������
//         long nWidth  [in]  Ҫ��������ͻ�����Ŀ��
//         long nHeight [in]  Ҫ��������ͻ�����ĸ߶�
//  ����ֵ�� true - �ɹ�   false - ʧ��
//-------------------------------------------------------
bool CDDraw::InitWindowMode(HWND hWnd, long nWidth, long nHeight)
{
    HRESULT hr;

    //  ���ѳ�ʼ���Ķ�������
    Term();

    //
    //  1.  ������ DirectDraw7 ����
    //
    if(FAILED(hr = DirectDrawCreateEx(NULL, (void**)&m_lpDD,
                                         IID_IDirectDraw7, NULL)))
        return false;

    //
    //  2.  ����Э���ȼ�����ͨ
    //
    hr = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if(FAILED(hr))
        return false;

    RECT  rcWork;
    RECT  rc;
    DWORD dwStyle;

    //
    //  3.  ����ģʽ�µĴ�����ת�������� WS_POP ���
    //
    dwStyle  = GetWindowStyle(hWnd);
    dwStyle &= ~WS_POPUP;
    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
    SetWindowLong(hWnd, GWL_STYLE, dwStyle);

    //
    //  4.  ���ô����С�����ͻ������ó�ָ���ķֱ���
    //
    SetRect(&rc, 0, 0, nWidth, nHeight);

    AdjustWindowRectEx(&rc, GetWindowStyle(hWnd), GetMenu(hWnd)!= NULL,
                        GetWindowExStyle(hWnd));

    SetWindowPos(hWnd, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
                  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                  SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

    //
    //  5.  ���ô���λ�ã���������ڿ�����������
    //      �����ô���λ��������ԭ��
    //
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
    GetWindowRect(hWnd, &rc);
    if(rc.left < rcWork.left)rc.left = rcWork.left;
    if(rc.top  < rcWork.top) rc.top  = rcWork.top;
    SetWindowPos(hWnd, NULL, rc.left, rc.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

    //
    //  6.  ���������
    //
    DDSURFACEDESC2 ddsd;
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    if(FAILED(m_lpDD->CreateSurface(&ddsd, &m_lpDDSPrimary, NULL)))
        return false;

    //
    //  7.  ������̨��壬����ģʽ���Ǵ������������Ϊ��̨���
    //
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;    
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
    ddsd.dwWidth        = nWidth;
    ddsd.dwHeight       = nHeight;
    
    if(FAILED(hr = m_lpDD->CreateSurface(&ddsd, &m_lpDDSBack, NULL)))
        return false;

    //
    //  8.  ��������ģʽ�±���Ĳü�������
    //
    LPDIRECTDRAWCLIPPER lpClipper;
    if(FAILED(hr = m_lpDD->CreateClipper(0, &lpClipper, NULL)))
        return false;

    //
    //  9.  ���ý����������������ü�������
    //
    if(FAILED(hr = lpClipper->SetHWnd(0, hWnd)))
    {
        lpClipper->Release();
        return false;
    }

    //
    //  10. ���ü��������������������
    //      ��ֹ�ڴ���ͻ����������ͼ��
    //
    if(FAILED(hr = m_lpDDSPrimary->SetClipper(lpClipper)))
    {
        lpClipper->Release();
        return false;
    }

    //
    //  11. �ü��������������������ü����������� 2
    //      ���ô˷�����ήΪ 1�������������ٽؼ�������
    //
    lpClipper->Release();

	//
	//	12. ���������ɫ���
	//
	HDC hdc;
	if(FAILED(m_lpDDSPrimary->GetDC(&hdc)))
	{
		return false;
	}
	m_nBPP = GetDeviceCaps(hdc, BITSPIXEL);
	m_lpDDSPrimary->ReleaseDC(hdc);

    //
    //  13. ��ʼ���ɹ���������Ϣ��ˢ��
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

	//	���ƾ���
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


