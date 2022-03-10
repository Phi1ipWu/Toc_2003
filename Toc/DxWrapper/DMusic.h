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



#ifndef _CDMUSIC_H
#define _CDMUSIC_H


#define DM_BGM_MIN_NUM      01      //  最小的音乐编号
#define DM_BGM_MAX_NUM      64      //  最大的音乐编号
#define DM_BGM_FILENAME ".\\Sounds\\Bgm%03d.kmp"

#define DM_EFF_MIN_NUM      01
#define DM_EFF_MAX_NUM      16
#define DM_EFF_FILENAME ".\\Sounds\\Eff%03d.kmp"

#define DM_EFF_VICTORY01    01
#define DM_EFF_VICTORY02    02
#define DM_EFF_VICTORY03    03
#define DM_EFF_VICTORY04    04
#define DM_EFF_DEFEAT01     05
#define DM_EFF_DEFEAT02     06
#define DM_EFF_FANFARE01    07
#define DM_EFF_FANFARE02    08
#define DM_EFF_FANFARE03    09
#define DM_EFF_ITEM01       10
#define DM_EFF_ITEM02       11
#define DM_EFF_GAG01        12
#define DM_EFF_GAG02        13
#define DM_EFF_INN          14
#define DM_EFF_MYSTERY      15
#define DM_EFF_SHOCK        16



#include <dmusici.h>
#include <windows.h>


#pragma comment(lib, "dxguid.lib")



namespace DxWrapper
{

    class CDMusic
    {
    public:
        static bool InitInstance();
        static void ExitInstance();
        static CDMusic* Instance()
		{	return m_pDMusic;	}

        //  初始化 DirectMusic
        bool Init(HWND, LPDIRECTSOUND = 0);

        //  终止 DirectMusic
        void Term();

        //  播放磁盘文件
        bool Play(const char*, const DWORD&);

        //  播放背景音乐
        bool PlayBGM(const long&);

        //  播放特效音乐
        bool PlayEFF(const long&);

        //  停止播放
        void Stop();

    private:

		//  默认构造函式
        CDMusic();
        
		//  析构函式
		~CDMusic();

    private:

        //  演奏对象
        IDirectMusicPerformance8* m_pPerformance;

        //  加载对象
        IDirectMusicLoader8* m_pLoader;

        //  段对象
        IDirectMusicSegment8* m_pSegment;

        //  播放音乐的编号
        long m_nBgmNum;

        //  COM 初始化 1，未初始化 0
        long m_nComInit;

        //  CDMusic 唯一实例的引用
        static CDMusic* m_pDMusic;

    };

};


#endif

