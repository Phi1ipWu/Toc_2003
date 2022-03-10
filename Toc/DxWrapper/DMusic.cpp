////////////////////////////////////////////////////////////////////////////
//
//  ���������ļ�������Ϸ���촫˵��һ���֣�ȫ����Ȩ���� COKUCO ����������
//        ������Ȩ��COKUCO �����ұ�����������Ȩ��
//
//  ������CDMusic ��C - Class  DMusic - DirectMusic��
//  ģʽ��Singleton Ψһʵ��
//  ������DirectMusic ��װ�֧࣬��ͨ����Ӧһ�����ֵ����� ID �����в���
//        ֧�ָ�ʽ��MIDI, WAV
//  ���ܣ���˵ DM ����ȫ COM������ȥ�ƺ��ܿ�
//        ���Ǹо��ö��ģ���Ҫ�� COM ��ʽ��������
//
//  ���ߣ�Kudoxinyiji
//  ���ڣ�2005-06-10
//
////////////////////////////////////////////////////////////////////////////



#include "stdio.h"
#include "DMusic.h"
using namespace DxWrapper;




//  CDMusic Ψһʵ����ָ��
CDMusic* CDMusic::m_pDMusic = 0;




//-----------------------------------------------------
//  ������ CDMusic Ψһʵ�����ڴ���䣨��̬��
//  ���ڣ� 2005/05/16
//-----------------------------------------------------
bool CDMusic::InitInstance()
{
    if(0 == m_pDMusic)
    {
        m_pDMusic = new CDMusic();
    }

    return true;
}




//-----------------------------------------------------
//  ������ CDMusic Ψһʵ�����ڴ���գ���̬��
//  ���ڣ� 2005/05/16
//-----------------------------------------------------
void CDMusic::ExitInstance()
{
    if(m_pDMusic)
    {
        delete m_pDMusic;
        m_pDMusic = 0;
    }
}




//-----------------------------------------------------
//  ������ ��� CDMusic Ψһʵ������̬��
//  ���ڣ� 2005/05/16
//  ����ֵ�� CDMusic Ψһʵ����ָ��
//-----------------------------------------------------
//CDMusic* CDMusic::Instance()
//{
//    return m_pDMusic;
//}




//-----------------------------------------------------
//  ������ CDMusic ���Ĭ�Ϲ��캯��
//  ���ڣ� 2005/05/23
//-----------------------------------------------------
CDMusic::CDMusic()
{
    m_pLoader       = 0;     //  ���ض�����Ϊ NULL
    m_pPerformance  = 0;     //  ���������Ϊ NULL
    m_pSegment      = 0;     //    �ζ�����Ϊ NULL

    m_nBgmNum       = 0;     //  �������ֱ���� 0
    m_nComInit      = 0;     //  COM ��Ϊδ��ʼ��
}




//-----------------------------------------------------
//  ������ CDMusic �����������
//  ���ڣ� 2005/05/20
//  ������ ��
//-----------------------------------------------------
CDMusic::~CDMusic()
{
    Term();
}




//-----------------------------------------------------
//  ������ CDMusic �����ʼ��
//  ���ڣ� 2005/05/29
//  ������ HWND hWnd            ��������
//         LPDIRECTSOUND pDS    DirectSound ����
//
//  ע�⣺ �����ͬʱʹ�� DirectSound �� DirectMusic
//         һ��Ҫ������ DirectSound�������� DirectSound
//         �������������ڶ�����������Ϊ DirectMusic
//         ���ջ���Ҫ���� DirectSound �ġ��м� ����
//-----------------------------------------------------
bool CDMusic::Init(HWND hWnd, LPDIRECTSOUND pDS)
{
    Term();

    //  ��ʼ�� COM
    if(FAILED(CoInitialize(0)))
    {
        return false;
    }

    //  ��־ COM �Ѿ���ʼ��
    m_nComInit = 1;

    // �����������
    if(FAILED(CoCreateInstance(
                CLSID_DirectMusicPerformance,
                0,
                CLSCTX_INPROC, 
                IID_IDirectMusicPerformance8,
                (void**)&m_pPerformance
                )))
    {
        Term();
        return false;   
    }

    //  �������ض���
    if(FAILED(CoCreateInstance(
                CLSID_DirectMusicLoader,
                0,
                CLSCTX_INPROC,
                IID_IDirectMusicLoader,
                (void**)&m_pLoader
                )))
    {
        Term();
        return false;
    }

    //  ��ʼ���������
    if(FAILED(m_pPerformance->InitAudio(
                NULL,      // IDirectMusic interface not needed.
                &pDS,      // IDirectSound interface not needed.
                hWnd,      // Window handle.
                DMUS_APATH_SHARED_STEREOPLUSREVERB,  // Default audiopath type.
                64,        // Number of performance channels.
                DMUS_AUDIOF_ALL,   // Features on synthesizer.
                NULL       // Audio parameters; use defaults.
                )))
    {
        Term();
        return false;
    }

    return true;
}




//-----------------------------------------------------
//  ������ CDMusic ������ֹ
//  ���ڣ� 2005/05/29
//-----------------------------------------------------
void CDMusic::Term()
{
    //  �ͷ��������
    if(0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pPerformance->CloseDown();
        m_pPerformance->Release();
        m_pPerformance = 0;
    }

    //  �ͷŶζ���
    if(0 != m_pSegment)
    {
        m_pSegment->Release();
        m_pSegment = 0;
    }

    //  �ͷż��ض���
    if(0 != m_pLoader)
    {
        m_pLoader->Release();
        m_pLoader = 0;
    }

    //  �ͷ� COM
    if(0 != m_nComInit)
    {
        CoUninitialize();
        m_nComInit = 0;
    }
}




//-----------------------------------------------------
//  ������ ���ű������֣��ظ����ţ�
//  ���ڣ� 2005/05/29
//  ������ long nBgn ��Ҫ���������ı��
//  ����ֵ�� ���ųɹ����� true��ʧ�ܷ��� false
//-----------------------------------------------------
bool CDMusic::PlayBGM(const long& nBgmNum)
{
    //  ����Ѽ��صĶ�
    if(0 != m_pSegment && 0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pSegment->Release();
        m_pSegment = 0;
        m_nBgmNum = 0;
    }

    //  �ж�Ҫ���ŵ����ֵı���Ƿ�����Ч��Χ��
    if(nBgmNum < DM_BGM_MIN_NUM || nBgmNum > DM_BGM_MAX_NUM)
    {
        return true;
    }

    //  �жϼ��ض������������Ƿ��Ѿ���ʼ��
    if(0 != m_pLoader && 0 != m_pPerformance)
    {
        char  szBufferA[MAX_PATH];
        WCHAR szBufferW[MAX_PATH];

        //  �����ļ���ʽ�������תΪ�ַ���
        sprintf(szBufferA, DM_BGM_FILENAME, nBgmNum);
        //  ���ַ����� ANSI תΪ WIDE ���ַ�
        MultiByteToWideChar(CP_ACP, 0, szBufferA, -1, szBufferW, MAX_PATH);

        //  �Ӵ����ļ��м���
        if (FAILED(m_pLoader->LoadObjectFromFile(
                    CLSID_DirectMusicSegment,   // Class identifier.
                    IID_IDirectMusicSegment8,   // ID of desired interface.
                    szBufferW,                  // Filename.
                    (void**)&m_pSegment         // Pointer that receives interface.
                    )))
        {
            return false;
        }

        //  DMUS_SEG_REPEAT_INFINITE = 0xFFFFFFFF
        m_pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);   //  �������֣��ظ�����
        m_pSegment->Download(m_pPerformance);

        m_pPerformance->PlaySegmentEx(
                    m_pSegment,     // Segment to play.
                    NULL,           // Used for songs; not implemented.
                    NULL,           // For transitions. 
                    0,              // Flags.
                    0,              // Start time; 0 is immediate.
                    NULL,           // Pointer that receives segment state.
                    NULL,           // Object to stop.
                    NULL            // Audiopath, if not default.
                    );  

    }

    m_nBgmNum = nBgmNum;

    return true;
}




//-----------------------------------------------------
//  ������ ������Ч���֣�ֻ��һ�飩
//  ���ڣ� 2005/05/29
//  ������ long nBgn ��Ҫ���������ı��
//  ����ֵ�� ���ųɹ����� true��ʧ�ܷ��� false
//-----------------------------------------------------
bool CDMusic::PlayEFF(const long& nEffNum)
{
    //  ����Ѽ��صĶ�
    if(0 != m_pSegment && 0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pSegment->Release();
        m_pSegment = 0;
    }

    //  �ж�Ҫ���ŵ������Ƿ�����Ч��Χ��
    if(nEffNum < DM_EFF_MIN_NUM || nEffNum > DM_EFF_MAX_NUM)
    {
        return true;
    }

    //  �жϼ��ض������������Ƿ��Ѿ���ʼ��
    if(0 != m_pLoader && 0 != m_pPerformance)
    {
        char  szBufferA[MAX_PATH];
        WCHAR szBufferW[MAX_PATH];

        //  �����ļ���ʽ�������תΪ�ַ���
        sprintf(szBufferA, DM_EFF_FILENAME, nEffNum);
        //  ���ַ����� ANSI תΪ WIDE ���ַ�
        MultiByteToWideChar(CP_ACP, 0, szBufferA, -1, szBufferW, MAX_PATH);

        if (FAILED(m_pLoader->LoadObjectFromFile(
                    CLSID_DirectMusicSegment,   // Class identifier.
                    IID_IDirectMusicSegment8,   // ID of desired interface.
                    szBufferW,                  // Filename.
                    (void**)&m_pSegment         // Pointer that receives interface.
                    )))
        {
            return false;
        }

        m_pSegment->SetRepeats(0);  //  ��Ч����ֻ��һ�鲻�ظ�
        m_pSegment->Download(m_pPerformance);

        m_pPerformance->PlaySegmentEx(
                    m_pSegment,     // Segment to play.
                    NULL,           // Used for songs; not implemented.
                    NULL,           // For transitions. 
                    0,              // Flags.
                    0,              // Start time; 0 is immediate.
                    NULL,           // Pointer that receives segment state.
                    NULL,           // Object to stop.
                    NULL            // Audiopath, if not default.
                    );

    }

    return true;
}




//-----------------------------------------------------
//  ������ ����ָ��·���������ļ�
//  ���ڣ� 2005/06/10
//  ������ szBufferA    �ļ�·��
//         nRepeat      �ظ�����
//  ����ֵ�� ���ųɹ����� true��ʧ�ܷ��� false
//  ע�� ֻ֧�� MIDI �� WAV �ļ�
//-----------------------------------------------------
bool CDMusic::Play(const char* szBufferA, const DWORD& nRepeat)
{
    //  ����Ѽ��صĶ�
    if(0 != m_pSegment && 0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pSegment->Release();
        m_pSegment = 0;
        m_nBgmNum = 0;
    }

    if(szBufferA && m_pLoader && m_pPerformance)
    {
        WCHAR szBufferW[MAX_PATH];
        //  ���ַ����� ANSI תΪ WIDE ���ַ�
        MultiByteToWideChar(CP_ACP, 0, szBufferA, -1, szBufferW, MAX_PATH);

        if(FAILED(m_pLoader->LoadObjectFromFile(
                    CLSID_DirectMusicSegment,   // Class identifier.
                    IID_IDirectMusicSegment8,   // ID of desired interface.
                    szBufferW,                  // Filename.
                    (void**)&m_pSegment         // Pointer that receives interface.
                    )))
        {
            return false;
        }

        m_pSegment->SetRepeats(nRepeat);  //  �����ظ�����
        m_pSegment->Download(m_pPerformance);

        m_pPerformance->PlaySegmentEx(
                    m_pSegment,     // Segment to play.
                    NULL,           // Used for songs; not implemented.
                    NULL,           // For transitions. 
                    0,              // Flags.
                    0,              // Start time; 0 is immediate.
                    NULL,           // Pointer that receives segment state.
                    NULL,           // Object to stop.
                    NULL            // Audiopath, if not default.
                    );  

    }

    return true;
}




//-----------------------------------------------------
//  ������ ֹͣ�κ����ڲ��ŵı�������
//  ���ڣ� 2005/05/23
//-----------------------------------------------------
void CDMusic::Stop()
{
    if(m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_nBgmNum = 0;
    }
}




