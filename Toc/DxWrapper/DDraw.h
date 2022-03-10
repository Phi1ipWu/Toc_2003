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

        //  DirectDraw ��ʼ��
        bool Init(HWND hWnd, long nWidth, long nHeight, long nBPP, long nIsWindow = 1);

        //  DirectDraw ��ֹ��������ͷ�
        void Term();

        //  ��ʼ���������
        bool InitSurface(long nID = 0);

        //  ��ֹ�������
        void TermSurface(long nID = 0);

        //  ���´����ܣ����ڴ���ģʽ��
        void UpdateBounds();

        //  λ�鴫��
        bool Blt(long nNum, LPRECT lpRect, long nX, long nY);

        //  λ�鴫�ͣ�ר���ڻ��Ƶ�ש��
        bool BltTile(long nTileNum, long nTileX, long nTileY, long nX, long nY);

        //  λ�鴫�ͣ�ר���ڻ��ƽ�ɫ��
        bool BltChar(long nCharNum, long nFace,  long nStep,  long nX, long nY);

        //  �������ͻ�����������
        bool Clear(DWORD = 0);

        //  ��֡
        bool Flip();

		//	���ƾ���
		bool DrawRectangle(RECT* pRect, long nFillColor, long nBorderColor = 0xFFFFFF);

        //  ��ú�����
        LPDIRECTDRAWSURFACE7 GetBackSurface();

		inline long GetBPP() {	return this->m_nBPP;  }

    private:

        //  Ĭ�Ϲ��캯��
        CDDraw();

        //  ��������
        ~CDDraw();

        //  ��ָ�����������������ʾλͼ
        bool DrawBitmap(long nID);

        //  ���ùؼ�ɫ������͸��
        void SetColorKey(LPDIRECTDRAWSURFACE7 lpTileDDS, unsigned long dwGDIColor);

        //  ��ʼ��ȫ����ʾģʽ
        bool InitFullScreenMode(HWND hWnd, long nWidth, long nHeight, long nBPP);

        //  ��ʼ��������ʾģʽ
        bool InitWindowMode(HWND hWnd, long nWidth, long nHeight);

    private:

        //  �� DDraw ����
        LPDIRECTDRAW7 m_lpDD;

        //  �����
        LPDIRECTDRAWSURFACE7 m_lpDDSPrimary;

        //  ��̨���
        LPDIRECTDRAWSURFACE7 m_lpDDSBack;

        //  ������ש���
        LPDIRECTDRAWSURFACE7 m_lpTileDDS[100];

        //  ��������
        HWND m_hWnd;

        //  ����������λ�õľ���
        RECT m_rcWindow;

        //  �ֱ��ʣ���ȣ�
        long m_nWidth;

        //  �ֱ��ʣ��߶ȣ�
        long m_nHeight;

        //  ��ɫ��
        long m_nBPP;

        //  ����ɫ
        long m_nBgColor;

        //  ��ʾģʽ 0 - ȫ��  1 - ����
        long m_nWindowMode;

        //  CDDraw �����Ψһʵ��
        static CDDraw* m_pDDraw;

    };

};


#endif

