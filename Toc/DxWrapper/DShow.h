//-----------------------------------------------------
//  类名：  CDShow
//  描述：  DirectShow 封装类
//  作者：  KudoXinyiji
//
//  模式：  Singleton
//-----------------------------------------------------



#ifndef _CDSHOW_H
#define _CDSHOW_H


#include <dshow.h>
#pragma comment(lib, "strmiids.lib")


namespace DxWrapper
{

    class CDShow
    {
    public:
        bool Init(HWND hWnd);
        void Term();

        bool Play(char*);
        bool Play(const long&);

        static bool InitInstance();
        static void ExitInstance();
        static CDShow*  Instance();

    private:
        CDShow();
        ~CDShow();

        HWND m_hWnd;

        IGraphBuilder* m_pGraph;        //IGraphBuilder 接口提供了生成Filter Graph相//关的方法
        IMediaControl* m_pMediaControl; //IMediaControl 接口提供了控制流经Filter //Graph数据流的相关方法
        IMediaEventEx* m_pEvent;        //IMediaEventEx 继承自IMediaEvent，提供了从//Filter Graph 管理器获取事件消息的方法
        IMediaSeeking* m_pMediaSeeking; //IMediaSeeking 提供了控制流的播放位置和播放//速度的方法

        char m_szFilePath[MAX_PATH];    //当前播放的媒体文件的名称
        bool m_isPlaying;               //当前的播放状态


        long m_nComInit;    //  COM 是否已经初始化 
                            //  0 - 未初始化
                            //  1 - 已初始化

        long m_nDDState;    //  DirectDraw 的状态
                            //  0 - 在类外部初始化或者未初始化，不需要负责清除
                            //  1 - 调用 InitDDraw 初始化，需要负责清除

        static CDShow* m_pDShow;    //  CDShow 唯一实例的引用

    };

};


#endif

