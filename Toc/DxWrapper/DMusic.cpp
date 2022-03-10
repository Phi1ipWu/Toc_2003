////////////////////////////////////////////////////////////////////////////
//
//  声明：此文件属于游戏创造传说的一部分，全部版权归属 COKUCO 工作室所有
//        如有侵权，COKUCO 工作室保留法律起诉权利
//
//  类名：CDMusic （C - Class  DMusic - DirectMusic）
//  模式：Singleton 唯一实例
//  描述：DirectMusic 封装类，支持通过对应一个音乐的数字 ID 来进行播放
//        支持格式：MIDI, WAV
//  感受：虽说 DM 是完全 COM，看上去似乎很酷
//        但是感觉好恶心，都要用 COM 方式创建对象
//
//  作者：Kudoxinyiji
//  日期：2005-06-10
//
////////////////////////////////////////////////////////////////////////////



#include "stdio.h"
#include "DMusic.h"
using namespace DxWrapper;




//  CDMusic 唯一实例的指针
CDMusic* CDMusic::m_pDMusic = 0;




//-----------------------------------------------------
//  描述： CDMusic 唯一实例的内存分配（静态）
//  日期： 2005/05/16
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
//  描述： CDMusic 唯一实例的内存回收（静态）
//  日期： 2005/05/16
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
//  描述： 获得 CDMusic 唯一实例（静态）
//  日期： 2005/05/16
//  返回值： CDMusic 唯一实例的指针
//-----------------------------------------------------
//CDMusic* CDMusic::Instance()
//{
//    return m_pDMusic;
//}




//-----------------------------------------------------
//  描述： CDMusic 类的默认构造函数
//  日期： 2005/05/23
//-----------------------------------------------------
CDMusic::CDMusic()
{
    m_pLoader       = 0;     //  加载对象设为 NULL
    m_pPerformance  = 0;     //  演奏对象设为 NULL
    m_pSegment      = 0;     //    段对象设为 NULL

    m_nBgmNum       = 0;     //  背景音乐编号设 0
    m_nComInit      = 0;     //  COM 设为未初始化
}




//-----------------------------------------------------
//  描述： CDMusic 类的析构函数
//  日期： 2005/05/20
//  参数： 无
//-----------------------------------------------------
CDMusic::~CDMusic()
{
    Term();
}




//-----------------------------------------------------
//  描述： CDMusic 对象初始化
//  日期： 2005/05/29
//  参数： HWND hWnd            主窗体句柄
//         LPDIRECTSOUND pDS    DirectSound 对象
//
//  注意： 如果想同时使用 DirectSound 和 DirectMusic
//         一定要先启动 DirectSound，并将主 DirectSound
//         对象的引用填入第二个参数，因为 DirectMusic
//         最终还是要经过 DirectSound 的。切记 ！！
//-----------------------------------------------------
bool CDMusic::Init(HWND hWnd, LPDIRECTSOUND pDS)
{
    Term();

    //  初始化 COM
    if(FAILED(CoInitialize(0)))
    {
        return false;
    }

    //  标志 COM 已经初始化
    m_nComInit = 1;

    // 创建演奏对象
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

    //  创建加载对象
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

    //  初始化演奏对象
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
//  描述： CDMusic 对象终止
//  日期： 2005/05/29
//-----------------------------------------------------
void CDMusic::Term()
{
    //  释放演奏对象
    if(0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pPerformance->CloseDown();
        m_pPerformance->Release();
        m_pPerformance = 0;
    }

    //  释放段对象
    if(0 != m_pSegment)
    {
        m_pSegment->Release();
        m_pSegment = 0;
    }

    //  释放加载对象
    if(0 != m_pLoader)
    {
        m_pLoader->Release();
        m_pLoader = 0;
    }

    //  释放 COM
    if(0 != m_nComInit)
    {
        CoUninitialize();
        m_nComInit = 0;
    }
}




//-----------------------------------------------------
//  描述： 播放背景音乐（重复播放）
//  日期： 2005/05/29
//  参数： long nBgn 是要播放声音的编号
//  返回值： 播放成功返回 true，失败返回 false
//-----------------------------------------------------
bool CDMusic::PlayBGM(const long& nBgmNum)
{
    //  清空已加载的段
    if(0 != m_pSegment && 0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pSegment->Release();
        m_pSegment = 0;
        m_nBgmNum = 0;
    }

    //  判断要播放的音乐的编号是否在有效范围内
    if(nBgmNum < DM_BGM_MIN_NUM || nBgmNum > DM_BGM_MAX_NUM)
    {
        return true;
    }

    //  判断加载对象和演奏对象是否已经初始化
    if(0 != m_pLoader && 0 != m_pPerformance)
    {
        char  szBufferA[MAX_PATH];
        WCHAR szBufferW[MAX_PATH];

        //  根据文件格式来将编号转为字符串
        sprintf(szBufferA, DM_BGM_FILENAME, nBgmNum);
        //  将字符串从 ANSI 转为 WIDE 宽字符
        MultiByteToWideChar(CP_ACP, 0, szBufferA, -1, szBufferW, MAX_PATH);

        //  从磁盘文件中加载
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
        m_pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);   //  背景音乐，重复播放
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
//  描述： 播放特效音乐（只放一遍）
//  日期： 2005/05/29
//  参数： long nBgn 是要播放声音的编号
//  返回值： 播放成功返回 true，失败返回 false
//-----------------------------------------------------
bool CDMusic::PlayEFF(const long& nEffNum)
{
    //  清空已加载的段
    if(0 != m_pSegment && 0 != m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_pSegment->Release();
        m_pSegment = 0;
    }

    //  判断要播放的音乐是否在有效范围内
    if(nEffNum < DM_EFF_MIN_NUM || nEffNum > DM_EFF_MAX_NUM)
    {
        return true;
    }

    //  判断加载对象和演奏对象是否已经初始化
    if(0 != m_pLoader && 0 != m_pPerformance)
    {
        char  szBufferA[MAX_PATH];
        WCHAR szBufferW[MAX_PATH];

        //  根据文件格式来将编号转为字符串
        sprintf(szBufferA, DM_EFF_FILENAME, nEffNum);
        //  将字符串从 ANSI 转为 WIDE 宽字符
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

        m_pSegment->SetRepeats(0);  //  特效音乐只放一遍不重复
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
//  描述： 播放指定路径的声音文件
//  日期： 2005/06/10
//  参数： szBufferA    文件路径
//         nRepeat      重复次数
//  返回值： 播放成功返回 true，失败返回 false
//  注： 只支持 MIDI 和 WAV 文件
//-----------------------------------------------------
bool CDMusic::Play(const char* szBufferA, const DWORD& nRepeat)
{
    //  清空已加载的段
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
        //  将字符串从 ANSI 转为 WIDE 宽字符
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

        m_pSegment->SetRepeats(nRepeat);  //  设置重复次数
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
//  描述： 停止任何正在播放的背景音乐
//  日期： 2005/05/23
//-----------------------------------------------------
void CDMusic::Stop()
{
    if(m_pPerformance)
    {
        m_pPerformance->Stop(0, 0, 0, 0);
        m_nBgmNum = 0;
    }
}




