//-----------------------------------------------------
//  类名：  CDShow
//  描述：  DirectShow 封装类
//  作者：  KudoXinyiji
//
//  模式：  Singleton
//-----------------------------------------------------


#include "DShow.h"
using namespace DxWrapper;



//  CDShow 唯一实例设 NULL
CDShow* CDShow::m_pDShow = 0;




//-----------------------------------------------------
//  描述： CDShow 唯一实例的内存分配（静态）
//  日期： 2005/07/08
//-----------------------------------------------------
bool CDShow::InitInstance()
{
    if(0 == m_pDShow)
    {
        m_pDShow = new CDShow();
    }
    
    return true;
}




//-----------------------------------------------------
//  描述： CDShow 唯一实例的内存回收（静态）
//  日期： 2005/07/08
//-----------------------------------------------------
void CDShow::ExitInstance()
{
    if(m_pDShow)
    {
        delete m_pDShow;
        m_pDShow = 0;
    }
}




//-----------------------------------------------------
//  描述： 获得 CDShow 唯一实例（静态）
//  日期： 2005/07/08
//  返回值： CDShow 唯一实例的指针
//-----------------------------------------------------
CDShow* CDShow::Instance()
{
    return m_pDShow;
}




//-----------------------------------------------------
//  描述： CDShow 类的默认构造函数
//  日期： 2005/07/08
//-----------------------------------------------------
CDShow::CDShow()
{
    m_pGraph        = 0;
    m_pMediaControl = 0;
    m_pEvent        = 0;
    m_pMediaSeeking = 0;

    m_hWnd          = 0;    //  主窗体句柄
    m_nComInit      = 0;    //  COM 未初始化

    m_isPlaying     = false;
    memset(m_szFilePath, 0, MAX_PATH);

}




//-----------------------------------------------------
//  描述： CDShow 类的析构函数
//  日期： 2005/07/08
//-----------------------------------------------------
CDShow::~CDShow()
{
    Term();
}




//-----------------------------------------------------
//  描述： CDShow 对象初始化
//  日期： 2005/07/11
//  参数： HWND hWnd [in]       主窗体句柄
//-----------------------------------------------------
bool CDShow::Init(HWND hWnd)
{
    if(0 == hWnd)
    {
        return false;
    }

    m_hWnd = hWnd;

    if(0 == m_nComInit)
    {
        //  初始化 COM
        if(FAILED(CoInitialize(0)))
        {
            return false;
        }

        m_nComInit = 1;     //  标记 COM 已经初始化
    }

    return true;
}




//-----------------------------------------------------
//  描述： DirectShow 的终止函式
//  日期： 2005/07/11
//-----------------------------------------------------
void CDShow::Term()
{
    if(0 != m_pGraph)
    {
        m_pGraph->Release();
        m_pGraph = 0;
    }

    if(0 != m_pMediaControl)
    {
        m_pMediaControl->Release();
        m_pMediaControl = 0;
    }

    if(0 != m_pEvent)
    {
        m_pEvent->Release();
        m_pEvent = 0;
    }

    if(0 != m_pMediaSeeking)
    {
        m_pMediaSeeking->Release();
        m_pMediaSeeking = 0;
    }

    if(0 != m_nComInit)
    {
        CoUninitialize();
        m_nComInit = 0;
    }
}




//-----------------------------------------------------
//  描述： 播放指定路径的影象或声音文件
//  日期： 2005/07/11
//  参数： szPathA      文件路径
//  返回值： 播放成功返回 true，失败返回 false
//  支持格式： Asf, Mpeg, Avi, Dv, Mp3, Wave
//-----------------------------------------------------
bool CDShow::Play(char* szPathA)
{
    CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void **)&m_pGraph);

    WCHAR szPathW[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, szPathA, -1, szPathW, MAX_PATH);

    if(FAILED(m_pGraph->RenderFile(szPathW, NULL)))
    {
        return false;
    }

    //指定父窗体
    IVideoWindow* pVidWin = NULL;
    m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);
    pVidWin->put_Owner((OAHWND)m_hWnd);
    pVidWin->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    RECT rc;
    GetClientRect(m_hWnd, &rc);

    pVidWin->SetWindowPosition(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    m_pGraph->QueryInterface(IID_IMediaEventEx, (void **)&m_pEvent);
    m_pGraph->QueryInterface(IID_IMediaSeeking, (void **)&m_pMediaSeeking);

    if(m_pGraph)
    {
        m_pGraph->QueryInterface(IID_IVideoWindow, (void **)&pVidWin);
        pVidWin->put_Visible(OATRUE);

        m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
        m_pMediaControl->Run();

        m_isPlaying = true;
    }

    return true;
}




bool CDShow::Play(const long&)
{
    return true;
}


