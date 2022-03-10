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



#ifndef _CDMUSIC_H
#define _CDMUSIC_H


#define DM_BGM_MIN_NUM      01      //  ��С�����ֱ��
#define DM_BGM_MAX_NUM      64      //  �������ֱ��
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

        //  ��ʼ�� DirectMusic
        bool Init(HWND, LPDIRECTSOUND = 0);

        //  ��ֹ DirectMusic
        void Term();

        //  ���Ŵ����ļ�
        bool Play(const char*, const DWORD&);

        //  ���ű�������
        bool PlayBGM(const long&);

        //  ������Ч����
        bool PlayEFF(const long&);

        //  ֹͣ����
        void Stop();

    private:

		//  Ĭ�Ϲ��캯ʽ
        CDMusic();
        
		//  ������ʽ
		~CDMusic();

    private:

        //  �������
        IDirectMusicPerformance8* m_pPerformance;

        //  ���ض���
        IDirectMusicLoader8* m_pLoader;

        //  �ζ���
        IDirectMusicSegment8* m_pSegment;

        //  �������ֵı��
        long m_nBgmNum;

        //  COM ��ʼ�� 1��δ��ʼ�� 0
        long m_nComInit;

        //  CDMusic Ψһʵ��������
        static CDMusic* m_pDMusic;

    };

};


#endif

