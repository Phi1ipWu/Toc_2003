//-----------------------------------------------------
//  ������  CDShow
//  ������  DirectShow ��װ��
//  ���ߣ�  KudoXinyiji
//
//  ģʽ��  Singleton
//-----------------------------------------------------


#include "DShow.h"
using namespace DxWrapper;



//  CDShow Ψһʵ���� NULL
CDShow* CDShow::m_pDShow = 0;




//-----------------------------------------------------
//  ������ CDShow Ψһʵ�����ڴ���䣨��̬��
//  ���ڣ� 2005/07/08
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
//  ������ CDShow Ψһʵ�����ڴ���գ���̬��
//  ���ڣ� 2005/07/08
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
//  ������ ��� CDShow Ψһʵ������̬��
//  ���ڣ� 2005/07/08
//  ����ֵ�� CDShow Ψһʵ����ָ��
//-----------------------------------------------------
CDShow* CDShow::Instance()
{
    return m_pDShow;
}




//-----------------------------------------------------
//  ������ CDShow ���Ĭ�Ϲ��캯��
//  ���ڣ� 2005/07/08
//-----------------------------------------------------
CDShow::CDShow()
{
    m_pGraph        = 0;
    m_pMediaControl = 0;
    m_pEvent        = 0;
    m_pMediaSeeking = 0;

    m_hWnd          = 0;    //  ��������
    m_nComInit      = 0;    //  COM δ��ʼ��

    m_isPlaying     = false;
    memset(m_szFilePath, 0, MAX_PATH);

}




//-----------------------------------------------------
//  ������ CDShow �����������
//  ���ڣ� 2005/07/08
//-----------------------------------------------------
CDShow::~CDShow()
{
    Term();
}




//-----------------------------------------------------
//  ������ CDShow �����ʼ��
//  ���ڣ� 2005/07/11
//  ������ HWND hWnd [in]       ��������
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
        //  ��ʼ�� COM
        if(FAILED(CoInitialize(0)))
        {
            return false;
        }

        m_nComInit = 1;     //  ��� COM �Ѿ���ʼ��
    }

    return true;
}




//-----------------------------------------------------
//  ������ DirectShow ����ֹ��ʽ
//  ���ڣ� 2005/07/11
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
//  ������ ����ָ��·����Ӱ��������ļ�
//  ���ڣ� 2005/07/11
//  ������ szPathA      �ļ�·��
//  ����ֵ�� ���ųɹ����� true��ʧ�ܷ��� false
//  ֧�ָ�ʽ�� Asf, Mpeg, Avi, Dv, Mp3, Wave
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

    //ָ��������
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


